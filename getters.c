/*
* All functions written in this file aren't getters
* since they do not return anything,
* they set the values of their variables instead.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include <unistd.h>

char *cpuName = NULL;
char userName[50];
char ramData[1000];
char *processes = NULL;

/*
* Gets the cpu type of the pc
*/
void getCPUName(){
  FILE *cpuInfoFile = fopen("/proc/cpuinfo", "r");
  size_t len = 0;
  ssize_t read;

  if(cpuInfoFile == NULL)
    exit(CPU_FILE_NOT_FOUND);

  while((read = getline(&cpuName, &len, cpuInfoFile)) != EOF){
    if(strstr(cpuName, "model name")){
      break;
    }
  }

  fclose(cpuInfoFile);
}

/*
* gets the current user of the pc
*/
void getUserName(){
  FILE *fp;
  char out[10];

  fp = popen("whoami", "r");
  if (fp == NULL) {
    exit(WHOAMI_CMD_FAILED);
  }

  while (fgets(out, sizeof(out), fp) != NULL) {
    if(!strncat(userName, out, 50)){
      exit(USERNAME_TOO_LONG);
    }
  }

  pclose(fp);
}

/*
* gets the ram data, including memory.
*/
void getRamData(){
  FILE *fp;
  char out[1000];
  
  ramData[0] = '\0';

  fp = popen("free -h --si", "r");
  if (fp == NULL) {
    exit(FREE_COMMAND_NOT_FOUND);
  }

  while (fgets(out, sizeof(out), fp) != NULL) {
    if(!strncat(ramData, out, 1000)){
      exit(RAM_DATA_TOO_LONG);
    }
  }

  pclose(fp);
}

/*
* get the current running processes
*/
void getProcesses(){
  FILE *fp;
  char out[1000];
  processes = calloc(sizeof(char), 1000);

  fp = popen("ps -A", "r");
  if (fp == NULL) {
    exit(PS_CMD_NOT_FOUND);
  }

  while (fgets(out, sizeof(out), fp) != NULL) {
    if(!strncat(processes, out, strlen(out))){
      if(strlen(processes) - strlen(out) <= 200){
        processes = realloc(processes, strlen(processes)+500*sizeof(char));
        if(processes == NULL){
          exit(OUT_OF_MEMORY);
        }
      }
    }
  }

  pclose(fp);
}

/*
gets the cpu parameters for the calculation of the cpu usage percentage.
*/
float getCPUUsage(){
  FILE *fp;
  char out[50];
  float avg = 0;
  char *token;


  fp = popen("cat /proc/uptime", "r");
  if (fp == NULL) {
    exit(NON_LINUX_OS);
  }

  while (fgets(out, sizeof(out), fp) != NULL);
  token = strtok(out, " ");
  while(token != NULL){
    avg += atof(token);
    token = strtok(NULL, " ");
  }
  avg /= 2;

  pclose(fp);
  return avg;
}


