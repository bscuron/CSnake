CC=gcc
CFLAGS=-Wall -Werror -lncurses -std=gnu99 -lc

main: main.c
	$(CC) -o main main.c $(CFLAGS)
