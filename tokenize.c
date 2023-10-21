
char **tokenize(const char *input){
  char **tokens = NULL;
  int token_count = 0;

  char *token = strtok(input, " \t\n");

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
  char input[255];
  printf("Enter a string: ");

}
