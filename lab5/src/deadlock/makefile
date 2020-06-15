CC=gcc
CFLAGS=-I. -pthread

all : mutex

mutex : mutex.o
	$(CC) -o mutex mutex.o $(CFLAGS)

mutex.o : mutex.c
	$(CC) -o mutex.o -c mutex.c $(CFLAGS)

clean :
	rm mutex.o
