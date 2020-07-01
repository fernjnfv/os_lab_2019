CC=gcc
CFLAGS=-I. -pthread

all : client server

client : client.o
	$(CC) -o client client.o $(CFLAGS)

client.o : client.c
	$(CC) -o client.o -c client.c $(CFLAGS)

server : server.o
	$(CC) -o server server.o $(CFLAGS)

server.o : server.c
	$(CC) -o server.o -c server.c $(CFLAGS)

clean :
	rm server.o client.o
