#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ctype.h>
#include <stdlib.h>

struct Msg {
  char name[20];
  char message[1024];
};

struct Room {
  int roomId;
  struct Msg *lastMsgs[10];
} ROOM;

void main() {
  for(int i = 0; i < 10; i++) {
    struct Msg newMsg;
    switch (i) {
      case 0:
        strcpy(newMsg.name,"Jan");
        strcpy(newMsg.message,"EBEBEBEBEBE EBEBE");
        break;
      case 1:
        strcpy(newMsg.name,"Wiktor");
        strcpy(newMsg.message,"sdasd EBEBE");
        break;
      case 2:
        strcpy(newMsg.name,"Jakub");
        strcpy(newMsg.message,"EBEBEBEBsdgsgsdgEBE EBEBE");
        break;
      case 3:
        strcpy(newMsg.name,"Bartek");
        strcpy(newMsg.message,"EBEBEBEBEBE EBEBE");
        break;
      case 4:
        strcpy(newMsg.name,"Jan");
        strcpy(newMsg.message,"EBEBEBEBEBE EBEBE");
        break;
      default:
        strcpy(newMsg.name,"Sheeesh");
        strcpy(newMsg.message,"EBEBEBEBEBE EBEBE");
        break;
    }
    ROOM.lastMsgs[i] = &newMsg;;
  }

  printf("Room Id: %d\n",ROOM.roomId);
  for(int i = 0; i < 10; i++) {
    printf("%s: %s\n",ROOM.lastMsgs[i]->name,ROOM.lastMsgs[i]->message);
  }
}