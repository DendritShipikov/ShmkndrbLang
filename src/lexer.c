#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "lexer.h"

int lexer_next_token(ShmkLexer_t* lex) {
  while (isspace(*lex->cur)) ++lex->cur;
  lex->tok = lex->cur;
  switch (*lex->cur) {
    case '\0':
      return lex->kind = 0;
    case '+':
    case '-':
    case '*':
    case '(':
    case ')':
    case ':':
    case ';':
      return lex->kind = *lex->cur++;
    default:
      break;
  }
  if (isdigit(*lex->cur)) {
    while (isdigit(*lex->cur)) ++lex->cur;
    return lex->kind = '0';
  } else if (isalpha(*lex->cur)) {
    while (isalpha(*lex->cur)) ++lex->cur;
    return lex->kind = 'a';
  }
  fprintf(stderr, "Lexer error: unknown token\n");
  return -1;
}