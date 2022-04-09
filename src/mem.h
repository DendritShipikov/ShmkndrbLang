#ifndef SHMK_MEM_H
#define SHMK_MEM_H

typedef unsigned char ShmkByte_t;

typedef struct ShmkMem {
  ShmkByte_t* begin;
  ShmkByte_t* top;
  ShmkByte_t* end;
  int (*grow)(struct ShmkMem*);
} ShmkMem_t;

ShmkByte_t* shmk_mem_alloc(ShmkMem_t* mem, size_t size);
int shmk_mem_init(ShmkMem_t* mem, size_t init_size);
void shmk_mem_free(ShmkMem_t* mem);

extern ShmkMem_t shmk_heap;

extern ShmkMem_t shmk_pool;

#endif