#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

enum { TIME_SECS = 60 };

int 
main(int argc, const char *argv[])
{
  int meg = -1;
  if (argc != 2 || (meg = atoi(argv[1])) <= 0) {
    fprintf(stderr, "usage: %s NUM_MB\n", argv[0]);
    exit(1);
  }
  char *p = malloc(meg * 1024 * 1024);
  fprintf(stdout, "%d MB allocated at %p\n", meg, p);
  sleep(TIME_SECS);
  return 0;
}
