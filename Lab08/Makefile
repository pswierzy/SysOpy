CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean

all: printer user master

printer: printer.c
	$(CC) $(CFLAGS) printer.c -o printer

master: master.c
	$(CC) $(CFLAGS) master.c -o master

user: user.c
	$(CC) $(CFLAGS) user.c -o user

clean: 
	rm printer user master