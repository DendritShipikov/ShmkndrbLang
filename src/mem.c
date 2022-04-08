#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

typedef char ShmkHeapUnit_t;

struct {
  ShmkHeapUnit_t* mem;
  ShmkHeapUnit_t* top;
  ShmkHeapUnit_t* end;
} heap;

#define MINIMAL_HEAP_SIZE 1000

int shmk_heap_create() {
  heap.mem = (ShmkHeapUnit_t*)malloc(MINIMAL_HEAP_SIZE);
  if (heap.mem == NULL) {
    fprintf(stderr, "Error: memory is out, cannot create heap");
    return 0;
  }
  heap.top = heap.mem;
  heap.end = heap.mem + MINIMAL_HEAP_SIZE;
  return 1;
}

void* shmk_heap_alloc(size_t size) {
  if (heap.top + size > heap.end) {
    fprintf(stderr, "Error: out of memory\n");
    return NULL;
  }
  void* obj = heap.top;
  heap.top += size;
  return obj;
}
