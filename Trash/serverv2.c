#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>

long clientList[5];
char clientNameList[5][20];

struct ConnectMsg {
  long mtype;
  char name[20];
  long Client;
};


struct Client *CLIENT_LIST;
// int NUM_OF_CLIENTS = 0;

struct Client {
  char name[20];
  long mtypeClient;
};

void printClientList() {
  for (int i = 0; i < 5; i++) {
    printf("id: %d name:%s mtype: %ld\n",i,clientNameList[i], clientList[i]);
  }
}

int main() {
    int control = -1;
    int key = msgget(123456789,0644|IPC_CREAT);

    while(control<0 || control >10){
        struct ConnectMsg connect;
        msgrcv(key, &connect, sizeof(connect)+1, 1, 0);
        printf("\nName: %s\tmtype: %ld\n",connect.name,connect.Client);
        struct Client newClient;
        strcpy(newClient.name,connect.name);
        newClient.mtypeClient = connect.Client;

        printf("Name: %s\tmtype: %ld",newClient.name,newClient.mtypeClient);

        for (int i = 0 ; i < 5 ; i++) {
          if (clientList[i]==0) {
              clientList[i]=newClient.mtypeClient;
              strcpy(clientNameList[i],newClient.name);
              break;
          }
        }

        printf("Wpisz 1 jesli chcesz zobaczyc liste klientow\n");
        int x = 0;
        scanf(" %d", &x);

        if (x == 1) {
          printClientList();
        }
       
       scanf(" %d", &control);
    }

  return 0;
}