#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <wait.h>
#include <stdlib.h>

#define CHANNELS 5

int KEY;
struct Server *SERVER_LIST;

struct JoinRoomMsg {
  long mtype;
  int clientKeyId;
  int roomId;
};

struct Room{
  int id;
  int numOfClients;
  int clientListId[5];
  char clientListNames[5][20];
};

struct RoomListMsg {
  long mtype;
  int clientKeyId;
  struct Room roomList[5]; 
};


struct ConnectMsg {
  long mtype;
  char name[20];
  int clientKeyId;
};

struct Client {
  char name[20];
  int clientKeyId;
};

struct Server {
  char name[10];
  int id;
  int numOfClients;
  int numOfRooms;
  struct Client clientList[5];
  struct Room roomList[5];
} THIS_SERVER;

struct Msg {
  long mtype;
  char message[1024];
  int clientKeyId;
  char toClientName[20];
};

void printClient(struct Client toPrint) {
  printf("%s\t%d\n",toPrint.name,toPrint.clientKeyId);
}

void printClientList() {
  printf("Aktualna lista klientow: \n");
  for(int i = 0; i < 5; i++) {
    printClient(SERVER_LIST[0].clientList[i]);
  }
}

void sendConnectionEstablishedMsg(int clientKeyId) {
  struct Msg message;

  message.mtype = 1;
  strcpy(message.message,"Poloczono z serwerem\n");

  msgsnd(clientKeyId, &message, sizeof(message), 0);
  printf("Wyslano wiadomosc do klienta o mtype: %d\n",clientKeyId);
}

void sendServerFullMsg(int clientKeyId) {
  struct Msg message;

  message.mtype = 1;
  strcpy(message.message,"Serwer pelen!\n");

  msgsnd(clientKeyId, &message, sizeof(message), 0);
  printf("Wyslano wiadomosc do klienta o mtype: %d\n",clientKeyId);
}

void disconnectClient() {
  struct ConnectMsg disconnect;
  if (msgrcv(KEY, &disconnect, sizeof(disconnect), 2, IPC_NOWAIT) != -1) {

    for (int i = 0; i < sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]);i++) {
      if (SERVER_LIST[0].clientList[i].clientKeyId == disconnect.clientKeyId) {
        strcpy(SERVER_LIST[0].clientList[i].name,"");
        SERVER_LIST[0].clientList[i].clientKeyId = 0;
        break;
      }
    }
    msgctl(disconnect.clientKeyId, IPC_RMID,&disconnect);
    printf("Klient %d oposcil serwer\n",disconnect.clientKeyId);
  } else {
    printf("Brak request-u disconnectClient\n");
  }
}

void sendUniqNameReq(int clientKeyId) {
  struct Msg message;

  message.mtype = 2;

  msgsnd(clientKeyId, &message, sizeof(message), 0);
  printf("Wyslano wiadomosc do klienta o keyId %d odnosnie nazwy\n",clientKeyId);
}

int checkIfNameUniq(char* name){
  for(int i=0;i<sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]);i++){
    printf("name1: %s\tname2: %s\n",SERVER_LIST[0].clientList[i].name,name);
    if(strcmp(name,SERVER_LIST[0].clientList[i].name) == 0){
      return 0;
    }
  }
  return 1;
}

int checkIfKeyIdUniq(int mtype) {
  for(int i=0;i<sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]);i++){
    if(SERVER_LIST[0].clientList[i].clientKeyId == mtype){
      return 0;
    }
  }
  return 1;
}

void getClientNameById(char* name, int id) {
  for (int i = 0; i < sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]); i++) {
    if (SERVER_LIST[0].clientList[i].clientKeyId == id) {
      strcpy(name,SERVER_LIST[0].clientList[i].name);
      break;
    }
  }
}

void addClientToRoom() {
  struct JoinRoomMsg join;
  if (msgrcv(KEY, &join, sizeof(join), 5, IPC_NOWAIT) != -1) {
    for(int i = 0; i < sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]); i++) {
      if (SERVER_LIST[0].roomList[i].id == join.roomId) {
        for(int j = 0; j < sizeof(SERVER_LIST[0].roomList[i].clientListId)/sizeof(SERVER_LIST[0].roomList[i].clientListId[0]); j++) {
          if(SERVER_LIST[0].roomList[i].clientListId[j] == 0) {
            SERVER_LIST[0].roomList[i].clientListId[j] = join.clientKeyId;
            char name[20];
            getClientNameById(name,join.clientKeyId);
            strcpy(SERVER_LIST[0].roomList[i].clientListNames[j],name);
            msgsnd(join.clientKeyId, &join, sizeof(join), 0);
            break;
          }
          if (j == 4) {
            printf("Pokoj nr. %d jest pelen!\n",join.roomId);
            join.roomId = -1;
            msgsnd(join.clientKeyId, &join, sizeof(join), 0);
          }
        }
        break;
      }
    }
  } else {
    printf("Brak request-u addClientToRoom\n");
  }
}

void removeClientFromRoom() {
  struct JoinRoomMsg leave;
  if (msgrcv(KEY, &leave, sizeof(leave), 6, IPC_NOWAIT) != -1) {
    for(int i = 0; i < sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]); i++) {
      if (SERVER_LIST[0].roomList[i].id == leave.roomId) {
        for(int j = 0; j < sizeof(SERVER_LIST[0].roomList[i].clientListId)/sizeof(SERVER_LIST[0].roomList[i].clientListId[0]); j++) {
          if(SERVER_LIST[0].roomList[i].clientListId[j] == leave.clientKeyId) {
            SERVER_LIST[0].roomList[i].clientListId[j] = 0;
            char name[20] = "                    ";
            strcpy(SERVER_LIST[0].roomList[i].clientListNames[j],name);
            msgsnd(leave.clientKeyId, &leave, sizeof(leave), 0);
            break;
          }
        }
        break;
      }
    }
  } else {
    printf("Brak request-u removeClientFromRoom\n");
  }
}

int checkIfClientIsValid(struct Client newClient) {
  int check = 0;
  if (!checkIfNameUniq(newClient.name)) {
    printf("Nazwa %s sie powtarza\n",newClient.name);
    sendUniqNameReq(newClient.clientKeyId);
    // printf("Wpisz 1 \n");
    // int x;
    // scanf("%d",&x);

    check++;
  }

  if (!checkIfKeyIdUniq(newClient.clientKeyId)) {
    printf("mType %d sie powtarza\n",newClient.clientKeyId);

    // printf("Wpisz 1 \n");
    // int x;
    // scanf("%d",&x);

    check++;
  }

  return check == 0;
}

void addClient() {
  struct ConnectMsg connect;
  if(msgrcv(KEY, &connect, sizeof(connect), 1, IPC_NOWAIT) != -1) {
    struct Client newClient;
    strcpy(newClient.name,connect.name);
    newClient.clientKeyId = connect.clientKeyId;

    if (checkIfClientIsValid(newClient) == 1) {
      printClient(newClient);

      for(int i = 0; i < 5; i++) {
        if (strcmp(SERVER_LIST[0].clientList[i].name,"") == 0) {
          SERVER_LIST[0].clientList[i] = newClient;
          sendConnectionEstablishedMsg(newClient.clientKeyId);
          break;
        }
        if(i == 4) {
          sendServerFullMsg(newClient.clientKeyId);
        }
      }
    } else {
      printf("Nie dodano kilenta z powodu powtarzajacej sie nazwy\n");
    }
  } else {
    printf("Brak request-u addClient\n");
  }
}

void sendClientList(){
  struct Msg clientListMsg;
  if (msgrcv(KEY, &clientListMsg, sizeof(clientListMsg), 3, IPC_NOWAIT) != -1) {
    strcpy(clientListMsg.message, "");
    char number[1];
    for (int i = 0; i < sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]);i++) {
      number[0] = i+1+'0';
      strcat(clientListMsg.message,number);
      strcat(clientListMsg.message,".\t");
      strcat(clientListMsg.message,SERVER_LIST[0].clientList[i].name);
      strcat(clientListMsg.message,"\n");
    }
    clientListMsg.mtype=3;
    msgsnd(clientListMsg.clientKeyId, &clientListMsg, sizeof(clientListMsg), 0);
  } else {
    printf("Brak request-u ClientsList\n");
  }
}

void sendChannelList(){
  struct RoomListMsg roomMsg;
  if (msgrcv(KEY, &roomMsg, sizeof(roomMsg), 4, IPC_NOWAIT) != -1) {
    for(int i = 0; i < sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]); i++) {
      roomMsg.roomList[i] = SERVER_LIST[0].roomList[i];
    }
    msgsnd(roomMsg.clientKeyId, &roomMsg, sizeof(roomMsg), 0);
  } else {
    printf("Brak request-u ChannelList\n");
  }
}

void printRoom(struct Room toPrint) {
  printf("Room Id: %d\n", toPrint.id);

  for (int i = 0; i < sizeof(toPrint.clientListId)/sizeof(toPrint.clientListId[0]); i++) {
    printf("Nr. %d\tClientId %d\n",i, toPrint.clientListId[i]);
  }
}

void printRoomsList() {
  for(int i = 0; i < sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]); i++) {
    printRoom(SERVER_LIST[0].roomList[i]);
    printf("\n");
  }
}

int main() {
  KEY = msgget(1234567890,0644|IPC_CREAT);
  int shmId = shmget(12345678,8,0644|IPC_CREAT);
  SERVER_LIST = (struct Server *)shmat(shmId,NULL,0);

  SERVER_LIST[0] = THIS_SERVER;
  
  
  for(int i=0;i<CHANNELS;i++){
    struct Room newRoom;
    newRoom.id = i;
    for(int j = 0 ; j < sizeof(newRoom.clientListId)/sizeof(newRoom.clientListId[0]); j++) {
      newRoom.clientListId[j] = 0;
      char name[20];
      strcpy(*newRoom.clientListNames,name);
    }
    SERVER_LIST[0].roomList[i] = newRoom;
  }

  while(1) {

    disconnectClient();
    addClient();
    sendClientList();
    sendChannelList();
    addClientToRoom();
    removeClientFromRoom();
    

    // printf("Wpisz 1 \n");
    // int x;
    // scanf("%d",&x);
    sleep(1);
    if (fork() == 0) {
      execlp("clear","clear", NULL);
    } else {
      wait(NULL);
      printClientList();
      printRoomsList();
    }
  }
  
  return 0;
}