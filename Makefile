CC=gcc
CFLAGS=-I.

all:	main

main:	main.c	utils.o
	$(CC) -o main main.c utils.o $(CFLAGS)

utils.o:	utils.c	utils.h
	$(CC) -c utils.c $(CFLAGS)

clean: 
	$(CC) -f main utils.o