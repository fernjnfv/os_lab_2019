CC=gcc
CFLAGS=-I. -pthread

all : parallel_sum

parallel_sum : parallel_sum.o
	$(CC) -o parallel_sum parallel_sum.o $(CFLAGS)

parallel_sum.o : parallel_sum.c
	$(CC) -o parallel_sum.o -c parallel_sum.c $(CFLAGS)

clean :
	rm parallel_sum.o
