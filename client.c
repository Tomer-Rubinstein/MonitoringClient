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

int main() {
  float prevCpuUsage = getCPUUsage();

  int sockfd, n;
  int sendbytes;
  struct sockaddr_in servaddr;
  char sendline_fmt[MAXLINE] = "POST /user=%s HTTP/1.1\r\n\r\n"
  char sendline[MAXLINE];
  char recvline[MAXLINE];

  getUserName();
  getCPUName();
  getRamData();
  getProcesses();
  printf("%s\n%s\n%s\n%s\n", cpuName, userName, ramData, processes);

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

  sprintf(sendline, sendline_fmt, userName);

  printf("%s\n", sendline);
  sendbytes = strlen(sendline);

  if(write(sockfd, sendline, sendbytes) != sendbytes){
    printf("Write error!\n");
    return 1;
  }

  memset(recvline, 0, MAXLINE);

  while((n = read(sockfd, recvline, MAXLINE-1)) > 0){
    printf("%s", recvline);
  }
  if(n < 0){
    printf("Read error\n");
    return 1;
  }

  /*while(1){
    getRamData();
    printf("%s\n", ramData);
    printf("[+] CPU: %.1f% -\n", myAbs(getCPUUsage()-prevCpuUsage)); /* calculate the cpu percentage every 3 seconds
    prevCpuUsage = getCPUUsage();

    sleep(3);
  }*/

  return 0;
}

float myAbs(float n){
  return (n < 0) ? -n : n;
}
