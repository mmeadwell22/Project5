#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "function.h"

char cwd[100];
pid_t pid;
int status;
int lengthList = 0;

void cd(char *arglist[])
{
    if (strstr(arglist[0], "cd") != arglist[0])
    {
        printf("error please restart\n");
        return;
    }
    if (arglist[1] == NULL)
    {
        //for linux
        chdir("/home");
    }
    else
    {
        char *path = malloc(sizeof(arglist[1]));
        strcpy(path, arglist[1]);
        int error = chdir(path);

        //error handling
        switch (error)
        {
        case EACCES:
            perror("Permission Denied");
            break;
        case EIO:
            perror("Error with input output");
            break;
        case ENAMETOOLONG:
            perror("Path is to long");
            break;
        case ENOTDIR:
            perror("A component of path not a directory");
            break;
        case ENOENT:
            perror("No such file or directory");
            break;
        case 0:
            break;
        default:
            perror("Could not change directory to");
            printf(" %s\n", path);
        }
    }
    return;
}

myNode *path(char *arglist[], myNode *head)
{
    myNode *current;
    if (strstr(arglist[0], "path") == arglist[0] && arglist[1] == NULL)
    {
        printPath(head);
    }
    else if (strstr(arglist[1], "+") == arglist[1])
    {
        current = head;
        myNode *temp;
        temp = (myNode *)malloc(sizeof(myNode));
        temp->path = malloc(strlen(arglist[2]) + 1);
        strcpy(temp->path, arglist[2]);
        temp->next = NULL;

        if (head == NULL)
        {
            head = temp;
        }
        else
        {
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = temp;
        }
    }
    else if (strstr(arglist[1], "-") == arglist[1])
    {
        equals(arglist[2], head, &head);
    }
    return head;
}

int quitShell()
{
    int q;
    printf("Quitting...\n");
    exit(1);
    return q = 0;
}

void printPrompt()
{
    getcwd(cwd, sizeof(cwd));
    printf("%s $ ", cwd);
}

void linCommand(char *arglist[], myNode *head, int numOfArgs)
{
    pid = fork();
    if (pid == -1)
    {
        printf("error with fork\n");
    }
    else if (pid == 0)
    {
        if (head == NULL)
        {
            printf("Path variable is empty please add a path to continue.\n");
            exit(1);
        }
        //list only has one entry
        if (head->next == NULL)
        {
            int i;
            //list only has one entry
            char *command = malloc(sizeof(ARGLENGTH));
            if (head->path[strlen(head->path) - 1] == '\n')
            {
                head->path[strlen(head->path) - 1] = '\0';
            }
            strcat(command, head->path);
            strcat(command, "/");
            strcat(command, arglist[0]);
            char *list[MAXARGS + 1];
            list[0] = command;
            for (i = 1; i < numOfArgs; i++)
            {
                list[i] = arglist[i];
            }
            list[i] = NULL;
            execv(command, list);
        }
        //list has multiple entries
        getLength(head);
        int i;
        for(i = 0; i <= lengthList; i++)
        {
            int i;
            char *command = malloc(sizeof(ARGLENGTH));
            if (head->path[strlen(head->path) - 1] == '\n')
            {
                head->path[strlen(head->path) - 1] = '\0';
            }
            strcat(command, head->path);
            strcat(command, "/");
            strcat(command, arglist[0]);
            char *list[MAXARGS + 1];
            list[0] = command;
            for (i = 1; i < numOfArgs; i++)
            {
                list[i] = arglist[i];
            }
            list[i] = NULL;
            execv(command, list);
            head = head->next;
        }
        printf("%s: Command not found.\n", arglist[0]);
        exit(1);
    }
    else
    {
        wait(&status);
        //printf("command not found\n");
    }
}

void equals(char *arg, myNode *head, myNode **headRef)
{
    int i;
    int ret;
    myNode *count;
    myNode *temp;
    myNode *prev;
    count = head;
    temp = head;
    int n = 0;
    for(i = 0; i < MAXARGS; i++)
    {
        if(count == NULL)
        {
            return;
        }
        ret = strcmp(arg, count->path);
        if (ret == 0)
        {
            if(temp != NULL && strcmp(arg, temp->path) == 0)
            {
                *headRef = temp->next;
                free(temp);
                return;
            }

            while(temp->next != NULL && strcmp(arg, temp->path) != 0)
            {
                prev = temp;
                temp = temp->next;
            }

            if(temp == NULL)
            {
                return;
            }
            prev->next = temp->next;
            free(temp);
        }
        n++;
        count = count->next;
    }
    printf("This is not in the path variable\n");
}

void printPath(myNode *head)
{
    if (head == NULL)
    {
        printf("List is empty\n");
        return;
    }
    while (head->next != NULL)
    {
        if (head->path[strlen(head->path) - 1] == '\n')
        {
            head->path[strlen(head->path) - 1] = '\0';
        }

        printf("%s:", head->path);
        head = head->next;
    }
    printf("%s\n", head->path);
}

void getLength(myNode *head)
{
    lengthList = 0;
    myNode *count;
    count = head;
    while (count->next != NULL)
    {
        count = count->next;
        lengthList++;
    }
}