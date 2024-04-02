#pragma once 

/***************************************************************************/ /**
   @file         utils.h
   @author       Jonathan Kuug & Abdul Wahab Abass
   @date         April 2024
   @brief        DSH (Dash Shell - Minimal Shell)
 *******************************************************************************/


typedef struct Token{
    char* value;
    struct Token* next;
} Token;

Token* createToken(char* value);
void tokenize(char* input, Token** args);
void printTokens(Token* head);