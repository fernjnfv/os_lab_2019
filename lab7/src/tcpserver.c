#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr

int main(int argc, char *argv[]) {
  int ans = 0;
  {
    const size_t kSize = sizeof(struct sockaddr_in);

    int lfd, cfd;
    int nread;
    int BUFSIZE = atoi(argv[1]);
    int SERV_PORT = atoi(argv[2]);
    char buf[BUFSIZE];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      exit(1);
    }

    memset(&servaddr, 0, kSize);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
      perror("bind");
      exit(1);
    }

    if (listen(lfd, 5) < 0) {
      perror("listen");
      exit(1);
    }

    while (1) {
      unsigned int clilen = kSize;

      if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {
        perror("accept");
        exit(1);
      }

      printf("connection established\n");
      ans = 0;
      if ((nread = read(cfd, buf, BUFSIZE)) > 0) {
        write(1, &buf, nread);

        if(buf[0] == 'y'){
          ans = 1;
        }
      }

      if (nread == -1) {
        perror("read");
        exit(1);
      }
      
      if(ans == 1){
        close(cfd);
        break;
      }
      else{
        while ((nread = read(cfd, buf, BUFSIZE)) > 0) {
          write(1, &buf, nread);
        }
        if (nread == -1) {
          perror("read");
          exit(1);
        }
      }
      close(cfd);
    }
  }

  if(ans == 1){
    const size_t kSize = sizeof(struct sockaddr_in6);

    int lfd, cfd;
    int nread;
    int BUFSIZE = atoi(argv[1]);
    int SERV_PORT = atoi(argv[2]) + 1;
    char buf[BUFSIZE];
    struct sockaddr_in6 servaddr;
    struct sockaddr_in6 cliaddr;

    if ((lfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      exit(1);
    }

    memset(&servaddr, 0, kSize);
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(SERV_PORT);

    if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
      perror("bind");
      exit(1);
    }

    if (listen(lfd, 5) < 0) {
      perror("listen");
      exit(1);
    }

    while (1) {
      unsigned int clilen = kSize;

      if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {
        perror("accept");
        exit(1);
      }
      printf("connection established\n");

      while ((nread = read(cfd, buf, BUFSIZE)) > 0) {
        write(1, &buf, nread);
      }

      if (nread == -1) {
        perror("read");
        exit(1);
      }
      close(cfd);
    }
  }
}
