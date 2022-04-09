#ifndef SHMK_EVALER_H
#define SHMK_EVALER_H

typedef unsigned short CodeUnit_t;

typedef struct ShmkFrame {
  ShmkObject_t* function;
  struct ShmkFrame* back;
  CodeUnit_t* current;
  ShmkObject_t* locals[1];
} ShmkFrame_t;

#endif