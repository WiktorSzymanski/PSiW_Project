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
  long mtypeClient;
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
  printf("mtype: %ld\tname: %s\tmtypeClient: %ld\n",toPrint.mtype,toPrint.name,toPrint.mtypeClient);
}

int disconnectFromServer(long mtypeClient) {
  struct ConnectMsg dissconect;

  dissconect.mtype = 2;
  dissconect.mtypeClient = mtypeClient;

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

long userKey() {
  int lower = 10;
  int upper = 300000;
  time_t time;
  long key = 0;

  key = (((random() % (upper - lower + 1)) + lower) * time);

  if (key < 0) {
    return key * -1;
  } else {
    return key;
  }
}

long connectToServer() {
  struct ConnectMsg connect;

  char name[20];
  getUserName(name);

  connect.mtype = 1;
  strcpy(connect.name,name);
  long mtypeClient = userKey();
  connect.mtypeClient = mtypeClient;

  printConnectMsg(connect);

  msgsnd(KEY, &connect, sizeof(connect),0);
  printf("Wyslano request\n");

  return mtypeClient;
}

int main() {
  KEY = msgget(123456789,0644|IPC_CREAT);
  long mtypeClient = connectToServer();

  struct Msg message;

  msgrcv(KEY, &message, sizeof(message), mtypeClient, 0);
  printf("%s", message.message);

  while(1){
    printf("[0] Wyjdz\nCo chcesz zrobic: ");
    int operation;
    scanf("%d",&operation);
    switch (operation) {
      case 0:
        if(disconnectFromServer(mtypeClient) == 0) {
          printf("Odlaczono od serwera\n");
        }
        exit(0);
        break;
    }
  }


  return 0;
}
