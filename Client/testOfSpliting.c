#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <stdlib.h>

int length(char* string) {
  int result = 0;
  while(1) {
    if (string[result] != '\0') {
      result++;
      continue;
    }
    return result - (string[result-1] == '\n');
  }
}

void splitStringIntoTwo(char *toSplit, char *str1, char *str2, char splitBy) {
  int lenOfFirstStr = 0;
  for(int i = 0; i < length(toSplit); i++) {
    if (lenOfFirstStr == 0) {
      if (toSplit[i] == splitBy) {
        lenOfFirstStr = i+1;
      } else {
        str1[i] = toSplit[i];
      }
    } else {
      str2[i - lenOfFirstStr] = toSplit[i];
    }
  }
}

int main() {
  while(1) {
    char *line = NULL;
    size_t len = 0;
    getline(&line,&len,stdin);

    printf("%s - %d\n", line,length(line));
    char str1[20];
    char str2[1024];
    splitStringIntoTwo(line, str1, str2, ' ');

    printf("%s\n",str1);
    printf("%s\n",str2);

  }

  return 0;
}