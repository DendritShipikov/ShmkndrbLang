#ifndef SHMK_MEM_H
#define SHMK_MEM_H

typedef unsigned char ShmkByte_t;

ShmkByte_t* shmk_heap_allocate(size_t size);

#endif