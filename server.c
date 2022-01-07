#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

struct Server {
  char name[10];
  int isOn;
  int id;
} THIS_SERVER;

struct Server getServerData() {
  struct Server newServer;
  char serverNumber[1];
  int inInt;

  printf("Podaj numer servera (od 0 do 9): ");
  scanf(" %c", serverNumber);
  printf("\n");
  strcpy(newServer.name,"ser");
  strcat(newServer.name,serverNumber);

  printf("Czy server ma byc wlaczony (1 - tak, 0 - nie): ");
  scanf("%d",&inInt);
  printf("\n");
  newServer.isOn = inInt;

  printf("Podaj klucz identyfikujacy server: ");
  scanf("%d",&inInt);
  printf("\n");
  newServer.id = inInt;

  return newServer;
}

struct Server checkIfUniq(struct Server currentServer,struct Server *serverList) {
  int i;
  for (i = 0; i < 5; i++) {
    if(strcmp(serverList[i].name,currentServer.name) == 0 || serverList[i].id == currentServer.id) {
      printf("Serwer posiada powtarzajaca sie nazwe lub id\n");
      currentServer = getServerData();
      i = -1;
    }
  }

  return currentServer;
}

void addServerToList(struct Server *serverList) {
  struct Server newServer = checkIfUniq(getServerData(), serverList);

  int i;
  for (i = 0; i < 5; i++) {

    if(serverList[i].name[0] != 's' || serverList[i].name[1] != 'e' || serverList[i].name[2] != 'r') {
      serverList[i] = newServer;
      THIS_SERVER = newServer;
      break;
    }
    
    if(i == 4) {
      printf("Lista serwerow jest pelna\n");
    }
  }
}

void printServerData(struct Server server) {
  printf("Name: %s\nIs Online: %d\nId: %d\n",server.name,server.isOn,server.id);
}

int main() {
  int shmId = shmget(12345678,8,0644|IPC_CREAT);
  struct Server *serverList = (struct Server *)shmat(shmId,NULL,0);

  addServerToList(serverList);

  printServerData(THIS_SERVER);
  
  return 0;
}