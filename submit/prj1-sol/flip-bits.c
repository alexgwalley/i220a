#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Return random int between 0 and max (inclusive). */
static
int getRandom(int max)
{
  return rand()%(max + 1); //may be biased
}

/**  Read unsigned int's (must be contained within bitWidth) from in.
 *   Randomly flip nFlips bits within each int and write to
 *   out.
 *   Return non-zero if not EOF.
 */
static int
doFlipBits(FILE *in, int bitWidth, int nFlips, FILE *out)
{
  do {
    unsigned long v;
    if (fscanf(in, "%lu", &v) != 1) break;
    const long long max1 = (1LL << bitWidth);
    if (v >= max1) {
      fprintf(stderr, "value %lu does not fit in %d bits\n", v, bitWidth);
      break;
    }
    int flipBits[nFlips]; //remember previously flipped bits
    for (int i = 0; i < nFlips; i++) {
      int flipBitN = -1;
      while (flipBitN < 0) {
        flipBitN = getRandom(bitWidth - 1);
        for (int j = 0; j < i - 1; j++) {
          if (flipBits[j] == flipBitN) { //flipped flipBitN previously
            flipBitN = -1;
            break;
          }
        }
      }
      flipBits[i] = flipBitN;  //remember we are flipping bit # flipBitN
      unsigned long mask = 1 << flipBitN;
      v ^= mask; //flip bit # flipBitN
    }
    fprintf(out, "%lu\n", v);
  } while (1);
  return !feof(in);
}

/** Flip random NUM-FLIPS bits in ints of up to BIT-WIDTH bits read
 *  from file (which defaults to stdin).  Write results to stdout.
 */
int
main(int argc, const char *argv[])
{
  if (argc != 3 && argc != 4) {
    fprintf(stderr, "usage: %s BIT-WIDTH NUM-FLIPS [FILENAME]\n", argv[0]);
    exit(1);
  }
  int bitWidth = atoi(argv[1]);
  if (bitWidth <= 0 || bitWidth > sizeof(long)*CHAR_BIT) {
    fprintf(stderr, "BIT-WIDTH must be a positive integer <= %lu\n",
            sizeof(long)*CHAR_BIT);
    exit(1);
  }
  int nFlips = atoi(argv[2]);
  if (nFlips <= 0 || nFlips > bitWidth) {
    fprintf(stderr, "NUM-FLIPS must be a positive integer <= BIT-WIDTH\n");
    exit(1);
  }
  FILE *f = stdin;
  if (argc == 4) {
    const char *fileName = argv[3];
    if (!(f = fopen(fileName, "r"))) {
      fprintf(stderr, "cannot read %s: %s\n", fileName, strerror(errno));
      exit(1);
    }
  }
  return doFlipBits(f, bitWidth, nFlips, stdout);
}
