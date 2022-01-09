#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct SndChar {
  long mtype;
  char bruh;
};

int main() {
  int key = msgget(10987,0644|IPC_CREAT);


  if (fork() == 0) {
    
    // process 2
    struct SndChar B;
    msgrcv(key, &B,1,1,0);
    printf("Process 2\t%c\n",B.bruh);
    msgrcv(key, &B,1,1,0);
    printf("Process 2\t%c\n",B.bruh);
    B.mtype = 2;
    B.bruh = 'b';
    msgsnd(key, &B,1,0);

    printf("Dziecko: \n");
    char name[3] = "abc";
    char asd[3] = "abc";
    int N = name;
    int Z = asd;

    printf("Bruh %d\n",Z);
    printf("audb %d\n",N);


  } else {
    // process 1
    char buff[1];
    struct SndChar A;
    A.mtype = 1;
    A.bruh = 'a';
    msgsnd(key, &buff,1,0);
    printf("Epic TEST %c",buff[0]);
    A.mtype = 1;
    A.bruh = 'c';
    msgsnd(key, &A,1,0);
    msgrcv(key, &A,1,2,0);
    sleep(1);
    printf("Process 1\t%c\n",A.bruh);
    printf("\n");

     printf("Rodzic: \n");
    char name[3] = "abc";
    char asd[3] = "abc";
    int N = name;
    int Z = asd;

    printf("Bruh %d\n",Z);
    printf("audb %d\n",N);
  }
}