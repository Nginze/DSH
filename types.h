#pragma once

/***************************************************************************/ /**
   @file         utils.h
   @author       Jonathan Kuug & Abdul Wahab Abass
   @date         April 2024
   @brief        DSH (Dash Shell - Minimal Shell)
 *******************************************************************************/

// Library Imports
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"

// Macros
#define MAX_COMMANDS_SIZE 4096

/**
 * @brief Structure representing the configuration settings for the shell.
 */
typedef struct Config
{
    bool promptTheme; /**< The theme for the shell prompt. */
    bool tabCompletion; /**< The theme for the shell prompt. */ 
    char *promptSym;    /**< The theme for the shell prompt. */
    char *historyFile; /**< The file to store command history. */
    int historySize;   /**< The maximum number of commands to store in history. */
    char *editor;      /**< The default text editor for the shell. */
} config_t;

/**
 * @brief Structure representing a command buffer.
 *
 * This structure holds an array of commands, along with the current index and size of the buffer.
 */
typedef struct CommandBuffer
{
    char *commands[MAX_COMMANDS_SIZE]; /**< Array of commands */
    int current;                       /**< Current index in the buffer */
    int size;                          /**< Size of the buffer */
} cmdBuffer_t;

/**
 * @brief Enumeration representing different types of commands.
 */
typedef enum CommandType
{
    SIMPLE,       // Simple command, e.g., "ls -l"
    PIPE,         // Pipe, e.g., "ls -l | grep .txt"
    REDIRECT_IN,  // Input redirection, e.g., "sort < file.txt"
    REDIRECT_OUT, // Output redirection, e.g., "ls -l > file.txt"
    REDIRECT_ERR, // Error redirection, e.g., "ls -l 2> file.txt"
    REDIRECT_APP, // Output append redirection, e.g., "ls -l >> file.txt"
    BACKGROUND,   // Background command, e.g., "sleep 10 &"
    SEQUENCE,     // Sequence of commands, e.g., "cd dir; ls -l"
    CONDITIONAL   // Conditional execution, e.g., "make && ./program"
} command_t;

/**
 * @brief Represents a command in the shell.
 *
 * This struct stores information about a command, including its type, the length of its arguments,
 * the arguments themselves, and a pointer to the next command in a linked list.
 */
typedef struct Command
{
    command_t type;
    int args_length;
    char **args;
    struct Command *next;
} Command;

/**
 * @brief Structure representing an input buffer.
 *
 * This structure holds information about an input buffer, including the buffer itself,
 * the parsed arguments, the token list, the command list, and the lengths of the buffer
 * and input.
 */
typedef struct InputBuffer
{
    char *buffer;
    char *args[64];
    Token *token_list;
    Command **command_list;
    size_t buffer_length;
    size_t input_length;
} buff_t;

/**
 * @brief Represents the state of the application.
 *
 * The `app_t` struct holds information about the current state of the application.
 * It includes a buffer for storing application data, a command buffer for storing
 * command-related data, the type of the current command, the length of the current
 * directory path, the current directory path itself, and a flag indicating whether
 * the application has been initialized.
 */
typedef struct AppState
{
    buff_t *app_buffer;
    cmdBuffer_t *cmd_buffer;
    command_t current_command_type;
    size_t current_directory_length;
    config_t *config;
    char *current_directory;
    bool has_init;

} app_t;

// App utils
buff_t *init_buffer();
app_t *init_app();
config_t *init_config();
cmdBuffer_t *init_cmd_buffer();
Command *new_command(command_t type, char **args, int args_length);
void load_config(const char *filename, config_t *config);