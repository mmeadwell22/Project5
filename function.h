#ifndef FUNCTION_H
#define FUNCTION_H
#define MAXARGS 20
#define ARGLENGTH 100

    typedef struct myNode
    {
        char *path;
        struct myNode *next;
    }myNode;

    /// Each entry in char ***command will be equivalent to your argbuf[].
    /// numOfArgs is equivalent to the total argument you pass to linCommand.
    /// count keeps track of the total number of commands supplied by the user.
    typedef struct commands_t{
        char ***command;
        int *numOfArgs;
        int count;
    } commands_t;

    void cd(char *[]);
    myNode *path(char *[], myNode *);
    int quitShell();
    /// thePipe is a pair of fd's created by pipe system call.
    /// pipeIn flag tells linCommand whether or not to redirect stdin.
    /// pipeOut flag tells linCommand whether or not to redirect stdout.
    void linCommand(char *[], myNode *, int, int* thePipe, int pipeIn, int pipeOut);
    void printPrompt();
    void equals(char *, myNode *, myNode **);
    void printPath(myNode *);
    void getLength(myNode *);
    int scanForPipes(const char *);
    int buildCommandList(commands_t *, char *);
    void createPipe(int*);

#endif