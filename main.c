/***************************************************************************/ /**
   @file         main.c
   @author       Jonathan Kuug & Abdul Wahab Abass
   @date         April 2024
   @brief        DSH (Dash Shell - Minimal Shell)
 *******************************************************************************/

// Library Imports
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <libgen.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include "linenoise.h"
#include "utils.h"
#include "types.h"

// App Macros
#define MAX_BUFFER_SIZE 4096
#define MAX_HISTORY_SIZE 250
#define MAX_ARGS 64
#define HISTORY_FILE ".dsh_history"
#define RC_FILE ".dshrc"

// Parsing utils
void parse_tokens(app_t *app);
char *print_prompt(app_t *app);
void read_input(app_t *app);
void prep_args(char *input, char **args);
void exec_handler(app_t *app);
void get_args(Token *token, char ***args, int *args_length);
void print_commands(Command *command);
void completion(const char *buf, linenoiseCompletions *lc);
char *hints(const char *buf, int *color, int *bold);

// Free app memory
void free_buffer(buff_t *buffer);
void free_app(app_t *app);
void free_cmd_buffer(cmdBuffer_t *buffer);
void free_commands(Command **command);

// Built-in commands (No system binaries)
void change_dir(char *path);

int main(int argc, char const *argv[])
{

    app_t *app = init_app();
    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);
    linenoiseHistoryLoad(HISTORY_FILE);
    linenoiseHistorySetMaxLen(MAX_HISTORY_SIZE);

    do
    {
        read_input(app);
        tokenize(app->app_buffer->buffer, &app->app_buffer->token_list);

        linenoiseFree(app->app_buffer->buffer);
        app->app_buffer->buffer = NULL;
        app->app_buffer->buffer_length = 0;

        parse_tokens(app);
        free(app->app_buffer->token_list);
        app->app_buffer->token_list = NULL;

        exec_handler(app);

        if (app->app_buffer->command_list != NULL)
        {
            free_commands(&(app->app_buffer->command_list[0]));
        }

    } while (1);

    // free line after use
    linenoiseFree(app->app_buffer->buffer);
    free_buffer(app->app_buffer);
    free_app(app);
}

/**
 * @brief Generates a prompt string for a shell application.
 *
 * This function generates a prompt string based on the current directory and the current Git branch (if available).
 * The prompt string is formatted as "<directory_name> (git:<branch_name>) >" if a Git branch is available,
 * otherwise it is formatted as "<directory_name> >".
 *
 * @param app A pointer to the shell application structure.
 * @return A pointer to the generated prompt string.
 */
char *print_prompt(app_t *app)
{
    char *prompt = (char *)malloc(MAX_BUFFER_SIZE * sizeof(char));
    if (prompt == NULL)
    {
        perror("Error allocating memory for prompt");
        exit(EXIT_FAILURE);
    }

    getcwd(app->current_directory, app->current_directory_length);

    FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
    char git_branch[1024] = "";

    if (fp != NULL)
    {
        fgets(git_branch, sizeof(git_branch), fp);
        git_branch[strcspn(git_branch, "\n")] = 0; // remove trailing newline
        pclose(fp);
    }
    else
    {
        perror("popen failed");
        free(prompt); // Free the allocated memory before returning NULL
        return NULL;
    }

    if (app->has_init == false)
    {
        printf("Welcome to DSH (Dash Shell) - A minimal shell\n");
        printf("Type 'exit' to quit the shell\n");
        printf("Type 'help' for list of commands\n");
        printf("\n");
        app->has_init = true;
    }

    if (strlen(git_branch) > 0)
    {
        snprintf(prompt, MAX_BUFFER_SIZE, "%s (git:%s) > ", basename(app->current_directory), git_branch);
    }
    else
    {
        snprintf(prompt, MAX_BUFFER_SIZE, "%s > ", basename(app->current_directory));
    }

    return prompt;
}

/**
 * Reads user input from the command line and stores it in the application buffer.
 *
 * @param app The application structure containing the buffer to store the input.
 */
void read_input(app_t *app)
{
    char *line_read = linenoise(print_prompt(app));

    if (line_read == NULL)
    {
        free(line_read);
        return;
    }

    if (*line_read)
    {
        linenoiseHistoryAdd(line_read);
        linenoiseHistorySave(HISTORY_FILE);
    }

    // handle tilde expansion
    char *home_dir = getenv("HOME");
    char *tilde_pos = NULL;
    while ((tilde_pos = strstr(line_read, "~")) != NULL)
    {
        char *new_line_read = malloc(strlen(line_read) + strlen(home_dir) + 1);
        strncpy(new_line_read, line_read, tilde_pos - line_read);
        new_line_read[tilde_pos - line_read] = '\0';
        strcat(new_line_read, home_dir);
        strcat(new_line_read, tilde_pos + 1);
        free(line_read);
        line_read = new_line_read;
    }

    // handle quote expansion
    char *quote_start = NULL;
    char *quote_end = NULL;
    char *new_line_read = NULL;
    while ((quote_start = strchr(line_read, '"')) != NULL)
    {
        quote_end = strchr(quote_start + 1, '"');
        if (quote_end == NULL)
        {
            printf("Mismatched quotes\n");
            free(line_read);
            return;
        }

        // Allocate memory for new_line_read
        new_line_read = malloc(strlen(line_read) - (quote_end - quote_start) + 1);
        if (new_line_read == NULL)
        {
            printf("Error allocating memory for new_line_read\n");
            free(line_read);
            return;
        }

        // Copy the part before the quote
        strncpy(new_line_read, line_read, quote_start - line_read);
        new_line_read[quote_start - line_read] = '\0';

        // Copy the part inside the quote
        strncat(new_line_read, quote_start + 1, quote_end - quote_start - 1);

        // Copy the part after the quote
        strcat(new_line_read, quote_end + 1);

        free(line_read);
        line_read = new_line_read;
    }

    app->app_buffer->buffer = strdup(line_read);
    app->app_buffer->buffer_length = strlen(app->app_buffer->buffer);

    if (app->app_buffer->buffer_length <= 0)
    {
        exit(0);
    }
}

/**
 * Parses the tokens in the app buffer and creates a command list.
 * Each token is checked for its value and a corresponding command is created.
 * The command list is stored in the app buffer.
 *
 * @param app The app structure containing the app buffer.
 */
void parse_tokens(app_t *app)
{

    Token *token_list = app->app_buffer->token_list;
    Command *last_command = new_command(SIMPLE, NULL, 0);

    int i = 0;
    while (token_list != NULL)
    {

        char **args = NULL;
        int args_length = 0;

        if (strcmp(token_list->value, "|") == 0)
        {
            last_command->next = app->app_buffer->command_list[i] = new_command(PIPE, args, args_length);
        }
        else if (strcmp(token_list->value, "<") == 0)
        {
            last_command->next = app->app_buffer->command_list[i] = new_command(REDIRECT_IN, args, args_length);
        }
        else if (strcmp(token_list->value, ">") == 0)
        {
            last_command->next = app->app_buffer->command_list[i] = new_command(REDIRECT_OUT, args, args_length);
        }
        else if (strcmp(token_list->value, "2>") == 0)
        {
            last_command->next = app->app_buffer->command_list[i] = new_command(REDIRECT_ERR, args, args_length);
        }
        else if (strcmp(token_list->value, ">>") == 0)
        {
            last_command->next = app->app_buffer->command_list[i] = new_command(REDIRECT_APP, args, args_length);
        }
        else if (strcmp(token_list->value, "&") == 0)
        {
            last_command->next = app->app_buffer->command_list[i] = new_command(BACKGROUND, args, args_length);
        }
        else if (strcmp(token_list->value, ";") == 0)
        {
            last_command->next = app->app_buffer->command_list[i] = new_command(SEQUENCE, args, args_length);
        }
        else if (strcmp(token_list->value, "&&") == 0)
        {
            last_command->next = app->app_buffer->command_list[i] = new_command(CONDITIONAL, args, args_length);
        }
        else
        {
            get_args(token_list, &args, &args_length);
            last_command->next = app->app_buffer->command_list[i] = new_command(SIMPLE, args, args_length);
        }

        last_command = last_command->next;
        i++;

        if (args_length > 0)
        {
            for (int i = 0; i < args_length; i++)
            {
                token_list = token_list->next;
            }
        }
        else
        {

            token_list = token_list->next;
        }
    }
}

/**
 * Extracts arguments from a linked list of tokens.
 *
 * @param token The head of the linked list of tokens.
 * @param args Pointer to the array of arguments.
 * @param args_length Pointer to the length of the args array.
 */
void get_args(Token *token, char ***args, int *args_length)
{
    Token *current = token;
    int i = 0;

    // Allocate memory for args
    *args = malloc(sizeof(char *) * (MAX_ARGS + 1)); // Allocate an extra element for the NULL pointer
    if (*args == NULL)
    {
        printf("Error allocating memory for args\n");
        return;
    }

    while (current != NULL)
    {
        if (strcmp(current->value, "|") == 0 || strcmp(current->value, "<") == 0 || strcmp(current->value, ">") == 0 || strcmp(current->value, "2>") == 0 || strcmp(current->value, ">>") == 0 || strcmp(current->value, "&") == 0 || strcmp(current->value, ";") == 0 || strcmp(current->value, "&&") == 0)
        {
            break;
        }
        (*args)[i] = current->value;
        current = current->next;
        i++;
    }

    (*args)[i] = NULL; // Add the NULL pointer at the end of the args array
    *args_length = i;
}

/**
 * Tokenizes the input string and stores the tokens in the args array.
 * Each token is separated by a space character.
 * The newline character at the end of each token is removed.
 *
 * @param input The input string to be tokenized.
 * @param args The array to store the tokens.
 */
void prep_args(char *input, char **args)
{
    char *token = strtok(input, " ");
    int i = 0;
    while (token != NULL)
    {
        // Remove newline character at the end of each token
        size_t len = strlen(token);
        if (token[len - 1] == '\n')
        {
            token[len - 1] = '\0';
        }

        args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
}

void change_dir(char *path)
{
    if (chdir(path) != 0)
    {
        perror("Error changing directory");
    }
}

void exec_handler(app_t *app)
{

    Command *current_command = app->app_buffer->command_list[0];

    if (current_command == NULL || current_command->args[0] == NULL)
    {
        return;
    }
    else if (strcmp(current_command->args[0], "cd") == 0)
    {
        change_dir(current_command->args[1]);
    }
    else if (strcmp(current_command->args[0], "exit") == 0)
    {
        // free_command(app->app_buffer->command_list[0]);
        // free_token(app->app_buffer->token_list);
        // free_buffer(app->app_buffer);
        // free_app(app);
        exit(0);
    }
    else
    {
        int pipefd[2];
        int in_fd = 0; // input file descriptor, start with stdin

        while (current_command != NULL)
        {
            if (current_command->next && current_command->next->type == PIPE)
            {
                if (pipe(pipefd) == -1)
                {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            pid_t pid = fork();
            if (pid == 0) // fork a child process to handle the command execution
            {

                if (in_fd != 0) // if not stdin, there's a pipe to read from
                {
                    dup2(in_fd, STDIN_FILENO);
                    close(in_fd);
                }

                if (current_command->next && current_command->next->type == PIPE)
                {
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                }

                if (current_command->args[0] != NULL && current_command->args != NULL)
                {
                    if (execvp(current_command->args[0], current_command->args) == -1)
                    {
                        perror("execvp");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                }
            }
            else if (pid < 0)
            {
                perror("Error forking process");
                exit(EXIT_FAILURE);
            }
            else
            {
                wait(NULL);

                if (in_fd != 0)
                    close(in_fd);

                if (current_command->next && current_command->next->type == PIPE)
                {
                    close(pipefd[1]);
                    in_fd = pipefd[0];
                }

                current_command = current_command->next ? current_command->next->next : NULL;
            }
        }
    }
}

/**
 * Prints the commands in a linked list of Command structures.
 *
 * @param command A pointer to the head of the linked list of Command structures.
 */
void print_commands(Command *command)
{
    Command *current = command;

    if (current == NULL)
    {
        printf("Command List is NULL\n");
        return;
    }

    while (current != NULL)
    {
        printf("Command type: %d\n", current->type);
        for (int i = 0; i < current->args_length; i++)
        {
            printf("Command arg: %s\n", current->args[i]);
        }
        current = current->next;
    }
}

/**
 * Searches for completions in a history file based on user input.
 *
 * @param buf The user's input.
 * @param lc A pointer to the linenoiseCompletions struct where completions will be added.
 */
void completion(const char *buf, linenoiseCompletions *lc)
{
    FILE *file = fopen(HISTORY_FILE, "r");
    if (file == NULL)
    {
        return;
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1)
    {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // If the line in the history file starts with the user's input, add it as a completion
        if (strncmp(buf, line, strlen(buf)) == 0)
        {
            linenoiseAddCompletion(lc, line);
        }
    }

    free(line);
    fclose(file);
}

/**
 * Returns a hint based on the user's input by searching through a history file.
 * The hint is a line from the history file that starts with the user's input.
 *
 * @param buf The user's input string.
 * @param color A pointer to an integer variable to store the color of the hint.
 * @param bold A pointer to an integer variable to store whether the hint should be bold or not.
 * @return A pointer to the hint string, or NULL if no hint is found.
 */
char *hints(const char *buf, int *color, int *bold)
{
    FILE *file = fopen(HISTORY_FILE, "r");
    if (file == NULL)
    {
        return NULL;
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1)
    {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // If the line in the history file starts with the user's input, return it as a hint
        if (strncmp(buf, line, strlen(buf)) == 0)
        {
            fclose(file);
            *color = 36; // Set hint color to magenta
            *bold = 0;
            return line + strlen(buf); // Return the part of the line that's not yet typed
        }
    }

    free(line);
    fclose(file);
    return NULL;
}

/**
 * Frees the memory allocated for a buffer.
 *
 * @param buffer The buffer to be freed.
 */
void free_buffer(buff_t *buffer)
{
    if (buffer)
    {
        free(buffer->buffer);
        free(buffer->command_list);
        free(buffer);
    }
}

/**
 * Frees the memory allocated for an app.
 *
 * @param app The app to be freed.
 */
void free_app(app_t *app)
{
    if (app)
    {
        free_buffer(app->app_buffer);
        free_cmd_buffer(app->cmd_buffer);
        free(app->current_directory);
        free(app);
    }
}

/**
 * Frees the memory allocated for a command buffer.
 *
 * @param buffer The command buffer to be freed.
 */
void free_cmd_buffer(cmdBuffer_t *buffer)
{
    if (buffer)
    {
        free(buffer);
    }
}

/**
 * Frees all commands in a linked list of Command structures.
 *
 * @param command A pointer to the head of the linked list of Command structures.
 */
void free_commands(Command **command)
{
    Command *current = *command;
    Command *next;

    while (current != NULL)
    {
        next = current->next;

        // Free the memory allocated for the command's arguments
        for (int i = 0; i < current->args_length; i++)
        {
            free(current->args[i]);
        }

        // Free the memory allocated for the command's args array
        free(current->args);

        // Free the memory allocated for the command itself
        free(current);

        current = next;
    }

    // Set the head of the linked list to NULL
    *command = NULL;
}
