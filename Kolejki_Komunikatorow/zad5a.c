#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

int main() {
  int kolejka1 = msgget(010203, 0644|IPC_CREAT);
  int kolejka2 = msgget(01020304, 0644|IPC_CREAT);
  int kolejkaExit = msgget(0102030405, 0644|IPC_CREAT);

  char messageIn[1024];
  char messageOut[1024];
  char exit[1024] = "exit\n";

  int pid;

  if ((pid = fork()) == 0) {
    while (1) {
      fgets(messageOut,sizeof(messageOut),stdin);
      msgsnd(kolejka1, messageOut, sizeof(messageOut), 0);
      if (!strcmp(messageOut, exit)) {
        kill(getppid(),SIGTERM);
        kill(getpid(),SIGTERM);
      }
    }
  } else {
    while (1) {
      msgrcv(kolejka2, messageIn, sizeof(messageIn), 0, MSG_NOERROR);
      printf("\n%s\n",messageIn);
      if (!strcmp(messageIn, exit)) {
        kill(pid,SIGTERM);
        kill(getpid(),SIGTERM);
      }
    }
  }
  return 0;
}