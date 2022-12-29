#include <string>
#include <iostream>
#include <string.h>

using namespace std;

int hflag = 0;
int vflag = 0;

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
            }
        }
    }

    if(hflag != 0)
    {
        cout << "Usage: dirname [OPTION] NAME...\n"
                "Output each NAME with its last non-slash component and trailing slashes\n"
                "removed; if NAME contains no /'s, output '.' (meaning the current directory).\n"
                "\n"
                "  -h, --help     display this help and exit\n"
                "  -v, --version  output version information and exit\n"
                "\n"
                "Examples:\n"
                "  dirname /usr/bin/          -> \"/usr\"\n"
                "  dirname dir1/str dir2/str  -> \"dir1\" followed by \"dir2\"\n"
                "  dirname stdio.h            -> \".\"\n"
                "\n"
                "GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n"
                "Full documentation <https://www.gnu.org/software/coreutils/dirname>\n"
                "or available locally via: info '(coreutils) dirname invocation'" << endl;
    }
    else if(vflag != 0)
    {
        cout << "dirname version 1\n"
                "Copyright (C) 2022\n"
                "Written by Catalina Chirtes" << endl;
    }
    else
    {
        if (argc < 2) {
            cerr << "dirname: missing operand\n"
                    "Try 'dirname --help' for more information." << endl;
            return 1;
        }
        for (int i = 1; i < argc; i++)
        {
            string path = argv[i];
            path.erase(path.find_last_not_of('\0') + 1);
            // removing any trailing '/' characters
            if (!path.empty() && path.back() == '/')
            {
                path.pop_back();
            }
            // finding the last occurrence of '/'
            size_t pos = path.find_last_of('/');
            // '/' not found, returning the current directory '.'
            if (pos == string::npos)
            {
                cout << "." << endl;
                continue;
            }
            // path is the root directory if the position is 0
            if (pos == 0)
            {
                cout << "/" << endl;
                continue;
            }
            // the substring before the last '/'
            cout << path.substr(0, pos) << endl;
        }
    }
    return 0;
}