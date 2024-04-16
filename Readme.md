# Minimal Shell Application for Linux

This project is a minimal shell application for Linux. It aims to provide a lightweight, customizable shell with a focus on common features and usability.

## Features

### Command Execution

The shell supports the execution of all Linux commands using the `execvp` system call.

### Built-in Commands

The shell includes support for built-in commands such as `cd`, `exit`, and `help`.

### Command Parsing

The shell includes a parser for command input, allowing for complex command structures and arguments.

### Command History

The shell supports command history cycling, allowing users to scroll through their previous commands.

### Command Autocompletion and Suggestions

The shell aims to support command autocompletion and suggestions, possibly with the help of AI or other solutions.

### Scripting Support

The shell aims to support scripting, allowing users to write and execute shell scripts.

### Piping and Redirection

The shell aims to support piping (`|`) and redirection (`<>`) of processes, allowing for complex command structures.

### Environment Variables

The shell aims to support environment variables, allowing users to customize their shell environment.

### Prompt Customization

The shell aims to support prompt customization with `rc` files, allowing users to customize their shell prompt.

### Signal Handling

The shell supports signal handling for `SIGINT` and `SIGTERM`.

## Getting Started

To compile the project, navigate to the project directory and run the `make` command:

```bash
make
```

To run the shell, use the following command:

```bash
./main
```

## Testing

To test the shell, you can use the provided test script. This script tests a variety of shell commands and checks if their output is correct. To run the test script, use the following command:

```bash
.scripts/test.sh
```

If a command is successful, the script will print "Success"; otherwise, it will print "Failure".

