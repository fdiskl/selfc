#include "lexer.h"
#include "common.h"
#include "string.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ident_buf[MAX_IDENT_LEN + 1];

// get next char from input string
static char next(lexer *l) {
  char c;

  if (l->putback) {
    c = l->putback;
    l->putback = 0;
    return c;
  }

  c = fgetc(l->f);
  if (c != EOF)
    ++l->pos;

  if (c == '\n')
    ++l->line;

  return c;
}

// put back unwanted char for future use
static void putback(lexer *l, char c) {
  if (c != 0 && c != EOF)
    --l->pos;
  l->putback = c;
}

static int skip_whitespaces(lexer *l) {
  char c;

  c = next(l);

  while (1)
    switch (c) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
    case '\f':
      break;
    default:
      goto stop;
    }

stop:
  return c;
}

// return pos of character c in string s, or -1 if not found
static int chrpos(char *s, int c) {
  char *p;

  p = strchr(s, c);
  return (p ? p - s : -1);
}

// scan and return an int literal
static int scan_int(lexer *l, int c) {
  int k, val = 0;

  // Convert each character into an int value
  while ((k = chrpos("0123456789", c)) >= 0) {
    val = val * 10 + k;
    c = next(l);
  }

  putback(l, c);
  return val;
}

// scan ident and write to the given buf
static int scan_ident(lexer *l, char c, char *buf, int buf_len) {
  int i = 0;

  while (isalpha(c) || isdigit(c) || c == '_') {
    if (buf_len - 1 == i) {
      fprintf(stderr, "identifier is too long, on line %d", l->line);
      exit(1);
    } else if (i < buf_len - 1)
      buf[i++] = c;
    c = next(l);
  }

  putback(l, c);
  buf[i] = '\0';
  return i;
}

// returns token type if string inside buf is recognised keyword
// or 0 if string isn't keyword
static int keyword(char *buf) {
  switch (*buf) {
  case 'i':
    return T_INT;
  case 'r':
    return T_RETURN;
  case 'v':
    return T_VOID;
  }

  return 0;
}

int scan(lexer *l, token *t) {
  char c;

  c = skip_whitespaces(l);

  t->line = l->line;
  t->pos_start = l->pos - 1;

  switch (c) {
  case EOF:
    return 0;
  case '(':
    t->token = T_LPAREN;
    break;
  case ')':
    t->token = T_RPAREN;
    break;
  case '{':
    t->token = T_LBRACE;
    break;
  case '}':
    t->token = T_RBRACE;
    break;
  default:
    if (isdigit(c)) {
      t->v.intval = scan_int(l, c);
      t->token = T_INTLIT;
      break;
    } else if (isalpha(c) || c == '_') {
      scan_ident(l, c, ident_buf, MAX_IDENT_LEN);

      if ((t->token = keyword(ident_buf)))
        break;

      t->token = T_IDENT;
      t->v.ident = new_string(ident_buf);
    }

    fprintf(stderr, "unrecognised char %c on line %d", c, l->line);
    exit(1);
  }

  t->pos_end = l->pos - 1;
  return 0;
}
