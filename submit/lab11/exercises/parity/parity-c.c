#include <stdio.h>
#include <stdlib.h>

int 
parity_tests(int nTests, int doDebug)
{
  int sum = 0;
  for (int i = 0; i < nTests; i++) {
    int value = rand() & (doDebug ? 0xFF : ~0);
    //open code count-1s routine
    int v = value;
    int nOnes = 0;
    while (v != 0) {
      v &= (v - 1);
      nOnes++;
    }
    int isEvenParity = (nOnes & 1) == 0;
    if (doDebug) {
      fprintf(stderr, "%x: %d\n", value, isEvenParity);
    }
    sum += isEvenParity;
  }
  return sum;
}
