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


        if (strcmp(tokens[0], "source") == 0) {
            if (tokens[1] == NULL) {
                printf("source: filename argument required\n");
                continue;
            }

            FILE *file = fopen(tokens[1], "r");
            if (file == NULL) {
                perror("source");
                continue;
            }

            char script_line[MAX_TOKEN_LENGTH];
            while (fgets(script_line, MAX_TOKEN_LENGTH, file) != NULL) {
                if (script_line[strlen(script_line) - 1] == '\n') {
                    script_line[strlen(script_line) - 1] = '\0';
                }

                char **script_tokens = tokenize(script_line);
                pid_t pid = fork();
                if (pid == 0) {
                    if (execvp(script_tokens[0], script_tokens) == -1) {
                        fprintf(stderr, "%s: command not found\n", script_tokens[0]);
                        exit(1);
                    }
                } else {
                    waitpid(pid, NULL, 0);
                }

                free_tokens(script_tokens);
            }

            fclose(file);
            continue;
        }


	if (strcmp(tokens[0], "help") == 0) {
	    if (tokens[1] == NULL) {
                printf("Help command requires a built-in command argument\n"
                       "Built-in Arguments:\n	exit\n	prev\n	cd\n	source\n	help\n");
	    } else if (strcmp(tokens[1], "exit") == 0){
		printf("exit: exit\n"
			"	Exit the shell.\n");
	    } else if (strcmp(tokens[1], "prev") == 0) {
		printf("prev: prev\n"
			"	Change the shell working directory.\n\n"
			"	Change the current directory to DIR.  The default DIR is the value of the HOME shell variable.\n");
	    } else if (strcmp(tokens[1], "cd") == 0) {
                printf("cd: cd [dir]\n"
			"	Change the shell working directory.\n\n"
			"	Change the current directory to DIR. The default DIR is the value of the HOME shell variable.\n");
	    } else if (strcmp(tokens[1], "source") == 0) {
                printf("source: source filename\n"
			"	Execute commands from a file in the current shell.\n\n"
			"	Read and execute commands from FILENAME in the current shell.\n");
	    } else if (strcmp(tokens[1], "help") == 0) {
                printf("help: help [command]\n"
			"	Display information about builtin commands.\n\n"
			"	Displays brief summaries of builtin commands.\n");
	    } else {
		printf("%s is not a built-in command!\n", tokens[1]);
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
