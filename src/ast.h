#ifndef SHMK_AST_H
#define SHMK_AST_H

struct Compiler;

typedef struct AST_List {
  struct AST_List *next;
  union {
    struct AST_Expr *expr;
    struct AST_Stmt *stmt;
    struct {
      const char *begin;
      const char *end;
    } name;
  } v;
} AST_List_t;

typedef struct AST_Expr {
  int (*compile)(struct Compiler *, struct AST_Expr *);
  union {
    Object_t *obj;
    struct {
      const char *begin;
      const char *end;
    } name;
    struct {
      struct AST_Expr *left;
      struct AST_Expr *right;
      char kind;
    } binop;
    struct {
      struct AST_Expr *callable;
      AST_List_t *args;
    } call;
    struct {
      AST_List_t* params;
      struct AST_Expr *body;
    } lambda;
  } v;
} AST_Expr_t;

typedef struct AST_Stmt {
  int (*compile)(struct Compiler *, struct AST_Stmt *);
  union {
    AST_Expr_t *expr;
    struct {
      struct {
        const char *begin;
        const char *end;
      } name;
      AST_Expr_t *expr;
    } def;
  } v;
} AST_Stmt_t;

AST_Expr_t *create_literal(Object_t *obj);
AST_Expr_t *create_name(const char *begin, const char *end);
AST_Expr_t *create_binop(AST_Expr_t *left, AST_Expr_t *right, char kind);
AST_Expr_t *create_call(AST_Expr_t *callable, AST_List_t *args);
AST_Expr_t *create_lambda(AST_List_t *params, AST_Expr_t *body);

#endif