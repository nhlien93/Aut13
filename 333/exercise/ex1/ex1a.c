/*
 * Lindsey Nguyen
 * Student ID: 1130418
 * nhlien93@uw.edu
 * 
 * Program that tests whether ints and array of ints are passed
 * by reference or by value, and prints what it discovers to the terminal.
 * .
 */

#include <stdio.h>
#include <stdlib.h>

void changeInt(int num) {
  num = 0;
}

void changeArray(int array[]) {
  array[0] = 0;
  array[1] = 1;
}

int main(int argc, char **argv) {
  if(argc != 1) {
    return EXIT_FAILURE;
  }

  int num = 1;
  changeInt(num);
  char* intPassBy = (num == 1) ? "value" : "reference";
  printf("int: pass-by-%s\n", intPassBy);

  int array[2] = {1, 2};
  changeArray(array);
  char* arrayPassBy = (array[0] == 1 && array[1] == 2) ? "value" : "reference";
  printf("array of ints: pass-by-%s\n", arrayPassBy);
  return EXIT_SUCCESS;
}
