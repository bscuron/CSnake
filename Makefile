CC=gcc
CFLAGS=-Wall -Werror -lncurses

main: main.c
	$(CC) -o main main.c $(CFLAGS)
