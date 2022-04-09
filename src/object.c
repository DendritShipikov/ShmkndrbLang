#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "object.h"

int shmk_object_print(ShmkObject_t* obj) {
  void (*print)(ShmkObject_t*);
  print = obj->vtable->print;
  if (print == NULL) {
    fprintf(stderr, "'print' N/I");
    return 0;
  }
  print(obj);
  return 1;
}

ShmkObject_t* shmk_object_add(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkObject_t* (*add)(ShmkObject_t*, ShmkObject_t*);
  add = left->vtable->add;
  if (add == NULL) {
    fprintf(stderr, "'add' N/I");
    return 0;
  }
  return add(left, right);
}

ShmkObject_t* shmk_object_sub(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkObject_t* (*sub)(ShmkObject_t*, ShmkObject_t*);
  sub = left->vtable->sub;
  if (sub == NULL) {
    fprintf(stderr, "'add' N/I");
    return 0;
  }
  return sub(left, right);
}

ShmkObject_t* shmk_object_mul(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkObject_t* (*mul)(ShmkObject_t*, ShmkObject_t*);
  mul = left->vtable->mul;
  if (mul == NULL) {
    fprintf(stderr, "'add' N/I");
    return 0;
  }
  return mul(left, right);
}

/* Integer */

ShmkInteger_t* new_integer(ShmkMem_t* mem, int data) {
  ShmkInteger_t* integer = (ShmkInteger_t*)shmk_mem_alloc(mem, sizeof(ShmkInteger_t));
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
  return (ShmkObject_t*)new_integer(&shmk_heap, self->data + other->data);
}

static ShmkObject_t* integer_sub(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkInteger_t* self = (ShmkInteger_t*)left;
  if (right->vtable != &shmk_integer_vtable) {
    fprintf(stderr, "Integer sub: unsupported type of right operand\n");
    return NULL;
  }
  ShmkInteger_t* other = (ShmkInteger_t*)right;
  return (ShmkObject_t*)new_integer(&shmk_heap, self->data - other->data);
}

static ShmkObject_t* integer_mul(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkInteger_t* self = (ShmkInteger_t*)left;
  if (right->vtable != &shmk_integer_vtable) {
    fprintf(stderr, "Integer mul: unsupported type of right operand\n");
    return NULL;
  }
  ShmkInteger_t* other = (ShmkInteger_t*)right;
  return (ShmkObject_t*)new_integer(&shmk_heap, self->data * other->data);
}

struct ShmkVTable shmk_integer_vtable = {
  integer_print,
  integer_add,
  integer_sub,
  integer_mul
};

/* Array */

ShmkArray_t* new_array(ShmkMem_t* mem, size_t size) {
  size_t allocated_size = offsetof(ShmkArray_t, objects) + size * sizeof(ShmkObject_t*);
  ShmkArray_t* array = (ShmkArray_t*)shmk_mem_alloc(mem, allocated_size);
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

/* Code */

ShmkCode_t* new_code(ShmkMem_t* mem, size_t size) {
  size_t allocated_size = offsetof(ShmkCode_t, units) + size * sizeof(ShmkCodeUnit_t);
  ShmkCode_t* code = (ShmkCode_t*)shmk_mem_alloc(mem, allocated_size);
  if (code == NULL) return NULL;
  code->base.vtable = &shmk_code_vtable;
  code->base.allocated_size = allocated_size;
  return code;
}

struct ShmkVTable shmk_code_vtable = {
  NULL,
  NULL,
  NULL,
  NULL
};

/* Function */

ShmkFunction_t* new_function(ShmkMem_t* mem, ShmkCode_t* code, size_t args_count, size_t captures_count) {
  size_t allocated_size = offsetof(ShmkFunction_t, captures) + captures_count * sizeof(ShmkObject_t*);
  ShmkFunction_t* function = (ShmkFunction_t*)shmk_mem_alloc(mem, allocated_size);
  if (function == NULL) return NULL;
  function->base.vtable = &shmk_function_vtable;
  function->base.allocated_size = allocated_size;
  function->args_count = args_count;
  function->code = code;
  return function;
}

struct ShmkVTable shmk_function_vtable = {
  NULL,
  NULL,
  NULL,
  NULL
};
