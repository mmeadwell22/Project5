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
    else if (strstr(arglist[1], "+") == arglist[1] && arglist[2] != NULL)
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
    else if (strstr(arglist[1], "-") == arglist[1] && arglist[2] != NULL)
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

int linCommand(char *arglist[], myNode *head, int numOfArgs, int pipeCount, int* thepipe, int pipeIn, int pipeOut)
{
    /// TODO: According to the values of the pipeIn / pipeOut flags
    /// close the appropriate end of the pipe. Use dup to redirect stdin.
    pid = fork();
    if (pid == -1)
    {
        printf("error with fork\n");
    }
    else if (pid == 0)
    {
        if( pipeCount == 1){
            // read end of pipe is thepipe[0]
            // write end of pipe is thepipe[1]

            if (pipeIn == 0 && pipeOut == 1){ //sender
                close(thepipe[0]);
                dup2(thepipe[1], 1);
                close(thepipe[1]);
            }
            else if(pipeIn == 1 && pipeOut == 0){ //receiver
                dup2(thepipe[0], 0);
                close(thepipe[0]);
            }
        }
        if( pipeCount == 2){
            // read end of pipe 1 is thepipe[0]
            // write end of pipe 1 is thepipe[1]

            // read end of pipe 2 is thepipe[2]
            // write end of pipe 2 is the pipe[3]

            if (pipeIn == 0 && pipeOut == 1){ //sender
                close(thepipe[0]);
                close(thepipe[2]);
                close(thepipe[3]);
                dup2(thepipe[1], 1);
                close(thepipe[1]);
            }
            else if(pipeIn == 1 && pipeOut == 1){ //intermediate
                close(thepipe[2]);
                dup2(thepipe[0], 0);
                dup2(thepipe[3], 1);
                close(thepipe[0]);
                close(thepipe[3]);
            }
            else if(pipeIn == 1 && pipeOut == 0){ //receiver
                dup2(thepipe[2], 0);
                close(thepipe[2]);
            }
        }


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
        while(wait(&status) != pid){
            return 0;
        }

        //printf("command not found\n");
    }
    return 0;
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

int scanForPipes(const char *input)
{
    /// loop through input and count the number of pipes
    int pipeCount = 0;
    int i = 0;
    for(; input[i] != 0; i++ ){
        if( input[i] == '|'){
            pipeCount ++;
        }
    }
    if(input[i-1] == '|'){
        return -1;
    }
    return pipeCount;
}

int buildCommandList(commands_t * command_list, char * input)
{
    char input_cp[strlen(input)];
    strcpy(input_cp, input);
    char *token;
    int command_count = 0;
    size_t char_count = 0;
    token = strtok(input_cp, "|");
    char_count += strlen(token) +1;
    while(token != NULL){
        int argc = 0;

        /// Count number of command arguments
        for(int i = 1; i < strlen(token) - 1; i++) {
            if( token[i] == ' '){
                argc++;
            }
        }

        /// Build list of arguments for each of the commands
        command_list->numOfArgs[command_count] = argc + 1;
        command_list->command[command_count] = (char **) calloc((size_t) argc+1, sizeof(char *));
        token = strtok(token, " ");
        for(int i = 0; i <= argc ; ++i) {
            if( token == NULL){break;}
            if( strlen(token)) {
                char *temp = calloc(strlen(token) + 1, sizeof(char));
                if( temp != NULL){
                    command_list->command[command_count][i] = temp;
                }
                strncpy(command_list->command[command_count][i], token, strlen(token));
                token = strtok(NULL, " ");
                }
            }
        command_count++;
        /// Parse input for next command
        if(command_count == command_list->count - 1){
            token = strtok(&input_cp[char_count], "\0");
        }
        else if( command_count < command_list->count - 1){
            token = strtok(&input_cp[char_count], "|");
            char_count += strlen(token) + 1;
        } else{
            break;
        }
    }
    return command_count;
}

void createPipe(int thepipe[2]){
    if( pipe(thepipe) == -1){
        perror("Pipe Failed");
    }
}

int garbageCollectCommandList(commands_t * commands){
    for(size_t i = 0; i < commands->count; i++){
        for(int j = 0; j < commands->numOfArgs[i]; j++){
            free(commands->command[i][j]);
        }
        free(commands->command[i]);
    }
    free(commands->command);
    free(commands->numOfArgs);
    return 0;
}
