#ifndef SHMK_LEXER_H
#define SHMK_LEXER_H

typedef struct ShmkLexer {
  const char* cur;
  const char* tok;
  int kind;
} ShmkLexer_t;

int lexer_next_token(ShmkLexer_t* lex);

#endif