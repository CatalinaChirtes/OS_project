#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstring>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <sys/wait.h>
#include <vector>

using namespace std;

int history(string &read)
{
    string line;
    line = readline("catalina@catalina-VirtualBox$> ");
    if (!line.empty())
    {
        read = line;
        if(!read.empty())
            add_history(read.c_str());
    }
    else
    {
        read = "";
    }
    return 0;
}

int execute(char **commandArray, int len)
{
    char loc[1024];
    string name = commandArray[0];
    string location = "/bin/" + name;
    strcpy(loc,location.c_str());

    if (strcmp(commandArray[0], "cp") == 0)
        execv("bin/cp", commandArray);
    else if (strcmp(commandArray[0], "tee") == 0)
        execv("bin/tee", commandArray);
    else if (strcmp(commandArray[0], "dirname") == 0)
        execv("bin/dirname", commandArray);
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

// we split our string every time we encounter the " " space character
// then we append the newly created strings, representing our arguments, into an array of arguments
void separateCommand(string &line, char **commandArray)
{
    size_t start = 0;
    size_t end = line.find(' ');
    int i = 0;
    while (end != string::npos)
    {
        commandArray[i] = new char[end - start + 1];
        strcpy(commandArray[i], line.substr(start, end - start).c_str());
        start = end + 1;
        end = line.find(' ', start);
        i++;
    }
    commandArray[i] = new char[line.size() - start + 1];
    strcpy(commandArray[i], line.substr(start).c_str());
    i++;
    commandArray[i] = NULL;
}

// we split our string every time we encounter the " | " pipe character
// then we append the newly created strings, representing our commands, into an array of commands
void separateCommandByPipes(string &line, char **pipedCommandsArray)
{
    size_t start = 0;
    string charPipe = " | ";
    size_t end = line.find(charPipe);
    int i = 0;
    while (end != string::npos)
    {
        pipedCommandsArray[i] = new char[end - start + 1];
        strcpy(pipedCommandsArray[i], line.substr(start, end - start).c_str());
        start = end + 3;
        end = line.find(charPipe, start);
        i++;
    }
    pipedCommandsArray[i] = new char[line.size() - start + 1];
    strcpy(pipedCommandsArray[i], line.substr(start).c_str());
    i++;
    pipedCommandsArray[i] = NULL;
}

int main(int argc, char **argv)
{
    string read_line;
    system("clear");
    char *pipedCommandsArray[1024];
    char *commandArray[1024];
    while (true)
    {
        history(read_line);
        separateCommandByPipes(read_line, pipedCommandsArray);

        // computing the length of the array that holds the commands separated by the '|' pipe
        int pipeArrayLen = 0;
        for (int i = 0; pipedCommandsArray[i] != NULL; i++)
            if(pipedCommandsArray[i][0] != 0)
                pipeArrayLen++;

        // the number of pipes '|' characters is the length of the array-1
        int pipesCount = pipeArrayLen - 1;
        int status;
        int fd[pipesCount][2];
        pid_t pids[pipesCount+1];

        // we create as many pipes as we need
        for (int pipeNumber = 0; pipeNumber < pipesCount; ++pipeNumber)
        {
            if (pipe(fd[pipeNumber]) == -1)
            {
                perror("Pipe failed");
                return 1;
            }
        }

        for(int i = 0; i < pipeArrayLen; i++)
        {
            string str(pipedCommandsArray[i]);
            separateCommand(str, commandArray);

            // computing the length of the array that holds the arguments of every command separated by space " "
            int commandArrayLen = 0;
            for (int j = 0; commandArray[j] != NULL; j++)
                if(commandArray[j] != NULL)
                    commandArrayLen++;

            // exiting if we input 'exit' in the terminal
            if(strcmp(commandArray[0], "exit") == 0)
                exit(0);

            pids[i] = fork();
            if (pids[i] < 0)
                cout << "Error: Failed to fork child process";

            if (pids[i] == 0)
            {
                // we check if the current command is not the first child process in the pipeline
                // if it's not the first, then the output of the previous command becomes available through a pipe as the input of this command
                if (i > 0)
                {
                    if (dup2(fd[i-1][0], STDIN_FILENO) == -1)
                    {
                        perror("dup2 failed");
                        return 1;
                    }
                }
                // we check if there is another command following this one
                // if there is any command to execute, then the output of this current process will be sent to the next process through a pipe, so that it will become the input of the next command
                if (pipedCommandsArray[i+1] != NULL)
                {
                    if (dup2(fd[i][1], STDOUT_FILENO) == -1)
                    {
                        perror("dup2 failed");
                        return 1;
                    }
                }

                // we close all the opened pipes in the child
                for (int i = 0; i < pipesCount; ++i)
                {
                    close(fd[i][0]);
                    close(fd[i][1]);
                }
                execute(commandArray, commandArrayLen);
                return 0;
            }
        }

        // we close all the opened pipes in the parent
        for (int i = 0; i < pipesCount; ++i)
        {
            close(fd[i][0]);
            close(fd[i][1]);
        }
        // waiting for all the child processes to finish
        for (int i = 0; i < pipesCount+1; ++i)
        {
            waitpid(pids[i], &status, 0);
        }
    }
}