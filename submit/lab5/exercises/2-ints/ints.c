#include <stdio.h>

int main() {
  while (1) {
    int i;
    short s;
    if (scanf("%d", &i) != 1) break;
    s = (short)i;
    printf("int = %d (0x%x); short = %d (0x%x)\n", i, i, s, s);
  }
  return 0;
}
