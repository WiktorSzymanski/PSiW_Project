#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

struct Server {
  char name[10];
  int isOn;
  int id;
};

int main() {
  int shmId = shmget(12345678,8,0644|IPC_CREAT);
  struct Server *serverList = (struct Server *)shmat(shmId,NULL,0);


  int i;
  for (i = 0; i < 5; i++) {
    printf("name: %s\tis on: %d\tserver id: %d\n",serverList[i].name,serverList[i].isOn,serverList[i].id);
  }

  printf("Wpisz 1 by wyczyscic liste serwereow: ");
  scanf("%d",&i);
  printf("\n");

  if (i == 1) {
    struct Server clearServer;
    strcpy(clearServer.name,"");
    clearServer.isOn = -1;
    clearServer.id = -1;

    for (i = 0; i < 5; i++) {
      serverList[i] = clearServer;
    }

    printf("Wyczyszczono liste serwerow\n");
  }

  return 0;
}