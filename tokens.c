#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokens.h"
#include <stdio.h>
#include <string.h>

/**
 * Check to see if char is a special character
 * @param c - char
 * @return integer: 1 if special charactor, 0 if not a special character
 */
static int special_token(char c) {
    return (c == '(' || c == ')' || c == '<' || c == '>' || c == ';' || c == '|');
}

/**
 * Check to see if char is a white space character
 * @param c - char
 * @return integer: 1 if white space charactor, 0 if not a white space character
 */
static int white_character(char c) {
    return (c == ' ' || c == '\t' || c == '\n');
}

/**
 * Check to see if char is a normal character
 * @param c - char
 * @return integer: 1 if normal charactor, 0 if not a normal character
 */
static int normal_char(char c) {
    return (!white_character(c) && c != '"' && !special_token(c));
}

/**
 * Find token in quotes
 * @param readTokenPtr - char *
 *        writeTokenPtr - char *
 * @return itr: integer length of the token in qoutes
 */
static int quoted_token(const char *readTokenPtr, char *writeTokenPtr) {
    int itr = 0;
    while (readTokenPtr[itr] != '\0' && readTokenPtr[itr] != '"') {
        writeTokenPtr[itr] = readTokenPtr[itr];
        ++itr;
    }
    writeTokenPtr[itr] = '\0';
    ++itr;
    return itr;
}

/**
 * Find normal token
 * @param readTokenPtr - char *
 *        writeTokenPtr - char *
 * @return itr: integer length of the normal token
 */
static int normal_token(const char *readTokenPtr, char *writeTokenPtr) {
    int itr = 0;
    while (readTokenPtr[itr] != '\0' && normal_char(readTokenPtr[itr])) {
        writeTokenPtr[itr] = readTokenPtr[itr];
        ++itr;
    }
    writeTokenPtr[itr] = '\0';
    return itr;
}

/**
 * Split up input command into individual tokens
 * @param tokens - char*
 * @return tokenArray: char ** list of all tokens from input
 */
char **tokenize(char *tokens) {
    char command[MAX_TOKEN_LENGTH];
    char **tokenArray = (char **)malloc(MAX_COMMANDS * sizeof(char *));
    int token_idx = 0;
    int char_itr = 0;
    // Iterate through each character of input until null (last pointer) is reached
    while (tokens[char_itr] != '\0') {
        if (tokens[char_itr] == '"') { //Find Quoted tokens and add to list of tokens
            char_itr++;
            char_itr += quoted_token(&tokens[char_itr], command);
            tokenArray[token_idx] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
            strcpy(tokenArray[token_idx], command);
            token_idx++;
            continue;
        }

        if (normal_char(tokens[char_itr])) { // Find Normal Tokens and add to list of tokens
            char_itr += normal_token(&tokens[char_itr], command);
            tokenArray[token_idx] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
            strcpy(tokenArray[token_idx], command);
            token_idx++;
            continue;
        }

        if (special_token(tokens[char_itr])) {// Find Special Tokens and add to list of tokens
            command[0] = tokens[char_itr];
            command[1] = '\0';
            tokenArray[token_idx] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
            strcpy(tokenArray[token_idx], command);
            token_idx++;
        }

        char_itr++;
    }
    tokenArray[token_idx] = NULL;
    return tokenArray;
}

/**
 * Free memory allocated for tokens using malloc
 * @param tokens - char **
 */
void free_tokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
