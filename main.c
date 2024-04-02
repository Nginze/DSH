/***************************************************************************/ /**
   @file         main.c
   @author       Jonathan Kuug & Abdul Wahab Abass
   @date         April 2024
   @brief        DSH (Dash Shell - Minimal Shell)
 *******************************************************************************/

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
#include "utils.h"

#define MAX_BUFFER_SIZE 1024
#define MAX_ARGS 64

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

typedef struct Command
{
    command_t type;
    int args_length;
    char **args;
    struct Command *next;
} Command;

typedef struct InputBuffer
{
    char *buffer;
    char *args[64];
    Token *token_list;
    Command **command_list;
    size_t buffer_length;
    size_t input_length;
} buff_t;

typedef struct AppState
{
    buff_t *app_buffer;
    command_t current_command_type;
    size_t current_directory_length;
    char *current_directory;
    bool has_init;
} app_t;

// App utils
buff_t *init_buffer();
app_t *init_app();
Command *new_command(command_t type, char **args, int args_length);
void parse_tokens(app_t *app);
void print_prompt(app_t *app);
void read_input(app_t *app);
void prep_args(char *input, char **args);
void exec_handler(app_t *app);
void get_args(Token *token, char ***args, int *args_length);
void print_commands(Command *command);

// Built-in commands (No system binaries)
void change_dir(char *path);

int main(int argc, char const *argv[])
{

    app_t *app = init_app();

    do
    {
        print_prompt(app);
        read_input(app);
        tokenize(app->app_buffer->buffer, &app->app_buffer->token_list);
        parse_tokens(app);
        exec_handler(app);

    } while (1);
}

void print_prompt(app_t *app)
{
    getcwd(app->current_directory, app->current_directory_length);

    FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
    char git_branch[1024] = "";

    if (fp != NULL)
    {
        fgets(git_branch, sizeof(git_branch), fp);
        git_branch[strcspn(git_branch, "\n")] = 0; // remove trailing newline
        pclose(fp);
    }

    if (app->has_init == false)
    {
        printf("DSH version 1.0 \n");
        app->has_init = true;
    }

    if (strlen(git_branch) > 0)
    {
        printf("%s (git:%s) > ", basename(app->current_directory), git_branch);
    }
    else
    {
        printf("%s > ", basename(app->current_directory));
    }
}

void read_input(app_t *app)
{
    int bytes_read = getline(&app->app_buffer->buffer, &app->app_buffer->buffer_length, stdin);
    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        exit(0);
    }
}

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
            for (int i = 0; i < args_length; i++)
            {
                token_list = token_list->next;
            }
        else
            token_list = token_list->next;
    }
}

void get_args(Token *token, char ***args, int *args_length)
{
    Token *current = token;
    int i = 0;

    // Allocate memory for args
    *args = malloc(sizeof(char *) * MAX_ARGS);
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
    *args_length = i;
}

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

    if (strcmp(current_command->args[0], "cd") == 0)
    {
        change_dir(current_command->args[1]);
    }
    else
    {
        int pipefd[2];
        int in_fd = 0;              // input file descriptor, start with stdin
        int out_fd = STDOUT_FILENO; // output file descriptor, start with stdout
        int err_fd = STDERR_FILENO; // error file descriptor, start with stderr

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

                switch (current_command->type)
                {
                case REDIRECT_IN:
                    in_fd = open(current_command->args[1], O_RDONLY);
                    dup2(in_fd, STDIN_FILENO);
                    break;
                case REDIRECT_OUT:
                    out_fd = open(current_command->args[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    dup2(out_fd, STDOUT_FILENO);
                    break;
                case REDIRECT_ERR:
                    err_fd = open(current_command->args[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    dup2(err_fd, STDERR_FILENO);
                    break;
                case REDIRECT_APP:
                    out_fd = open(current_command->args[1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    dup2(out_fd, STDOUT_FILENO);
                    break;
                case BACKGROUND:
                    // TODO: handle background command
                    break;
                case SEQUENCE:
                    // TODO: handle sequence of commands
                    break;
                case CONDITIONAL:
                    // TODO: handle conditional execution
                    break;
                default:
                    break;
                }

                if (execvp(current_command->args[0], current_command->args) == -1)
                {
                    perror("execvp");
                    exit(EXIT_FAILURE);
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


buff_t *init_buffer()
{
    buff_t *buffer = (buff_t *)malloc(sizeof(buff_t));
    buffer->buffer = NULL;
    buffer->command_list = (Command **)malloc(sizeof(Command *) * MAX_ARGS);
    return buffer;
}

app_t *init_app()
{
    app_t *app = (app_t *)malloc(sizeof(app_t));
    app->app_buffer = init_buffer();
    app->has_init = false;
    app->current_directory = (char *)malloc(1024);
    app->current_directory_length = 1024;
    return app;
}

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

void print_commands(Command *command)
{
    Command *current = command;
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