CC=gcc
CFLAGS=-I. -pthread

all : tcpserver tcpclient udpserver udpclient

tcpserver : tcpserver.o
	$(CC) -o tcpserver tcpserver.o $(CFLAGS)

tcpserver.o : tcpserver.c
	$(CC) -o tcpserver.o -c tcpserver.c $(CFLAGS)

tcpclient : tcpclient.o
	$(CC) -o tcpclient tcpclient.o $(CFLAGS)

tcpclient.o : tcpclient.c
	$(CC) -o tcpclient.o -c tcpclient.c $(CFLAGS)

udpserver : udpserver.o
	$(CC) -o udpserver udpserver.o $(CFLAGS)

udpserver.o : udpserver.c
	$(CC) -o udpserver.o -c udpserver.c $(CFLAGS)

udpclient : udpclient.o
	$(CC) -o udpclient udpclient.o $(CFLAGS)

udpclient.o : udpclient.c
	$(CC) -o udpclient.o -c udpclient.c $(CFLAGS)

clean :
	rm tcpserver.o tcpclient.o udpserver.o udpclient.o
