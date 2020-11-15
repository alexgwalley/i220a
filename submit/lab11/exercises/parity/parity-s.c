#include <stdio.h>
#include <stdlib.h>

int 
parity_tests(int nTests, int doDebug)
{
  int sum = 0;
  for (int i = 0; i < nTests; i++) {
    int value = rand() & (doDebug ? 0xFF : ~0);
    int isEvenParity = 0;
    __asm__("testl %1, %1; jpo 1f; movl $1, %0; 1:":
	    "=r"(isEvenParity):"r"(value),"0"(isEvenParity));
    if (doDebug) {
      fprintf(stderr, "%x: %d\n", value, isEvenParity);
    }
    sum += isEvenParity;
  }
  return sum;
}
