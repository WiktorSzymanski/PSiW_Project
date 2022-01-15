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
#define MAX_NUM_OF_SEC_OF_INACTIVITY 40

int counter[CHANNELS] = {0,0,0,0,0};

int KEY;
struct Server *SERVER_LIST;

struct RoomClientMsg{
  char name[20];
  time_t sendTime;
  char message[1024];
};

struct RoomBuf{
  long mtype;
  int roomId;
  struct RoomClientMsg roomMsgArray[10];
} ROOM_BUFFER[5];

struct JoinRoomMsg {
  long mtype;
  int clientKeyId;
  int roomId;
};

struct ConnectMsg {
  long mtype;
  char name[20];
  int clientKeyId;
};

struct Client {
  char name[20];
  int clientKeyId;
  int secOfInactivity;
};

struct Msg {
  long mtype;
  char message[1024];
  int clientKeyId;
  char toClientName[20];
  int roomId;
};

// struct MessageData {
//   char message[1024];
//   char clientName[20];
// }

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

struct Server {
  char name[10];
  int id;
  int numOfClients;
  int numOfRooms;
  struct Client clientList[5];
  struct Room roomList[5];
} THIS_SERVER;


void addMsgToBuf(struct RoomClientMsg newMsg, int i) {
  printf("wiadomosc %s \n", newMsg.message);
  int j = 9 - counter[i];
  // if(ROOM_BUFFER[i].roomMsgArray[j].message[0]==' '){
  strcpy(ROOM_BUFFER[i].roomMsgArray[j].name, newMsg.name);
  strcpy(ROOM_BUFFER[i].roomMsgArray[j].message, newMsg.message);
  ROOM_BUFFER[i].roomMsgArray[0].sendTime = newMsg.sendTime;
  //  break;
  // }
  if (j == 9)
  {

    for (int k = 9; k >= 1; k--)
    {
      strcpy(ROOM_BUFFER[i].roomMsgArray[k].name, ROOM_BUFFER[i].roomMsgArray[k - 1].name);
      strcpy(ROOM_BUFFER[i].roomMsgArray[k].message, ROOM_BUFFER[i].roomMsgArray[k - 1].message);
      ROOM_BUFFER[i].roomMsgArray[k].sendTime = ROOM_BUFFER[i].roomMsgArray[k - 1].sendTime;
    }

    strcpy(ROOM_BUFFER[i].roomMsgArray[0].name, newMsg.name);
    strcpy(ROOM_BUFFER[i].roomMsgArray[0].message, newMsg.message);
    ROOM_BUFFER[i].roomMsgArray[0].sendTime = newMsg.sendTime;
  }
  //   break;
  // }
  if (j < 9)
  {
    counter[i] = (counter[i] + 1);
    j++;
  }
}

void sendClientMsg(int room, int clientKeyId) {
  struct Msg clientMsgList;
  strcpy(clientMsgList.message, "");
  for (int i = 9; i >= 0 ;i--) {
    strcat(clientMsgList.message,ROOM_BUFFER[room].roomMsgArray[i].name);
    strcat(clientMsgList.message,": ");
    strcat(clientMsgList.message,ROOM_BUFFER[room].roomMsgArray[i].message);
    strcat(clientMsgList.message,"\n");
  }
  clientMsgList.mtype=5;
  msgsnd(clientKeyId, &clientMsgList, sizeof(clientMsgList), 0);
}

void catchActivity(int clientKeyId) {
  for (int i = 0; i < sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]); i++) {
    if (SERVER_LIST[0].clientList[i].clientKeyId == clientKeyId) {
      SERVER_LIST[0].clientList[i].secOfInactivity = 0;
    }
  }
}

void printClient(struct Client toPrint) {
  printf("%s\t%d\tsecOfInactivity:%d\n",toPrint.name,toPrint.clientKeyId,toPrint.secOfInactivity);
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

void disconnect(int clientKeyId) {
  for (int i = 0; i < sizeof(SERVER_LIST[0].clientList)/sizeof(SERVER_LIST[0].clientList[0]);i++) {
    if (SERVER_LIST[0].clientList[i].clientKeyId == clientKeyId) {
      strcpy(SERVER_LIST[0].clientList[i].name,"");
      SERVER_LIST[0].clientList[i].clientKeyId = 0;
      break;
    }
  }
  msgctl(clientKeyId, IPC_RMID,0);
  printf("Klient %d oposcil serwer\n",clientKeyId);
}

void disconnectClient() {
  struct ConnectMsg dis;
  if (msgrcv(KEY, &dis, sizeof(dis), 2, IPC_NOWAIT) != -1) {
    disconnect(dis.clientKeyId);
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
            sendClientMsg(join.roomId,join.clientKeyId);

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
            catchActivity(leave.clientKeyId);
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

void getAndSendMsg() {
  struct Msg message;
  if(msgrcv(KEY, &message, sizeof(message), 7, IPC_NOWAIT) != -1) {
    catchActivity(message.clientKeyId);
    for(int i = 0; i < sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]); i++) {
      if(SERVER_LIST[0].roomList[i].id == message.roomId) {
        for(int j = 0; j < sizeof(SERVER_LIST[0].roomList[i].clientListId)/sizeof(SERVER_LIST[0].roomList[i].clientListId[0]); j++) {
          if(strcmp(SERVER_LIST[0].roomList[i].clientListNames[j],message.toClientName) == 0) {
            char name[20];
            getClientNameById(name,message.clientKeyId);
            strcpy(message.toClientName,name);
            message.mtype = 7;
            printf("%s\n",message.toClientName);
            printf("%d\n",SERVER_LIST[0].roomList[i].clientListId[j]);
            // sleep(10);
            msgsnd(SERVER_LIST[0].roomList[i].clientListId[j], &message, sizeof(message), 0);
            printf("Odebrano i przeslano wiadomosc\n");
            break;
          }
          if(j == sizeof(SERVER_LIST[0].roomList[i].clientListId)/sizeof(SERVER_LIST[0].roomList[i].clientListId[0]) - 1) {
            printf("Brak klienta w pokokju\n");
          }
        }
        break;
      }
    }
  } else {
    printf("Brak request-u getAndSendMsg\n");
  }
}

void getAndSendRoomMsg() {
  struct Msg message;
  if(msgrcv(KEY, &message, sizeof(message), 8, IPC_NOWAIT) != -1) {
    catchActivity(message.clientKeyId);
    for(int i = 0; i < sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]); i++) {
      if(SERVER_LIST[0].roomList[i].id == message.roomId) {
        struct RoomClientMsg newMsg;
        getClientNameById(newMsg.name,message.clientKeyId);
        strcpy(newMsg.message, message.message);
        printf("Nazwa uzytkownika %s \n", newMsg.name);
        addMsgToBuf(newMsg, i);
        for(int j = 0; j < sizeof(SERVER_LIST[0].roomList[i].clientListId)/sizeof(SERVER_LIST[0].roomList[i].clientListId[0]); j++) {
          if(SERVER_LIST[0].roomList[i].clientListId[j] != 0 && SERVER_LIST[0].roomList[i].clientListId[j] != message.clientKeyId) {
            char name[20];
            getClientNameById(name,message.clientKeyId);
            strcpy(message.toClientName,name);
            message.mtype = 8;
            msgsnd(SERVER_LIST[0].roomList[i].clientListId[j], &message, sizeof(message), 0);
            printf("Odebrano i przeslano wiadomosc do uzytkownika pokoju %d\n", message.roomId);
          }
        }
        break;
      }
    }
  } else {
    printf("Brak request-u getAndSendRoomMsg\n");
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
    newClient.secOfInactivity = 0;

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
    catchActivity(clientListMsg.clientKeyId);
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
    catchActivity(roomMsg.clientKeyId);
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

void addRooom() {
  struct Msg roomMsg;
  if (msgrcv(KEY, &roomMsg, sizeof(roomMsg), 9, IPC_NOWAIT) != -1) {
    catchActivity(roomMsg.clientKeyId);
    for(int i = 0; i < sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]); i++) {
      if (SERVER_LIST[0].roomList[i].id == -1) {
        SERVER_LIST[0].roomList[i].id = i;
        roomMsg.roomId = i;
        msgsnd(roomMsg.clientKeyId, &roomMsg, sizeof(roomMsg), 0);
        break;
      }

      if (i == sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]) - 1) {
        roomMsg.roomId = -1;
        msgsnd(roomMsg.clientKeyId, &roomMsg, sizeof(roomMsg), 0);
        printf("Max 5 pokoi\n");
      }
    }
  } else {
    printf("Brak request-u addRooom\n");
  }
}

void kickFromRoom(int clientKeyId) {
  printf("Kickin %d\n",clientKeyId);
  sleep(1);
  for (int i = 0; i < sizeof(SERVER_LIST[0].roomList)/sizeof(SERVER_LIST[0].roomList[0]); i++) {
    for (int j = 0; j < sizeof(SERVER_LIST[0].roomList[i].clientListId)/sizeof(SERVER_LIST[0].roomList[i].clientListId[0]); j++) {
      if (SERVER_LIST[0].roomList[i].clientListId[j] == clientKeyId) {
        SERVER_LIST[0].roomList[i].clientListId[j] = 0;
        strcpy(SERVER_LIST[0].roomList[i].clientListNames[j],"\0");
      }
    }
  }
}

void updateInactivity() {
  for(int i = 0; i < 5; i++) {
    if (SERVER_LIST[0].clientList[i].clientKeyId != 0) {
      if (SERVER_LIST[0].clientList[i].secOfInactivity >= MAX_NUM_OF_SEC_OF_INACTIVITY) {
        printf("Client with id %d out due to Inactivity\n",SERVER_LIST[0].clientList[i].clientKeyId);
        struct Msg out;
        out.mtype = 10;
        msgsnd(SERVER_LIST[0].clientList[i].clientKeyId, &out, sizeof(out),0);
        printf("Wyslano disconect wiadomosc do %d o mtype %ld\n",SERVER_LIST[0].clientList[i].clientKeyId,out.mtype);
        sleep(1);
        kickFromRoom(SERVER_LIST[0].clientList[i].clientKeyId);
        disconnect(SERVER_LIST[0].clientList[i].clientKeyId);
        SERVER_LIST[0].clientList[i].secOfInactivity = 0;
      } else {
        SERVER_LIST[0].clientList[i].secOfInactivity++;
      }
    }
  }
}

int main() {
  KEY = msgget(1234567890,0644|IPC_CREAT);
  int shmId = shmget(12345678,8,0644|IPC_CREAT);
  SERVER_LIST = (struct Server *)shmat(shmId,NULL,0);

  SERVER_LIST[0] = THIS_SERVER;
  
  
  for(int i=0;i<CHANNELS;i++){
    struct Room newRoom;
    if (i != 0) {
      newRoom.id = -1;
    } else {
      newRoom.id = i;
    }
    for(int j = 0 ; j < sizeof(newRoom.clientListId)/sizeof(newRoom.clientListId[0]); j++) {
      newRoom.clientListId[j] = 0;
      char name[20];
      strcpy(*newRoom.clientListNames,name);
    }
    SERVER_LIST[0].roomList[i] = newRoom;
  }

  for(int i=0;i<5;i++){
    ROOM_BUFFER[i].roomId = i;
    struct RoomClientMsg newMsg;
    for(int j = 0;j<10;j++){

      newMsg.message[0] = '\0';
      newMsg.name[0]= '\0';
      ROOM_BUFFER[i].roomMsgArray[j] = newMsg;
    }
  }

  while(1) {

    disconnectClient();
    addClient();
    sendClientList();
    sendChannelList();
    addClientToRoom();
    removeClientFromRoom();
    getAndSendMsg();
    getAndSendRoomMsg();
    addRooom();
    updateInactivity();

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