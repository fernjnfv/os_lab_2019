CC=gcc
CFLAGS=-I. -pthread

all : deadlock

deadlock : deadlock.o
	$(CC) -o deadlock deadlock.o $(CFLAGS)

deadlock.o : deadlock.c
	$(CC) -o deadlock.o -c deadlock.c $(CFLAGS)

clean :
	rm deadlock.o
