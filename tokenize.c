#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char **tokenize(const char *input){
  char **tokens = NULL;
  int token_count = 0;

  char *token = strtok(strdup(input), " \t\n");

  while(token != NULL){
    token_count++;
    tokens = realloc(tokens, token_count * sizeof(char*));
    tokens[token_count-1] = strdup(token);
    token = strtok(NULL, " \t\n");
  }

  tokens = realloc(tokens, (token_count+1) * sizeof(char*));
  tokens[token_count] = NULL;

  return tokens;

}

int main(int argc, char **argv) {
  char *input = NULL;
  size_t input_size = 256;

  if(getline(&input, &input_size, stdin) != -1){
    char **tokens = tokenize(input);

    for (int i = 0; tokens[i] != NULL; i++) {
      printf("%s\n", tokens[i]);
    }

  } else {
    printf("Error Reading Input.\n");
  }

  return 0;

}
