#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
  int shm = shmget(123456, 4, 0644|IPC_CREAT);
  int *honey = (int *)shmat(shm, NULL, 0);

  *honey = 100;

  return 0;
}