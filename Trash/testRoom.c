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
  struct Msg lastMsgs[10];
} ROOM;

void main() {
  
  while(1) {
    struct Msg newMsg;
    char *line = NULL;
    size_t len = 0;
    printf("Name: \n");
    getline(&line,&len,stdin);
    strcpy(newMsg.name,line);
    printf("Message: \n");
    getline(&line,&len,stdin);
    strcpy(newMsg.message,line);

    pushMsgsBack();
    ROOM.lastMsgs[9] = newMsg;
  }

  printf("Room Id: %d\n",ROOM.roomId);
  for(int i = 0; i < 10; i++) {
    printf("%s: %s\n",ROOM.lastMsgs[i].name,ROOM.lastMsgs[i].message);
  }
}