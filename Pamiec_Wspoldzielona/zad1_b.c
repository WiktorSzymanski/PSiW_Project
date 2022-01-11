#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

int main() {
  int key =  shmget(12345,7,0644|IPC_CREAT);

  char *pointer = (char*) shmat(key,NULL,0);

  while(1) {
    printf("%s",pointer);
    if (strcmp(pointer,"haaaa") != 0 && strcmp(pointer,"hoooooo") != 0) {
      printf("\t\t ERROR - wartosc bledna\n");
    } else {
      printf("\n");
    }
  }

  return 0;
}