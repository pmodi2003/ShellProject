#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include "tokens.h"

static char *prev = NULL; // holds on to previous command

/** Update previous command saved in memory **/
void update_prev(char **command) {
    free(prev);
    prev = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
    strcpy(prev, "");
    while(*command != NULL){
        strcat(prev, *command);
        strcat(prev, " ");
        command++;
    }
}

/** Goes through array and makes each character null **/
void null_commands(char **commands) {
    for (int i = 0; i < MAX_COMMANDS; i++) {
        commands[i] = NULL;
    }
}

/** Goes through input string and calls tokenizer on it
 *
 *  Checks for and handles sequencing (;) in input line
 *
 *
 */
void tokenize_input(char *input) {
    char **command_itr;
    char *commands[MAX_COMMANDS];
    null_commands(&commands);
    int command_idx = 0;

    char **tokens = tokenize(input);
    assert(tokens != NULL);

    command_itr = tokens;
    while (*command_itr != NULL) {
        if (strcmp(*command_itr,";") == 0) {
            command_idx++;
            commands[command_idx] = NULL;
            command_type(commands);
            null_commands(&commands);
            command_idx = 0;

            command_itr++;
            continue;
        }

        commands[command_idx] = *command_itr;
        command_itr++;
        command_idx++;
    }

    command_type(commands);
    free_tokens(tokens);
}

/** Checks for and handles pipes & redirects in command
 *
 *  Executes command directly if no special command calls exist
 */
void command_type(char **commands) {
    if (pipe_command(commands)) {
        run_pipe(commands);
    } else if (input_redirect_command(commands) == 1) {
        run_redirect(commands, 0);
    } else if (output_redirect_command(commands) == 1) {
        run_redirect(commands, 1);
}else {
        exec_commands(commands);
    }
}

/** Checks to see if command has a pipe operator **/
int pipe_command(char **commands) {
    for (int i = 0; commands[i] != NULL; i++) {
        if (strcmp(commands[i], "|") == 0) {
            return 1;
        }
    }
    return 0;
}

/** Handles pipe execution **/
void run_pipe(char **commands) {
    char *left[MAX_COMMANDS];
    char *right[MAX_COMMANDS];

    for (int i = 0; commands[i] != NULL; i++) {
        if (strcmp(commands[i], "|") == 0) {
            null_commands(&left);
            for (int j = 0; j < i; j++) {
                left[j] = commands[j];
            }

            null_commands(&right);
            for(int k = 0; commands[k+i+1] != NULL; k++) {
                right[k] = commands[k+i+1];
            }

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("Pipe error");
                exit(1);
            }

            int pid1 = fork();

            if(pid1 == 0) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                command_type(left);
                exit(1);
            }

            int pid2 = fork();

            if (pid2 == 0) {
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                command_type(right);
                exit(1);
            }

            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            return;
        }
    }
}

/** Checks to see if command has an input redirect operator **/
int input_redirect_command(char **commands) {
    for (int i = 0; commands[i] != NULL; i++) {
        if (strcmp(commands[i], "<") == 0) {
            return 1;
        }
    }
    return 0;
}

/** Checks to see if command has an output redirect operator **/
int output_redirect_command(char **commands) {
    for (int i = 0; commands[i] != NULL; i++) {
        if (strcmp(commands[i], ">") == 0) {
            return 1;
        }
    }
    return 0;
}

/** Handles redirect execution **/
void run_redirect(char **commands, int direction) {
    char *redirect_commands[MAX_COMMANDS];
    char *fileName = NULL;
    for (int i = 0; commands[i] != NULL; i++) {
        if (strcmp(commands[i], "<") == 0 || strcmp(commands[i], ">") == 0) {
            if(commands[i+1] != NULL) {
                fileName = commands[i + 1];
                redirect_commands[i] = NULL;
            } else {
                printf("Redirect needs a file!\n");
                return;
            }
            break;
        }
        redirect_commands[i] = commands[i];
    }

    int pid1 = fork();

    if (pid1 == 0) {
        if (direction == 0) { // INPUT REDIRECT
            if (close(0) == -1) {
                perror("Error closing stdin");
                exit(1);
            }
            int fd = open(fileName, O_RDONLY);
            assert(fd == 0);
        } else { // OUTPUT REDIRECT
            if (close(1) == -1) {
                perror("Error closing stdout");
                exit(1);
            }

            int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("Unable to open file!");
                exit(1);
            }
            assert(fd == 1);
        }
        execvp(redirect_commands[0], redirect_commands);
        exit(1);
    } else {
        waitpid(pid1, NULL, 0);
    }

}

/** Handles command execution
 *
 *  Handles built-in commands as well executable commands
 */
void exec_commands(char **commands){

    if (commands[0] == NULL) {
        return;
    }

    if (strcmp(commands[0], "exit") == 0) {
        printf("Bye bye.\n");
        free(prev);
        exit(0);
    } else if (strcmp(commands[0], "prev") == 0) {
        if (prev != NULL) {
            printf("%s\n", prev);
            tokenize_input(prev);
        } else {
            printf("No previous commands!\n");
        }
        return;
    } else if (strcmp(commands[0], "cd") == 0) {
        if(chdir(commands[1]) == -1){
            if(commands[1] == NULL || strcmp(commands[1], "~") == 0){
                chdir("/home/user");
            } else {
                char *directory = (char *)malloc(sizeof(char) * (strlen(commands[1])+1));
                strcpy(directory, "/");
                strcat(directory, commands[1]);
                if (chdir(directory) == -1) {
                    printf("Error changing directory\n");
                }
            }
        }
    } else if (strcmp(commands[0], "source") == 0) {
        FILE *source = fopen(commands[1], "r");
        if (source == NULL) {
            printf("Error trying to open %s\n", commands[1]);
        }
        char line[MAX_TOKEN_LENGTH];

        while (fgets(line, MAX_TOKEN_LENGTH, source) != NULL) {
            tokenize_input(line);
        }
    } else if (strcmp(commands[0], "help") == 0) {
        if (commands[1] == NULL) {
            printf("Help command requires a built-in command argument\n"
                   "Built-in Arguments:\n       exit\n  prev\n  cd\n    source\n        help\n");
        } else if (strcmp(commands[1], "exit") == 0){
            printf("exit: exit\n"
                   "    Exit the shell.\n");
        } else if (strcmp(commands[1], "prev") == 0) {
            printf("prev: prev\n"
                   "    Change the shell working directory.\n\n"
                   "    Change the current directory to DIR.  The default DIR is the value of the HOME shell variable.\n");
        } else if (strcmp(commands[1], "cd") == 0) {
            printf("cd: cd [dir]\n"
                   "    Change the shell working directory.\n\n"
                   "    Change the current directory to DIR. The default DIR is the value of the HOME shell variable.\n");
        } else if (strcmp(commands[1], "source") == 0) {
            printf("source: source filename\n"
                   "    Execute commands from a file in the current shell.\n\n"
                   "    Read and execute commands from FILENAME in the current shell.\n");
        } else if (strcmp(commands[1], "help") == 0) {
            printf("help: help [command]\n"
                   "    Display information about builtin commands.\n\n"
                   "    Displays brief summaries of builtin commands.\n");
        } else {
            printf("%s is not a built-in command!\n", commands[1]);
        }
    } else {
        int child_pid = fork();
        if (child_pid == 0) {
            execvp(commands[0], commands);
            fprintf(stderr, "%s: command not found\n", commands[0]);
            exit(1);
        } else {
            waitpid(child_pid, NULL, 0);
        }
    }
    update_prev(commands);
}

/** Obtains input and executes input from shell **/
int main(int argc, char **argv) {
    printf("Welcome to mini-shell.\n");

    char input[MAX_TOKEN_LENGTH];
    char *commands[MAX_COMMANDS];

    while (1) {
        printf("shell $ ");
        if (fgets(input, MAX_TOKEN_LENGTH, stdin) == NULL) {
            printf("\nBye bye.\n");
            free(prev);
            return 0;
        }

        tokenize_input(input);
    }
    free(prev);
    return 0;
}
