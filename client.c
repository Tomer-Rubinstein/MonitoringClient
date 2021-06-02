#include "getters.c"

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

float myAbs(float);
char *encodeStrToUrl(char *);
char *removeSpaces(char *);

int main() {
  float prevCpuUsage = getCPUUsage();
  float currCpu = 0;

  int sockfd, n;
  int sendbytes;
  struct sockaddr_in servaddr;
  char *request_fmt = "POST /api HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 100\n\r\ncpuType=%s&ram=%s&username=%s&processes=%s&cpuUsage=%.1f";
  char sendline[MAXLINE];
  char recvline[MAXLINE];
  char *token;
  sendline[0] = '\0';

  /* getting pc's information */
  getUserName();
  getCPUName();
  getRamData();
  getProcesses();
  printf("%s\n%s\n%s\n%s\n",   cpuName, userName, ramData, processes);

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

  /* parsing the cpu name and encoding spaces to %20 */
  token = strtok(cpuName, ":");
  token = strtok(NULL, ":");
  cpuName = token;
  cpuName = encodeStrToUrl(cpuName);

  /* parsing the ram */
  token = NULL;
  token = strtok(ramData, "Mem:");
  ramData[0] = '\0';
  strcat(ramData, removeSpaces(token));
  token = NULL;
  token = strtok(ramData, "G");
  strcat(ramData, "G");

  /* sending the first POST request to the webserver */
  snprintf(sendline, 1000, request_fmt, cpuName, ramData, userName, "processes", currCpu);
  printf("%s\n", sendline);
  sendbytes = strlen(sendline);
  if(write(sockfd, sendline, sendbytes) != sendbytes){
    printf("Write error!\n");
    return 1;
  }

  /* reading the response */
  memset(recvline, 0, MAXLINE);
  while((n = read(sockfd, recvline, MAXLINE-1)) > 0){
    printf("%s", recvline);
  }
  if(n < 0){
    printf("Read error\n");
    return 1;
  }

  /* sending live data every 5 seconds */
  /* current cpu percentage & current ram available */
  while(1){
    /* iteration initialization */
    sleep(5);
    sendline[0] = '\0';
    
    /* updating the ram data */
    getRamData();

    /* calculating the current CPU percentage */
    currCpu = myAbs(getCPUUsage()-prevCpuUsage);

    /* creating the request */
    snprintf(sendline, 300, request_fmt, "0", "16G", "0", "0", currCpu);
    printf("%s\n", sendline);

    /* sending the data */
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

    /* sending the first POST request to the webserver */
    snprintf(sendline, 1000, request_fmt, cpuName, ramData, userName, "processes", currCpu);
    printf("%s\n", sendline);
    sendbytes = strlen(sendline);
    if(write(sockfd, sendline, sendbytes) != sendbytes){
      printf("Write error!\n");
      return 1;
    }

    /* reading response */
    memset(recvline, 0, MAXLINE);
    while((n = read(sockfd, recvline, MAXLINE-1)) > 0){
      printf("%s", recvline);
    }
    if(n < 0){
      printf("Read error\n");
      return 1;
    }

    /* used in the next iteration for calculating the cpu percentage */
    /* in the next 5 seconds */
    prevCpuUsage = getCPUUsage();
  }

  return 0;
}

/* returns an absolute float */
float myAbs(float n){
  return (n < 0) ? -n : n;
}

/* returns a string but spaces are replaced with %20 (url encoded) */
char *encodeStrToUrl(char *str){
  char *newStr = (char *)malloc(strlen(str) * sizeof(char));
  char *validSpace = "%20";
  int i;

  newStr[0] = '\0';
  for(i=0; i < strlen(str); i++){
    if(i != 0){
      if(str[i] == ' ')
        strcat(newStr, validSpace);
      else
        strncat(newStr, &str[i], 1);
    }
  }
  return newStr;
}

/* returns a string without spaces */
char *removeSpaces(char *str){
  char *newStr = (char *)malloc(strlen(str) * sizeof(char));
  int i;

  newStr[0] = '\0';
  for(i=0; i < strlen(str); i++){
    if(i != 0){
      if(str[i] != ' ')
        strncat(newStr, &str[i], 1);
    }
  }
  return newStr;
}
