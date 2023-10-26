#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokens.h"

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
