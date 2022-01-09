#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <wait.h>

int KEY;

struct Room{
  int id;
  long arrayOfClients[5];
} ROOM_LIST[5] = {
  {0,{}},
  {1,{}},
  {2,{}},
  {3,{}},
  {4,{}}
};

struct ConnectMsg {
  long mtype;
  char name[19];
  long mtypeClient;
};

struct Client {
  char name[19];
  long mtypeClient;
} CLIENT_LIST[5] = {
  {"0",0},
  {"0",0},
  {"0",0},
  {"0",0},
  {"0",0}
};

struct Msg {
  long mtype;
  char message[1024];
};

// struct nameChangeMsg {
//   long mtype;
  
// }

void printClient(struct Client toPrint) {
  printf("%s\t%ld\n",toPrint.name,toPrint.mtypeClient);
}

void printClientList() {
  printf("Aktualna lista klientow: \n");
  for(int i = 0; i < 5; i++) {
    printClient(CLIENT_LIST[i]);
  }
}

void sendConnectionEstablishedMsg(long mtypeClient) {
  struct Msg message;

  message.mtype = mtypeClient;
  strcpy(message.message,"Poloczono z serwerem\n");

  msgsnd(KEY, &message, sizeof(message), 0);
  printf("Wyslano wiadomosc do klienta o mtype: %ld\n",mtypeClient);
}

void disconnectClient() {
  struct ConnectMsg disconnect;
  msgrcv(KEY, &disconnect, sizeof(disconnect), 2, 0);

  for (int i = 0; i < sizeof(CLIENT_LIST)/sizeof(CLIENT_LIST[0]);i++) {
    if (CLIENT_LIST[i].mtypeClient == disconnect.mtypeClient) {
      strcmp(CLIENT_LIST[i].name,"0");
      CLIENT_LIST[i].mtypeClient = 0;
      break;
    }
  }
  printf("Klient %ld oposcil serwer\n",disconnect.mtypeClient);
}

// void sendUniqNameReq(long mtypeClient) {
//   struct Msg message;

//   message.mtype = mtypeClient;
//   strcpy(message.message,"Poloczono z serwerem\n");

//   msgsnd(KEY, &message, sizeof(message), 0);
//   printf("Wyslano wiadomosc do klienta o mtype: %ld\n",mtypeClient);
// }

int checkIfNameUniq(char* name){
  for(int i=0;i<sizeof(CLIENT_LIST)/sizeof(CLIENT_LIST[0]);i++){
    if(strcmp(name,CLIENT_LIST[i].name) == 0){
      return 0;
    }
  }
  return 1;
}

int checkIfmTypeUniq(long mtype) {
  for(int i=0;i<sizeof(CLIENT_LIST)/sizeof(CLIENT_LIST[0]);i++){
    if(CLIENT_LIST[i].mtypeClient == mtype){
      return 0;
    }
  }
  return 1;
}

int checkIfClientIsValid(struct Client newClient) {
  int check = 0;
  if (!checkIfNameUniq(newClient.name)) {
    printf("Nazwa %s sie powtarza\n",newClient.name);

    check++;
  }

  if (!checkIfmTypeUniq(newClient.mtypeClient)) {
    printf("mType %ld sie powtarza\n",newClient.mtypeClient);
    check++;
  }

  return check == 0;
}

void addClient() {
  struct ConnectMsg connect;
  msgrcv(KEY, &connect, sizeof(connect), 1, 0);

  struct Client newClient;
  strcpy(newClient.name,connect.name);
  newClient.mtypeClient = connect.mtypeClient;

  checkIfClientIsValid(newClient);

  //printClient(newClient);

  for(int i = 0; i < 5; i++) {
    if (strcmp(CLIENT_LIST[i].name,"0") == 0) {
      CLIENT_LIST[i] = newClient;
      sendConnectionEstablishedMsg(newClient.mtypeClient);
      break;
    }
  }
}

int main() {
  KEY = msgget(123456789,0644|IPC_CREAT);

  while(1) {
    if (fork() == 0) {
      disconnectClient();
    } else {
      addClient();
    }

    printf("Wpisz 1 \n");
    int x;
    scanf("%d",&x);
    if (fork() == 0) {
      execlp("clear","clear", NULL);
    } else {
      wait(NULL);
      printClientList();
    }
  }
  
  return 0;
}