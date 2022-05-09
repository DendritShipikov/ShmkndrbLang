#ifndef SHMK_COMPILER_H
#define SHMK_COMPILER_H

int compile_literal(struct Compiler *, AST_Expr_t *expr);
int compile_name(struct Compiler *, AST_Expr_t *expr);
int compile_binop(struct Compiler *, AST_Expr_t *expr);
int compile_call(struct Compiler *, AST_Expr_t *expr);
int compile_lambda(struct Compiler *, AST_Expr_t *expr);

void compile(AST_Expr_t *expr);

#endif