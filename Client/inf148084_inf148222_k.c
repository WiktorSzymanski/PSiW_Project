#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <wait.h>

int KEY;
int CLIENT_KEY_ID;

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

struct Msg {
  long mtype;
  char message[1024];
  int clientKeyId;
  char toClientName[20];
  int roomId;
  char time[10];
};

void clear() {
  if (fork() == 0) {
    execlp("clear","clear", NULL);
  } else {
    wait(NULL);
  }
}

int length(char* string) {
  int result = 0;
  while(1) {
    if (string[result] != '\0') {
      result++;
      continue;
    }
    return result - (string[result-1] == '\n');
  }
}

int isDigit(char input[]) {
  for(int i = 0; i < length(input); i++) {
    if(isdigit(input[i]) == 0) {
      return 0;
    }
  }
  return 1;
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
  printf("Podaj nazwe uzytkownika (min. 6 max 20 znakow, nazwa nie moze zawierac spacji)\n");
  scanf("%s", name);
  if(length(name) > 20 || length(name) < 6) {
    printf("Nazwa musi mieć conajmniej 6 znaków,nie więcej jak 20 i bez spacji!\n");
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

void printRoom(struct Room toPrint) {
  printf("Room Id: %d\n", toPrint.id);

  for (int i = 0; i < sizeof(toPrint.clientListId)/sizeof(toPrint.clientListId[0]); i++) {
    if (toPrint.clientListId[i] != 0) {
      printf("Nr. %d\t %s\n",i, toPrint.clientListNames[i]);
    } else {
      printf("Nr. %d\t \n",i);
    }

  }
}

void getChannelList(){
  struct RoomListMsg listRequest;
  listRequest.mtype = 4;
  listRequest.clientKeyId = CLIENT_KEY_ID;
  msgsnd(KEY,&listRequest, sizeof(listRequest),0);
  msgrcv(CLIENT_KEY_ID, &listRequest, sizeof(listRequest), 4, 0);

  for(int i = 0; i < sizeof(listRequest.roomList)/sizeof(listRequest.roomList[0]); i++) {
    if (listRequest.roomList[i].id != -1) {
      printRoom(listRequest.roomList[i]);
      printf("\n");
    }
  }
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

void leaveRoom(int roomId) {
  struct JoinRoomMsg leaveRoom;
  leaveRoom.mtype = 6;
  leaveRoom.clientKeyId = CLIENT_KEY_ID;
  leaveRoom.roomId = roomId;
  msgsnd(KEY, &leaveRoom, sizeof(leaveRoom),0);
  msgrcv(CLIENT_KEY_ID, &leaveRoom, sizeof(leaveRoom), 6, 0);
  printf("Pomyslnie opuszczono pokoj\n");
  return;
}

void splitStringIntoTwo(char *toSplit, char *str1, char *str2, char splitBy) {
  int lenOfFirstStr = 0;
  for(int i = 0; i < length(toSplit); i++) {
    if (lenOfFirstStr == 0) {
      if (toSplit[i] == splitBy) {
        lenOfFirstStr = i+1;
      } else {
        str1[i] = toSplit[i];
      }
    } else {
      str2[i - lenOfFirstStr] = toSplit[i];
    }
  }
  str2[length(toSplit) - lenOfFirstStr] = '\0';
}

void inRoom(int roomId) {

  printf(":q - wyjdz z pokoju\n");
  printf(":p <nazwa klienta> <wiadomosc> - wyslij wiadomosc prywadna\n");
  
  int childPID = fork();
  if(childPID == 0) {
    struct Msg message;
    while(1) {
      if (msgrcv(CLIENT_KEY_ID, &message, sizeof(message), 7, IPC_NOWAIT) != -1) {
        printf("%s Private Message from %s: %s\n",message.time,message.toClientName,message.message);
      } else if (msgrcv(CLIENT_KEY_ID, &message, sizeof(message), 8, IPC_NOWAIT) != -1) {
        printf("%s %s: %s",message.time,message.toClientName, message.message);
      } else {
        // printf("\n");
        sleep(0.01);
      }
    }
  } else {
    while(1) {
      char *line = NULL;
      size_t len = 0;
      getline(&line,&len,stdin);
      struct Msg message;

      time_t seconds;
      struct tm *timeStruct;

      
      if(line[0] == ':') {
        if(line[1] == 'q') {
          clear();
          printf("Wychodzenie z pokoju\n");
          leaveRoom(roomId);
          kill(childPID,SIGTERM);
          return;
        }

        if(line[1] == 'p') {
          char str1[20];
          char str2[1045];
          char str3[1024];
          splitStringIntoTwo(line, str1, str2, ' ');
          splitStringIntoTwo(str2, str1, str3, ' ');
          message.mtype = 7;
          message.clientKeyId = CLIENT_KEY_ID;
          message.roomId = roomId;
          strcpy(message.message,str3);
          strcpy(message.toClientName,str1);

          seconds = time(NULL);
          timeStruct = localtime(&seconds);

          sprintf(message.time,"%02d:%02d:%02d", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);

          msgsnd(KEY, &message, sizeof(message), 0);
        }
      } else {
        message.mtype = 8;
        message.clientKeyId = CLIENT_KEY_ID;
        message.roomId = roomId;

        seconds = time(NULL);
        timeStruct = localtime(&seconds);

        sprintf(message.time,"%02d:%02d:%02d", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);

        strcpy(message.message,line);
        msgsnd(KEY, &message, sizeof(message), 0);
      }
    }
  }
}

void addRoom() {
  struct Msg roomMsg;
  roomMsg.mtype = 9;
  roomMsg.clientKeyId = CLIENT_KEY_ID;
  msgsnd(KEY,&roomMsg, sizeof(roomMsg), 0);
  msgrcv(CLIENT_KEY_ID,&roomMsg, sizeof(roomMsg), 9, 0);
  if (roomMsg.roomId == -1) {
    printf("Maksymalna liczba pokoi osiągnieta, nie można dodać kolejnego\n");
  } else {
    printf("Dodano pokoj o id %d\n", roomMsg.roomId);
  }
}

void joinRoom() {
  struct JoinRoomMsg join;
  struct Msg chatMsg;
  join.mtype = 5;
  join.clientKeyId = CLIENT_KEY_ID;

  int roomId;
  printf("Podaj id pokoju do ktorego chcesz dolaczyc: ");
  scanf("%d",&roomId);
  printf("\n");
  clear();
  join.roomId = roomId;

  msgsnd(KEY, &join, sizeof(join), 0);
  msgrcv(CLIENT_KEY_ID, &join, sizeof(join), 5, 0);
  if (join.roomId == -2) {
    printf("Brak pokoju o takim id!\n\n");
  } else if (join.roomId == -1) {
    printf("Pokoj jest pelen!\n\n");
  } else {
    printf("Dolaczono do pokoju o id %d\n",roomId);
    msgrcv(CLIENT_KEY_ID, &chatMsg, sizeof(chatMsg), 5, 0);
    printf("Ostatnie 10 wiadomosci zapisane w buforze\n%s", chatMsg.message);
    inRoom(roomId);
  }
}

void loggedInMenu() {
  KEY = msgget(1234567890,0644|IPC_CREAT);
  CLIENT_KEY_ID = connectToServer();

  struct Msg message;

  int childPID = fork();
  if (childPID == 0) {
    while(1) {
      if (msgrcv(CLIENT_KEY_ID,&message, sizeof(message),10,IPC_NOWAIT) != -1) {
        printf("\nWyrzucono cie z serwera z powodu nieaktywnosci\n");
        sleep(1);
        kill(getppid(),SIGTERM);
        exit(0);
      } else {
        sleep(1);
      }
    }
  } else {
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
    clear();
    sleep(1);
    if(strcmp(message.message,"Poloczono z serwerem\n") == 0) {
      printf("%s", message.message);
    } else {
      printf("%s", message.message);
      return;
    }

    while(1){
      printf("[1] Wyswietl liste kanalow\n[2] Wyswietl liste uzytkownikow\n[3] Dolacz do pokoju\n[4] Stworz nowy pokoj\n[9] Wyloguj sie\n[0] Wyjdz\nCo chcesz zrobic: ");
      char operation[2];
      scanf("%s",operation);

      while (isDigit(operation) == 0) {
        printf("Bledne dane wejsciowe!\nPodaj wlasciwa operacje: ");
        scanf("%s",operation);
      }

      switch (atoi(operation)) {
        case 0:
          clear();
          if(disconnectFromServer() == 0) {
            sleep(1);
            printf("Odlaczono od serwera clientKeyId: %d\n",CLIENT_KEY_ID);
            msgctl(CLIENT_KEY_ID, IPC_RMID,0);
            printf("Wychodznie z programu\n");
            kill(childPID,SIGTERM);
            exit(0);
          }
          break;
        case 1:
          clear();
          printf("Lista kanalow: \n");
          getChannelList();
          break;
        case 2:
          clear();
          printf("Lista uzytkownikow: \n");
          getClientList();
          break;
        case 3:
          joinRoom();
          break;
        case 4:
          clear();
          addRoom();
          break;
        case 9:
          clear();
          if(disconnectFromServer() == 0) {
            sleep(1);
            printf("Odlaczono od serwera clientKeyId: %d\n",CLIENT_KEY_ID);
            msgctl(CLIENT_KEY_ID, IPC_RMID,0);
            kill(childPID,SIGTERM);
            return;
          }
          break;
        default:
          printf("Brak takiej operaci!!!\n");
          break;
      }
    }
  }
}

int main() {
  while(1){
    printf("[1] Zaloguj sie\n[0] Wyjdz\nCo chcesz zrobic: ");
    char operation[2];
    scanf("%s",operation);

    while (isDigit(operation) == 0) {
      printf("Bledne dane wejsciowe!\nPodaj wlasciwa operacje: ");
      scanf("%s",operation);
    }

    switch (atoi(operation)) {
      case 1:
        loggedInMenu();
        break;
      case 0:
        printf("Wychodznie z programu\n");
        kill(getppid(),SIGTERM);
        exit(0);
        break;
      default:
        printf("Brak takiej operaci!!!\n");
        break;
    }
  }
  return 0;
}