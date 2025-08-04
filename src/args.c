#include "args.h"
#include <stdio.h>
#include <string.h>

void init_driver_options(driver_options *opt) {
  opt->lex = false;
  opt->parse = false;
  opt->validate = false;
  opt->codegen = false;
  opt->s = false;
  opt->c = false;
  opt->output = NULL;
  opt->input = NULL;
}

void print_help() {
  printf("Usage: ascc [options] input\n\n");
  printf("Compilation mode flags (optional, mutually exclusive):\n");
  printf("  --lex              Stop after lexer\n");
  printf("  --parse            Stop after parser\n");
  printf("  --validate         Stop after semantic analysis\n");
  printf("  --sema             Alias for --validate\n");
  printf("  --codegen          Stop after TAC codegen\n");
  printf("  --tac              Alias for --codegen\n");
  printf("  -S, -s             Stop after ASM generation (.s file)\n");
  printf("  -c, -C             Stop after assembling (.o file)\n");
  printf("  (If no mode flag is given, the full compile pipeline runs)\n\n");

  printf("Other options:\n");
  printf("  -o <file>          Set output file name\n");
  printf("  --output <file>    Same as -o\n");
  printf("  -h, --help         Show this help message\n\n");

  printf("Positional arguments:\n");
  printf("  input              Path to input source file (required)\n");
}

void print_options(driver_options *opt) {
  printf("Driver Options:\n");

  printf("  Mode:\n");
  if (opt->lex) {
    printf("    Stop after: lexer (--lex)\n");
  } else if (opt->parse) {
    printf("    Stop after: parser (--parse)\n");
  } else if (opt->validate) {
    printf("    Stop after: semantic analysis (--validate / --sema)\n");
  } else if (opt->codegen) {
    printf("    Stop after: TAC codegen (--codegen / --tac)\n");
  } else if (opt->s) {
    printf("    Stop after: assembly generation (-S / -s)\n");
  } else if (opt->c) {
    printf("    Stop after: object file generation (-c / -C)\n");
  } else {
    printf("    Full pipeline (no stop flag given)\n");
  }

  printf("  Output file: %s\n", opt->output ? opt->output : "(none)");
  printf("  Input file:  %s\n", opt->input ? opt->input : "(none)");
}

int parse_options(driver_options *opt, int argc, char *argv[]) {
  bool *mode_flag = NULL;

  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];

    // mutually exclusive flags
    if (strcmp(arg, "--lex") == 0) {
      if (mode_flag)
        goto conflict;
      opt->lex = true;
      mode_flag = &opt->lex;
    } else if (strcmp(arg, "--parse") == 0) {
      if (mode_flag)
        goto conflict;
      opt->parse = true;
      mode_flag = &opt->parse;
    } else if (strcmp(arg, "--validate") == 0 || strcmp(arg, "--sema") == 0) {
      if (mode_flag)
        goto conflict;
      opt->validate = true;
      mode_flag = &opt->validate;
    } else if (strcmp(arg, "--codegen") == 0 || strcmp(arg, "--tac") == 0) {
      if (mode_flag)
        goto conflict;
      opt->codegen = true;
      mode_flag = &opt->codegen;
    } else if (strcmp(arg, "-S") == 0 || strcmp(arg, "-s") == 0) {
      if (mode_flag)
        goto conflict;
      opt->s = true;
      mode_flag = &opt->s;
    } else if (strcmp(arg, "-c") == 0 || strcmp(arg, "-C") == 0) {
      if (mode_flag)
        goto conflict;
      opt->c = true;
      mode_flag = &opt->c;
    } else if (strcmp(arg, "-o") == 0 || strcmp(arg, "--output") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "Missing argument for %s\n", arg);
        return 0;
      }
      opt->output = argv[++i];
    } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
      print_help();
      return 0;
    } else if (!opt->input) {
      opt->input = arg;
    } else {
      fprintf(stderr, "Unexpected argument: %s\n", arg);
      return 0;
    }
  }

  if (!opt->input) {
    fprintf(stderr, "Error: no input file specified\n");
    return 0;
  }

  return 1;

conflict:
  fprintf(stderr, "Error: only one mode flag (--lex, --parse, --validate, "
                  "--codegen, -S, -c) may be used at a time\n");
  return 0;
}
