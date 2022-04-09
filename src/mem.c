#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

ShmkByte_t* shmk_chunk_bump(ShmkChunk_t* chunk, size_t size) {
  if (chunk->used + size > chunk->capacity) return NULL;
  ShmkByte_t* top = chunk->data + chunk->used;
  chunk->used += size;
  return top;
}

ShmkByte_t* shmk_mem_allocate(ShmkMem_t* mem, size_t size) { return (mem->allocate)(mem, size); }

/* Heap */

ShmkMem_t shmk_heap;

/* Pool */

static ShmkByte_t* shmk_pool_allocate(ShmkMem_t* mem, size_t size) {
  ShmkChunk_t* chunk = &mem->chunk;
  ShmkByte_t* res = shmk_chunk_bump(chunk, size);
  if (res != NULL) return res;
  size_t new_capacity = 2 * chunk->capacity + size;
  ShmkByte_t* new_data = (ShmkByte_t*)realloc(chunk->data, new_capacity);
  if (chunk->data == NULL) {
    fprintf(stderr, "Error: memory is out\n");
    return NULL;
  }
  chunk->capacity = new_capacity;
  chunk->data = new_data;
  return shmk_chunk_bump(chunk, size);
}

ShmkMem_t shmk_pool;