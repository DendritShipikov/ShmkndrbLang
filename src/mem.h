#ifndef SHMK_MEM_H
#define SHMK_MEM_H

typedef unsigned char ShmkByte_t;

typedef struct ShmkChunk {
  ShmkByte_t* data;
  size_t used;
  size_t capacity;
} ShmkChunk_t;

ShmkByte_t* shmk_chunk_bump(ShmkChunk_t* chunk, size_t size);

typedef struct ShmkMem {
  ShmkChunk_t chunk;
  ShmkByte_t* (*allocate)(struct ShmkMem*, size_t);
} ShmkMem_t;

ShmkByte_t* shmk_mem_allocate(ShmkMem_t* mem, size_t size);

extern ShmkMem_t shmk_heap;

extern ShmkMem_t shmk_pool;

#endif