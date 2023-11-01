#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokens.h"

char **tokenize(char *input) {
    char **tokens = (char **)malloc(MAX_TOKEN_LENGTH * sizeof(char *));
    char *token = strtok(input, " \t\n");
    int token_idx = 0;

    while (token != NULL) {
        tokens[token_idx] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
        if (token[0] == '"') {
            char *result = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
            if (result == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(1);
            }
            strcpy(result, token + 1);
            while (token[strlen(token) - 1] != '"') {
                token = strtok(NULL, " \t\n");
                if (token == NULL) {
                    fprintf(stderr, "No closing quote found\n");
                    char **error_token = (char **)malloc(2 * sizeof(char *));
                    error_token[0] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
                    strcpy(error_token[0], "<error>");
                    error_token[1] = NULL;
                    return error_token;
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
