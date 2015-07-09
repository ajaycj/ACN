/* A simple, TCP based echo client */
#include <netinet/in.h>
#include <stdio.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

void str_cli(int, char**);

int main(int argc, char **argv) {
  int                sockfd;
  struct sockaddr_in servaddr;

  if (argc != 2) {
    printf("\nUsage: tcp-client <IPaddress>");
    exit(0);
  }
 
  /* creating a socket */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n%s: Error in socket", argv[0]);
    exit(0);
  }

  /* configuring server address structure */
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(9945);

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
    printf("\n%s: Error in inet_pton", argv[0]); 
    exit(0);
  }

  /* connecting to the server */
  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    printf("\n%s: Error in connect", argv[0]);
    exit(0);
  }

  str_cli(sockfd, argv);
}

void str_cli (int sockfd, char **argv) {
  char sendline[1024], recvline[1024];
  int n;

  /* read a line from stdin */
  if (fgets(sendline, 1024, stdin) == NULL) {
    printf("\n%s: Error in fgets", argv[0]);
    exit(0);
  }

  /* send it to echo server */
  if (write(sockfd, (const void*) sendline, strlen(sendline)) < 0) {
    printf("\n%s: Error in write", argv[0]);
    exit(0);
  }

  /* get the echo from the echo server and display it on the terminal*/
  while ( (n = read(sockfd, recvline, 1024)) > 0) {
    recvline[n] = 0; /* null terminate the string */
    
    fputs(recvline, stdout);
  }

  close(sockfd);
}
