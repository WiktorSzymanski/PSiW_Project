#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>

struct ConnectMsg {
  long mtype;
  char name[20];
  long Client;
};

void getUserName(char* s);
long int userKey(char* s);

void connectToServer(int key, char* s) {
    struct ConnectMsg connect;
    char name[20];
    strcpy(name, s);
    long mtypeClient = userKey(s);

    connect.mtype = 1;
    strcpy(connect.name,name);
    connect.Client = mtypeClient;

    msgsnd(key, &connect, sizeof(connect),0);
    printf("Wyslano request\n");
    }

int main() {
    
  int key = msgget(123456789,0644|IPC_CREAT);

    char name[20] = "";
        getUserName(name);
        while((name=="")){
            printf("String err\n");
            getUserName(name);
        }

  connectToServer(key, name);

  return 0;
}

void getUserName(char* s)
{
    char nametab[20];
    printf("Podaj nazwe uzytkownika \n");
    scanf("%s", &nametab);
    strcpy(s,nametab);
}
long int userKey(char *s){
    char tab[20]="____________________";
    strcpy(tab, s);
    long int result;
    for(int i=0;i<sizeof(tab);i++){
        if(tab[i]>=65 && tab[i]<=90 || tab[i]>=97 && tab[i]<=122){
            result+=tab[i];
            result*=100;
        }
        
    }
    return result;
}