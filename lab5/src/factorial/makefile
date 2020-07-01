CC=gcc
CFLAGS=-I. -pthread

all : factorial	

factorial : factorial.o
	$(CC) -o factorial factorial.o $(CFLAGS)

factorial.o : factorial.c
	$(CC) -o factorial.o -c factorial.c $(CFLAGS)

clean :
	rm factorial.o