#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>

// struct Server {
//   char name[10];
//   int id;
// } THIS_SERVER;

struct ConnectMsg {
  long mtype;
  char name[20];
  long mtypeClient;
};


struct Client *CLIENT_LIST;
// int NUM_OF_CLIENTS = 0;

struct Client {
  char name[20];
  long mtypeClient;
};

void printClientList() {
  for (int i = 0; i < 5; i++) {
    printf("Name: %s\t mtype: %ld\n",CLIENT_LIST[i].name,CLIENT_LIST[i].mtypeClient);
  }
};

int main() {
  printf("CHUJ 1");
  int key = msgget(123456789,0644|IPC_CREAT);

  printf("CHUJ 2");
  // while(1) {
  //   if (fork() == 0) {

      // Przyjmowanie nowych użytkowników
      // while(1) {

        printf("CHUJ");
        struct ConnectMsg connect;
        msgrcv(key, &connect, sizeof(connect), 1, 0);
        printf("Name: %s\tmtype: %ld",connect.name,connect.mtypeClient);

        // struct Client newClient;
        // strcpy(newClient.name,connect.name);
        // newClient.mtypeClient = connect.mtypeClient;

        // printf("Name: %s\tmtype: %ld",newClient.name,newClient.mtypeClient);

        for (int i = 0 ; i < 5 ; i++) {
          if (isalpha(CLIENT_LIST[i].name[0])) {
            CLIENT_LIST[i] = newClient;
            // NUM_OF_CLIENTS++;
          }
        }

        // printf("Wpisz 1 jesli chcesz zobaczyc liste klientow\n");
        // int x = 0;
        // scanf(" %d", &x);

        // if (x == 1) {
        //   printClientList();
        // }
      // }

    // } else {
    //   // Przesylanie wiadomosci
    // }
  // }

  return 0;
}