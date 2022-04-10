#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "object.h"
#include "evaler.h"
#include "opcodes.h"

#define CHECK_OVERFLOW(e) if ((ShmkByte_t*)evaler->sp > evaler->end) { fprintf(stderr, "Error: stack overflow\n"); return e; }
#define STACK_POP() *(--evaler->sp)
#define STACK_PUSH(o)                    \
  do {                                   \
    ShmkObject_t** pp = evaler->sp++;    \
    CHECK_OVERFLOW(0)                    \
    *pp = o;                             \
  } while (0)
#define NEXT_OPCODE *(evaler->frame->pc)++

ShmkFrame_t* shmk_evaler_create_frame(ShmkEvaler_t* evaler, ShmkFunction_t* function) {
  ShmkCode_t* code = function->code;
  size_t frame_size = offsetof(ShmkFrame_t, locals) + code->nlocals * sizeof(ShmkObject_t*);
  size_t nargs = function->nargs;
  evaler->sp -= nargs;
  ShmkFrame_t* frame = (ShmkFrame_t*)evaler->sp;
  ShmkObject_t** args = evaler->sp;
  evaler->sp += frame_size;
  CHECK_OVERFLOW(NULL)
  for (size_t i = 0; i != nargs; ++i) frame->locals[nargs - 1 - i] = args[nargs - 1 - i];
  frame->back = evaler->frame;
  frame->function = function;
  frame->pc = code->units;
  evaler->frame = frame;
  return frame;
}

int shmk_evaler_eval(ShmkEvaler_t* evaler) {
  while (evaler->frame != NULL) {
    switch (NEXT_OPCODE) {
      case OP_INVOKE: {
        size_t nargs = NEXT_OPCODE;
        ShmkObject_t* obj = STACK_POP();
        if (obj->vtable != &shmk_function_vtable) { fprintf(stderr, "Error: op_invoke, object is not a function\n"); return 0; }
        ShmkFunction_t* function = (ShmkFunction_t*)obj;
        if (nargs != function->nargs) { fprintf(stderr, "Error: op_invoke, nargs != function.nargs\n"); return 0; }
        ShmkFrame_t* frame = shmk_evaler_create_frame(evaler, function);
        if (frame == NULL) return 0;
        break;
      }
      case OP_RETURN_VALUE: {
        ShmkObject_t* obj = STACK_POP();
        evaler->sp = (ShmkObject_t**)evaler->frame;
        evaler->frame = evaler->frame->back;
        STACK_PUSH(obj);
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
      default:
        fprintf(stderr, "Error: unknown opcode\n");
        return 0;
    }
  }
  return 1;
}
