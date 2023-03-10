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
                    if (argv[i+1] != NULL && isDir(argv[i+1]) == 0 && isFileExists(argv[i+1]) == 1)
                    {
                        strcpy(destination,argv[i+1]);
                    }
                    else if(isFileExists(argv[i+1]) == 0)
                    {
                        cout << "cp: failed to access '" << argv[i+1] << "': No such file or directory" << endl;
                    }
                    else
                    {
                        cout << "cp: target '" << argv[i+1] << "' is not a directory" << endl;
                    }
                    return;
                }
            }
        }
    }
    else
    {
        int argumenteCP = 0;
        for(int i=1;i<argc;i++)
        {
            if (argv[i][0] != '-')
            {
                argumenteCP++;
            }
        }
        if(argc >= 3 && argumenteCP == 2)
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
                            cout << "Too many parameters! Only the first two parameters were used in the copy command. \n";
                            return;
                        }
                        strcpy(dest_path,argv[i]);
                        dest_allocated = true;
                    }
                }
            }
        }
        else
        {
            for(int i = argc - 1; i > 1; i--)
            {
                if (argv[i][0] != '-')
                {
                    if (isDir(argv[i]) == 0 && isFileExists(argv[i]) == 1)
                    {
                        strcpy(destination,argv[i]);
                        dest_allocated = true;
                    }
                    else
                    {
                        cout<< "cp: target '" << argv[i] << "' is not a directory" << endl;
                    }

                    if (dest_allocated == true)
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
                        sourceArraySize = count;
                    }

                    return;
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
        cout << "Source file does not exist!\n";
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
            cout << "'" << source << "'" << " -> " << "'" << destFile << "'" << "\n";
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
    struct dirent *dp;
    char destFile[1024];
    char sourceFile[1024];

    if (vflag != 0)
    {
        cout << "'" << source << "'" << " -> " << "'" << dest << "'" << "\n";
    }
    if (iflag != 0)
    {
        return 1;
    }

    DIR *dir = opendir(source.c_str());

    if (!dir)
        return 1;
    else
    {
        if (mkdir(dest, 0777) == -1)
            cout << "Error :  " << strerror(errno) << endl;

        while ((dp = readdir(dir)) != NULL)
        {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
            {
                strcpy(sourceFile, source.c_str());
                strcat(sourceFile, "/");
                strcat(sourceFile, dp->d_name);

                strcpy(destFile, dest);
                strcat(destFile, "/");
                strcat(destFile, dp->d_name);

                if (isDir(sourceFile) == 0)
                {
                    copy_recursive(sourceFile, destFile);
                }
                else
                {
                    copy(sourceFile, destFile);
                }
            }
        }
        closedir(dir);
    }

    return 0;
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

        copy_recursive(source_path,dest_path);
    }

    else if(tflag != 0)
    {
        if (isDir(destination) != 0)
        {
            cout << "When using -t flag the destination must be a folder!\n";
        }
        else
        {
            int countFolder = 0;
            for(int i=0; i<sourceArraySize; i++)
            {
                if(isDir(sourceArray[i]) == 0)
                {
                    countFolder++;
                    break;
                }
            }
            if(countFolder == 0)
            {
                for(int i=0; i<sourceArraySize; i++)
                {
                    copy(sourceArray[i],destination);
                }
            }
            else
            {
                cout << "cp: -r not specified; not allowed to copy a folder to a file or a folder to a folder" << endl;
            }
        }
    }
    else
    {
        int argumenteCP = 0;
        for(int i=1;i<argc;i++)
        {
            if (argv[i][0] != '-')
            {
                argumenteCP++;
            }
        }
        if (argc < 3)
        {
            cout << "cp: missing file operand\n"
                    "Try 'cp --help' for more information." << endl;
            return 0;
        }
        else if(argc >= 3 && argumenteCP == 2)
        {
            if(isDir(source_path) == 0)
            {
                cout << "cp: -r not specified; the source '" << source_path << "' must be a file" << endl;
            }
            else
            {
                copy(source_path,dest_path);
            }
        }
        else
        {
            int countFolder = 0;
            for(int i=0; i<sourceArraySize; i++)
            {
                if(isDir(sourceArray[i]) == 0)
                {
                    countFolder++;
                    break;
                }
            }
            if(countFolder == 0)
            {
                for(int i=0; i<sourceArraySize; i++)
                {
                    copy(sourceArray[i],destination);
                }
            }
            else
            {
                cout << "cp: -r not specified; not allowed to copy a folder to a file or a folder to a folder" << endl;
            }
        }
    }
}