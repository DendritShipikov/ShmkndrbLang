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

extern ShmkMem_t heap;

int shmk_create_heap(size_t size);
void shmk_delete_heap();

#endif