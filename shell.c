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
    char **tokens = NULL;

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
	
	if (strcmp(input, "prev") != 0) {
	    tokens = tokenize(input);
	} else {
	    if (tokens == NULL){
  	        printf("No previous commands!\n");
	        continue;
	    } else {
		int token_idx = 0;
		while(tokens[token_idx] != NULL) {
		    printf("%s ", tokens[token_idx]);
		    token_idx++;
	    	}
		printf("\n");
	    }
	}

        if (strcmp(tokens[0], "<error>") == 0) {
            free_tokens(tokens);
            continue; 
        }
	
	if (strcmp(tokens[0], "cd") == 0) {
	    if(chdir(tokens[1]) == -1){
		if(tokens[1] == NULL || strcmp(tokens[1], "~") == 0){
		    chdir("/home/user");
		} else {
		    char *directory = (char *)malloc(sizeof(char) * (strlen(tokens[1])+1));
		    strcpy(directory, "/");
		    strcat(directory, tokens[1]);
		    if (chdir(directory) == -1) { 
		        printf("Error changing directory\n");
		    }
		}
	    }
	    continue;
	}
        
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
