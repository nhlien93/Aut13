#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int key1;
  int key2;
} Section2Struct, *Section2StructPtr;

Section2StructPtr Section2StructConstruct(int one, int two) {
  // allocate the section 2 struct record
  Section2StructPtr ss = (Section2StructPtr) malloc(sizeof(Section2Struct));
  if (ss == NULL) {
    // out of memory
    return NULL;
  }
  ss->key1 = one;
  ss->key2 = two;
  return ss;
}

void FreeSection2Struct(Section2StructPtr ss) {
  free(ss);
}

int main(int argc, char** argv) {
  Section2StructPtr ss = Section2StructConstruct(5, 6);
  printf("Struct Address: %p, with key1 = %d and key2 = %d\n", ss, ss->key1, ss->key2);
  return EXIT_SUCCESS;
}


