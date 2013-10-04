/*
 * Lindsey Nguyen
 * Student ID: 1130418
 * nhlien93@uw.edu
 *
 * Small exercise that computes the factorial of a non-negative integer
 * and prints the result to the terminal. The integer is provided as a
 * command line argument
 */
#include <stdlib.h>
#include <stdio.h>

int factorial(int num);

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "./ex0 has one arg\n");
    return EXIT_FAILURE;
  }
  int num;
  if (sscanf(argv[1], "%d", &num) != 1) {
    return EXIT_FAILURE;
  }
  // must be non-negative integer that is less than 13 (or else overflows)
  if (num < 0) {
    fprintf(stderr, "./ex0 arg should be non-negative\n");
    return EXIT_FAILURE;
  }
  if (num >= 13) {
    fprintf(stderr, "./ex0 arg should be less then 13 to not overflow\n");
    return EXIT_FAILURE;
  }
  // print out factorial of arg passed in.
  printf("%d\n",factorial(num));
  return EXIT_SUCCESS;
}

// factorial function
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
