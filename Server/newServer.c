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
struct Server *SERVER_LIST;

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
};

struct Server {
  char name[10];
  int id;
  struct Client clientList[5];
} THIS_SERVER;

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
    printClient(SERVER_LIST[0].clientList[i]);
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

  for (int i = 0; i < sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]);i++) {
    if (SERVER_LIST[0].clientList[i].mtypeClient == disconnect.mtypeClient) {
      strcpy(SERVER_LIST[0].clientList[i].name,"");
      SERVER_LIST[0].clientList[i].mtypeClient = 0;
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
  for(int i=0;i<sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]);i++){
    if(strcmp(name,SERVER_LIST[0].clientList[i].name) == 0){
      return 0;
    }
  }
  return 1;
}

int checkIfmTypeUniq(long mtype) {
  for(int i=0;i<sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]);i++){
    if(SERVER_LIST[0].clientList[i].mtypeClient == mtype){
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

  printClient(newClient);

  for(int i = 0; i < 5; i++) {
    if (strcmp(SERVER_LIST[0].clientList[i].name,"") == 0) {
      SERVER_LIST[0].clientList[i] = newClient;
      sendConnectionEstablishedMsg(newClient.mtypeClient);
      break;
    }
  }
}

int main() {
  KEY = msgget(123456789,0644|IPC_CREAT);
  int shmId = shmget(12345678,8,0644|IPC_CREAT);
  SERVER_LIST = (struct Server *)shmat(shmId,NULL,0);

  SERVER_LIST[0] = THIS_SERVER;

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