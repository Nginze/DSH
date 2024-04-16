#!/bin/bash

# Compile the code
make

# Check if make was successful
if [ $? -eq 0 ]; then
    # Run the executable
    ./main
else
    echo "Compilation failed"
fi