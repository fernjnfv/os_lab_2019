CC=gcc
CFLAGS=-I.

all : zombie

zombie : zombie.o
	$(CC) -o zombie zombie.o $(CFLAGS)

zombie.o : zombie.c
	$(CC) -o zombie.o -c zombie.c $(CFLAGS)

clean :
	rm zombie.o
