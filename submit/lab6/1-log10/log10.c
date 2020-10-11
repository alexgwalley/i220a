#include <math.h>
#include <stdio.h>

int 
main() {
  do {
    double d;
    if (scanf("%lg", &d) != 1) break;
    printf("log10(%g) = %g\n", d, log10(d));
  } while (1);
  return 0;
}
