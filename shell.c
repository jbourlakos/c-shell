#include "shell.h"
#include <string.h>

shell_t* shell;

int shellFetchInput(shell_t* sh, char* buf);
int shellTokenizeBuffer(char* buffer, char*** argvPtr);

int shellMain(shell_t* sh) {
    char* buffer;
    int result = 0, i = 0;
    int argc;
    char** argv = (char**)malloc((sh->bufsiz/2)*sizeof(char*));
    if (sh == NULL) return SHELL_EXEC_OK;
    buffer = (char*)malloc(sh->bufsiz*sizeof(char));
    if (!buffer) return SHELL_EXEC_OUT_OF_MEM;
    shell = sh;
    if (sh->preShellFunction != NULL) sh->preShellFunction();
    while (1) {
        //fprintf(sh->out,"\n");
        fprintf(sh->out,"%s> ",sh->name);
        // read input line
        result = shellFetchInput(sh,buffer);
        if (result != SHELL_EXEC_OK) break;
        // tokenize it on spaces
        argc = shellTokenizeBuffer(buffer,&argv);
        if (argc == 0) continue; // empty line
        // find the command (argv[0])
        // EXIT: if it is "exit" command, quit the shell
        if (strcmp(argv[0],sh->exitCommandName) == 0) { // if "exit"
            if (sh->preExitFunction != NULL) sh->preExitFunction();
            result = SHELL_EXEC_OK;
            break;
        }
        // HELP: if it is "help" command, search and print help message
        if (strcmp(argv[0],sh->helpCommandName) == 0) { // if "help"
            result = shellHelp(argc-1,argv+1);
            continue;
        }
        // OTHERWISE: search for the command
        for (i = 0; i < sh->commandsLength; i++) {
            if (strcmp(argv[0],sh->commands[i].name)==0) {
                result = sh->commands[i].function(argc-1,argv+1);
                break;
            }
        }
        // command not found
        if (i == sh->commandsLength) {
            fprintf(sh->err,"%s: %s: command not found.\n", sh->name,argv[0]);
        }
    }
    if (sh->postShellFunction != NULL) sh->postShellFunction();

    free(argv);
    free(buffer);
    return result;
}

int shellFetchInput(shell_t* sh, char* buf) {
    if(feof(sh->in)) return SHELL_EXEC_INPUT_EOF;
    fgets(buf,sh->bufsiz,sh->in);
    return SHELL_EXEC_OK;
}

int shellTokenizeBuffer(char* buffer, char*** argvPtr) {
    int tokens = 0, inToken = 0;
    for (;;buffer++) { // night's achievements, the day sees and laughs
        if (*buffer == SHELL_END_OF_LINE) {  // if you meet new-line, stop
            *buffer = '\0';
            break;
        }
        if ((*buffer == SHELL_DELIMITER)&&(!inToken)) continue; // keep consuming spaces
        if (*buffer == SHELL_DELIMITER) { // in token met space, token ended
            *buffer = '\0';
            inToken = 0;
            continue;
        }
        if (inToken) continue; // not a space and in token => keep parsing the token
        // ELSE
        // not a space, neither in token
        inToken = 1; // changing status
        (*argvPtr)[tokens] = buffer;
        tokens++; // tokens plus plus, what's not to understand?
    }
    return tokens;
}

int shellEcho(int n, char** args) {
    int i = 0;
    for (i = 0; i < n; i++) {
        fprintf(shell->out,"%s", args[i]);
        fprintf(shell->out," ");
    }
    fprintf(shell->out,"\n");
    return 0;
}

int shellExec(int n, char** args) {
    char buffer[BUFSIZ];
    int i = 0, l = 0;
    char* cp = buffer;
    for (i = 0; i < n; i++) {
        l = strlen(args[i]);
        strcpy(cp, args[i]);
        *(cp + l) = ' ';
        *(cp + l + 1) = '\0';
        cp = cp + l + 1;
    }
    //    fprintf(shell->out,buffer);
    //    fprintf(shell->out,"\n");
    return system(buffer);
}

int shellPray(int n, char** args) {
    fprintf(shell->out,"\"God grant me the serenity to accept the things I cannot change;\n");
    fprintf(shell->out,"the courage to change the things I can;\n");
    fprintf(shell->out,"and the wisdom to know the difference.\"\n");
    fprintf(shell->out,"                        --Reinhold Niebuhr, The Serenity Prayer");
    fprintf(shell->out,"\n");
    return SHELL_NO_ERROR;
}

int shellHelp(int n, char** args) {
    int i = 0;
    if (n == 0) {
        fprintf(shell->out,"Supported commands are: ");
        for (i = 0; i < shell->commandsLength; i++) {
            fprintf(shell->out,"%s, ",shell->commands[i].name);
        }
        fprintf(shell->out, "%s ", shell->helpCommandName);
        fprintf(shell->out, "and %s.\n", shell->exitCommandName);
        fprintf(shell->out,"Type %s and a command's name for assistance on a specific command.\n");
        return SHELL_NO_ERROR;
    }
    for (i = 0; i < shell->commandsLength; i++) {
        if (strcmp(args[0],shell->commands[i].name) == 0) {
            if (shell->commands[i].helpMessage != NULL) {
                fprintf(shell->out,"%s\n",shell->commands[i].helpMessage);
            } else {
                fprintf(shell->out,"%s\n","No help message for this command.");
            }
            return SHELL_NO_ERROR;
        }
    }
    fprintf(shell->out,"\"%s\" command is unknown.\n",args[0]);
    return SHELL_ERROR_UNKNOWN_COMMAND;
}
