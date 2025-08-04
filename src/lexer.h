#ifndef ASCC_LEXER_H
#define ASCC_LEXER_H

#include "string.h"
#include <stdio.h>
typedef struct _token token;
typedef struct _lexer lexer;

// scan input and return next token.
// returns 1 if token was found, 0 if not tokens left (EOF)
int scan(lexer *l, token *t);

struct _lexer {
  int line;
  char putback;
  int pos;
  FILE *f;
};

struct _token {
  int token;
  int line;
  int pos_start;
  int pos_end;
  union {
    int intval;
    string ident;
  } v;
};

enum {
  T_IDENT,  // <ident> (foo, bar)
  T_INTLIT, // <int-lit> (123, 555)

  T_INT,    // int
  T_RETURN, // return
  T_VOID,   // void

  T_LPAREN, // (
  T_RPAREN, // )
  T_LBRACE, // {
  T_RBRACE, // }
  T_SEMI,   // ;
};

#endif
