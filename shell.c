#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "tokens.h"

#define MAX_COMMANDS 100

static char *prev = NULL;

void execute_commands(char *command) {
    char **tokens = NULL;
    char *input_file = NULL;
    char *output_file = NULL;
    int input_fd, output_fd;

    if (strcmp(command, "exit") == 0) {
        printf("Bye bye.\n");
        exit(0);
    }

    if (strcmp(command, "prev") != 0) {
        free(prev);
        prev = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
        strcpy(prev, command);
        tokens = tokenize(command);
    } else {
        if (prev  == NULL) {
            printf("No previous commands!\n");
            return;
        } else {
    	    tokens = tokenize(prev);
            int token_idx = 0;
            while (tokens[token_idx] != NULL) {
                printf("%s ", tokens[token_idx]);
                token_idx++;
            }
            printf("\n");
        }
    }


    char *input_redirect = strchr(command, '<');
    if (input_redirect) {
        *input_redirect = '\0';
        input_redirect++;
        while (*input_redirect == ' ') {
            input_redirect++;
        }
        input_file = input_redirect;
    }

    char *output_redirect = strchr(command, '>');
    if (output_redirect) {
        *output_redirect = '\0';
        output_redirect++;
        while (*output_redirect == ' ') {
            output_redirect++;
        }
        output_file = output_redirect;
    }

    char *pipe_operator = strchr(command, '|');
    if (pipe_operator) {
        *pipe_operator = '\0';
        pipe_operator++;
        while (*pipe_operator == ' ') {
            pipe_operator++;
        }

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("Pipe error");
            exit(1);
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(pipefd[0]); 
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            execute_commands(command); 
            exit(0);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]);

            execute_commands(pipe_operator); 
            exit(0);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        waitpid(pid, NULL, 0);
        waitpid(pid2, NULL, 0);
        return;
    }

    if (strcmp(tokens[0], "<error>") == 0) {
        free_tokens(tokens);
        return;
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
        return;
    }


    if (strcmp(tokens[0], "source") == 0) {
        if (tokens[1] == NULL) {
            printf("source: filename argument required\n");
            return;
        }

        FILE *file = fopen(tokens[1], "r");
        if (file == NULL) {
            perror("source");
            return;
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
        return;
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
	    return;
	}


    pid_t pid = fork();
    if (pid == 0) {
         if (input_file) {
            input_fd = open(input_file, O_RDONLY);
            if (input_fd == -1) {
                perror("Input file error");
                exit(1);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (output_file) {
            output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd == -1) {
                perror("Output file error");
                exit(1);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        if (execvp(tokens[0], tokens) == -1) {
            fprintf(stderr, "%s: command not found\n", tokens[0]);
            exit(1);
        }
    } else {
        waitpid(pid, NULL, 0);
    }
    free_tokens(tokens);

    return;
}

int main(int argc, char **argv) {
    printf("Welcome to mini-shell.\n");

    char input[MAX_TOKEN_LENGTH];
    char *commands[MAX_COMMANDS];

    while (1) {
        printf("shell $ ");
        if (fgets(input, MAX_TOKEN_LENGTH, stdin) == NULL) {
            printf("\nBye bye.\n");
            break;
        }

        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        int num_commands = 0;
        char *command = strtok(input, ";");
        while (command != NULL) {
            commands[num_commands] = command;
            num_commands++;
            command = strtok(NULL, ";");
        }

        for (int i = 0; i < num_commands; i++) {
            execute_commands(commands[i]);
        }
    }

    return 0;
}
