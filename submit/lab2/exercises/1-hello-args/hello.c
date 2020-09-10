#include <stdio.h>
#include <stdlib.h>

static void
say_hello(const char *name)
{
  printf("hello %s\n", name);
}

int 
main(int argc, const char *argv[])
{
  if (argc == 1) {
    fprintf(stderr, "usage: %s NAME...\n", argv[0]);
    exit(1);
  }
  else {
    for (int i = 1; i <= argc; i++) {
      say_hello(argv[i]);
    }
  }
  return 0;
}
