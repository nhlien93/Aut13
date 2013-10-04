/*
 * Lindsey Nguyen
 * Student ID: 1130418
 * nhlien93@uw.edu
 * 
 * Program that creates an array of the first 10 factorials, copies this
 * into another array in reverse order, and prints it out.
 */

#include <stdlib.h>
#include <stdio.h>

int factorial(int num);

int main(int argc, char** argv) {
  const int arraysize = 10;
  int* array = (int*)malloc(sizeof(int) * arraysize);
  if(array == NULL) return EXIT_FAILURE;
  int i;
  // assign factorial(i) to array element i
  for (i = 0; i < arraysize; i++) {
    array[i] = factorial(i);
  }
 
  int* array2 = (int*)malloc(sizeof(int) * arraysize);
  if (array2 == NULL) {
    free(array);
    return EXIT_FAILURE;
  }
  // assign factorial(arraysize - 1 - i) to array2 element i
  for(i = 0; i < arraysize; i++) {
    array2[i] = array[arraysize-1 -i];    
  }
  // prints out the second array to the terminal
  printf("[%d", array2[0]);
  for(i = 1; i < arraysize; i++) {
    printf(", %d", array2[i]);
  }
  printf("]\n");
  free(array);
  free(array2);
  return EXIT_SUCCESS;
}

int factorial(int num) {
  int i, result = 1;
  if (num == 0) {
    return 1;
  } else {
    for(i = 1; i <= num; i++) {
      result *= i;
    }
    return result;
  }
}
