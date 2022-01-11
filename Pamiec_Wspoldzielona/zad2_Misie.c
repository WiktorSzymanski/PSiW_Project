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
    int honeyEaten = 0;
    int sleepTime = 0;

    int lower = 8;
    int upper = 30;
    while(1) {
      honeyEaten = (rand() % (upper - lower + 1)) + lower;
      sleepTime = (rand() % (upper - lower + 1)) + lower;

      if (*honey < honeyEaten) {
        honeyEaten = *honey;
        *honey = 0;
      } else {
        *honey -= honeyEaten;
      }

      printf("Bear_1: Eaten %d of Honey and goes to sleep for %d seconds\tHoney in hive: %d\n",honeyEaten,sleepTime,*honey);
      sleep(sleepTime);
    }
  } else {
    sleep(1);
    int honeyEaten = 0;
    int sleepTime = 0;

    int lower = 5;
    int upper = 25;
    while(1) {
      honeyEaten = (rand() % (upper - lower + 1)) + lower;
      sleepTime = (rand() % (upper - lower + 1)) + lower;

      if (*honey < honeyEaten) {
        honeyEaten = *honey;
        *honey = 0;
      } else {
        *honey -= honeyEaten;
      }

      printf("Bear_2: Eaten %d of Honey and goes to sleep for %d seconds\tHoney in hive: %d\n",honeyEaten,sleepTime,*honey);
      sleep(sleepTime);
    }
  }

  return 0;
}