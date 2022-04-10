#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

ShmkByte_t* shmk_mem_allocate(ShmkMem_t* mem, size_t size) { return (mem->allocate)(mem, size); }

/* Heap */

ShmkChunk_t shmk_heap;

ShmkByte_t* shmk_heap_allocate(ShmkMem_t* mem, size_t size) {
  ShmkChunk_t* chunk = (ShmkChunk_t*)mem;
  if (chunk->used + size > chunk->capacity) {
    fprintf(stderr, "Error: memory is out\n");
    return NULL;
  }
  ShmkByte_t* res = chunk->data + chunk->used;
  chunk->used += size;
  return res;
}

/* Pool */

ShmkChunk_t shmk_pool;

ShmkByte_t* shmk_pool_allocate(ShmkMem_t* mem, size_t size) {
  ShmkChunk_t* chunk = (ShmkChunk_t*)mem;
  if (chunk->used + size > chunk->capacity) {
    size_t new_capacity = 2 * chunk->capacity + size;
    ShmkByte_t* new_data = (ShmkByte_t*)realloc(chunk->data, new_capacity);
    if (new_data == NULL) {
      fprintf(stderr, "Error: memory is out\n");
      return NULL;
    }
    chunk->data = new_data;
    chunk->capacity = new_capacity;
  }
  ShmkByte_t* res = chunk->data + chunk->used;
  chunk->used += size;
  return res;
}