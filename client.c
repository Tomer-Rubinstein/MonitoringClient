#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "getters.h"
#include "postData.h"
#include "stringOperations.h"

int main() {
  float prevCpuUsage = getCPUUsage();
  float currCpu = 0;

  UserData *user = getUserData();

  postRequest(encodeStrToUrl(user->cpuType), user->username, user->ram, encodeStrToUrl(user->processes), currCpu);

  /* sending live data every 5 seconds */
  /* current cpu percentage & current ram available */
  while(1){ 
    /* iteration initialization */
    user = getUserData();
    sleep(5); /* sleep 5 second between each post request */
    
    /* calculating the current CPU percentage */
    currCpu = myAbs(getCPUUsage()-prevCpuUsage);
    printf("\t\t\t%.1f\n", currCpu);
    postRequest(encodeStrToUrl(user->cpuType), user->username, user->ram, encodeStrToUrl(user->processes), currCpu);

    /* used in the next iteration for calculating the cpu percentage */
    /* in the next 5 seconds */
    prevCpuUsage = getCPUUsage();
  }

  return 0;
}

