#! /usr/bin/env bash
set -e

# Run the shell in a loop
while true
do
    # Start a new instance of the shell in a subshell
    (trap - INT; exec -- "$@") || echo "The command exited with an error status."

    # Wait for a key press to restart the shell
    printf 'Press any key to restart %s\n' "${*@Q}"
    read -n 1 -r -s key
done