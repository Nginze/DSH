#!/bin/bash

# Get the directory of the script
DIR="$(dirname "$0")"

# Change to the root directory of the project
cd "$DIR/.."

# Pull the latest commits from the repository
git pull origin master

# Check if the pull was successful
if [ $? -eq 0 ]; then
    echo " "
else
    echo " "
fi