#include <stdio.h>
#include <stdlib.h>

/** Say hello to who on stream out. */
static void
hello(FILE *out, const char *who)
{
  fprintf(out, "hello %s\n", who);
}


/** Say hello to 1-or-more command-line arguments. */
int
main(int argc, const char *argv[])
{
  if (argc == 1) {
    fprintf(stderr, "usage: %s NAME...\n", argv[0]);
    exit(0xdeadbeef);
  }
  for (int i = 1; i < argc; i++) {
    hello(stdout, argv[i]);
  }
  return 0;
}
