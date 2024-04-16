CC=gcc
CFLAGS = -g -Wall

all:	main

main:	main.c	utils.o	linenoise.o types.o
	$(CC) $(CFLAGS) -o main main.c utils.o linenoise.o types.o

utils.o:	utils.c	utils.h
	$(CC) $(CFLAGS) -c utils.c 

linenoise.o:	linenoise.c	linenoise.h
	$(CC) $(CFLAGS) -c linenoise.c

types.o:	types.c	types.h
	$(CC) $(CFLAGS) -c types.c

clean: 
	rm -f main *.o