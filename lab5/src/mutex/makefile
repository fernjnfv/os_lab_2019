CC=gcc
CFLAGS=-I. -pthread

all : mutex no_mutex

mutex : mutex.o
	$(CC) -o mutex mutex.o $(CFLAGS)

mutex.o : mutex.c
	$(CC) -o mutex.o -c mutex.c $(CFLAGS)

no_mutex : no_mutex.o
	$(CC) -o no_mutex no_mutex.o $(CFLAGS)

no_mutex.o : no_mutex.c
	$(CC) -o no_mutex.o -c no_mutex.c $(CFLAGS)

clean :
	rm mutex.o no_mutex.o
