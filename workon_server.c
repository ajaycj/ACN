/* A simple, TCP based echo server */

#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
int create_tcp_socket();
char *get_ipaddr(char *host);
void getDataFromRealServer(char*, int, char*,char*,int);
static void *doit(void *);
void string_read(int);
char *get_host_web_page(char *host, char *web_page);
#define USERAGENT "HTMLGET 1.0"

int main(int argc, char **argv) {
   int clilen;
   /* variables section */
   pthread_t tid;
   int   listenfd, connfd;
   struct sockaddr_in cli_addr, servaddr;

   /* cnreating a socket */
   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     printf("\n%s: Error in socket", argv[0]);
     exit(0);
   }

   /* configuring server address structure */
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
   servaddr.sin_port      = htons(9945); 

   /* binding our socket to the service port */
   if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
     printf("\n%s: binding error", argv[0]); 
     exit(0);
   } 

   /* convert our socket to a listening socket */
   if (listen(listenfd, 5) < 0) {
     printf("\n%s: listening error", argv[0]); 
     exit(0);
   } 

   for ( ; ; ) {
     clilen = sizeof(cli_addr);

     /* accept a new connection and return a new socket descriptor to 
     handle this new client */
     if ((connfd = accept(listenfd, (struct sockaddr*) &cli_addr, &clilen)) < 0) {
       printf("\n%s: Error in accept", argv[0]); 
       exit(0);
     } 
    printf("\nEstablished connection from client, connection no. = %d\n", connfd);
    pthread_create(&tid, NULL, &doit, (void *) connfd);   
  }
}

static void * doit(void *arg) {
    pthread_detach(pthread_self());
    string_read((int) arg);
    close((int) arg);
    return (NULL);
}

void string_read (int sockfd) {
    ssize_t n;
    char line[512];

    /* read from socket */
    n = read(sockfd, (void *) line, 512);

    if (n < 0) {
        printf("\nError in read"); 
        exit(0);
    } else if (n == 0) {
        return;
    } 

    char host_name[100];
    char web_page[100];
    int port;

    sscanf(line, "%99[^:]:%99d:%99[^\n]", host_name,&port,web_page );
    printf("\nReceived message from client, connection no. = %d\n", sockfd);
    printf("host_name = %s \n", host_name);

    printf("port = %d \n", port);

    printf("web_page = %s \n", web_page);
    char html[9999];
    getDataFromRealServer(host_name,port,web_page,html,9999);


    /* echo back to the client */
    if (write(sockfd, (const void*) html, 9999) < 0) {
        printf("\nError in write");
        exit(0);
    } 

        close(sockfd);
}


    void getDataFromRealServer(char *host, int port, char *web_page,
            char* readBuf, int bufSize)
    {
      struct sockaddr_in *serv_sock_read;
      int sock;
      int recv_value;
      char *ipaddr;
      char *get;

      sock = create_tcp_socket();
      ipaddr = get_ipaddr(host);
      printf("IP is %s\n", ipaddr);
      serv_sock_read = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
      serv_sock_read->sin_family = AF_INET;
      recv_value = inet_pton(AF_INET, ipaddr, (void *)(&(serv_sock_read->sin_addr.s_addr)));
      if( recv_value < 0)  
      {
        perror("Can't set serv_sock_read->sin_addr.s_addr");
        exit(1);
      }else if(recv_value == 0)
      {
       printf( "%s is not a valid IP address\n", ipaddr);
        exit(1);
      }
      serv_sock_read->sin_port = htons(port);
     
      if(connect(sock, (struct sockaddr *)serv_sock_read, sizeof(struct sockaddr)) < 0){
        perror("connect unable");
        exit(1);
      }
      get = get_host_web_page(host, web_page);
      printf("Query is:\n<<START>>\n%s<<END>>\n", get);
     
      //Send the query to the server
      int sent = 0;
      while(sent < strlen(get))
      {
        recv_value = send(sock, get+sent, strlen(get)-sent, 0);
        if(recv_value == -1){
          perror("query unable");
          exit(1);
        }
        sent += recv_value;
      }
      //receive the web_page from serer
   
      recv_value = recv(sock, readBuf, bufSize, 0);
      
      if(recv_value < 0)
      {
        perror("Error receiving data");
      }
      printf("\n %s \n",readBuf);
      free(get);
      free(serv_sock_read);
      free(ipaddr);
      close(sock);
    }
     
     
    char *get_ipaddr(char *host)
    {
      struct hostent *hent;
      int ipaddr_length = 16; //say 4*4
      char *ipaddr = (char *)malloc(ipaddr_length);
      memset(ipaddr, 0, ipaddr_length);
      if((hent = gethostbyname(host)) == NULL)
      {
        herror("Can't get required IP");
        exit(1);
      }
      if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ipaddr, ipaddr_length) == NULL)
      {
        perror("host cannot be resolved");
        exit(1);
      }
      return ipaddr;
    }


 int create_tcp_socket()
    {
      int sock;
      if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        perror("Can't create TCP socket");
        exit(1);
      }
      return sock;
    }
     
    char *get_host_web_page(char *host, char *web_page)
    {
      char *query;
      char *getweb_page = web_page;
      char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
      if(getweb_page[0] == '/'){
        getweb_page = getweb_page + 1;
        printf("Removing leading \"/\", converting %s to %s\n", web_page, getweb_page);
      }
      query = (char *)malloc(strlen(host)+strlen(getweb_page)+strlen(USERAGENT)+strlen(tpl));
      sprintf(query, tpl, getweb_page, host, USERAGENT);
      return query;
    }
