CC=gcc
CFLAGS=-Wall -Werror -lncurses -std=c99

main: main.c
	$(CC) -o main main.c $(CFLAGS)
