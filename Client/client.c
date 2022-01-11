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
int CLIENT_KEY_ID;

struct ConnectMsg {
  long mtype;
  char name[20];
  int clientKeyId;
};

struct Msg {
  long mtype;
  char message[1024];
  int clientKeyId;
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

int disconnectFromServer() {
  struct ConnectMsg dissconect;


  dissconect.mtype = 2;
  dissconect.clientKeyId = CLIENT_KEY_ID;

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

void getClientList(){
  struct Msg listRequest;
  listRequest.mtype = 3;
  listRequest.clientKeyId = CLIENT_KEY_ID;
  strcpy(listRequest.message, "Daj prosze liste uzytkownikow");
  msgsnd(KEY,&listRequest, sizeof(listRequest),0);
  msgrcv(CLIENT_KEY_ID, &listRequest, sizeof(listRequest), 3, 0);
  printf("%s",listRequest.message);
}

void getChannelList(){
  struct Msg listRequest;
  listRequest.mtype = 4;
  listRequest.clientKeyId = CLIENT_KEY_ID;
  strcpy(listRequest.message, "Daj prosze liste kanalow");
  msgsnd(KEY,&listRequest, sizeof(listRequest),0);
  msgrcv(CLIENT_KEY_ID, &listRequest, sizeof(listRequest), 4, 0);
  printf("%s",listRequest.message);
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
  CLIENT_KEY_ID = connectToServer();

  struct Msg message;

  while(1) {
    if (msgrcv(CLIENT_KEY_ID, &message, sizeof(message), 1, IPC_NOWAIT) != -1) {
      break;
    }
    if (msgrcv(CLIENT_KEY_ID, &message, sizeof(message), 2, IPC_NOWAIT) != -1) {
      struct ConnectMsg connect;
      printf("Podana nazwa się powtarza!\n");
      char name[20];
      getUserName(name);
      connect.mtype = 1;
      strcpy(connect.name,name);
      connect.clientKeyId = CLIENT_KEY_ID;
      msgsnd(KEY, &connect, sizeof(connect),0);
    }
  }

  printf("%s", message.message);

  while(1){
    printf("[1] Wyswietl liste kanalow\n[2] Wyswietl liste uzytkownikow\n[9] Wyloguj sie\n[0] Wyjdz\nCo chcesz zrobic: ");
    int operation;
    scanf("%d",&operation);
    switch (operation) {
      case 1:
        printf("Lista kanalow: \n");
        getChannelList();
        break;
      break;
      case 9:
        if(disconnectFromServer() == 0) {
          sleep(1);
          printf("Odlaczono od serwera clientKeyId: %d\n",CLIENT_KEY_ID);
          msgctl(CLIENT_KEY_ID, IPC_RMID,&message);
        }
        return;
      case 0:
        if(disconnectFromServer() == 0) {
          sleep(1);
          printf("Odlaczono od serwera clientKeyId: %d\n",CLIENT_KEY_ID);
          msgctl(CLIENT_KEY_ID, IPC_RMID,&message);
        }
        exit(0);
        break;
      case 2:
        printf("Lista uzytkownikow: \n");
        getClientList();
        break;
    }
  }
}

int main() {
  while(1){
    printf("[0] Wyjdz\n[1] Zaloguj sie\nCo chcesz zrobic: ");
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