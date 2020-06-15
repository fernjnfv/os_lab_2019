#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in6)

int main(int argc, char **argv) {
  int sockfd, n;
  int BUFSIZE = atoi(argv[1]);
  int SERV_PORT = atoi(argv[2]);
  char mesg[BUFSIZE], ipadr[16];
  struct sockaddr_in6 servaddr;
  struct sockaddr_in6 cliaddr;

  if ((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }

  memset(&servaddr, 0, SLEN);
  servaddr.sin6_family = AF_INET6;
  servaddr.sin6_addr = in6addr_any;
  servaddr.sin6_port = htons(SERV_PORT);

  if (bind(sockfd, (SADDR *)&servaddr, SLEN) < 0) {
    perror("bind problem");
    exit(1);
  }
  printf("SERVER starts...\n");

  while (1) {
    unsigned int len = SLEN;

    if ((n = recvfrom(sockfd, mesg, BUFSIZE, 0, (SADDR *)&cliaddr, &len)) < 0) {
      perror("recvfrom");
      exit(1);
    }
    mesg[n] = 0;

    printf("REQUEST %s      FROM %s : %d\n", mesg,
           inet_ntop(AF_INET6, (void *)&cliaddr.sin6_addr, ipadr, 16),
           ntohs(cliaddr.sin6_port));

    if (sendto(sockfd, mesg, n, 0, (SADDR *)&cliaddr, len) < 0) {
      perror("sendto");
      exit(1);
    }
  }
}
