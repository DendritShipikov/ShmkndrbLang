#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "object.h"
#include "evaler.h"

ShmkByte_t* shmk_heap_begin = NULL;
ShmkByte_t* shmk_heap_end = NULL;
ShmkByte_t* shmk_heap_top = NULL;

#define MARKED(o) ((o)->move_to != NULL)
#define UNMARKED(o) ((o)->move_to == NULL)
#define MARK(o) (o)->move_to = (ShmkObject_t*)1
#define UNMARK(o) (o)->move_to = NULL

void mark_closure_work(ShmkClosure_t* cl, ShmkObject_t** pp) {
  ShmkObject_t* obj = *pp;
  if (UNMARKED(obj)) {
    MARK(obj);
    shmk_object_atho(obj, cl);
  }
}

void update_pointers_closure_work(ShmkClosure_t* cl, ShmkObject_t** pp) {
  ShmkObject_t* obj = *pp;
  *pp = obj->move_to;
}

void shmk_gc() {
  ShmkClosure_t mark_closure = { mark_closure_work };
  shmk_evaler_atho(&mark_closure);
  ShmkByte_t* top = shmk_heap_begin;
  ShmkByte_t* cp = top;
  while (top < shmk_heap_end) {
    ShmkObject_t* obj = (ShmkObject_t*)top;
    size_t size = shmk_object_allocated(obj);
    if (MARKED(obj)) {
      obj->move_to = (ShmkObject_t*)cp;
      cp += size;
    }
    top += size;
  }
  top = shmk_heap_begin;
  ShmkClosure_t update_pointers_closure = { update_pointers_closure_work };
  while (top < shmk_heap_end) {
    ShmkObject_t* obj = (ShmkObject_t*)top;
    size_t size = shmk_object_allocated(obj);
    if (MARKED(obj)) shmk_object_atho(obj, &update_pointers_closure);
    top += size;
  }
  shmk_evaler_atho(&update_pointers_closure);
  top = shmk_heap_begin;
  while (top < shmk_heap_end) {
    ShmkObject_t* obj = (ShmkObject_t*)top;
    size_t size = shmk_object_allocated(obj);
    ShmkObject_t* move_to = obj->move_to;
    memcpy(move_to, obj, size);
    UNMARK(move_to);
    top += size;
  }
  shmk_heap_top = top;
}

ShmkByte_t* shmk_heap_allocate(size_t size) {
  ShmkByte_t* mem = shmk_heap_top;
  shmk_heap_top += size;
  if (shmk_heap_top <= shmk_heap_end) return mem;
  shmk_heap_top -= size;
  shmk_gc();
  mem = shmk_heap_top;
  shmk_heap_top += size;
  if (shmk_heap_top > shmk_heap_end) {
    fprintf(stderr, "Error: heap memory is out\n");
    return NULL;
  }
  return mem;
}