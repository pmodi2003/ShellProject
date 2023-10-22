#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 255

char **tokenize(char *input) {
    char **tokens = (char **)malloc(MAX_TOKENS * sizeof(char *));
    char *token = strtok(input, " \t\n");
    int token_idx = 0;

    while (token != NULL) {
        tokens[token_idx] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
        if (token[0] == '"') {
            char *result = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
            if (result == NULL) {
                fprintf(stderr, "Memory allocation failed");
                exit(1);
            }
            strcpy(result, token + 1);
            while (token[strlen(token) - 1] != '"') {
                token = strtok(NULL, " \t\n");
                if (token == NULL) {
                    fprintf(stderr, "No closing quote found");
                    exit(1);
                }
                strcat(result, " ");  
                strcat(result, token);
            }
            // Remove the closing quote
            result[strlen(result) - 1] = '\0';
            strcpy(tokens[token_idx], result);
            free(result);  
        } else {
            strcpy(tokens[token_idx], token);
        }
        token_idx++;
        token = strtok(NULL, " \t\n");
    }
    tokens[token_idx] = NULL;
    return tokens;
}

void free_tokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

int main() {
    char input[MAX_TOKEN_LENGTH];
    fgets(input, MAX_TOKEN_LENGTH, stdin);

    char **tokens = tokenize(input);

    for (int i = 0; tokens[i] != NULL; i++) {
        printf("%s\n", tokens[i]);
    }

    free_tokens(tokens);
    return 0;
}
