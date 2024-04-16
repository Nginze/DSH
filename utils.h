#pragma once

/***************************************************************************/ /**
   @file         utils.h
   @author       Jonathan Kuug & Abdul Wahab Abass
   @date         April 2024
   @brief        DSH (Dash Shell - Minimal Shell)
 *******************************************************************************/

/**
 * @struct Token
 * @brief Represents a token in a shell command.
 *
 * The Token struct contains a string value and a pointer to the next token in the command.
 */
typedef struct Token
{
  char *value;        /**< The string value of the token. */
  struct Token *next; /**< Pointer to the next token in the command. */
} Token;

// Function Prototypes
Token *createToken(char *value);
void tokenize(char *input, Token **args);
void printTokens(Token *head);