#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "object.h"

Byte_t *meta_alloc(size_t size) {
  Byte_t *p = (Byte_t *)malloc(size);
  if (p == NULL) {
    fprintf(stderr, "Error: memory is out\n");
    return NULL;
  }
  return p;
}

Byte_t *heap_alloc(size_t size) {
  Byte_t *p = (Byte_t *)malloc(size);
  if (p == NULL) {
    fprintf(stderr, "Error: memory is out\n");
    return NULL;
  }
  return p;
}
