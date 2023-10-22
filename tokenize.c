#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 255

char **tokenize(char *input) {
	char **tokens = (char **)malloc(MAX_TOKENS * sizeof(char *));
	char *token = strtok(input, " \t\n");
	int token_index = 0;

	while (token != NULL) {
		tokens[token_index] = (char *)malloc(MAX_TOKEN_LENGTH * sizeof(char));
		strcpy(tokens[token_index], token);
		token_index++;
		token = strtok(NULL, " \t\n");
	}

	tokens[token_index] = NULL;
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

