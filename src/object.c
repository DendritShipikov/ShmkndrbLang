#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "object.h"

size_t shmk_object_allocated(ShmkObject_t* obj) { return (obj->vtable->allocated)(obj); }

void shmk_object_apply_to_each(ShmkObject_t* obj, ShmkClosure_t* cl) {
  void (*apply_to_each)(ShmkObject_t*, ShmkClosure_t*);
  apply_to_each = obj->vtable->apply_to_each;
  if (apply_to_each != NULL) apply_to_each(obj, cl);
}

int shmk_object_print(ShmkObject_t* obj) {
  void (*print)(ShmkObject_t*);
  print = obj->vtable->print;
  if (print == NULL) {
    fprintf(stderr, "'print' N/I\n");
    return 0;
  }
  print(obj);
  return 1;
}

ShmkObject_t* shmk_object_add(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkObject_t* (*add)(ShmkObject_t*, ShmkObject_t*);
  add = left->vtable->add;
  if (add == NULL) {
    fprintf(stderr, "'add' N/I\n");
    return 0;
  }
  return add(left, right);
}

ShmkObject_t* shmk_object_sub(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkObject_t* (*sub)(ShmkObject_t*, ShmkObject_t*);
  sub = left->vtable->sub;
  if (sub == NULL) {
    fprintf(stderr, "'add' N/I\n");
    return 0;
  }
  return sub(left, right);
}

ShmkObject_t* shmk_object_mul(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkObject_t* (*mul)(ShmkObject_t*, ShmkObject_t*);
  mul = left->vtable->mul;
  if (mul == NULL) {
    fprintf(stderr, "'add' N/I\n");
    return 0;
  }
  return mul(left, right);
}

/* Integer */

void shmk_integer_construct(ShmkInteger_t* integer, int data) {
  integer->base.vtable = &shmk_integer_vtable;
  integer->data = data;
}

static size_t integer_allocated(ShmkObject_t* obj) { return sizeof(ShmkInteger_t); }

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
  ShmkInteger_t* result = (ShmkInteger_t*)shmk_heap_allocate(sizeof(ShmkInteger_t));
  if (result == NULL) return NULL;
  shmk_integer_construct(result, self->data + other->data);
  return (ShmkObject_t*)result;
}

static ShmkObject_t* integer_sub(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkInteger_t* self = (ShmkInteger_t*)left;
  if (right->vtable != &shmk_integer_vtable) {
    fprintf(stderr, "Integer sub: unsupported type of right operand\n");
    return NULL;
  }
  ShmkInteger_t* other = (ShmkInteger_t*)right;
  ShmkInteger_t* result = (ShmkInteger_t*)shmk_heap_allocate(sizeof(ShmkInteger_t));
  if (result == NULL) return NULL;
  shmk_integer_construct(result, self->data + other->data);
  return (ShmkObject_t*)result;
}

static ShmkObject_t* integer_mul(ShmkObject_t* left, ShmkObject_t* right) {
  ShmkInteger_t* self = (ShmkInteger_t*)left;
  if (right->vtable != &shmk_integer_vtable) {
    fprintf(stderr, "Integer mul: unsupported type of right operand\n");
    return NULL;
  }
  ShmkInteger_t* other = (ShmkInteger_t*)right;
  ShmkInteger_t* result = (ShmkInteger_t*)shmk_heap_allocate(sizeof(ShmkInteger_t));
  if (result == NULL) return NULL;
  shmk_integer_construct(result, self->data + other->data);
  return (ShmkObject_t*)result;
}

struct ShmkVTable shmk_integer_vtable = {
  integer_allocated,
  NULL,
  integer_print,
  integer_add,
  integer_sub,
  integer_mul
};

/* Array */

void shmk_array_construct(ShmkArray_t* array, size_t size) {
  array->base.vtable = &shmk_array_vtable;
  array->size = size;
}

static size_t array_allocated(ShmkObject_t* obj) {
  ShmkArray_t* array = (ShmkArray_t*)obj;
  return SHMK_ARRAY_STA(array->size);
}

static void array_apply_to_each(ShmkObject_t* obj, ShmkClosure_t* cl) {
  ShmkArray_t* array = (ShmkArray_t*)obj;
  void (*work)(ShmkClosure_t*, ShmkObject_t**);
  work = cl->work;
  for (size_t i = 0; i < array->size; ++i) work(cl, &array->objects[i]);
}

struct ShmkVTable shmk_array_vtable = {
  array_allocated,
  array_apply_to_each,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Code */

void shmk_code_construct(ShmkCode_t* code, size_t size) {
  code->base.vtable = &shmk_code_vtable;
  code->size = size;
}

static size_t code_allocated(ShmkObject_t* obj) {
  ShmkCode_t* code = (ShmkCode_t*)obj;
  return SHMK_CODE_STA(code->size);
}

struct ShmkVTable shmk_code_vtable = {
  code_allocated,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Function */

void shmk_function_construct(ShmkFunction_t* function, ShmkCode_t* code, size_t nargs, size_t ncaptures) {
  function->base.vtable = &shmk_function_vtable;
  function->nargs = nargs;
  function->ncaptures = ncaptures;
  function->code = code;
}

static size_t function_allocated(ShmkObject_t* obj) {
  ShmkFunction_t* function = (ShmkFunction_t*)obj;
  return SHMK_FUNCTION_STA(function->ncaptures);
}

static void function_apply_to_each(ShmkObject_t* obj, ShmkClosure_t* cl) {
  ShmkFunction_t* function = (ShmkFunction_t*)obj;
  void (*work)(ShmkClosure_t*, ShmkObject_t**);
  work = cl->work;
  work(cl, (ShmkObject_t**)&function->code);
  for (size_t i = 0; i < function->ncaptures; ++i) work(cl, &function->captures[i]);
}

struct ShmkVTable shmk_function_vtable = {
  function_allocated,
  function_apply_to_each,
  NULL,
  NULL,
  NULL,
  NULL
};
