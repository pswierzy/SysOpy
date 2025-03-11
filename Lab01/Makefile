CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: all clean

all: countdown

countdown: countdown.c
	$(CC) $(CFLAGS) countdown.c -o countdown

clean: 
	rm countdown