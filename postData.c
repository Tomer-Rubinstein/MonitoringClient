#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define MAXLINE 4096
#define SA struct sockaddr

int sockfd, n;
int sendbytes;
struct sockaddr_in servaddr;

char *request_fmt = "POST /api HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s";
char *request_body_fmt = "cpuType=%s&ram=%s&username=%s&cpuUsage=%.1f&processes=%s";

char sendline[MAXLINE];
char recvline[MAXLINE];
char requestBody[MAXLINE];

int postRequest(char *cpuName, char *userName, char *ramData, char *processes, float cpuUsage){
  sendline[0] = '\0';
  int contentLength = 0;

  char *cpuUsageDigits = calloc(5, sizeof(char));
  sprintf(cpuUsageDigits, "%.1f", cpuUsage);

  snprintf(requestBody, 1000, request_body_fmt, cpuName, ramData, userName, cpuUsage, processes);
  contentLength = strlen(requestBody);

  printf("sending data: %s %s %s %s %f\n", cpuName, userName, ramData, processes, cpuUsage);

  /* initializing the socket */
  if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("Socket couldn't be created!\n");
    return 1;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERVER_PORT);

  if(inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0){
    printf("Error: %s\n", SERVER_IP);
    return 1;
  }

  if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
    printf("Connection failed :(\n");
    return 1;
  }

  /* sending the POST request to the webserver */
  snprintf(sendline, 1000, request_fmt, contentLength, requestBody);
  printf("%s\n", sendline);
  sendbytes = strlen(sendline);
  
  if(write(sockfd, sendline, sendbytes) != sendbytes){
    printf("Write error!\n");
    return 1;
  }

  printf("data has been sent, waiting for read..\n");

  /* reading the response */
  memset(recvline, 0, MAXLINE);
  while((n = read(sockfd, recvline, MAXLINE-1)) > 0){
    printf("%s", recvline);
  }

  if(n < 0){
    printf("Read error\n");
    return 1;
  }



  return 0;
}
