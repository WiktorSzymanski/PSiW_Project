#include <stdio.h>

int main() {

  int *pointer;

  int zmienna = 20;

  pointer = &zmienna;

  printf("%d\n",*pointer);

  *pointer = 30;

  printf("%d\n",*pointer);
  return 0;
}