#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "memory.h"
#include "object.h"
#include "ast.h"
#include "parser.h"

typedef struct Tok {
  const char *begin;
  const char *end;
  int kind;
} Tok_t;

int next_tok(Tok_t *tok) {
  while (isspace(*tok->end)) ++tok->end;
  tok->begin = tok->end;
  switch (*tok->end) {
    case '\0':
      return tok->kind = 0;
    case '+':
    case '-':
    case '*':
    case '(':
    case ')':
    case '=':
    case ';':
    case ':':
      return tok->kind = *tok->end++;
    default:
      break;
  }
  if (isdigit(*tok->end)) {
    do {
      ++tok->end;
    } while (isdigit(*tok->end));
    return tok->kind = '0';
  }
  if (isalpha(*tok->end)) {
    do {
      ++tok->end;
    } while (isalpha(*tok->end));
    return tok->kind = 'a';
  }
  return -1;
}

AST_Expr_t *parse_expr(Tok_t *tok);
AST_Expr_t *parse_term(Tok_t *tok);
AST_Expr_t *parse_prim(Tok_t *tok);

AST_Expr_t *parse_expr(Tok_t *tok) {
  AST_Expr_t *expr = parse_term(tok);
  if (expr == NULL) {
    return NULL;
  }
  while (tok->kind == '+' || tok->kind == '-') {
    char kind = tok->kind;
    next_tok(tok);
    AST_Expr_t *term = parse_term(tok);
    if (term == NULL) {
      return NULL;
    }
    expr = create_binop(expr, term, kind);
    if (expr == NULL) {
      return NULL;
    }
  }
  return expr;
}

AST_Expr_t *parse_term(Tok_t *tok) {
  AST_Expr_t *term = parse_prim(tok);
  if (term == NULL) {
    return NULL;
  }
  while (tok->kind == '*') {
    char kind = tok->kind;
    next_tok(tok);
    AST_Expr_t *prim = parse_prim(tok);
    if (prim == NULL) {
      return NULL;
    }
    term = create_binop(term, prim, kind);
    if (term == NULL) {
      return NULL;
    }
  }
  return term;
}

AST_Expr_t *parse_prim(Tok_t *tok) {
  AST_Expr_t *expr;
  Integer_t *integer;
  int data;
  switch (tok->kind) {
    case '0':
      data = 0;
      for (const char *p = tok->begin; p != tok->end; ++p) {
        data = 10 * data + *p - '0';
      }
      integer = create_integer(data);
      if (integer == NULL) {
        return NULL;
      }
      expr = create_literal((Object_t *)integer);
      if (expr == NULL) {
        return NULL;
      }
      next_tok(tok);
      return expr;
    case 'a':
      expr = create_name(tok->begin, tok->end);
      if (expr == NULL) {
        return NULL;
      }
      next_tok(tok);
      return expr;
    case '(':
      next_tok(tok);
      expr = parse_expr(tok);
      if (expr == NULL) {
        return NULL;
      }
      if (tok->kind != ')') {
        fprintf(stderr, "Parser error: ')' expected\n");
        return NULL;
      }
      next_tok(tok);
      return expr;
    default:
      fprintf(stderr, "Parser error: wrong prim\n");
      return NULL;
  }
}

AST_Expr_t *parse(const char *source) {
  Tok_t tok = { NULL, source, -1 };
  next_tok(&tok);
  AST_Expr_t *expr = parse_expr(&tok);
  return expr;
}