/*
 * Lindsey Nguyen
 * Student ID: 1130418
 * nhlien93@uw.edu
 * 
 * Program that tests method in dumphex.c and method prototype in dumphex.h
 */

#include <stdlib.h>
#include "dumphex.h"

int main(int argc, char **argv) {
  char     charVal = '0';
  int32_t  intVal = 1;
  float    floatVal = 1.0;
  double   doubleVal  = 1.0;

  typedef struct { 
    char     charVal;
    int32_t  intVal;
    float    floatVal;
    double   doubleVal;
  } Ex2Struct;
  Ex2Struct structVal = { '0', 1, 1.0, 1.0 };

  DumpHex(&charVal, sizeof(char));
  DumpHex(&intVal, sizeof(int32_t));
  DumpHex(&floatVal, sizeof(float));
  DumpHex(&doubleVal, sizeof(double));
  DumpHex(&structVal, sizeof(structVal));

  return EXIT_SUCCESS;  // EXIT_SUCCESS defined in stdlib.h.
}
