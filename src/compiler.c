#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "object.h"
#include "ast.h"
#include "parser.h"
#include "opcodes.h"

typedef struct SymMapEntry {
  struct {
    const char *begin;
    const char *end;
  } name;
  size_t index;
  struct SymMapEntry *back;
} SymMapEntry_t;

typedef struct SymMap {
  SymMapEntry_t *entry;
} SymMap_t;

SymMapEntry_t *sym_map_get(SymMap_t *sym_map, const char *begin, const char *end) {
  for (SymMapEntry_t *entry = sym_map->entry; entry != NULL; entry = entry->back) {
    const char *q = entry->name.begin;
    const char *p = begin;
    for (;;) {
      if (q == entry->name.end && p == end) {
        return entry;
      }
      if (q == entry->name.end || p == end) {
        break;
      }
      if (*p != *q) {
        break;
      }
      ++p;
      ++q;
    }
  }
  return NULL;
}

typedef struct CodeBuilder {
  size_t size;
  CodeUnit_t *segments[1];
} CodeBuilder_t;

CodeBuilder_t *create_code_builder() {
  CodeBuilder_t *builder = (CodeBuilder_t *)meta_alloc(offsetof(CodeBuilder_t, segments) + sizeof(CodeUnit_t *) * 64);
  if (builder == NULL) {
    return NULL;
  }
  builder->size = 0;
  return builder;
}

int code_builder_addop(CodeBuilder_t *builder, CodeUnit_t unit) {
  size_t size = builder->size;
  size_t isegment = size >> 10;
  size_t offset = size & 1023;
  if (offset == 0) {
    if (isegment >= 64) {
      return 0;
    }
    CodeUnit_t *segment = (CodeUnit_t *)meta_alloc(1024);
    if (segment == NULL) {
      return 0;
    }
    builder->segments[isegment] = segment;
  }
  *(builder->segments[isegment] + offset) = unit;
  return 1;
}

void code_builder_set(CodeBuilder_t *builder, size_t i, CodeUnit_t unit) {
  *(builder->segments[i >> 10] + (i & 1023)) = unit;
}

typedef struct Consts {
  struct Consts *back;
  Object_t *obj;
} Consts_t;

typedef struct Compiler {
  SymMap_t *locals_map;
  //SymMap_t *caps_map;
  Consts_t *consts;
  size_t nconsts;
  CodeBuilder_t *builder;
  int ret;
} Compiler_t;

int compiler_add_const(Compiler_t *compiler, Object_t *obj) {
  Consts_t *consts = (Consts_t *)meta_alloc(sizeof(Consts_t));
  if (consts == NULL) {
    return 0;
  }
  consts->back = compiler->consts;
  consts->obj = obj;
  compiler->consts = consts;
  return 1;
}

#define ADDOP(C, OP) if (!code_builder_addop((C)->builder, OP)) return 0

int compile_literal(struct Compiler *compiler, AST_Expr_t *expr) {
  if (!compiler_add_const(compiler, expr->v.obj)) {
    return 0;
  }
  ADDOP(compiler, PUSH_CONST);
  ADDOP(compiler, compiler->nconsts++);
  if (compiler->ret) {
    ADDOP(compiler, RETURN_VALUE);
  }
  return 1;
}

int compile_name(struct Compiler *compiler, AST_Expr_t *expr) {
  SymMapEntry_t *entry = sym_map_get(compiler->locals_map, expr->v.name.begin, expr->v.name.end);
  if (entry != NULL) {
    ADDOP(compiler, PUSH_LOCAL);
    ADDOP(compiler, entry->index);
  }
  if (compiler->ret) {
    ADDOP(compiler, RETURN_VALUE);
  }
  return 1;
}

int compile_binop(struct Compiler *, AST_Expr_t *expr) {
  return 0;
}

int compile_call(struct Compiler *, AST_Expr_t *expr) {
  return 0;
}

int compile_lambda(struct Compiler *, AST_Expr_t *expr) {
  return 0;
}
