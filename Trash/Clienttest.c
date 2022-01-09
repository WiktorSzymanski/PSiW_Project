#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

int length(char* string) {
  int result = 0;
  while(1) {
    if (string[result] != '\0') {
      result++;
      continue;
    }
    return result;
  }
}

long userKey(char *string) {
  int lower = 10;
  int upper = 300000;
  time_t time;
  long key = 0;

  key = (((rand() % (upper - lower + 1)) + lower) * time);

  return key;
}

void main() {
  char name[20] = "asdasf";
  name[20] = '\0';
  printf("length: %d\n",length(name));
  printf("mtype: %ld\n",userKey(name));
}