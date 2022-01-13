#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <stdlib.h>

int main() {
  if (fork() == 0) {
    while(1) {
      printf("YOo\n");
      sleep(1);
    }
  } else {
    while(1) {
      char msg[1024];
      //getline
      scanf("%s",&msg);
    }
  }
}