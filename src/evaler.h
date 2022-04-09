#ifndef SHMK_EVALER_H
#define SHMK_EVALER_H

typedef struct ShmkFrame {
  ShmkObject_t* function;
  struct ShmkFrame* back;
  ShmkCodeUnit_t* current;
  size_t itop;
  size_t allocated_size;
  ShmkObject_t* locals[1];
} ShmkFrame_t;

typedef struct ShmkEvaler {
  ShmkMem_t mem;
  ShmkFrame_t* frame;
} ShmkEvaler_t;

void shmk_evaler_eval(ShmkEvaler_t* evaler);

#endif