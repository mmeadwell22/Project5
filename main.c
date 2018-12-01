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
    char *argbuf = (char *)malloc(sizeof(ARGLENGTH));
    int numOfArgs;
    numOfArgs = 0;
    int i = 1;
    size_t in = BUFFER_SIZE;
    char *token = malloc(sizeof(ARGLENGTH));
    char command[ARGLENGTH];
    myNode *head;
    head = (myNode *)malloc(sizeof(myNode));
    head = NULL;
    memset(arglist, 0, sizeof(arglist));
    commands_t command_list;
    int thepipe[2]; /// Holds two fd's for piping.

    while (q)
    {
        printPrompt();
        getline(&argbuf, &in, stdin);
        if(argbuf[0] == '\n')
        {

        }
        else
        {
            char *holder = strchr(argbuf, '\n');
            if(holder)
            {
                *holder = 0;
            }
        }

        /// Scan input for pipes
        int pipeCount = scanForPipes(argbuf);
        /// Pipes were found.
        /// 1) Build a list of the commands that need to be executed.
        /// 2) Create pipe.
        /// 3) Run modified linCommand
        if(pipeCount > 0){
            command_list.command = (char ***) calloc((size_t) pipeCount + 1, sizeof(char **));
            command_list.count = pipeCount + 1;
            command_list.numOfArgs = (int *) calloc((size_t) pipeCount + 1, sizeof(int));
            /// @mmeadwell22 I am still trying to get this function to properly organize the user input.
            /// you can proceed with the writing the code and assume that it works.

            buildCommandList(&command_list, argbuf);
            createPipe(thepipe);
            /// Loop through each command in the command list and set the pipe flags accordingly.
            for(int j = 0; j < command_list.count; j++){
                if(j == 0){ /// Only redirect stdout
                    //linCommand(command_list.command[j], head, command_list.numOfArgs[j], thepipe, 0, 1);
                }
                else if (j == 1 && command_list.count == 3 ){ /// Redirect both stdin & stdout
                    //linCommand(command_list.command[j], head, command_list.numOfArgs[j], thepipe, 1, 1);
                }
                else if( j == 1 && command_list.count == 2){ /// Only redirect stdin
                    //linCommand(command_list.command[j], head, command_list.numOfArgs[j], thepipe, 1, 0);
                }
                else if( j == 2){ /// Only redirect stdin
                    //linCommand(command_list.command[j], head, command_list.numOfArgs[j], thepipe, 1, 0);
                }
            }

        }
        else if (pipeCount == -1){
            /// Hanging pipe character detected.
            printf("please re-try your command");
            break;
        }

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
                linCommand(arglist, head, total, thepipe, 0, 0);
            }
        }
    }
    return 0;
}