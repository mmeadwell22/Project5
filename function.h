#ifndef FUNCTION_H
#define FUNCTION_H
#define MAXARGS 20
#define ARGLENGTH 100

    typedef struct myNode
    {
        char *path;
        struct myNode *next;
    }myNode;

    void cd(char *[]);
    myNode *path(char *[], myNode *);
    int quitShell();
    void linCommand(char *[], myNode *, int);
    void printPrompt();
    void equals(char *, myNode *, myNode **);
    void printPath(myNode *);
    void getLength(myNode *);

#endif