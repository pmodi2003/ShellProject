#ifndef TOKENS_H
#define TOKENS_H

#define MAX_TOKEN_LENGTH 255

char **tokenize(char *input);

void free_tokens(char **tokens);

#endif
