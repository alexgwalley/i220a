#define _XOPEN_SOURCE 1

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matmul.h"

static void
initMatrix(int n, double matrix[][n])
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      matrix[i][j] = lrand48();
    }
  }
}

static void
out_matrix(int n, double a[][n])
{
  FILE *out = stdout;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      fprintf(out, "%8g ", a[i][j]);
    }
    fprintf(out, "\n");
  }
}

enum { MAX_TEST_MATRIX_SIZE = 6 };

int
main(int argc, const char *argv[])
{
  int matrixSize = -1;
  int numTests = -1;
  if (argc != 3 || (matrixSize = atoi(argv[1])) <= 0 ||
      (numTests = atoi(argv[2])) <= 0) {
    fprintf(stderr, "usage: %s MATRIX_SIZE N_TRIALS\n", argv[0]);
    exit(1);
  }
  int n = matrixSize;
  double (*a)[n] = malloc(sizeof(double[n][n]));
  double (*b)[n] = malloc(sizeof(double[n][n]));
  double (*c)[n] = malloc(sizeof(double[n][n]));
  if (!a || !b || !c) {
    fprintf(stderr, "could not malloc matrices: %s\n", strerror(errno));
    exit(1);
  }
  initMatrix(n, a); initMatrix(n, b);
  for (int t = 0; t < numTests; t++) {
    matrix_multiply(n, a, b, c);
  }
  if (matrixSize <= MAX_TEST_MATRIX_SIZE) out_matrix(n, c);
  return 0;
}
