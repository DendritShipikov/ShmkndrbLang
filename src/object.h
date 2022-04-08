#ifndef SHMK_OBJECT_H
#define SHMK_OBJECT_H

struct ShmkVTable;

typedef struct ShmkObject {
  struct ShmkVTable* vtable;
  int is_marked;
  size_t allocated_size;
} ShmkObject_t;

struct ShmkVTable {
  void (*print)(ShmkObject_t*);
  ShmkObject_t* (*add)(ShmkObject_t*, ShmkObject_t*);
  ShmkObject_t* (*sub)(ShmkObject_t*, ShmkObject_t*);
  ShmkObject_t* (*mul)(ShmkObject_t*, ShmkObject_t*);
};

/* Integer */

typedef struct ShmkInteger {
  ShmkObject_t base;
  int data;
} ShmkInteger_t;

ShmkInteger_t* new_integer(int data);

extern struct ShmkVTable shmk_integer_vtable;

/* Array */

typedef struct ShmkArray {
  ShmkObject_t base;
  ShmkObject_t* objects[1];
} ShmkArray_t;

ShmkArray_t* new_array(size_t size);

extern struct ShmkVTable shmk_array_vtable;

/* Vector */

typedef struct ShmkVector {
  ShmkObject_t base;
  ShmkArray_t* array;
  size_t size;
} ShmkVector_t;

ShmkVector_t* new_vector(size_t capacity);

extern struct ShmkVTable shmk_vector_vtable;

#endif