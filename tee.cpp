#include <string>
#include <iostream>
#include <string.h>

using namespace std;

int hflag = 0;
int vflag = 0;
int aflag = 0;

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
                if (argv[i][j] == 'v')
                {
                    vflag = 1;
                }
                if (argv[i][j] == 'a')
                {
                    aflag = 1;
                }
            }
        }
    }

    if(hflag != 0)
    {
        cout << "Usage: tee [OPTION]... [FILE]...\n"
                "Copy standard input to each FILE, and also to standard output.\n"
                "\n"
                "  -a, --append              append to the given FILEs, do not overwrite\n"
                "  -h, --help     display this help and exit\n"
                "\n"
                "GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n"
                "Full documentation <https://www.gnu.org/software/coreutils/tee>\n"
                "or available locally via: info '(coreutils) tee invocation'" << endl;
    }
    else if(vflag != 0)
    {
        cout << "tee version 1\n"
                "Copyright (C) 2022\n"
                "Written by Catalina Chirtes" << endl;
    }
    else if(aflag != 0)
    {
        cout << "aflag" << endl;
    }
    else
    {
        if (argc < 2) {
            cerr << "tee: missing operand\n"
                    "Try 'tee --help' for more information." << endl;
            return 1;
        }
    }
    return 0;
}