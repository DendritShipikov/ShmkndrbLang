#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

ShmkByte_t* shmk_mem_alloc(ShmkMem_t* mem, size_t size) {
  if (mem->top + size > mem->end) {
    int (*grow)(ShmkMem_t*);
    grow = mem->grow;
    if (!grow(mem)) return NULL;
  }
  ShmkByte_t* obj = mem->top;
  mem->top += size;
  return obj;
}

static int heap_grow(ShmkMem_t* mem) {
  fprintf(stderr, "Error: memory is out");
  return 0;
}

ShmkMem_t heap = {
  NULL,
  NULL,
  NULL,
  heap_grow
};

int shmk_create_heap(size_t size) {
  heap.begin = (ShmkByte_t*)malloc(size);
  if (heap.begin == NULL) {
    fprintf(stderr, "Error: memory is out, can not create heap");
    return 0;
  }
  heap.top = heap.begin;
  heap.end = heap.begin + size;
  return 1;
}

void shmk_delete_heap() {
  free(heap.begin);
}
