#include <stdio.h>

const char* myName() {
  return "Flavio";
}

int main(void) {
  printf("%s", myName());
}