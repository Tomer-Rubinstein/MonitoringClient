/*
* All functions written in this file aren't getters
* since they do not return anything,
* they set the values of their variables instead.
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "client.h"
#include "getters.h"
#include "stringOperations.h"

UserData *getUserData(){
  UserData *data = calloc(1, sizeof(UserData));

  char *cpuType = getCPUName();
  char *username = getUserName();
  char *ram = getRamData();
  char *procs = getProcesses();

  data->ram = (char *) calloc(strlen(ram), sizeof(char));
  strcat(data->ram, ram);

  data->username = (char *) calloc(strlen(username), sizeof(char));
  strcat(data->username, username);


  data->cpuType = (char *) calloc(strlen(cpuType), sizeof(char));
  strcat(data->cpuType, cpuType);

  data->processes = (char *) calloc(strlen(procs), sizeof(char));
  strcat(data->processes, procs);

  return data;
}

/*
* Gets the cpu type of the pc
*/
char *getCPUName(){
  FILE *cpuInfoFile = fopen("/proc/cpuinfo", "r");
  char *cpuName = NULL;

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
  return cpuName;
}

/*
* gets the current user of the pc
*/
char *getUserName(){
  FILE *fp;
  char out[10];
  char *userName = (char *) calloc(1, sizeof(char));


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
  return userName;
}

/*
* gets the ram data, including memory.
*/
char *getRamData(){
  FILE *fp;
  bool flag = false;

  char *ramData = (char *) calloc(1000, sizeof(char));
  memset(ramData, '\0', 1000);

  char *totalRam = (char *) calloc(10, sizeof(char));
  char *ramUsage = (char *) calloc(10, sizeof(char));

  char *memoryUsage = (char *) calloc(20, sizeof(char));

  fp = popen("free -h --si | grep Mem", "r");
  if (fp == NULL) {
    exit(FREE_COMMAND_NOT_FOUND);
  }

  while (fgets(ramData, 1000, fp) != NULL);
  memset(ramData, ' ', 16);
  ramData = removeSpaces(ramData);

  for(int i=0; i < strlen(ramData); i++){
    if(ramData[i] == 'G'){
      flag = true;
      continue;
    }

    if(flag){
      strncat(ramUsage, &ramData[i], 1);
      if(i+1 < strlen(ramData) && ramData[i+1] == 'G')
        break;
    }else{
      strncat(totalRam, &ramData[i], 1);
    }
  }


  sprintf(memoryUsage, "%s/%s (GB)", ramUsage, totalRam);
  pclose(fp);
  return memoryUsage;
}

/*
* get the current running processes
*/
char *getProcesses(){
  FILE *fp;
  char out[1000];
  char *processes = (char *) calloc(1000, sizeof(char));

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
  return processes;
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


