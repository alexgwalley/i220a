#include <stdio.h>

int main() {
  char chars[] = { 'a', 'b', 'c', 'd', 'e' };
  int ints[] = { 1, 2, 3, 4, 5 };
  char *cp = (char *)ints;
  int *ip = (int *)chars;
  for (unsigned i = 0; i < sizeof(chars)/sizeof(chars[0]); i++) {
    printf("char pointer cp = %p, pointing at int %d (0x%x)\n",
           cp, *cp, *cp);
    cp = (char *)(((int *)cp) + 1);
  }
  for (unsigned i = 0; i < sizeof(ints)/sizeof(ints[0]); i++) {
    printf("int pointer ip = %p, pointing at char '%c' (0x%x)\n", ip, *ip, *ip);
    ip = (int *)(((char *)ip) + 1);
  }
  return 0;
}
