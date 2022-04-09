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

int shmk_mem_init(ShmkMem_t* mem, size_t init_size) {
  mem->top = mem->begin = (ShmkByte_t*)malloc(init_size);
  if (mem->begin == NULL) {
    fprintf(stderr, "Error: memory is out");
    return 0;
  }
  mem->end = mem->begin + init_size;
  return 1;
}

void shmk_mem_free(ShmkMem_t* mem) { free(mem->begin); }

/* Heap */

static int heap_grow(ShmkMem_t* mem) {
  fprintf(stderr, "Error: memory is out");
  return 0;
}

ShmkMem_t shmk_heap = {
  NULL,
  NULL,
  NULL,
  heap_grow
};

/* Pool */

static int pool_grow(ShmkMem_t* mem) {
  size_t top_offset = mem->top - mem->begin;
  size_t size = mem->end - mem->begin;
  size_t new_size = 2 * size;
  ShmkByte_t* begin = (ShmkByte_t*)realloc(mem->begin, new_size);
  if (begin == NULL) {
    fprintf(stderr, "Error: memory is out");
    return 0;
  }
  mem->begin = begin;
  mem->top = begin + top_offset;
  mem->end = begin + new_size;
  return 1;
}

ShmkMem_t shmk_pool = {
  NULL,
  NULL,
  NULL,
  pool_grow
};
