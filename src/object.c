#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "object.h"

Object_t *create_object(size_t size) {
  Object_t *obj = (Object_t *)heap_alloc(size);
  if (obj == NULL) {
    return NULL;
  }
  obj->size = size;
  return obj;
}

Integer_t *create_integer(int data) {
  Integer_t *integer = (Integer_t *)create_object(sizeof(Integer_t));
  if (integer == NULL) {
    return NULL;
  }
  integer->data = data;
  return integer;
}