#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstring>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <sys/wait.h>
#include <vector>
#include <regex>
#include <fcntl.h>
#include <fstream>

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

// we split our string every time we encounter the " | " pipe character
// then we append the newly created strings, representing our commands, into an array of commands
void separateCommandByPipes(string &line, char **pipedCommandsArray)
{
    size_t start = 0;
    string charPipe = "|";
    size_t end = line.find(charPipe);
    int i = 0;
    while (end != string::npos)
    {
        pipedCommandsArray[i] = new char[end - start + 1];
        strcpy(pipedCommandsArray[i], line.substr(start, end - start).c_str());
        start = end + 1;
        end = line.find(charPipe, start);
        i++;
    }
    pipedCommandsArray[i] = new char[line.size() - start + 1];
    strcpy(pipedCommandsArray[i], line.substr(start).c_str());
    i++;
    pipedCommandsArray[i] = NULL;
}

// we split our string every time we encounter the " " space character
// then we append the newly created strings, representing our arguments, into an array of arguments
// this function recognizes also arguments with "" or ''
void separateCommand(string &line, char **commandArray) {
    line = regex_replace(line, regex(" +"), " ");
    line = regex_replace(line, regex("^ +"), "");
    line = regex_replace(line, regex(" +$"), "");
    size_t start = 0;
    size_t end = line.find(' ');
    int i = 0;
    char currentChar = ' ';
    while (end != string::npos || start < line.size())
    {
        if (start >= line.size())
        {
            break;
        }
        if (line[start] == '\'' || line[start] == '\"')
        {
            currentChar = line[start];
            start++;
            end = line.find(currentChar, start);
            if (end == string::npos)
            {
                end = line.size();
            }
        }
        else
        {
            end = line.find(' ', start);
            if (end == string::npos)
            {
                end = line.size();
            }
        }
        commandArray[i] = new char[end - start + 1];
        strcpy(commandArray[i], line.substr(start, end - start).c_str());
        start = end + 1;
        i++;
    }
    commandArray[i] = NULL;
}

int execute(char **commandArray, int len)
{
    char loc[1024];
    string name = commandArray[0];
    string location = "/bin/" + name;
    strcpy(loc,location.c_str());

    // searching in the array for the redirect signs until we find them
    int redirectIndex = -1;
    for(int i=0; commandArray[i] != NULL; i++)
    {
        if(strcmp(commandArray[i], ">") == 0 || strcmp(commandArray[i], ">>") == 0)
        {
            redirectIndex = i;
            break;
        }
    }

    int inversRedirectIndex = -1;
    for(int i=0; commandArray[i] != NULL; i++)
    {
        if(strcmp(commandArray[i], "<") == 0)
        {
            inversRedirectIndex = i;
            break;
        }
    }

    if(redirectIndex == -1 && inversRedirectIndex == -1)
    {
        if (strcmp(commandArray[0], "cp") == 0)
            execv("bin/cp", commandArray);
        else if (strcmp(commandArray[0], "tee") == 0)
            execv("bin/tee", commandArray);
        else if (strcmp(commandArray[0], "dirname") == 0)
            execv("bin/dirname", commandArray);
        else if (strcmp(commandArray[0], "cd") == 0)
            cout<<"The cd command is not supported yet."<<endl;
        else if(execv(loc, commandArray) == -1 && strcmp(commandArray[0], "") != 0)
        {
            for(int i=0;i<len;i++)
                cout<<commandArray[i]<<" ";
            cout<<": command not found"<<endl;
        }
        else
            execv(loc, commandArray);
    }
    else if(redirectIndex != -1)
    {
        string filename = commandArray[redirectIndex + 1];
        int f;
        if (strcmp(commandArray[redirectIndex], ">") == 0)
        {
            f = open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
        }
        else // if commandArray[redirectIndex] is ">>"
        {
            f = open(filename.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0644);
        }
        // redirecting the output of the previous command to the file
        dup2(f, STDOUT_FILENO);
        // removing the redirect ">" or ">>" and the name of the file from the array
        commandArray[redirectIndex] = NULL;
        // we execute the remaining command, the command that is situated before the redirect
        return execvp(commandArray[0], commandArray);
    }
    // doesn't work yet
    else if(inversRedirectIndex != -1)
    {
        string filename = commandArray[inversRedirectIndex + 1];
        int f;
        if (strcmp(commandArray[inversRedirectIndex], "<") == 0)
        {
            f = open(filename.c_str(), O_RDONLY);
        }
        // redirecting the input of the file to the command
        dup2(f, STDIN_FILENO);
        // closing the file descriptor
        close(f);
        // removing the redirect ">" and the name of the file from the array
        commandArray[inversRedirectIndex] = NULL;
        // we execute the remaining command, the command that is situated after the redirect
        return execvp(commandArray[0], commandArray);
    }

    return 0;
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