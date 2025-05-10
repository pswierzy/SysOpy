CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean

all: chat_server chat_user

chat_server: chat_server.c
	$(CC) $(CFLAGS) chat_server.c -o chat_server

chat_user: chat_user.c
	$(CC) $(CFLAGS) chat_user.c -o chat_user

clean: 
	rm chat_server chat_user