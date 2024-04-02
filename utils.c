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

Token *new_token(char *value)
{
    Token *token = malloc(sizeof(Token));
    token->value = strdup(value);
    token->next = NULL;
    return token;
}

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

void printTokens(Token *head)
{
    for (Token *current = head; current != NULL; current = current->next)
    {
        printf("%s\n", current->value);
    }
}
