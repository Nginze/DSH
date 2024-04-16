/***************************************************************************/ /**
   @file         utils.c
   @author       Jonathan Kuug & Abdul Wahab Abass
   @date         April 2024
   @brief        DSH (Dash Shell - Minimal Shell)
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/**
 * Creates a new token with the given value.
 *
 * @param value The value of the token.
 * @return A pointer to the newly created token.
 */
Token *new_token(char *value)
{
    Token *token = malloc(sizeof(Token));
    token->value = strdup(value);
    token->next = NULL;
    return token;
}

/**
 * Tokenizes the input string based on the given delimiters and stores the tokens in a linked list.
 *
 * @param input The input string to be tokenized.
 * @param args  A pointer to a Token pointer, which will be updated to point to the head of the linked list of tokens.
 */
void tokenize(char *input, Token **args)
{
    const char *delimiters = " \t\r\n\a";
    char *token_value = strtok(input, delimiters);
    if (token_value == NULL)
    {
        return;
    }

    Token *head = new_token(token_value);
    Token *current = head;

    while ((token_value = strtok(NULL, delimiters)) != NULL)
    {
        current->next = new_token(token_value);
        current = current->next;
    }

    *args = head;
}

/**
 * Prints the values of each token in a linked list.
 *
 * @param head The head of the linked list of tokens.
 */
void printTokens(Token *head)
{
    for (Token *current = head; current != NULL; current = current->next)
    {
        printf("%s\n", current->value);
    }
}
