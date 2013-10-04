/*
 * Lindsey Nguyen
 * Student ID: 1130418
 * nhlien93@uw.edu
 * 
 * Contains the function DumpHex which takes in a pointer of any type
 * and the number of bytes for that type. Then prints out the address
 * that the type is stored in and what is stored there for all the
 * bytes of that type.
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

void DumpHex(void* pData, int byteLen) {
  printf("The %d bytes starting at %p are:", byteLen, pData);
  for (int i = 0; i < byteLen; i++) {
    printf(" %02" PRIx8, *(uint8_t*)pData);
    pData++;
  }
  printf("\n");
}
