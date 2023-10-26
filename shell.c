#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "tokens.h"

int main(int argc, char **argv) {
    printf("Welcome to mini-shell.\n");

    char input[MAX_TOKEN_LENGTH];

    while (1) {
        printf("shell $ ");
        if (fgets(input, MAX_TOKEN_LENGTH, stdin) == NULL) {
            printf("\nBye bye.\n");
            break;  
        }

        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        if (strcmp(input, "exit") == 0) {
            printf("Bye bye.\n");
            break; 
        }

        char **tokens = tokenize(input);
        
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(tokens[0], tokens) == -1) {
                fprintf(stderr, "%s: command not found\n", tokens[0]);
                exit(1);
            }
        } else {
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}
