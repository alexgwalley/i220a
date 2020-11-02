#include <stdio.h>
#include <stdlib.h>

/** Return non-zero iff n has even parity. */
int get_parity(int n);

int 
main(int argc, const char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "usage: %s INT1...\n", argv[0]);
    exit(1);
  }
  for (int i = 1; i < argc; i++) {
    int n = atoi(argv[i]);
    printf("parity(%d) = %d\n", n, get_parity(n));
  }
  return 0;
}
