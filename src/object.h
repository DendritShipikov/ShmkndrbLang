#ifndef SHMK_OBJECT_H
#define SHMK_OBJECT_H

struct ShmkVTable;

typedef struct ShmkObject {
  struct ShmkVTable* vtable;
  struct ShmkObject* move_to;
} ShmkObject_t;

typedef struct ShmkClosure {
  void (*work)(struct ShmkClosure*, ShmkObject_t**);
} ShmkClosure_t;

struct ShmkVTable {
  size_t (*allocated)(ShmkObject_t*);
  void (*apply_to_each)(ShmkObject_t*, ShmkClosure_t*); // apply to held objects
  void (*print)(ShmkObject_t*);
  ShmkObject_t* (*add)(ShmkObject_t*, ShmkObject_t*);
  ShmkObject_t* (*sub)(ShmkObject_t*, ShmkObject_t*);
  ShmkObject_t* (*mul)(ShmkObject_t*, ShmkObject_t*);
};

size_t shmk_object_allocated(ShmkObject_t* obj);
void shmk_object_apply_to_each(ShmkObject_t* obj, ShmkClosure_t* cl);
int shmk_object_print(ShmkObject_t* obj);
ShmkObject_t* shmk_object_add(ShmkObject_t* left, ShmkObject_t* right);
ShmkObject_t* shmk_object_sub(ShmkObject_t* left, ShmkObject_t* right);
ShmkObject_t* shmk_object_mul(ShmkObject_t* left, ShmkObject_t* right);

/* Integer */

typedef struct ShmkInteger {
  ShmkObject_t base;
  int data;
} ShmkInteger_t;

void shmk_integer_construct(ShmkInteger_t* integer, int data);

extern struct ShmkVTable shmk_integer_vtable;

/* Array */

typedef struct ShmkArray {
  ShmkObject_t base;
  size_t size;
  ShmkObject_t* objects[1];
} ShmkArray_t;

#define SHMK_ARRAY_STA(size) (offsetof(ShmkArray_t, objects) + (size) * sizeof(ShmkObject_t*))

void shmk_array_construct(ShmkArray_t* array, size_t size);

extern struct ShmkVTable shmk_array_vtable;

/* Code */

typedef unsigned short ShmkCodeUnit_t;

typedef struct ShmkCode {
  ShmkObject_t base;
  size_t size;
  size_t nlocals;
  ShmkCodeUnit_t units[1];
} ShmkCode_t;

#define SHMK_CODE_STA(size) (offsetof(ShmkCode_t, units) + (size) * sizeof(ShmkCodeUnit_t))

void shmk_code_construct(ShmkCode_t* code, size_t size);

extern struct ShmkVTable shmk_code_vtable;

/* Function */

typedef struct ShmkFunction {
  ShmkObject_t base;
  size_t nargs;
  size_t ncaptures;
  ShmkCode_t* code;
  ShmkObject_t* captures[1];
} ShmkFunction_t;

#define SHMK_FUNCTION_STA(ncaptures) (offsetof(ShmkFunction_t, captures) + (ncaptures) * sizeof(ShmkObject_t*))

void shmk_function_construct(ShmkFunction_t* function, ShmkCode_t* code, size_t nargs, size_t ncaptures);

extern struct ShmkVTable shmk_function_vtable;

#endif