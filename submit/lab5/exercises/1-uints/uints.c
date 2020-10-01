#include <stdio.h>

/** Read integer's from stdin and print out value as int or short in 
 *  both decimal and hex.
 */
int main() {
  while (1) {
    unsigned int i;
    unsigned short s;
    if (scanf("%d", &i) != 1) break;
    s = (unsigned short)i;
    printf("uint = %u (0x%x); ushort = %u (0x%x)\n", i, i, s, s);
  }
  return 0;
}
