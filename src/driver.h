#ifndef ASCC_ARGS_H
#define ASCC_ARGS_H

#include "common.h"
#include "string.h"

typedef struct _driver_options driver_options;

struct _driver_options {
  bool lex;      // stop after lexer                      | --lex
  bool parse;    // stop after parser                     | --parse
  bool validate; // stop after sema                       | --validate, --sema
  bool codegen;  // stop after TAC codegen                | --codegen, --tac
  bool s;        // stop after asm gen (emits .s file)    | -S, -s
  bool c;        // stop after assembling (emits .o file) | -c, -C

  const char *output; // output file name, NULL if output file is not specified
                      // | -o <name>, --output <name>
  const char *input;  // input file path
};

void init_driver_options(driver_options *opt);
void print_options(driver_options *opt);
void print_help();

// parses options, returns 1 if succeeded
int parse_options(driver_options *opt, int argc, char *argv[]);

#endif
