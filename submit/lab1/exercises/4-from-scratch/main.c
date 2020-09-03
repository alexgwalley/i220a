#include "process-file.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>


int 
main(int argc, const char *argv[])
{
  if (argc == 1) {
    process_file("<stdin>", stdin);
  }
  else {
    for (int i = 1; i < argc; i++) {
      FILE *f = fopen(argv[i], "r");
      if (f == NULL) {
	fprintf(stderr, "cannot read %s: %s\n", argv[i], strerror(errno));
	continue;
      }
      process_file(argv[i], f);
      if (fclose(f) != 0) {
	fprintf(stderr, "cannot close %s: %s\n", argv[i], strerror(errno));
      }
    }
  }
  return 0;
}
