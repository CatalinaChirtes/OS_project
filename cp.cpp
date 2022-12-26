#include <unistd.h>
#include <string>
#include <getopt.h>
#include <iostream>

using namespace std;

struct option long_options[] = {
        {"help", no_argument, 0, 'h'}
};

int main(int argc, char **argv)
{
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
                "Examples:\n"
                "  dirname /usr/bin/          -> \"/usr\"\n"
                "  dirname dir1/str dir2/str  -> \"dir1\" followed by \"dir2\"\n"
                "  dirname stdio.h            -> \".\"\n"
                "\n"
                "GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n"
                "Full documentation <https://www.gnu.org/software/coreutils/cp>\n"
                "or available locally via: info '(coreutils) cp invocation'" << endl;
    else if(iflag != 0)
    {

    }
    else if(rflag != 0)
    {

    }
    else if(tflag != 0)
    {

    }
    else if(vflag != 0)
    {

    }
    else
    {
        if (argc < 3) {
            cerr << "cp: missing file operand\n"
                    "Try 'cp --help' for more information." << endl;
            return 1;
        }
    }
    return 0;
}