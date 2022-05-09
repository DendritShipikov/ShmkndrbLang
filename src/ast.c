#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "object.h"
#include "ast.h"
#include "compiler.h"

AST_Expr_t *create_literal(Object_t *obj) {
  AST_Expr_t *expr = (AST_Expr_t *)meta_alloc(sizeof(AST_Expr_t));
  if (expr == NULL) {
    return NULL;
  }
  expr->compile = compile_literal;
  expr->v.obj = obj;
  return expr;
}

AST_Expr_t *create_name(const char *begin, const char *end) {
  AST_Expr_t *expr = (AST_Expr_t *)meta_alloc(sizeof(AST_Expr_t));
  if (expr == NULL) {
    return NULL;
  }
  expr->compile = compile_name;
  expr->v.name.begin = begin;
  expr->v.name.end = end;
  return expr;
}

AST_Expr_t *create_binop(AST_Expr_t *left, AST_Expr_t *right, char kind) {
  AST_Expr_t *expr = (AST_Expr_t *)meta_alloc(sizeof(AST_Expr_t));
  if (expr == NULL) {
    return NULL;
  }
  expr->compile = compile_binop;
  expr->v.binop.left = left;
  expr->v.binop.right = right;
  expr->v.binop.kind = kind;
  return expr;
}

AST_Expr_t *create_call(AST_Expr_t *callable, AST_List_t *args) {
  AST_Expr_t *expr = (AST_Expr_t *)meta_alloc(sizeof(AST_Expr_t));
  if (expr == NULL) {
    return NULL;
  }
  expr->compile = compile_call;
  expr->v.call.callable = callable;
  expr->v.call.args = args;
  return expr;
}

AST_Expr_t *create_lambda(AST_List_t *params, AST_Expr_t *body) {
  AST_Expr_t *expr = (AST_Expr_t *)meta_alloc(sizeof(AST_Expr_t));
  if (expr == NULL) {
    return NULL;
  }
  expr->compile = compile_lambda;
  expr->v.lambda.params = params;
  expr->v.lambda.body = body;
  return expr;
}