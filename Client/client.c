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



// int isAlphaOnly(char* string) {
//   for(int i = 0; i < length(string); i++) {
//     if(!isalpha(string[i])) {
//       return 0;
//     }
//   }
//   return 1;
// }

// int nameLengthAndAlphaOnlyCheck(char* string, int maxNumOfChar) {
//   int check = 0;
//   if(length(string) > maxNumOfChar) {
//     printf("Max 19 znakow!\n");
//     check++;
//   }

//   if(!isAlphaOnly(string)) {
//     printf("Tylko znaki alfabetu lacinskiego!\n");
//     check++;
//   }

//   return check == 0;
// }

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

int userKey() {
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
  int clientKey = userKey();

  int clientKeyId = msgget(clientKey, 0644|IPC_CREAT);

  connect.clientKeyId = clientKeyId;

  printConnectMsg(connect);

  msgsnd(KEY, &connect, sizeof(connect),0);
  printf("Wyslano request\n");

  return clientKeyId;
}

int main() {
  KEY = msgget(123456789,0644|IPC_CREAT);
  int clientKeyId = connectToServer();

  struct Msg message;

  msgrcv(clientKeyId, &message, sizeof(message), 1, 0);
  printf("%s", message.message);

  while(1){
    printf("[0] Wyjdz\nCo chcesz zrobic: ");
    int operation;
    scanf("%d",&operation);
    switch (operation) {
      case 0:
        if(disconnectFromServer(clientKeyId) == 0) {
          printf("Odlaczono od serwera\n");
        }
        exit(0);
        break;
    }
  }


  return 0;
}
