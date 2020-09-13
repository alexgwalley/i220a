#include "hamming.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** Read whitespace-separated HammingWord's from stream in.  If
 *  doDecode is non-zero, then hamming-decode them onto stream out;
 *  otherwise hamming-encode them on stream out.
 *  If isVerbose (must be isDecode), then output a * after every
 *  corrected output
 */
static int
do_hamming(FILE *in, int nParityBits, bool isDecode, bool isVerbose,
           FILE *out)
{
  assert(isVerbose ? isDecode : true);
  int hasError = 0;  //got an error on any word
  int nEncodedBits = (1 << nParityBits) - 1;
  int nInBits = (isDecode) ? nEncodedBits : nEncodedBits - nParityBits;
  HammingWord maxIn = (1ULL << nInBits) - 1;
  HammingWord v;
  while (fscanf(in, "%llu", &v) == 1) {
    if (v > maxIn) {
      fprintf(stderr, "value %llu does not fit in %d bits\n", v, nInBits);
      hasError = 1;
      break;
    }
    if (isDecode) {
      int isError = 0;  //for error in this word
      HammingWord z = hamming_decode(v, nParityBits, &isError);
      hasError = hasError || isError;
      const char *errStr = (isVerbose && isError) ? "*" : "";
      fprintf(out, "%llu%s\n", z, errStr);
    }
    else {
      fprintf(out, "%llu\n", hamming_encode(v, nParityBits));
    }
    fflush(out);
  } //while
  return !feof(in) || hasError;
}


static void
usage(void)
{
  fprintf(stderr,
          "usage:\thamming-encode N_HAMMING_PARITY_BITS [IN_FILE_NAME]\n");
  fprintf(stderr,
          "\thamming-decode [-v] N_HAMMING_PARITY_BITS [IN_FILE_NAME]\n");
  exit(1);
}

/** Invoked as per above usage message.  If no IN_FILE_NAME, then input
 *  defaults to stdin.
 */
int
main(int argc, const char *argv[])
{
  // does checking and conversion of args;
  // all actual work relegated to do_hamming()

  if (argc == 1) usage();
  const char *parityBitsArg = (argc >= 2) ? argv[1] : NULL;
  bool isDecode = strstr(argv[0], "decode") != NULL;
  bool isVerbose = false;
  if (isDecode) {
    isVerbose = strcmp(argv[1], "-v") == 0;
    int maxNArgs = (isVerbose) ? 4 : 3;
    if (argc > maxNArgs) usage();
    if (isVerbose) parityBitsArg = argv[2];
  }
  else {
    if (argc > 3) usage();
  }

  int nParityBits = atoi(parityBitsArg);
  if (nParityBits <= 0) {
    fprintf(stderr, "N_HAMMING_PARITY_BITS \"%s\" not a positive integer\n",
            argv[0]);
    exit(1);
  }
  unsigned nHammingWordBits = sizeof(HammingWord) * CHAR_BIT;
  if ((1ULL << nParityBits) - 1 > nHammingWordBits) {
    fprintf(stderr, "total Hamming word bit-length must be a positive integer "
            "<= %u\n", nHammingWordBits);
    exit(1);
  }

  const int inFileNameIndex = (isVerbose) ? 3 : 2;
  const char *inFileName =
    (inFileNameIndex < argc) ? argv[inFileNameIndex] : NULL;
  FILE *in = (inFileName) ? fopen(inFileName, "r") : stdin;
  if (!in) {
    fprintf(stderr, "cannot read '%s': %s\n", inFileName, strerror(errno));
    return 1;
  }

  return do_hamming(in, nParityBits, isDecode, isVerbose, stdout);
}
