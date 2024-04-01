CC=gcc
CFLAGS=-I.

all: main

main: main.c
	$(CC) -o main main.c $(CFLAGS)

clean: 
	rm -f main