#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "client.h"
#include "getters.h"
#include "stringOperations.h"

/*
  [TODO] implement err_n_die function
  [TODO] parse and send processes to the webserver
*/

/* returns an object containing all needed data */
UserData *getUserData(){
  UserData *data = malloc(sizeof(UserData));

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

/* returns the CPU's model name */
char *getCPUName(){
  FILE *cpuInfoFile = fopen("/proc/cpuinfo", "r");
  char *cpuName = (char *) calloc(100, sizeof(char));
  char *result;

  bool isCPUName = false;

  if(cpuInfoFile == NULL)
    exit(CPU_FILE_NOT_FOUND);
  
  while (fgets(cpuName, 100, cpuInfoFile) != NULL) {
    if(strstr(cpuName, "model name")){
      break;
    }
  }

  cpuName[strlen(cpuName)-1] = '\0';

  result = (char *) calloc(strlen(cpuName), sizeof(char));
  memset(result, '\0', strlen(cpuName));

  /* parsing the cpu name out of the string */
  for(int i=0; i < strlen(cpuName); i++){
    if(cpuName[i] == ':'){
      isCPUName = true;
      i++;
      continue;
    }

    if(isCPUName)
      strncat(result, &cpuName[i], 1);
  }

  fclose(cpuInfoFile);
  result[strlen(result)-1] = 0;



  return result;
}

/* returns the machine's name */
char *getUserName(){
  FILE *fp;
  char out[10];
  char *userName = (char *) calloc(1, sizeof(char));


  fp = popen("whoami", "r");
  if (fp == NULL) {
    exit(WHOAMI_CMD_FAILED);
  }

  while (fgets(out, 10, fp) != NULL) {
    if(!strncat(userName, out, 50)){
      exit(USERNAME_TOO_LONG);
    }
  }

  pclose(fp);
  userName[strlen(userName)-1] = '\0'; /* remove the newline character */
  return userName;
}

/* returns a formmated string representing the memory usage and the total ram size in gigabytes */
char *getRamData(){
  FILE *fp;
  bool flag = false;

  char *ramData = (char *) calloc(100, sizeof(char)); /* the output of the command fp */
  char *totalRam = (char *) calloc(10, sizeof(char)); /* the ram of the machine, parsed from ramData */
  char *ramUsage = (char *) calloc(10, sizeof(char)); /* the memory usage of the machine, parsed from ramData */
  char *memoryUsage = (char *) calloc(20, sizeof(char)); /* the final string, formatted nicely `USAGE/RAM (GB)` */

  /* only taking the memory part of this command */
  fp = popen("free -h --si | grep Mem", "r");
  if (fp == NULL) {
    exit(FREE_COMMAND_NOT_FOUND);
  }

  while (fgets(ramData, 100, fp) != NULL);

  /* string parsing, works for every case */
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

  /* final string formmating */
  sprintf(memoryUsage, "%s/%s(GB)", ramUsage, totalRam);
  pclose(fp);

  return memoryUsage;
}

/* returns the running processes on the machine */
char *getProcesses(){
  FILE *fp;
  char out[20];
  char *dup = (char *) calloc(20, sizeof(char)); /* to remove different services but run with the same command */
  char *processes = (char *) calloc(100, sizeof(char));

  memset(dup, '\0', 20);

  fp = popen("ps -A | awk \'{print $4}\'", "r");
  if (fp == NULL) {
    exit(PS_CMD_NOT_FOUND);
  }

  while (fgets(out, 20, fp) != NULL) {
    /* replace the new line character with a space for easier url encoding */
    out[strlen(out)-1] = ' ';

    /* if the previous service and the current service names are the same, don't add to the processes list */
    if(strcmp(dup, out) == 0)
      continue;
    
    /* to avoid the column name of the command ps -A */
    if(strlen(dup) == 0)
      goto set;
    
    if(!strncat(processes, out, strlen(out))){
      if(strlen(processes) - strlen(out) <= 200){
        processes = realloc(processes, strlen(processes)+500*sizeof(char));
        if(processes == NULL){
          exit(OUT_OF_MEMORY);
        }
      }
    }

    set:
      memset(dup, '\0', 20);
      strcat(dup, out);
  }

  /* fence posting */
  processes[strlen(processes)-1] = '\0';
  pclose(fp);

  return processes;
}

/* returns the needed variables in order to calculate the cpu usage of the machine */
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


