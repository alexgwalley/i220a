#include "y86.h"
#include "yas.h"

#include "y86-util.h"

#include "ysim.h"
#include "stall-sim.h"

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

/************************** Disassembler ********************************/

typedef enum {
  NO_ARG,
  REGA_ARG,
  REGB_ARG,
  IMMED_ARG,
  REGB_DISP_ARG,
  ADDR_ARG
} ArgType;

typedef void OpLabelFn(Byte opByte, const char *baseLabel, char *buf);

const char *conds[] = { "", "le", "l", "e", "ne", "ge", "g", };
static void
cond_label(Byte opByte, const char *baseLabel, char *buf)
{
  const Byte fn = get_nybble(opByte, 0);
  assert(fn < sizeof(conds)/sizeof(conds[0]));
  if (fn == 0) {
    const char *op = (strcmp(baseLabel, "j") == 0) ? "jmp" : "rrmovq";
    strcat(buf, op);
  }
  else {
    strcat(buf, baseLabel);
    strcat(buf, conds[fn]);
  }
}

static const char *ops[] = { "addq", "subq", "andq", "xorq", };
static void
op1_label(Byte opByte, const char *baseLabel, char *buf)
{
  const Byte fn = get_nybble(opByte, 0);
  assert(fn < sizeof(ops)/sizeof(ops[0]));
  strcat(buf, ops[fn]);
}

static void
base_label(Byte opByte, const char *baseLabel, char *buf)
{
  strcat(buf, baseLabel);
}

typedef struct {
  BaseOpCode op;
  const char *label;
  OpLabelFn *labelFn;
  ArgType arg1;
  ArgType arg2;
} OpInfo;



static OpInfo opInfos[] = {
  { .op = HALT_CODE,
    .label = "halt",
    .labelFn = base_label,
    .arg1 = NO_ARG,
    .arg2 = NO_ARG,
  },
  { .op = NOP_CODE,
    .label = "nop",
    .labelFn = base_label,
    .arg1 = NO_ARG,
    .arg2 = NO_ARG,
  },
  { .op = CMOVxx_CODE,
    .label = "cmov",
    .labelFn = cond_label,
    .arg1 = REGA_ARG,
    .arg2 = REGB_ARG,
  },
  { .op = IRMOVQ_CODE,
    .label = "irmovq",
    .labelFn = base_label,
    .arg1 = IMMED_ARG,
    .arg2 = REGB_ARG,
  },
  { .op = RMMOVQ_CODE,
    .label = "rmmovq",
    .labelFn = base_label,
    .arg1 = REGA_ARG,
    .arg2 = REGB_DISP_ARG,
  },
  { .op = MRMOVQ_CODE,
    .label = "mrmovq",
    .labelFn = base_label,
    .arg1 = REGB_DISP_ARG,
    .arg2 = REGA_ARG,
  },
  { .op = OP1_CODE,
    .label = "",
    .labelFn = op1_label,
    .arg1 = REGA_ARG,
    .arg2 = REGB_ARG,
  },
  { .op = Jxx_CODE,
    .label = "j",
    .labelFn = cond_label,
    .arg1 = ADDR_ARG,
    .arg2 = NO_ARG,
  },
  { .op = CALL_CODE,
    .label = "call",
    .labelFn = base_label,
    .arg1 = ADDR_ARG,
    .arg2 = NO_ARG,
  },
  { .op = RET_CODE,
    .label = "ret",
    .labelFn = base_label,
    .arg1 = NO_ARG,
    .arg2 = NO_ARG,
  },
  { .op = PUSHQ_CODE,
    .label = "pushq",
    .labelFn = base_label,
    .arg1 = REGA_ARG,
    .arg2 = NO_ARG,
  },
  { .op = POPQ_CODE,
    .label = "popq",
    .labelFn = base_label,
    .arg1 = REGA_ARG,
    .arg2 = NO_ARG,
  },
};

const char *regs[] = {
  "%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi",
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14",
};

static void
append_reg(Y86 *y86, int nybblePos, char buf[])
{
  Address pc = read_pc_y86(y86);
  const Byte regByte = read_memory_byte_y86(y86, pc + 1);
  assert(read_status_y86(y86) == STATUS_AOK);
  const Byte regN = get_nybble(regByte, nybblePos);
  assert(regN < sizeof(regs)/sizeof(regs[0]));
  strcat(buf, regs[regN]);
}

static void
append_op_word(Y86 *y86, Word pcDisp, char buf[])
{
  Address pc = read_pc_y86(y86);
  const Word word = read_memory_word_y86(y86, pc + pcDisp);
  assert(read_status_y86(y86) == STATUS_AOK);
  char *p = buf + strlen(buf);
  sprintf(p, "$0x%lx", word);
}
static void
append_arg(Y86 *y86, ArgType arg, char buf[])
{
  switch (arg) {
    case NO_ARG:
      break;
    case REGA_ARG:
      append_reg(y86, 1, buf);
      break;
    case REGB_ARG:
      append_reg(y86, 0, buf);
      break;
    case IMMED_ARG:
      append_op_word(y86, 2, buf);
      break;
    case REGB_DISP_ARG:
      append_op_word(y86, 2, buf);
      strcat(buf, "(");
      append_reg(y86, 0, buf);
      strcat(buf, ")");
      break;
    case ADDR_ARG:
      append_op_word(y86, 1, buf);
      break;
    default:
      assert(0);
  }
}

//assume that buf is large enough: no overflow checking
static const char *
dis_yas(Y86 *y86, char buf[])
{
  const Word pc = read_pc_y86(y86);
  const Byte op = read_memory_byte_y86(y86, pc);
  assert(read_status_y86(y86) == STATUS_AOK);
  const Byte baseOp = get_nybble(op, 1);
  assert(baseOp < sizeof(opInfos)/sizeof(opInfos[0]));
  const OpInfo *opInfo = &opInfos[baseOp];
  buf[0] = '\0';
  opInfo->labelFn(op, opInfo->label, buf);
  strcat(buf, "\t");
  append_arg(y86, opInfo->arg1, buf);
  if (opInfo->arg2 != NO_ARG) {
    strcat(buf, ", ");
    append_arg(y86, opInfo->arg2, buf);
  }
  return buf;
}
/*************************** Main Simulation ****************************/

static void
simulate(const Args *args, Y86 *y86, FILE *out)
{
  enum { DIS_YAS_BUF_SIZE = 80 };
  StallSim *stallSim = new_stall_sim(y86);
  setup_params(args, y86);
  bool isRunning = true;
  bool isVeryVerbose = (args->verbosity == VERY_VERBOSE);
  int clockN = 0;
  //fprintf(out, "%10s \t%6s\t  %s\n", "CLOCK #", "PC", "OP");
  while (isRunning) {
    fprintf(out, "%4d:\t%04lx\t", clockN++, read_pc_y86(y86));
    Address pc = read_pc_y86(y86);
    if (clock_stall_sim(stallSim)) {
      char buf[DIS_YAS_BUF_SIZE];
      fprintf(out, "%s\n", dis_yas(y86, buf));
      step_ysim(y86);
    }
    else {
      fprintf(out, "bubble\n");
    }
    isRunning = read_status_y86(y86) == STATUS_AOK;
    if (isRunning) {
      if (isVeryVerbose) {
        fprintf(out, "pc: %0*lx\n", (int)sizeof(Address)*2, pc);
        dump_changes_y86(y86, false, out);
        fprintf(out, "\n");
      }
      if (args->isStep) {
        char line[80];
        fgets(line, sizeof(line), stdin);
      }
    }
  }
  if (args->verbosity != SILENT_VERBOSE) dump_changes_y86(y86, true, out);
  free_stall_sim(stallSim);
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
          "          -v:  verbose: dump state at completion\n"
          "          -V:  very verbose: dump changes after each "
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
