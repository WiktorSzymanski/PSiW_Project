#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

struct Server {
  char name[10];
  int id;
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Podaj jeden plik\n");
    return -1;
  }

  int file = open(argv[1],O_RDONLY);
  int buffCount = 0;
  char buffor[1];

  while ((buffCount = read(file,buffor,1)) > 0) {
    printf("%c",buffor[0]);

  }

  printf("\nOperacja wypisania pliku w odwrotnej kolejnosci zakonczona pomyslnie\n");

  return 0;
}