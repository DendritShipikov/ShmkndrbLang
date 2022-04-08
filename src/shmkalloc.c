#include <stdlib.h>
#include <stdio.h>

typedef char ShmkHeapUnit_t;

struct {
  ShmkHeapUnit_t* mem;
  ShmkHeapUnit_t* top;
  ShmkHeapUnit_t* end;
} heap;

void* shmk_alloc(size_t size) {
  if (heap.top + size > heap.end) {
    fprintf(stderr, "Error: out of memory\n");
    return NULL;
  }
  void* obj = heap.top;
  heap.top += size;
  return obj;
}
