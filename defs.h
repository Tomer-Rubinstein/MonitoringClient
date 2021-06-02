#include "client.h"

#include "getters.c"
#include "postData.c"



void getCPUName();
void getUserName();
void getRamData();
void getProcesses();
float getCPUUsage();

int postRequest(char *cpuName, char *userName, char *ramData, char *processes, float cpuUsage);
