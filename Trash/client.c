#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>

struct ConnectMessage {
  long mtype;
  char UserName[50];
  int ConnectionKey;
} THIS_CONNECT_MESSAGE;

struct Message {
  long mtype;
  int userId;
  char message[1024];
  // destination
}

struct Server {
  char name[10];
  int isOn;
  int id;
} CONNECTED_SERVER;

void printServerData(struct Server server) {
  printf("Name: %s\nIs Online: %d\nId: %d\n",server.name,server.isOn,server.id);
}

void getNameOfServer(char name[10]) {
  printf("Podaj nazwe serwera do ktorego chcesz sie polaczyc (serX - gdzie x to numer servera): ");
  scanf(" %s",name);
  printf("\n");
}

void checkIfServerIsOnList(char name[10],struct Server *serverList) {
  int i;
  for (i = 0; i < 5; i++) {
    if(strcmp(name,serverList[i].name) == 0) {
      printf("Jest na liscie\n");
      return;
    }
  }
  printf("Nie ma na liscie\n");
}

int main() {
  int shmId = shmget(12345678,8,0644|IPC_CREAT);
  struct Server *serverList = (struct Server *)shmat(shmId,NULL,0);

  char ConnectTo[10];
  getNameOfServer(ConnectTo);

  checkIfServerIsOnList(ConnectTo, serverList);

  int key = msgget(99999999,0644|IPC_CREAT);
  // THIS_CONNECT_MESSAGE.name
  
  return 0;
}