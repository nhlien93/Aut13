/* 
 * Lindsey Nguyen
 * Student ID: 1130418
 * nhlien93@uw.edu
 *
 * Implement the multiset_union function which does a union of
 * two sets that are passed in. The sets are passed in by there
 * pointers.
 *
 * copyright 2013 John Zahorjan
 *
 * This file is part of the UW CSE 333 course exercises (333exer).
 *
 * 333exer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 333exer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 333exer.  If not, see .
 */

#include <stdio.h>    // for printf, NULL
#include <stdlib.h>   // for EXIT_SUCCESS, malloc, free

typedef struct {
  size_t  size;
  int valueVec[20];
} MultiSet;

MultiSet* multiset_union(MultiSet *A, MultiSet *B) {
  // You must implement this method
  MultiSet *set = (MultiSet*)malloc(sizeof(MultiSet));
  set->size = A->size + B->size;
  int i;
  for (i = 0; i < A->size; i++) {
    set->valueVec[i] = A->valueVec[i];
  }
  for(int j = 0; j < B->size; j++) {
    set->valueVec[i] = B->valueVec[j];
    i++;
  }
  return set;
}

int main(int argc, char **argv) {
  MultiSet A = {5, {1, 2, 2, 3, 2}};
  MultiSet B = {3, {2, 3, 3}};

  MultiSet* C = multiset_union(&A, &B);
  if ( C == NULL ) {
    printf("union failed\n");
    exit(EXIT_FAILURE);
  }
  for ( size_t i = 0; i < C->size; i++ ) {
    printf("%2u: %d\n", (unsigned int)i, C->valueVec[i]);
  }

  // It worked!  Clean up by freeing the allocated memory.
  free(C);
  return EXIT_SUCCESS;  // EXIT_SUCCESS defined in stdlib.h.
}
