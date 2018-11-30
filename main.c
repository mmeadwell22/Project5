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
        arglist[0] = strtok(argbuf, " ");
        numOfArgs = 1;
        strcpy(command, arglist[0]);
        //first argument cotains command
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
                linCommand(arglist, head, total);
            }
        }
    }
    return 0;
}