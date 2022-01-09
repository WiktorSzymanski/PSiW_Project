#include <stdio.h>
#include <string.h>

struct Man {
  char name[10];
  int age;
};

struct Group {
  char name[10];
  struct Man listOfPpl[5];
} NEW_GROUP;

void main() {
  strcpy(NEW_GROUP.name,"Klasa 4D");
  struct Man pp1, pp2, pp3;

  strcpy(pp1.name,"Jan");
  pp1.age = 5;
  strcpy(pp2.name,"Janek");
  pp2.age = 15;
  strcpy(pp3.name,"Janula");
  pp3.age = 163;

  NEW_GROUP.listOfPpl[0] = pp1;
  NEW_GROUP.listOfPpl[1] = pp2;
  NEW_GROUP.listOfPpl[2] = pp3;

  void printMan(struct Man men) {
    printf("Name: %s\tAge: %d\n",men.name,men.age);
  }

  for (int i = 0; i < 5; i++) {
    printMan(NEW_GROUP.listOfPpl[i]);
  }
}