ls -l
pwd
echo Hello, World!
ls non_existent_directory
cat /etc/passwd
echo $?
echo 'This is a test' | cat
cat < /etc/passwd > output.txt
exit

# Append redirect
echo 'This is a test' >> output.txt
echo 'This is another test' >> output.txt
cat output.txt

# Error redirect
ls non_existent_directory 2> error.txt
cat error.txt

# Background command
echo 'Start'
sleep 5 &
echo 'End'
