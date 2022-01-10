#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <stdlib.h>

int KEY;

struct ConnectMsg {
  long mtype;
  char name[20];
  int clientKeyId;
};

struct Msg {
  long mtype;
  char message[1024];
};

int length(char* string) {
  int result = 0;
  while(1) {
    if (string[result] != '\0') {
      result++;
      continue;
    }
    return result;
  }
}

void printConnectMsg(struct ConnectMsg toPrint) {
  printf("mtype: %ld\tname: %s\tclientKey: %d\n",toPrint.mtype,toPrint.name,toPrint.clientKeyId);
}

int disconnectFromServer(int clientKey) {
  struct ConnectMsg dissconect;


  dissconect.mtype = 2;
  dissconect.clientKeyId = clientKey;

  return msgsnd(KEY, &dissconect, sizeof(dissconect), 0);
}

void getUserName(char* s) {
  char name[20];
  printf("Podaj nazwe uzytkownika \n");
  scanf("%s", name);
  if(length(name) > 20) {
    getUserName(name);
  }
  name[20] = '\0';
  strcpy(s,name);
}

int getUserKey() {
  int lower = 10;
  int upper = 300000;
  time_t time;
  int key = 0;

  key = (((random() % (upper - lower + 1)) + lower) * time);

  if (key < 0) {
    return key * -1;
  } else {
    return key;
  }
}

int connectToServer() {
  struct ConnectMsg connect;

  char name[20];
  getUserName(name);

  connect.mtype = 1;
  strcpy(connect.name,name);
  int clientKey = getUserKey();

  int clientKeyId = msgget(clientKey, 0644|IPC_CREAT);

  connect.clientKeyId = clientKeyId;

  printConnectMsg(connect);

  msgsnd(KEY, &connect, sizeof(connect),0);
  printf("Wyslano request\n");

  return clientKeyId;
}

void loggedInMenu() {
  KEY = msgget(1234567890,0644|IPC_CREAT);
  int clientKeyId = connectToServer();

  struct Msg message;

  while(1) {
    if (msgrcv(clientKeyId, &message, sizeof(message), 1, IPC_NOWAIT) != -1) {
      break;
    }
    if (msgrcv(clientKeyId, &message, sizeof(message), 2, IPC_NOWAIT) != -1) {
      struct ConnectMsg connect;
      printf("Podana nazwa się powtarza!\n");
      char name[20];
      getUserName(name);
      connect.mtype = 1;
      strcpy(connect.name,name);
      connect.clientKeyId = clientKeyId;
      msgsnd(KEY, &connect, sizeof(connect),0);
    }
  }

  printf("%s", message.message);

  while(1){
    printf("[9] Wyloguj sie\n[0] Wyjdz\nCo chcesz zrobic: ");
    int operation;
    scanf("%d",&operation);
    switch (operation) {
      case 9:
        if(disconnectFromServer(clientKeyId) == 0) {
          sleep(1);
          printf("Odlaczono od serwera clientKeyId: %d\n",clientKeyId);
          // msgctl(clientKeyId, IPC_RMID,&message);
        }
        return;
      case 0:
        if(disconnectFromServer(clientKeyId) == 0) {
          sleep(1);
          printf("Odlaczono od serwera clientKeyId: %d\n",clientKeyId);
          // msgctl(clientKeyId, IPC_RMID,&message);
        }
        exit(0);
        break;
    }
  }
}

int main() {
  while(1){
    printf("[1] Zaloguj sie\n[0] Wyjdz\nCo chcesz zrobic: ");
    int operation;
    scanf("%d",&operation);
    switch (operation) {
      case 1:
        loggedInMenu();
        break;
      case 0:
        printf("Wychodznie z programu\n");
        exit(0);
        break;
    }
  }
  return 0;
}
