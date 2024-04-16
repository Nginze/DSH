/***************************************************************************/ /**
   @file         utils.h
   @author       Jonathan Kuug & Abdul Wahab Abass
   @date         April 2024
   @brief        DSH (Dash Shell - Minimal Shell)
 *******************************************************************************/

// Library Imports
#include "types.h"
#include <stdio.h>

// Macros
#define MAX_BUFFER_SIZE 4096
#define MAX_ARGS 64

/**
 * Initializes a buffer by allocating memory for the buffer and command list.
 *
 * @return A pointer to the initialized buffer.
 */
buff_t *init_buffer()
{
    buff_t *buffer = (buff_t *)malloc(sizeof(buff_t));
    buffer->buffer = malloc(MAX_BUFFER_SIZE);
    buffer->command_list = (Command **)malloc(sizeof(Command *) * MAX_ARGS);
    return buffer;
}

/**
 * @brief Initializes an app object.
 *
 * This function allocates memory for an app object and initializes its members.
 * It creates a buffer for the app, a command buffer, and sets the initial state of the app.
 * The current directory is also allocated with a length of 1024 characters.
 *
 * @return A pointer to the initialized app object.
 */
app_t *init_app()
{
    app_t *app = (app_t *)malloc(sizeof(app_t));
    app->app_buffer = init_buffer();
    app->cmd_buffer = init_cmd_buffer();
    app->has_init = false;
    app->current_directory = (char *)malloc(1024);
    app->current_directory_length = 1024;
    return app;
}

/**
 * Initializes a command buffer.
 *
 * @return A pointer to the newly initialized cmdBuffer_t object.
 */
cmdBuffer_t *init_cmd_buffer()
{
    cmdBuffer_t *buffer = (cmdBuffer_t *)malloc(sizeof(cmdBuffer_t));
    buffer->current = 0;
    buffer->size = 0;

    return buffer;
}

/**
 * Creates a new command with the specified type, arguments, and arguments length.
 *
 * @param type The type of the command.
 * @param args The arguments of the command.
 * @param args_length The length of the arguments array.
 * @return A pointer to the newly created Command struct, or NULL if memory allocation fails.
 */
Command *new_command(command_t type, char **args, int args_length)
{
    Command *new_command = (Command *)malloc(sizeof(Command));
    if (new_command == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for new command.\n");
        return NULL;
    }

    new_command->type = type;
    new_command->args = args;
    new_command->next = NULL;
    new_command->args_length = args_length;

    return new_command;
}