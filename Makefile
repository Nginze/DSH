CC=gcc
CFLAGS = -g -Wall
LDFLAGS = -lreadline

all:	main

main:	main.c	utils.o
	$(CC) $(CFLAGS) -o main main.c utils.o $(LDFLAGS)

utils.o:	utils.c	utils.h
	$(CC) $(CFLAGS) -c utils.c 

clean: 
	rm -f main *.o