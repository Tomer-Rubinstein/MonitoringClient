typedef struct{
  char *username;
  char *cpuType;
  char *ram;
  char *processes;
} UserData;

UserData *getUserData();
char *getCPUName();
char *getUserName();
char *getRamData();
char *getProcesses();
float getCPUUsage();

float myAbs(float);
char *encodeStrToUrl(char *);
char *removeSpaces(char *);