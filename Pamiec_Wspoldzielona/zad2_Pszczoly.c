#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int shm = shmget(123456, 4, 0644|IPC_CREAT);
  int *honey = (int *)shmat(shm, NULL, 0);

  printf("Bruh\n");

  if (fork() == 0) {
    int honeyMade = 0;
    int workTime = 0;

    int lower = 3;
    int upper = 15;
    while(1) {
      honeyMade = (rand() % (upper - lower + 1)) + lower;
      workTime = (rand() % (upper - lower + 1)) + lower;

      *honey += honeyMade;

      printf("Bee_1: Made %d of Honey and goes to work for %d seconds\tHoney in hive: %d\n",honeyMade,workTime,*honey);
      sleep(workTime);
    }
  } else {
    sleep(1);
    int honeyMade = 0;
    int workTime = 0;

    int lower = 1;
    int upper = 10;
    while(1) {
      honeyMade = (rand() % (upper - lower + 1)) + lower;
      workTime = (rand() % (upper - lower + 1)) + lower;

      *honey += honeyMade;

      printf("Bee_2: Made %d of Honey and goes to work for %d seconds\tHoney in hive: %d\n",honeyMade,workTime,*honey);
      sleep(workTime);
    }
  }

  return 0;
}