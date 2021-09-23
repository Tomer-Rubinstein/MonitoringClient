#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "getters.h"
#include "postData.h"
#include "stringOperations.h"

/*
  [TODO] implement err_n_die function
  [TODO] refactor code
*/

int main() {
  float prevCpuUsage = getCPUUsage();
  float currCpu = 0;

  UserData *user;

  /* sending live data every 5 seconds */
  while(1){ 
    user = getUserData();
    
    /* calculating the current CPU percentage */
    currCpu = myAbs(getCPUUsage()-prevCpuUsage);
    postRequest(encodeStrToUrl(user->cpuType), user->username, user->ram, encodeStrToUrl(user->processes), currCpu);

    /* 
      used in the next iteration for calculating the cpu percentage
      in the next 5 seconds
    */
    prevCpuUsage = getCPUUsage();
    
    sleep(5); /* sleep 5 second between each post request */
  }

  return 0;
}

