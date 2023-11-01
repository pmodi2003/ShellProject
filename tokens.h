#ifndef TOKENS_H
#define TOKENS_H

#define MAX_TOKEN_LENGTH 255

/**
 * Splits given input string into tokens
 *
 * @param input input string
 *
 * @return null-terminated heap-allocated array of token strings
 */
char **tokenize(char *input);
/**
 * Frees given array of token strings created using tokenize()
 *
 * Both array and non-null pointers need to be allocated 
 * using malloc
 * 
 * @param tokens null-terminated heap-allocated array of token strings
 */
void free_tokens(char **tokens);

#endif
