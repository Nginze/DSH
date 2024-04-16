#!/bin/bash

# Test a simple command
output=$(echo 'ls' | ../main)
if [ $? -eq 0 ]; then
    echo "ls command: Success"
else
    echo "ls command: Failure"
fi

# Test a command with options
output=$(echo 'ls -l' | ../main)
if [ $? -eq 0 ]; then
    echo "ls -l command: Success"
else
    echo "ls -l command: Failure"
fi

# Test a command with a pipe
output=$(echo 'ls | grep test.txt' | ../main)
if [ $? -eq 0 ]; then
    echo "Pipe command: Success"
else
    echo "Pipe command: Failure"
fi

# Test a command with redirection
output=$(echo 'ls > test.txt' | ../main)
if [ $? -eq 0 ]; then
    echo "Redirection command: Success"
else
    echo "Redirection command: Failure"
fi