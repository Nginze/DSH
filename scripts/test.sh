#!/bin/bash

# Test script for your shell

# Function to display test results
display_result() {
    if [ $1 -eq 0 ]; then
        echo -e "\e[32mTest Passed: $2\e[0m"
    else
        echo -e "\e[31mTest Failed: $2\e[0m"
    fi
}

# Function to run command with pipe
run_with_pipe() {
    echo "$1" | grep "test"
}

# Run your shell with each command and check the exit code
# Example commands to test
commands=(
    "ls -l"
    "pwd"
    "echo Hello, World!"
    "ls non_existent_directory"
    "cat /etc/passwd"
    "echo $?"
    "run_with_pipe 'echo \"This is a test\"'"
    "exit"
)

for cmd in "${commands[@]}"; do
    echo -e "\nExecuting command: $cmd"
    if [[ "$cmd" == "run_with_pipe"* ]]; then
        eval "$cmd"
    else
        ./main "$cmd"
    fi
    display_result $? "$cmd"
done


