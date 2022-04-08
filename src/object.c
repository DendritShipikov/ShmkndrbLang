#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "object.h"

/* Integer */

ShmkInteger_t* new_integer(int data) {
  ShmkInteger_t* integer = (ShmkInteger_t*)shmk_alloc(sizeof(ShmkInteger_t));
  if (integer == NULL) return NULL;
  integer->base.vtable = &shmk_integer_vtable;
  integer->base.allocated_size = sizeof(ShmkInteger_t);
  integer->data = data;
  return integer;
}

static void integer_print(ShmkObject_t* obj) {
  ShmkInteger_t* self = (ShmkInteger_t*)obj;
  printf("%d\n", self->data);
}

static ShmkObject_t* integer_add(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkInteger_t* self = (ShmkInteger_t*)left;
  if (right->vtable != &shmk_integer_vtable) {
    fprintf(stderr, "Integer add: unsupported type of right operand\n");
    return NULL;
  }
  ShmkInteger_t* other = (ShmkInteger_t*)right;
  return (ShmkObject_t*)new_integer(self->data + other->data);
}

static ShmkObject_t* integer_sub(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkInteger_t* self = (ShmkInteger_t*)left;
  if (right->vtable != &shmk_integer_vtable) {
    fprintf(stderr, "Integer sub: unsupported type of right operand\n");
    return NULL;
  }
  ShmkInteger_t* other = (ShmkInteger_t*)right;
  return (ShmkObject_t*)new_integer(self->data - other->data);
}

static ShmkObject_t* integer_mul(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkInteger_t* self = (ShmkInteger_t*)left;
  if (right->vtable != &shmk_integer_vtable) {
    fprintf(stderr, "Integer mul: unsupported type of right operand\n");
    return NULL;
  }
  ShmkInteger_t* other = (ShmkInteger_t*)right;
  return (ShmkObject_t*)new_integer(self->data * other->data);
}

struct ShmkVTable shmk_integer_vtable = {
  integer_print,
  integer_add,
  integer_sub,
  integer_mul
};

/* Array */

ShmkArray_t* new_array(size_t size) {
  size_t allocated_size = offsetof(ShmkArray_t, objects) + size * sizeof(ShmkObject_t*);
  ShmkArray_t* array = (ShmkArray_t*)shmk_alloc(allocated_size);
  if (array == NULL) return NULL;
  array->base.vtable = &shmk_array_vtable;
  array->base.allocated_size = allocated_size;
  return array;
}

struct ShmkVTable shmk_array_vtable = {
  NULL,
  NULL,
  NULL,
  NULL
};

/* Vector */

ShmkVector_t* new_vector(size_t capacity) {
  size_t allocated_size = sizeof(ShmkVector_t);
  ShmkVector_t* vector = (ShmkVector_t*)shmk_alloc(allocated_size);
  if (vector == NULL) return NULL;
  vector->base.vtable = &shmk_vector_vtable;
  vector->base.allocated_size = allocated_size;
  vector->array = new_array(capacity);
  if (vector->array == NULL) return NULL;
  vector->size = 0;
  return vector;
}

struct ShmkVTable shmk_vector_vtable = {
  NULL,
  NULL,
  NULL,
  NULL
};
