#ifndef SHMK_OBJECT_H
#define SHMK_OBJECT_H

typedef struct Object {
  struct Object *reserved;
  size_t size;
} Object_t;

#define OBJECT_HEAD Object_t base;

typedef struct Variable {
  OBJECT_HEAD
  Object_t *obj;
} Variable_t;

typedef struct Integer {
  OBJECT_HEAD
  int data;
} Integer_t;

typedef unsigned char CodeUnit_t;

typedef struct Code {
  OBJECT_HEAD
  size_t nlocals;
  size_t depth;
  size_t nunits;
  CodeUnit_t units[1];
} Code_t;

typedef struct Function {
  OBJECT_HEAD
  Code_t *code;
  size_t nargs;
  size_t ncaps;
  Object_t *caps[1];
} Function_t;

Integer_t *create_integer(int data);

#endif