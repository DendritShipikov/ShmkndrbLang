#ifndef SHMK_MEM_H
#define SHMK_MEM_H

typedef unsigned char ShmkByte_t;

/* Mem (basic allocator interface) */

typedef struct ShmkMem {
  ShmkByte_t* (*allocate)(struct ShmkMem*, size_t);
} ShmkMem_t;

ShmkByte_t* shmk_mem_allocate(ShmkMem_t* mem, size_t size);

/* Chunk */

typedef struct ShmkChunk {
  ShmkMem_t base;
  ShmkByte_t* data;
  size_t used;
  size_t capacity;
} ShmkChunk_t;

extern ShmkChunk_t shmk_heap;

extern ShmkChunk_t shmk_pool;

#endif