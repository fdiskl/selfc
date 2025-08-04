#include "arena.h"
#include "driver.h"
#include "lexer.h"
#include "string.h"
#include <assert.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
  init_arena(&string_arena);

  driver_options opts;
  init_driver_options(&opts);
  if (!parse_options(&opts, argc, argv)) {
    return 1;
  }

#ifdef DEBUG_INFO
  print_options(&opts);
#endif

  FILE *in = fopen(opts.input, "r");
  assert(in != NULL);

  lexer l;
  init_lexer(&l, in);

  if (opts.lex) {
    token tok;
    while (scan(&l, &tok))
      print_token(&tok);
  }

  return 0;
}
