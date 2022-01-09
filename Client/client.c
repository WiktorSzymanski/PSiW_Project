#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <stdlib.h>

struct ConnectMsg {
  long mtype;
  char name[20];
  long mtypeClient;
};

// int length(char* string) {
//   int result = 0;
//   while(1) {
//     if (string[result] != '\0') {
//       result++;
//       continue;
//     }
//     return result;
//   }
// }

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

void getUserName(char* s) {
  char name[20];
  printf("Podaj nazwe uzytkownika \n");
  scanf("%s", name);
  if(!nameLengthAndAlphaOnlyCheck(name,19)) {
    getUserName(name);
  }
  name[20] = '\0';
  strcpy(s,name);
}

long userKey(char *string) {
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

void connectToServer(int key) {
  struct ConnectMsg connect;

  char name[20];
  getUserName(name);

  connect.mtype = 1;
  strcpy(connect.name,name);
  connect.mtypeClient = userKey(name);

  printConnectMsg(connect);

  msgsnd(key, &connect, sizeof(connect),0);
  printf("Wyslano request\n");
}

int main() {
  int key = msgget(123456789,0644|IPC_CREAT);
  connectToServer(key);

  return 0;
}