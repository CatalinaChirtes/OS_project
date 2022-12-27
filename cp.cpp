#include <string.h>
#include <string>
#include <getopt.h>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fstream>

using namespace std;

struct option long_options[] = {
        {"help", no_argument, 0, 'h'}
};

int isDir(const char* fileName)
{
    struct stat path;
    stat(fileName, &path);
    return S_ISREG(path.st_mode);
}

void argType(int argc, char **argv,char **sourceArray,char *destination)
{
    for(int i=1;i<argc;i++)
        if(argv[i][0] == '-')
        {
            if(argv[i][1] == 't')
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
                strcpy(destination,argv[i+1]);
            }
            else
            {
                if(i == 1)
                {
                    sourceArray[0] = argv[i+1];
                    strcpy(destination,argv[i+2]);
                }
                else if(i == 2)
                {
                    sourceArray[0] = argv[i-1];
                    strcpy(destination,argv[i+1]);
                }
                else if(i == 3)
                {
                    sourceArray[0] = argv[i-2];
                    strcpy(destination,argv[i-1]);
                }
            }
        }
}

int copy_folder(char *source_path, char *dest_path)
{
    char destFile[1024];

    strcpy(destFile, dest_path);
    strcat(destFile, "/");
    strcat(destFile, source_path);

    FILE *source = fopen(source_path, "r");

    FILE *dest = fopen(destFile, "w");
    char buffer[1024];
    while (true) {
        size_t bytes_read = fread(buffer, 1, sizeof(buffer), source);
        if (bytes_read == 0)
            break;
        fwrite(buffer, 1, bytes_read, dest);
    }
    fclose(dest);
    fclose(source);

    return 0;
}

int copy_file(char *source_path, char *dest_path)
{
    char destFile[1024];

    strcpy(destFile, dest_path);
    FILE *source = fopen(source_path, "r");
    ofstream newFile(destFile);

    FILE *dest = fopen(destFile, "w");
    char buffer[1024];
    while (true) {
        size_t bytes_read = fread(buffer, 1, sizeof(buffer), source);
        if (bytes_read == 0)
            break;
        fwrite(buffer, 1, bytes_read, dest);
    }
    fclose(dest);
    fclose(source);

    return 0;
}

int main(int argc, char **argv)
{
    char *sourceArray[1024];
    char destination[1024];
    char *source_path = argv[1];
    char *dest_path = argv[2];
    int hflag = 0;
    int iflag = 0;
    int rflag = 0;
    int tflag = 0;
    int vflag = 0;
    int n;
    while ((n = getopt_long(argc, argv, "hirRtv", long_options, NULL)) != -1)
    {
        if(n == 'h')
        {
            hflag = 1;
            break;
        }
        if(n == 'i')
        {
            iflag = 1;
            break;
        }
        if(n == 'r' || n == 'R')
        {
            rflag = 1;
            break;
        }
        if(n == 't')
        {
            tflag = 1;
            break;
        }
        if(n == 'v')
        {
            vflag = 1;
            break;
        }
        else
        {
            cerr << "Error: invalid option" << endl;
            break;
        }
    }
    if(hflag != 0)
        cout << "Usage: cp [OPTION]... [-T] SOURCE DEST\n"
                "  or:  cp [OPTION]... SOURCE... DIRECTORY\n"
                "  or:  cp [OPTION]... -t DIRECTORY SOURCE...t\n"
                "Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n"
                "\n"
                "Mandatory arguments to long options are mandatory for short options too.\n"
                "  -i, --interactive            prompt before overwrite (overrides a previous -n\n"
                "                                  option)\n"
                "  -R, -r, --recursive          copy directories recursively\n"
                "  -t, --target-directory=DIRECTORY  copy all SOURCE arguments into DIRECTORY\n"
                "  -v, --verbose                explain what is being done\n"
                "      --help     display this help and exit\n"
                "\n"
                "GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n"
                "Full documentation <https://www.gnu.org/software/coreutils/cp>\n"
                "or available locally via: info '(coreutils) cp invocation'" << endl;
    else if(iflag != 0)
    {
        argType(argc,argv,sourceArray,destination);
        cout << "cp: overwrite '" << destination <<"'?";
        getchar();
        if (isDir(destination) == 0)
            copy_folder(sourceArray[0],destination);
        else
            copy_file(sourceArray[0],destination);
    }
    else if(rflag != 0)
    {

    }
    else if(tflag != 0)
    {
        argType(argc,argv,sourceArray,destination);
        for(int i=0;i<sizeof(sourceArray);i++)
            copy_folder(sourceArray[i],destination);
    }
    else if(vflag != 0)
    {
        argType(argc,argv,sourceArray,destination);
        cout << "'" << sourceArray[0] << "' -> '" << destination << "'" << endl;
        if (isDir(destination) == 0)
            copy_folder(sourceArray[0],destination);
        else
            copy_file(sourceArray[0],destination);
    }
    else
    {
        argType(argc,argv,sourceArray,destination);
        if (argc < 3) {
            cout << "cp: missing file operand\n"
                    "Try 'cp --help' for more information." << endl;
            return 1;
        }
        if (isDir(dest_path) == 0)
            copy_folder(source_path,dest_path);
        else
            copy_file(source_path,dest_path);
    }
    return 0;
}