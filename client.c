#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "getters.h"
#include "postData.h"

int main() {
  float prevCpuUsage = getCPUUsage();
  float currCpu = 0;
  char *token;

  UserData *user;
  user = getUserData();

  char *cpuName = user->cpuType;
  char *ramData = user->ram;
  char *processes = user->processes;
  char *userName = user->username;


  printf("%s\n%s\n%s\n%s\n", user->cpuType, user->username, user->ram, user->processes);

  /* parsing the cpu name and encoding spaces to %20 */
  token = strtok(cpuName, ":");
  token = strtok(NULL, ":");
  cpuName = token;
  cpuName = encodeStrToUrl(cpuName);

  /* parsing the ram */
  token = NULL;
  token = strtok(ramData, "Mem:");
  ramData = removeSpaces(token);
  token = NULL;
  token = strtok(ramData, "G");
  strcat(ramData, "G");

  strtok(userName, "\n");

  postRequest(cpuName, userName, ramData, processes, currCpu);

  /* sending live data every 5 seconds */
  /* current cpu percentage & current ram available */
  while(1){
    /* iteration initialization */
    sleep(5);

    /* updating the ram data */
    token = NULL;
    token = strtok(ramData, "Mem:");
    ramData = removeSpaces(token);
    token = NULL;
    token = strtok(ramData, "G");
    strcat(ramData, "G");
    
    /* calculating the current CPU percentage */
    currCpu = myAbs(getCPUUsage()-prevCpuUsage);
    printf("\t\t\t%.1f\n", currCpu);
    postRequest(cpuName, userName, ramData, processes, currCpu);

    /* used in the next iteration for calculating the cpu percentage */
    /* in the next 5 seconds */
    prevCpuUsage = getCPUUsage();
  }

  return 0;
}

