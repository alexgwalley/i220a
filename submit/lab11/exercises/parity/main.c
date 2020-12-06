#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_OPT  "-d"

int parity_tests(int nTests, int doDebug);

int 
main(int argc, const char *argv[])
{
  int nTests = -1;
  if (argc == 1 || argc > 3 || 
      (argc == 3 && strcmp(argv[1], DEBUG_OPT) != 0) ||
      ((nTests = atoi(argv[argc - 1])) <= 0)) {
    fprintf(stderr, "usage: %s [%s] NUM_TESTS\n", argv[0], DEBUG_OPT);
    exit(1);
  }
  printf("%d\n", parity_tests(nTests, argc == 3));
  return 0;
}	
