CC = gcc
CFLAGS = -g -Wall

.PHONY: default all client server clean

default: $(TARGET)
	all: default

all: client server

client: client.o 
	$(CC) client.c -o client.o

server: server.o
	$(CC) server.c -o server.o -l sqlite3

clean: 
	rm -f client.o
	rm -f server.o
