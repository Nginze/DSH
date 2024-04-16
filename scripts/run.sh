#!/bin/bash

# Get the directory of the script
DIR="$(dirname "$0")"

# Change to the root directory of the project
cd "$DIR/.."

# Compile the code
make

# Check if make was successful
if [ $? -eq 0 ]; then
    # Run the executable
    ./main
else
    echo "Compilation failed"
fi