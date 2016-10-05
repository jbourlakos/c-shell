#ifndef _SHELL_H
#define	_SHELL_H

#include <stdlib.h>
#include <stdio.h>

#define SHELL_NO_ERROR 0
#define SHELL_ERROR_UNKNOWN_COMMAND (-1)
#define SHELL_ERROR_WRONG_PARAMETERS_NUMBER (-2)
#define SHELL_ERROR_WRONG_PARAMETER (-3)
#define SHELL_ERROR_UNDEFINED (-128)

#define SHELL_EXEC_OK 0
#define SHELL_EXEC_OUT_OF_MEM 1
#define SHELL_EXEC_INPUT_EOF 2
#define SHELL_EXEC_INPUT_ERROR 3
#define SHELL_EXEC_OUTPUT_ERROR 4

#define SHELL_DELIMITER ' '
#define SHELL_END_OF_LINE '\n'

typedef struct {
    char* name;
    int (*function)(int,char**);
    char* helpMessage;
} command_t;

typedef struct {
    char* name;
    command_t* commands;
    int commandsLength;
    char* exitCommandName;
    char* helpCommandName;
    void (*preShellFunction)();
    void (*postShellFunction)();
    int (*preExitFunction)();
    FILE* in;
    FILE* out;
    FILE* err;
    int bufsiz;
} shell_t;


int shellMain(shell_t* sh);
int shellEcho(int n, char** args);
int shellExec(int n, char** args);
int shellPray(int n, char** args);
int shellHelp(int n, char** args);

#endif	/* _SHELL_H */

