#include "quadr.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void 
processFile(const char *fileName, FILE *f)
{
  enum { LINE_MAX = 80 };
  char line[LINE_MAX];
  int lineNum = 0;
  while (fgets(line, LINE_MAX, f) != NULL) {
    lineNum++;
    if (line[strlen(line) - 1] != '\n') {
      fprintf(stderr, "%s:%d: line too long", fileName, lineNum);
      continue;
    }
    double a, b, c;
    if (sscanf(line, "%lg %lg %lg", &a, &b, &c) != 3) {
      fprintf(stderr, "%s:%d: must have 3 doubles: bad input line %s", 
	      fileName, lineNum,line);
      continue;
    }
    Roots roots = quadratic_roots(a, b, c);
    printf("a = %g, b = %g, c = %g, root1 = %g, root2 = %g\n",
	   a, b, c, roots.root1, roots.root2);
  }
}

int 
main(int argc, const char *argv[])
{
  if (argc == 1) {
    processFile("<stdin>", stdin);
  }
  else {
    for (int i = 1; i < argc; i++) {
      FILE *f = fopen(argv[i], "r");
      if (f == NULL) {
	fprintf(stderr, "cannot read %s: %s\n", argv[i], strerror(errno));
	continue;
      }
      processFile(argv[i], f);
      if (fclose(f) != 0) {
	fprintf(stderr, "cannot close %s: %s\n", argv[i], strerror(errno));
      }
    }
  }
  return 0;
}
