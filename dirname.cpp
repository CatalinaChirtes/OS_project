#include <unistd.h>
#include <string>
#include <getopt.h>
#include <iostream>

using namespace std;

struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'}
};

int main(int argc, char **argv)
{
    int hflag = 0;
    int vflag = 0;
    int n;
    while ((n = getopt_long(argc, argv, "hv", long_options, NULL)) != -1)
    {
        if(n == 'h')
        {
            hflag = 1;
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
    else if(vflag != 0)
        cout << "dirname version 1\n"
                "Copyright (C) 2022\n"
                "Written by Catalina Chirtes" << endl;
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
            if (!path.empty() && path.back() == '/')  // removing any trailing '/' characters
            {
                path.pop_back();
            }
            size_t pos = path.find_last_of('/');  // finding the last occurrence of '/'
            if (pos == string::npos)
            {
                cout << "." << endl;  // '/' not found, returning the current directory '.'
                continue;
            }
            if (pos == 0) {
                cout << "/" << endl;  // path is the root directory
                continue;
            }
            cout << path.substr(0, pos) << endl;  // the substring before the last '/'
        }
    }
    return 0;
}