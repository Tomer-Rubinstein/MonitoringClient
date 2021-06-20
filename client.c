#include "getters.c"
#include "postData.c"

float myAbs(float);
char *encodeStrToUrl(char *);
char *removeSpaces(char *);

int main() {
  float prevCpuUsage = getCPUUsage();
  float currCpu = 0;
  char *token;

  /* getting pc's information */
  getUserName();
  getCPUName();
  getRamData();
  getProcesses();
  printf("%s\n%s\n%s\n%s\n", cpuName, userName, ramData, processes);

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

  printf("user: %s\n", userName);
  strtok(userName, "\n");
  printf("after user: %s\n", userName);

  postRequest(cpuName, userName, ramData, processes, currCpu);

  /* sending live data every 5 seconds */
  /* current cpu percentage & current ram available */
  while(1){
    /* iteration initialization */
    sleep(5);

    /* updating the ram data */
    getRamData();
    token = NULL;
    token = strtok(ramData, "Mem:");
    ramData[0] = '\0';
    strcat(ramData, removeSpaces(token));
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


  printf("Bye bye!\n\n");
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
