#include "vectors.h"

#include <stdio.h>
#include <stdlib.h>

static void
outvec(FILE *out, int vec[], int n) {
  for (int i = 0; i < n; i++) {
    fprintf(out, "%d ", vec[i]);
  }
}

int 
main(int argc, const char *argv[])
{
  if (argc == 1) {
    fprintf(stderr, "usage: %s INT1 ...\n", argv[0]);
    exit(1);
  }
  int n = argc - 1;
  int a[n], b[n];
  for (int i = 0; i < n; i++) {
    a[i] = b[i] = atoi(argv[i + 1]);
  }
  int sum[n], product[n];
  addvec(a, b, sum, n);
  multvec(a, b, product, n);
  printf("sum = "); outvec(stdout, sum, n); printf("\n");
  printf("product = "); outvec(stdout, product, n); printf("\n");
  return 0;
}
