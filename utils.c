/***************************************************************************/ /**
   @file         utils.c
   @author       Jonathan Kuug & Abdul Wahab Abass
   @date         April 2024
   @brief        DSH (Dash Shell - Minimal Shell)
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

// void tokenize(char *input, Token **args)
// {

//     if (input == NULL)
//     {
//         return;
//     }

//     const char *delimiters = " \t\r\n\a";
//     char *token_value = strtok(input, delimiters);

//     if (token_value == NULL)
//     {
//         return;
//     }

//     Token *head = new_token(token_value);
//     Token *current = head;

//     while ((token_value = strtok(NULL, delimiters)) != NULL)
//     {
//         current->next = new_token(token_value);
//         current = current->next;
//     }

//     *args = head;
// }

void tokenize(char *input, Token **args)
{
    if (input == NULL)
    {
        return;
    }

    char *token_value;
    Token *head = NULL;
    Token *current = NULL;

    char *start_ptr = input;
    char *end_ptr = input;

    while (*end_ptr != '\0')
    {
        if (*start_ptr == '"')
        {
            end_ptr = strchr(start_ptr + 1, '"');
            if (end_ptr != NULL)
            {
                *end_ptr = '\0';
                token_value = start_ptr + 1;
                end_ptr++;
            }
            else
            {
                // If there's no matching end quote, treat the rest of the string as a single token
                token_value = start_ptr + 1;
                end_ptr = start_ptr + strlen(start_ptr);
            }
        }
        else
        {
            end_ptr = strpbrk(start_ptr, " \t\r\n\a");
            if (end_ptr != NULL)
            {
                *end_ptr = '\0';
                token_value = start_ptr;
                end_ptr++;
            }
            else
            {
                // If there's no more delimiters, treat the rest of the string as a single token
                token_value = start_ptr;
                end_ptr = start_ptr + strlen(start_ptr);
            }
        }

        if (head == NULL)
        {
            head = new_token(token_value);
            current = head;
        }
        else
        {
            current->next = new_token(token_value);
            current = current->next;
        }

        start_ptr = end_ptr;
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
