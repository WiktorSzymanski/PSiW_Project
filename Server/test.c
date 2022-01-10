#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <wait.h>

struct ConnectMsg {
  long mtype;
  char name[20];
  int clientKeyId;
} connect;

void main() {
  int key = msgget(123456789,0644|IPC_CREAT);
  while (1) {
    int catchData = msgrcv(key, &connect, sizeof(connect), 1, IPC_NOWAIT);
    if (catchData != -1) {
      printf("mtype: %ld\tname: %s\tkeyId: %d\n",connect.mtype, connect.name, connect.clientKeyId);
    } else {
      printf("NOPE\n");
    }
    sleep(1);
  }
}