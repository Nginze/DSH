CC=gcc
CFLAGS = -g -Wall

all:	main

main:	main.c	utils.o	linenoise.o
	$(CC) $(CFLAGS) -o main main.c utils.o linenoise.o

utils.o:	utils.c	utils.h
	$(CC) $(CFLAGS) -c utils.c 

linenoise.o:	linenoise.c	linenoise.h
	$(CC) $(CFLAGS) -c linenoise.c

clean: 
	rm -f main *.o
