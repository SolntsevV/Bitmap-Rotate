CC=gcc
CFLAGS=-ansi -pedantic -Wall -Werror

all: main

main: clean main.o bmp.o 
	$(CC) $(CFLAGS) -o main main.o bmp.o
	
clean:
	rm -f  main
