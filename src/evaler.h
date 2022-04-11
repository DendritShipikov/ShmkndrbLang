#ifndef SHMK_EVALER_H
#define SHMK_EVALER_H

#define SHMK_STACK_SIZE 16384

typedef struct ShmkFrame {
  struct ShmkFrame* back;
  ShmkCodeUnit_t* pc;
  ShmkFunction_t* function;
  ShmkObject_t** locals;
} ShmkFrame_t;

int shmk_evaler_eval();
void shmk_evaler_aoth(ShmkClosure_t* cl);

#endif