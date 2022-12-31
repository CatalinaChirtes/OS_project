#include <string.h>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>

using namespace std;
int hflag = 0;
int iflag = 0;
int rflag = 0;
int tflag = 0;
int vflag = 0;
char source_path[1024];
char dest_path[1024];
char *sourceArray[1024];
char destination[1024];
int sourceArraySize = 0;
string sourceFolder[1024];
int sourceFolderSize = 0;

int isDir(const char* fileName)
{
    // check if path is folder
    // returns 0 if given path is a folder
    struct stat path;
    stat(fileName, &path);
    return S_ISREG(path.st_mode);
}

int isFileExists(const char *path)
{
    // check for file existence
    // returns 1 if file exists otherwise returns 0
    if (access(path, F_OK) == -1)
        return 0;

    return 1;
}

void argType(int argc, char **argv, char **sourceArray, char *destination)
{
    bool src_allocated = false;
    bool dest_allocated = false;
    if (tflag != 0)
    {
        for(int i=1;i<argc;i++)
        {
            if(argv[i][0] == '-')
            {
                if(argv[i][1] == 't') // -t flag - allocate source as array of strings for multiple source files and destination
                {
                    int count = 0;
                    for (int k = i - 1; k > 0; k--)
                    {
                        if(argv[k][0] != '-')
                        {
                            sourceArray[count] = argv[k];
                            count++;
                        }
                    }
                    for (int k=i+2;k<argc;k++)
                    {
                        if(argv[k][0] != '-')
                        {
                            sourceArray[count] = argv[k];
                            count++;
                        }
                    }
                    sourceArraySize = count;
                    if (argv[i+1] != NULL)
                    {
                        strcpy(destination,argv[i+1]);
                    }
                    else
                    {
                        cout << "Please provide destination folder for -t parameter\n";
                    }
                    return;
                }
            }
        }
    }
    else
    {
        for(int i=1;i<argc;i++)
        {
            if (argv[i][0] != '-')
            {
                if (src_allocated == false)
                {
                    strcpy(source_path,argv[i]);
                    src_allocated = true;
                }
                else
                {
                    if (dest_allocated == true)
                    {
                        cout << "Too many parameters! Only the first two paramaters were used in the copy command. \n";
                        return;
                    }
                    strcpy(dest_path,argv[i]);
                    dest_allocated = true;
                }
            }
        }
    }
}

int copy(char *source, char *dest)
{
    char destFile[1024];
    strcpy(destFile, dest);
    // check if source file exists
    if (isFileExists(source) == 0){
        cout << "Source file does not exists!\n";
        return 0;
    }

    // check if destination exists and it is a folder
    if ((isFileExists(dest) == 1) && (isDir(dest)) == 0){
        strcpy(destFile, dest);
        strcat(destFile, "/");
        strcat(destFile, source);
    }

    // check if destination file exists and creates it if not
    if (isFileExists(destFile) == 0)
    {
        ofstream newFile(destFile);
    }
    else
    {
        if (iflag != 0){
            cout << "cp: overwrite '" << destFile <<"'?";
            getchar();
        }
    }

    if ((isFileExists(destFile) == 1) && (isFileExists(source) == 1))
    {
        if (vflag != 0)
        {
            cout << source << " -> " << destFile << "\n";
        }

        FILE *file_open = fopen(source, "r");
        FILE *file_write = fopen(destFile, "w");
        char buffer[1024];
        while (true)
        {
            size_t bytes_read = fread(buffer, 1, sizeof(buffer), file_open);
            if (bytes_read == 0)
                break;
            fwrite(buffer, 1, bytes_read, file_write);
        }
        fclose(file_open);
        fclose(file_write);
    }
    else
    {
        cout << "Source or destination are not valid!\n";
    }

    return 0;
}

int copy_recursive(string source, char *dest)
{
    char destFile[1024];
    strcpy(destFile, dest);

    if ((isDir(dest)) == 0)
    {
        if (isFileExists(dest) == 1)
        {
            strcpy(destFile, dest);
            strcat(destFile, "/");
            strcat(destFile, source.c_str());
        }
        else
        {
            if (mkdir(dest, 0777) == -1)
                cout << "Error :  " << strerror(errno) << endl;
            strcpy(destFile, dest);
            strcat(destFile, "/");
            strcat(destFile, source.c_str());
        }
    }

    if (isDir(source.c_str()) == 0)
    {
        if (isFileExists(destFile) != 1)
        {
            if (mkdir(destFile, 0777) == -1)
                cout << "Error :  " << strerror(errno) << endl;
        }
    }

    if (isDir(source.c_str()) != 0)
    {
        if (isFileExists(destFile) == 0)
        {
            ofstream newFile(destFile);
        }
        else
        {
            if (iflag != 0)
            {
                cout << "cp: overwrite '" << destFile <<"'?";
                getchar();
            }
        }
        if (vflag != 0)
        {
            cout << source << " -> " << destFile << "\n";
        }

        FILE *file_open = fopen(source.c_str(), "r");
        FILE *file_write = fopen(destFile, "w");
        char buffer[1024];
        while (true)
        {
            size_t bytes_read = fread(buffer, 1, sizeof(buffer), file_open);
            if (bytes_read == 0)
                break;
            fwrite(buffer, 1, bytes_read, file_write);
        }
        fclose(file_open);
        fclose(file_write);

    }

    return 0;
}

void generate_source_array(string source_folder, const int root)
{
    int i;
    string path;
    struct dirent *dp;
    DIR *dir = opendir(source_folder.c_str());

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            path = (source_folder + "/" + dp->d_name);
            sourceFolder[sourceFolderSize] = path;
            sourceFolderSize++;

            generate_source_array(path, root + 2);
        }
    }
}

int main(int argc, char **argv)
{
    for (int i=0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for(int j=1; j<strlen(argv[i]); j++)
            {
                if (argv[i][j] == 'h')
                {
                    hflag = 1;
                }
                if (argv[i][j] == 'i')
                {
                    iflag = 1;
                }
                if (argv[i][j] == 'v')
                {
                    vflag = 1;
                }
                if (argv[i][j] == 't')
                {
                    tflag = 1;
                }
                if ((argv[i][j] == 'r')||(argv[i][j] == 'R'))
                {
                    rflag = 1;
                }
            }
        }
    }

    argType(argc,argv,sourceArray,destination);

    if(hflag != 0)
    {
        cout << "Usage: cp [OPTION]... [-T] SOURCE DEST\n"
                "  or:  cp [OPTION]... SOURCE... DIRECTORY\n"
                "  or:  cp [OPTION]... -t DIRECTORY SOURCE...t\n"
                "Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n"
                "\n"
                "  -i                 prompt before overwrite (overrides a previous -n\n"
                "                                  option)\n"
                "  -R, -r             copy directories recursively\n"
                "  -t                 copy all SOURCE arguments into DIRECTORY\n"
                "  -v                 explain what is being done\n"
                "  -h                 display this help and exit\n"
                "\n"
                "GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n"
                "Full documentation <https://www.gnu.org/software/coreutils/cp>\n"
                "or available locally via: info '(coreutils) cp invocation'" << endl;
    }
    else if(rflag != 0)
    {

        if (isDir(source_path) != 0)
        {
            cout << "When using -r/-R flag the source must be a folder!\n";
            return 1;
        }
        if (isDir(dest_path) != 0)
        {
            cout << "When using -r/-R flag the destination must be a folder!\n";
            return 1;
        }

        if (isFileExists(source_path) != 1)
        {
            cout << "Source folder is invalid!\n";
            return 1;
        }

        if (isFileExists(dest_path) != 1)
        {
            if (mkdir(dest_path, 0777) == -1)
                cout << "Error :  " << strerror(errno) << endl;
        }

        sourceFolderSize = 0;
        generate_source_array(source_path, 0);
        for (int i=0; i<sourceFolderSize; i++)
        {
            size_t pos = sourceFolder[i].find('/');
            if (pos != string::npos)
            {
                sourceFolder[i] = sourceFolder[i].substr(pos+1);
            }
            copy_recursive(sourceFolder[i],dest_path);
        }
    }

    else if(tflag != 0)
    {
        if (isDir(destination) != 0)
        {
            cout << "When using -t flag the destination must be a folder!\n";
        }
        else
        {
            for(int i=0; i<sourceArraySize; i++)
            {
                copy(sourceArray[i],destination);
            }
        }
    }
    else
    {
        if (argc < 3)
        {
            cout << "cp: missing file operand\n"
                    "Try 'cp --help' for more information.\n" << endl;
            return 0;
        }
        copy(source_path,dest_path);
    }
}