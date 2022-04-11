#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

ShmkByte_t* shmk_heap_begin = NULL;
ShmkByte_t* shmk_heap_end = NULL;
ShmkByte_t* shmk_heap_top = NULL;

ShmkByte_t* shmk_heap_allocate(size_t size) {
  ShmkByte_t* mem = shmk_heap_top;
  shmk_heap_top += size;
  if (shmk_heap_top > shmk_heap_end) {
    fprintf(stderr, "Error: heap memory is out");
    return NULL;
  }
  return mem;
}