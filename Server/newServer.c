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
  char name[19];
  long mtypeClient;
};

struct Client {
  char name[19];
  long mtypeClient;
} CLIENT_LIST[5] = {
  {"0",1},
  {"0",1},
  {"0",1},
  {"0",1},
  {"0",1},
};

void printClient(struct Client toPrint) {
  printf("%s\t%ld\n",toPrint.name,toPrint.mtypeClient);
}

void printClientList() {
  printf("Aktualna lista klientow: \n");
  for(int i = 0; i < 5; i++) {
    printClient(CLIENT_LIST[i]);
  }
}

void addClient(int key) {
  struct ConnectMsg connect;
  msgrcv(key, &connect, sizeof(connect), 1, 0);

  struct Client newClient;
  strcpy(newClient.name,connect.name);
  newClient.mtypeClient = connect.mtypeClient;

  //printClient(newClient);

  for(int i = 0; i < 5; i++) {
    if (strcmp(CLIENT_LIST[i].name,"0") == 0) {
      CLIENT_LIST[i] = newClient;
      break;
    }
  }
}

int main() {

  int key = msgget(123456789,0644|IPC_CREAT);

  while(1) {
    addClient(key);

    if (fork() == 0) {
      execlp("clear","clear", NULL);
    } else {
      wait(NULL);
      printClientList();
    }
  }
  
  return 0;
}