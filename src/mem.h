#ifndef SHMK_MEM_H
#define SHMK_MEM_H

int shmk_create_heap();
void* shmk_alloc(size_t size);

#endif