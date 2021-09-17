#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  char *newStr = (char *) malloc(strlen(str) * sizeof(char));
  int i;

  memset(newStr, '\0', strlen(str));
  for(i=0; i < strlen(str); i++){
    if(i != 0){
      if(str[i] != ' ')
        strncat(newStr, &str[i], 1);
    }
  }
  return newStr;
}