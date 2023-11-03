/**
 * A simple arithmetic expression tokenizer. Use this example to get you 
 * started on the first task.
 */
#include <stdio.h>
#include <string.h>

int isSpecialToken(char c) {
    // Check if a character is one of the special tokens: (, ), <, >, ;, |
    return (c == '(' || c == ')' || c == '<' || c == '>' || c == ';' || c == '|');
}

int isNonSpecialCharacter(char c) {
    // You can define your own logic to check if a character is non-special
    // For example, you can check if it's a digit or any other condition.
    // Replace this logic as needed.  
    return (c != ' ' && c != '\t' &&  c != '\n' && !isSpecialToken(c));
}

int read_quotes(const char *input, char *output) {
    int i = 0;
    while (input[i] != '\0' && input[i] != '"') {
        output[i] = input[i];
        ++i;
    }
    output[i] = '\0';
    ++i;
    return i;
}

int read_normal(const char *input, char *output) {
    int i = 0;
    while (input[i] != '\0' && isNonSpecialCharacter(input[i])) {
        output[i] = input[i];
        ++i;
    }
    output[i] = '\0';
    return i;
}



int main(int argc, char **argv) {
  char buf[256]; // temp buffer
        char expr[256]; // input buffer

        if (fgets(expr, sizeof(expr), stdin) == NULL) {
        }
  

  // Remove the trailing newline character, if present
        size_t len = strlen(expr);
        if (len > 0 && expr[len - 1] == '\n') {
            expr[len - 1] = '\0';
        }
  
  
  int i = 0;
    while (expr[i] != '\0') {

        if (expr[i] == '"') {
            i++;
            i += read_quotes(&expr[i], buf);
            printf("%s\n", buf);
	    continue;
        }

        if (isNonSpecialCharacter(expr[i])) {
            i += read_normal(&expr[i], buf);
            printf("%s\n", buf);

	    continue;
        }

        if (isSpecialToken(expr[i])) {
            buf[0] = expr[i];
            buf[1] = '\0';
	    printf("%s\n", buf);

        }

        i++;  // Move to the next character in the input.
	      //
     }
    return 0;
 }
