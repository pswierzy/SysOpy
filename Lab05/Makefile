CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean

all: ex1 catcher sender

ex1: ex1.c
	$(CC) $(CFLAGS) ex1.c -o ex1
	
catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher
	
sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

clean: 
	rm ex1 catcher sender