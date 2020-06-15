#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)
#define SIZE6 sizeof(struct sockaddr_in6)

int main(int argc, char *argv[]) {
  int v6 = 0;
  {
    int fd;
    int nread;
    int BUFSIZE = atoi(argv[1]);
    char buf[BUFSIZE];
    struct sockaddr_in servaddr;
    if (argc < 3) {
      printf("Too few arguments \n");
      exit(1);
    }

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket creating");
      exit(1);
    }

    memset(&servaddr, 0, SIZE);
    servaddr.sin_family = AF_INET;

    if (inet_pton(AF_INET, argv[2], &servaddr.sin_addr) <= 0) {
      perror("bad address");
      exit(1);
    }

    servaddr.sin_port = htons(atoi(argv[3]));

    if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
      perror("connect");
      exit(1);
    }

    write(1, "Input message to send\n", 22);
    if ((nread = read(0, buf, BUFSIZE)) > 0) {
      if (write(fd, buf, nread) < 0) {
        perror("write");
        exit(1);
      }
      if(buf[0] == 'y'){
        v6 = 1;
      }
    }
    if(v6 == 1){
      close(fd);
    }
    else{
      while ((nread = read(0, buf, BUFSIZE)) > 0) {
        if (write(fd, buf, nread) < 0) {
          perror("write");
          exit(1);
        }
      }
    }
    close(fd);
  }
  sleep(1);
  if(v6 == 1){
    int fd;
    int nread;
    int BUFSIZE = atoi(argv[1]);
    char buf[BUFSIZE];
    struct sockaddr_in6 servaddr;
    if (argc < 3) {
      printf("Too few arguments \n");
      exit(1);
    }

    if ((fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
      perror("socket creating");
      exit(1);
    }

    memset(&servaddr, 0, SIZE6);
    servaddr.sin6_family = AF_INET6;

    if (inet_pton(AF_INET6, "::1", &servaddr.sin6_addr) <= 0) {
      perror("bad address");
      exit(1);
    }

    servaddr.sin6_port = htons(atoi(argv[3])+1);

    if (connect(fd, (SADDR *)&servaddr, SIZE6) < 0) {
      perror("connect");
      exit(1);
    }
    
    write(1, "Input message to send\n", 22);
    while ((nread = read(0, buf, BUFSIZE)) > 0) {
      if (write(fd, buf, nread) < 0) {
        perror("write");
        exit(1);
      }
    }
    close(fd);
  }
  exit(0);
}
