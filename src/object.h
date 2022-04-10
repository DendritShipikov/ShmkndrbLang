#ifndef SHMK_OBJECT_H
#define SHMK_OBJECT_H

struct ShmkVTable;

typedef struct ShmkObject {
  struct ShmkVTable* vtable;
  int is_marked;
} ShmkObject_t;

struct ShmkVTable {
  size_t (*allocated)(ShmkObject_t*);
  void (*print)(ShmkObject_t*);
  ShmkObject_t* (*add)(ShmkObject_t*, ShmkObject_t*);
  ShmkObject_t* (*sub)(ShmkObject_t*, ShmkObject_t*);
  ShmkObject_t* (*mul)(ShmkObject_t*, ShmkObject_t*);
};

size_t shmk_object_allocated(ShmkObject_t* obj);
int shmk_object_print(ShmkObject_t* obj);
ShmkObject_t* shmk_object_add(ShmkObject_t* left, ShmkObject_t* right);
ShmkObject_t* shmk_object_sub(ShmkObject_t* left, ShmkObject_t* right);
ShmkObject_t* shmk_object_mul(ShmkObject_t* left, ShmkObject_t* right);

/* Integer */

typedef struct ShmkInteger {
  ShmkObject_t base;
  int data;
} ShmkInteger_t;

ShmkInteger_t* new_integer(ShmkMem_t* mem, int data);

extern struct ShmkVTable shmk_integer_vtable;

/* Array */

typedef struct ShmkArray {
  ShmkObject_t base;
  size_t size;
  ShmkObject_t* objects[1];
} ShmkArray_t;

ShmkArray_t* new_array(ShmkMem_t* mem, size_t size);

extern struct ShmkVTable shmk_array_vtable;

/* Code */

typedef unsigned short ShmkCodeUnit_t;

typedef struct ShmkCode {
  ShmkObject_t base;
  size_t size;
  size_t nlocals;
  ShmkCodeUnit_t units[1];
} ShmkCode_t;

ShmkCode_t* new_code(ShmkMem_t* mem, size_t size);

extern struct ShmkVTable shmk_code_vtable;

/* Function */

typedef struct ShmkFunction {
  ShmkObject_t base;
  size_t nargs;
  size_t ncaptures;
  ShmkCode_t* code;
  ShmkObject_t* captures[1];
} ShmkFunction_t;

ShmkFunction_t* new_function(ShmkMem_t* mem, ShmkCode_t* code, size_t nargs, size_t ncaptures);

extern struct ShmkVTable shmk_function_vtable;

#endif