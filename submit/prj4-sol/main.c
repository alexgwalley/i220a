#include "y86.h"
#include "yas.h"
#include "ysim.h"

#include "errors.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
  int numFileNames;
  const char **fileNames;
  int numParams;
  Word *params;
  int verbosity;
  bool isStep;
  bool isList;
} Args;

enum { SILENT_VERBOSE, VERBOSE, VERY_VERBOSE };

/**************************** Y86 Parameter Setup ***********************/


static void
setup_params(const Args *args, Y86 *y86)
{
  Word argc = args->numParams;
  if (argc > 0) {
    Address top = get_memory_size_y86(y86);
    Address argv = top - argc * sizeof(Word);
    for (int i = 0; i < argc; i++) {
      const Address argvi = argv + i * sizeof(Word);
      printf("argvi = %08lx\n", argvi);
      write_memory_word_y86(y86, argvi, args->params[i]);
      assert(read_status_y86(y86) == STATUS_AOK);
    }
    write_register_y86(y86, REG_RDI, argc);
    write_register_y86(y86, REG_RSI, argv);
  }
}

/*************************** Main Simulation ****************************/

static void
simulate(const Args *args, Y86 *y86, FILE *out)
{
  setup_params(args, y86);
  bool isRunning = true;
  bool isVeryVerbose = (args->verbosity == VERY_VERBOSE);
  while (isRunning) {
    Address pc = read_pc_y86(y86);
    step_ysim(y86);
    isRunning = read_status_y86(y86) == STATUS_AOK;
    if (isRunning) {
      if (args->verbosity != SILENT_VERBOSE) {
        fprintf(out, "pc: %0*lx\n", (int)sizeof(Address)*2, pc);
        dump_changes_y86(y86, isVeryVerbose, out);
        fprintf(out, "\n");
      }
      if (args->isStep) {
        char line[80];
        fgets(line, sizeof(line), stdin);
      }
    }
  }
  dump_changes_y86(y86, true, out);
}


/************************* Parse Command Line **************************/

static void
usage(const char *prog)
{
  fprintf(stderr,
          "usage: %s [-s] [-v] [-V] YAS_FILE_NAMES... INT_INPUTS...\n", prog);
  fprintf(stderr,
          "          -l:  produce assembler listing only\n"
          "          -s:  single-step program\n"
          "          -v:  verbose: dump changes after each instruction\n"
          "          -V:  very verbose: dump all registers after each "
          "instruction\n");
  exit(1);
}


static void
first_pass_args(int argc, const char *argv[], Args *args)
{
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      args->verbosity = (args->verbosity > VERBOSE) ? args->verbosity : VERBOSE;
    }
    else if (strcmp(argv[i], "-V") == 0) {
      args->verbosity = VERY_VERBOSE;
    }
    else if (strcmp(argv[i], "-s") == 0) {
      args->isStep = true;
    }
    else if (strcmp(argv[i], "-l") == 0) {
      args->isList = true;
    }
    else if (argv[i][0] == '-' && !isdigit(argv[i][1])) {
      fprintf(stderr, "unknown option '%s'\n", argv[i]);
      usage(argv[0]);
    }
    else if (isdigit(argv[i][0]) ||
             (argv[i][0] == '-' && isdigit(argv[i][1]))) {
      args->numParams++;
    }
    else {
      args->numFileNames++;
    }
  }
  if (args->numFileNames == 0) {
    fprintf(stderr, "no files specified\n");
    usage(argv[0]);
  }
}

static void
second_pass_args(int argc, const char *argv[], Args *args)
{
  args->numFileNames = args->numParams = 0;
  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (arg[0] == '-' && !isdigit(arg[1])) {
      continue;
    }
    else if (isdigit(arg[0]) || (arg[0] == '-' && isdigit(arg[1]))) {
      char *p;
      args->params[args->numParams++] = strtol(arg, &p, 0);
      if (*p != '\0') {
        fprintf(stderr, "bad parameter '%s'\n", arg);
        usage(argv[0]);
      }
    }
    else {
      args->fileNames[args->numFileNames++] = arg;
    }
  }
}

int
main(int argc, const char *argv[])
{
  if (argc == 1) {
    usage(argv[0]);
  }
  Args args;
  memset(&args, 0, sizeof(args));
  first_pass_args(argc, argv, &args);
  const char *fileNames[args.numFileNames];
  Word params[args.numParams];
  args.fileNames = fileNames; args.params = params;
  second_pass_args(argc, argv, &args);
  if (args.isList) {
    yas_to_listing(stdout, args.numFileNames, args.fileNames);
  }
  else {
    Y86 *y86 = new_y86_default();
    if (yas_to_y86(y86, args.numFileNames, args.fileNames)) {
      simulate(&args, y86, stdout);
    }
    free_y86(y86);
  }
}
