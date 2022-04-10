#ifndef SHMK_EVALER_H
#define SHMK_EVALER_H

#define SHMK_STACK_SIZE 16384

typedef struct ShmkFrame {
  struct ShmkFrame* back;
  ShmkCodeUnit_t* pc;
  ShmkFunction_t* function;
  ShmkObject_t* locals[1];
} ShmkFrame_t;

typedef struct ShmkEvaler {
  ShmkByte_t* begin;
  ShmkByte_t* end;
  ShmkObject_t** sp;
  ShmkFrame_t* frame;
} ShmkEvaler_t;

int shmk_evaler_eval(ShmkEvaler_t* evaler);

#endif