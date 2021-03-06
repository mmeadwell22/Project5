
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "function.h"

#define BUFFER_SIZE 1024

int getInput(char *input)
{
    if (strstr(input, "cd") == input)
    {
        return 1;
    }
    if (strstr(input, "path") == input)
    {
        return 2;
    }
    if (strstr(input, "quit") == input)
    {
        return 3;
    }
    return 0;
}

int main()
{
    int q = 1;
    char *arglist[MAXARGS + 1];
    /// @mmeadwell22 Did you intend `` char *argbuf = (char *)malloc(ARGLENGTH) `` ?
    char *argbuf = (char *)malloc(sizeof(ARGLENGTH));
    int numOfArgs;
    numOfArgs = 0;
    int i = 1;
    size_t in = BUFFER_SIZE;
    /// @mmeadwell22 Did you intend `` char *token = malloc(ARGLENGTH) `` ?
    char *token = malloc(sizeof(ARGLENGTH));
    char command[ARGLENGTH];
    myNode *head;
    head = (myNode *)malloc(sizeof(myNode));
    head = NULL;
    memset(arglist, 0, sizeof(arglist));
    commands_t command_list;
    int stdin_cp = dup(0);
    int stdout_cp = dup(1);

    while (q)
    {
        close(0);
        close(1);
        dup2(stdin_cp, 0);
        dup2(stdout_cp, 1);
        printPrompt();
        getline(&argbuf, &in, stdin);
        if (argbuf[0] == '\n')
        {
        }
        else
        {
            char *holder = strchr(argbuf, '\n');
            if (holder)
            {
                *holder = 0;
            }
        }

        int pipeCount = scanForPipes(argbuf);   /// Scan input for pipes
        if (pipeCount > 0)                      /// Pipes were found.
        {
            command_list.command = (char ***)calloc((size_t)pipeCount + 1, sizeof(char **));
            command_list.count = pipeCount + 1;
            command_list.numOfArgs = (int *)calloc((size_t)pipeCount + 1, sizeof(int));

            buildCommandList(&command_list, argbuf);
            if (pipeCount == 1)
            {                                   /// One pipe
                int thepipe[2];                 /// Holds two fd's for piping.
                createPipe(thepipe);
                linCommand(command_list.command[0], head, command_list.numOfArgs[0], pipeCount, thepipe, 0, 1);
                close(thepipe[1]);
                linCommand(command_list.command[1], head, command_list.numOfArgs[1], pipeCount, thepipe, 1, 0);
                close(thepipe[0]);
            }
            else if (pipeCount == 2)
            {                                   /// Two pipes
                int thepipe[4];                 /// Holds four fd's for piping.
                createPipe(thepipe);
                createPipe(&thepipe[2]);
                linCommand(command_list.command[0], head, command_list.numOfArgs[0], pipeCount, thepipe, 0, 1);
                close(thepipe[1]);
                linCommand(command_list.command[1], head, command_list.numOfArgs[1], pipeCount, thepipe, 1, 1);
                close(thepipe[0]);
                close(thepipe[3]);
                linCommand(command_list.command[2], head, command_list.numOfArgs[2], pipeCount, thepipe, 1, 0);
                close(thepipe[2]);
            }
            //garbageCollectCommandList(&command_list);
        }
        else if (pipeCount == -1)               /// Hanging pipe character detected.
        {
            printf("please re-try your command\n");
            break;
        }
        else
        {
            arglist[0] = strtok(argbuf, " ");
            numOfArgs = 1;
            strcpy(command, arglist[0]);
            //first argument contains command
            i = 1;
            //rest of the command line arguments
            int tempcount = 0;
            while (numOfArgs < MAXARGS)
            {
                token = strtok(NULL, " ");
                if (token == NULL)
                {
                    tempcount++;
                }
                arglist[i] = token;
                numOfArgs++;
                i++;
            }
            switch (getInput(command))
            {
            case 1:
                cd(arglist);
                memset(arglist, 0, sizeof(argbuf));
                break;
            case 2:
                head = path(arglist, head);
                memset(arglist, 0, sizeof(argbuf));
                break;
            case 3:
                q = quitShell();
                exit(1);
                break;
            default:
                if (argbuf[0] == '\n')
                {
                }
                else
                {
                    int total = numOfArgs - tempcount;
                    int *thepipe = NULL;
                    pipeCount = 0;
                    linCommand(arglist, head, total, pipeCount, thepipe, 0, 0);
                    close(0);
                    close(1);
                    dup2(stdin_cp, 0);
                    dup2(stdout_cp, 1);
                }
            }
        }
    }
    return 0;

}