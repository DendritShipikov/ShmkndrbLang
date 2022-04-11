#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "object.h"
#include "evaler.h"
#include "opcodes.h"

ShmkByte_t* shmk_evaler_mem = NULL;
ShmkObject_t** shmk_evaler_sp = NULL;
ShmkFrame_t* shmk_evaler_frame = NULL;

#define CHECK_OVERFLOW if ((ShmkByte_t*)shmk_evaler_sp < shmk_evaler_mem) { fprintf(stderr, "Error: stack overflow\n"); return 0; }
#define STACK_POP() *shmk_evaler_sp++
#define STACK_PUSH(o)      \
  do {                     \
    shmk_evaler_sp--;      \
    CHECK_OVERFLOW         \
    *shmk_evaler_sp = o;   \
  } while (0)
#define NEXT_OPCODE *shmk_evaler_frame->pc++

int shmk_evaler_eval() {
  while (shmk_evaler_frame != NULL) {
    switch (NEXT_OPCODE) {
      case OP_INVOKE: {
        size_t nargs = NEXT_OPCODE;
        ShmkObject_t* obj = STACK_POP();
        if (obj->vtable != &shmk_function_vtable) { fprintf(stderr, "Error: op_invoke, object is not a function\n"); return 0; }
        ShmkFunction_t* function = (ShmkFunction_t*)obj;
        if (nargs != function->nargs) { fprintf(stderr, "Error: op_invoke, nargs != function.nargs\n"); return 0; }
        ShmkCode_t* code = function->code;
        ShmkObject_t** locals = shmk_evaler_sp += nargs;
        size_t frame_size = sizeof(ShmkFrame_t) + code->nlocals * sizeof(ShmkObject_t*);
        shmk_evaler_sp = (ShmkObject_t**)((ShmkByte_t*)locals - frame_size);
        CHECK_OVERFLOW
        ShmkFrame_t* frame = (ShmkFrame_t*)shmk_evaler_sp;
        frame->function = function;
        frame->pc = code->units;
        frame->locals = locals;
        frame->back = shmk_evaler_frame;
        shmk_evaler_frame = frame;
        break;
      }
      case OP_RETURN_VALUE: {
        ShmkObject_t* obj = STACK_POP();
        shmk_evaler_sp = shmk_evaler_frame->locals;
        shmk_evaler_frame = shmk_evaler_frame->back;
        STACK_PUSH(obj);
        break;
      }
      case OP_PUSH_LOCAL: {
        size_t ilocal = NEXT_OPCODE;
        ShmkObject_t* obj = shmk_evaler_frame->locals[-ilocal];
        STACK_PUSH(obj);
        break;
      }
      case OP_PUSH_CAPTURE: {
        size_t icapture = NEXT_OPCODE;
        ShmkObject_t* obj = shmk_evaler_frame->function->captures[icapture];
        STACK_PUSH(obj);
        break;
      }
      case OP_STORE_LOCAL: {
        size_t ilocal = NEXT_OPCODE;
        shmk_evaler_frame->locals[-ilocal] = STACK_POP();
        break;
      }
      case OP_ADD: {
        ShmkObject_t* right = STACK_POP();
        ShmkObject_t* left = STACK_POP();
        ShmkObject_t* obj = shmk_object_add(left, right);
        if (obj == NULL) return 0;
        STACK_PUSH(obj);
        break;
      }
      case OP_SUB: {
        ShmkObject_t* right = STACK_POP();
        ShmkObject_t* left = STACK_POP();
        ShmkObject_t* obj = shmk_object_sub(left, right);
        if (obj == NULL) return 0;
        STACK_PUSH(obj);
        break;
      }
      case OP_MUL: {
        ShmkObject_t* right = STACK_POP();
        ShmkObject_t* left = STACK_POP();
        ShmkObject_t* obj = shmk_object_mul(left, right);
        if (obj == NULL) return 0;
        STACK_PUSH(obj);
        break;
      }
      case OP_PRINT: {
        ShmkObject_t* obj = STACK_POP();
        if (!shmk_object_print(obj)) return 0;
        break;
      }
      case OP_MAKE_FUNCTION: {
        size_t nargs = NEXT_OPCODE;
        size_t ncaptures = NEXT_OPCODE;
        ShmkObject_t* obj = STACK_POP();
        if (obj->vtable != &shmk_code_vtable) { fprintf(stderr, "Error: op_make_function, object is not a code\n"); return 0; }
        ShmkCode_t* code = (ShmkCode_t*)obj;
        ShmkFunction_t* function = (ShmkFunction_t*)shmk_heap_allocate(SHMK_FUNCTION_STA(ncaptures));
        if (function == NULL) return 0;
        shmk_function_construct(function, code, nargs, ncaptures);
        for (size_t i = 0; i < ncaptures; ++i) function->captures[i] = STACK_POP();
        STACK_PUSH((ShmkObject_t*)function);
        break;
      }
      default:
        fprintf(stderr, "Error: unknown opcode\n");
        return 0;
    }
  }
  return 1;
}

void shmk_evaler_atho(ShmkClosure_t* cl) {
  void (*work)(ShmkClosure_t*, ShmkObject_t**);
  work = cl->work;
  for (ShmkFrame_t* frame = shmk_evaler_frame; frame != NULL; frame = frame->back) {
    work(cl, (ShmkObject_t**)&frame->function);
    ShmkObject_t** iter = (ShmkObject_t**)((ShmkByte_t*)frame + sizeof(ShmkFrame_t));
    ShmkObject_t** end = frame->locals;
    for (; iter != end; ++iter) work(cl, iter);
  }
}