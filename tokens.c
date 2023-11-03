#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokens.h"
#include <stdio.h>
#include <string.h>

#define MAX_COMMANDS 100

int special_tokens(char c) {
    return (c == '(' || c == ')' || c == '<' || c == '>' || c == ';' || c == '|');
}

int read_tokens(char c) {
    return (c != ' ' && c != '\t' &&  c != '\n' && !special_tokens(c));
}

int in_quotes(const char *input, char *output) {
    int i = 0;
    while (input[i] != '\0' && input[i] != '"') {
        output[i] = input[i];
        ++i;
    }
    output[i] = '\0';
    ++i;
    return i;
}

int stream(const char *input, char *output) {
    int i = 0;
    while (input[i] != '\0' && read_tokens(input[i])) {
        output[i] = input[i];
        ++i;
    }
    output[i] = '\0';
    return i;
}


char **tokenize(char *tokens) {
    char buf[MAX_TOKEN_LENGTH];
    char **tokenArray = (char **)malloc(MAX_COMMANDS * sizeof(char *)); 
    int token_idx = 0;
    int i = 0;
    while (tokens[i] != '\0') {
        if (tokens[i] == '"') {
            i++;
            i += in_quotes(&tokens[i], buf);
            tokenArray[token_idx] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char)); 
            strcpy(tokenArray[token_idx], buf);
            token_idx++;
            continue;
        }

        if (read_tokens(tokens[i])) {
            i += stream(&tokens[i], buf);
            tokenArray[token_idx] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char)); 
            strcpy(tokenArray[token_idx], buf); 
            token_idx++;
            continue;
        }

        if (special_tokens(tokens[i])) {
            buf[0] = tokens[i];
            buf[1] = '\0';
            tokenArray[token_idx] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char)); 
            strcpy(tokenArray[token_idx], buf); 
            token_idx++;
        }

        i++;
    }
    tokenArray[token_idx] = NULL; 
    return tokenArray; 
}

void free_tokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}