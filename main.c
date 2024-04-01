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
#include <string.h>

typedef struct InputBuffer
{
    char *buffer;
    char *args[64];
    size_t buffer_length;
    size_t input_length;
} buff_t;

typedef struct AppState
{
    buff_t *app_buffer;
    char *current_directory;
    size_t current_directory_length;
    bool has_init;
} app_t;

// App utils
buff_t *init_buffer();
app_t *init_app();
void print_prompt(app_t *app);
void read_input(app_t *app);
void prep_args(char *input, char **args);
void exec_handler(app_t *app);

// Built-in commands (No system binaries)
void change_dir(char *path);

int main(int argc, char const *argv[])
{

    app_t *app = init_app();

    do
    {
        print_prompt(app);
        read_input(app);
        prep_args(app->app_buffer->buffer, app->app_buffer->args);
        exec_handler(app);

    } while (1);
}

// void print_prompt(app_t *app)
// {

//     getcwd(app->current_directory, app->current_directory_length);

//     FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
//     char git_branch[1024] = "";

//     if (fp != NULL)
//     {
//         fgets(git_branch, sizeof(git_branch), fp);
//         git_branch[strcspn(git_branch, "\n")] = 0; // remove trailing newline
//         pclose(fp);
//     }

//     if (app->has_init == false)
//     {
//         printf("DSH version 1.0 \n");
//         if (strlen(git_branch) > 0)
//         {
//             printf("\033[38;2;6;152;154m%s\033[0m ", basename(app->current_directory));
//             printf("\033[38;2;52;101;164mgit:(\033[0m\033[38;2;204;0;0m%s\033[0m\033[38;2;52;101;164m) \033[0m\033[38;2;196;160;0m>\033[0m ", git_branch);
//         }
//         else
//         {
//             printf("\033[38;2;6;152;154m%s >\033[0m ", basename(app->current_directory));
//         }

//         app->has_init = true;
//     }
//     else
//     {
//         if (strlen(git_branch) > 0)
//         {
//             printf("\032[38;2;6;152;154m%s >\033[0m ", basename(app->current_directory));
//             printf("\033[38;2;52;101;164mgit:(\033[0m\033[38;2;204;0;0m%s\033[0m\033[38;2;52;101;164m) \033[0m\033[38;2;196;160;0m\033[0m ", git_branch);
//         }
//         else
//         {
//             printf("\033[38;2;6;152;154m%s >\033[0m ", basename(app->current_directory));
//         }
//     }
// }

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
    if (strcmp(app->app_buffer->args[0], "cd") == 0)
    {
        change_dir(app->app_buffer->args[1]);
    }
    else
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            if (execvp(app->app_buffer->args[0], app->app_buffer->args) == -1)
            {
                perror("Error executing command");
            }
            exit(0);
        }
        else if (pid < 0)
        {
            perror("Error forking process");
        }
        else
        {
            wait(NULL);
        }
    }
}

buff_t *init_buffer()
{
    buff_t *buffer = (buff_t *)malloc(sizeof(buff_t));
    buffer->buffer = NULL;
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