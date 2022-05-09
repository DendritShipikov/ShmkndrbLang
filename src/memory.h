#ifndef SHMK_MEMORY_H
#define SHMK_MEMORY_H

typedef unsigned char Byte_t;

Byte_t *heap_alloc(size_t size);
Byte_t *meta_alloc(size_t size);

#endif