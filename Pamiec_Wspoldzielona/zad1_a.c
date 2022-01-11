#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

int main() {
  int key =  shmget(12345,7,0644|IPC_CREAT);

  char *pointer = (char*) shmat(key,NULL,0);

  while(1) {
    strcpy(pointer,"haaaa");
    strcpy(pointer,"hoooooo");
  }

  return 0;
}