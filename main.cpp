#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstring>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <sys/wait.h>
using namespace std;

char projectDirectory[1024];

int history(char *read)
{
    char *line;
    line = readline("\033[95mcatalina@catalina-VirtualBox\033[0m:\033[96m$\033[0m> ");
    if (line)
    {
        strcpy(read,line);
        if(strcmp(read,"") != 0)
            add_history(read);
        free(line);
    }
    return 0;
}

void path()
{
    string path = "PATH";
    string env = getenv("PATH");
    string newPath = path + "="+ env + ":" + projectDirectory + "/bin";
    setenv(path.c_str(),newPath.c_str(),1);
}

int execute(char **commandArray, int len)
{
    char loc[1024];
    string name = commandArray[0];
    string location = "/bin/" + name;
    strcpy(loc,location.c_str());

    if (strcmp(commandArray[0], "cp") == 0)
        execv("/home/catalina/CLionProjects/terminalOS/bin/cp", commandArray);
    else if (strcmp(commandArray[0], "tee") == 0)
        execv("/home/catalina/CLionProjects/terminalOS/bin/tee", commandArray);
    else if (strcmp(commandArray[0], "dirname") == 0)
        execv("/home/catalina/CLionProjects/terminalOS/bin/dirname", commandArray);
    else if (strcmp(commandArray[0], "cd") == 0)
        cout<<"Here will be the cd command..."<<endl;
    else if(execv(loc, commandArray) == -1 && strcmp(commandArray[0], "") != 0)
    {
        for(int i=0;i<len;i++)
            cout<<commandArray[i]<<" ";
        cout<<": command not found"<<endl;
    }
    else
        execv(loc, commandArray);

    return 0;
}

void separateCommand(char *line, char **commandArray)
{
    int i = 0;
    char *p = line;
    commandArray[i] = p;
    i++;
    while (*p != '\0')
    {
        if (*p == ' ')
        {
            *p = '\0';
            p++;
            commandArray[i] = p;
            i++;
        }
        else
            p++;
    }
    commandArray[i] = NULL;
}

int main(int argc, char **argv)
{
    int pid;
    char read_line[1024];
    system("clear");
    getcwd(projectDirectory, sizeof(projectDirectory));
    path();
    char *commandArray[1024];
    while (true)
    {
        history(read_line);
        separateCommand(read_line, commandArray);

        int length = 0;
        for (int i = 0; commandArray[i] != NULL; i++)
            if(commandArray[i][0] != 0)
                length++;

        if (strcmp(commandArray[0], "exit") == 0)
             exit(0);
        else
        {
            pid = fork();
            if (pid < 0)
                cout << "Error: Failed to fork child process";
            if (pid == 0)
            {
                execute(commandArray,length);
            }
            else
            {
                wait(NULL);
                continue;
            }
            return 0;
        }
    }
}