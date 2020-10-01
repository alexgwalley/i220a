#include "mystery.h"

#include <stdio.h>

int main() {
  while (1) {
    unsigned int i;
    if (scanf("%x", &i) != 1) break;
    unsigned int m = mystery(i);
    printf("mystery = %d (0x%x)\n", m, m);
  }
  return 0;
}
