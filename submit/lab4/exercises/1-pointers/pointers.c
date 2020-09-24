#include <stdio.h>

int main() {
  char chars[] = { 'a', 'b', 'c', 'd', 'e' };
  char *cp = chars;
  for (unsigned i = 0; i < sizeof(chars)/sizeof(chars[0]); i++) {
    printf("char pointer cp = %p, pointing at char %c\n", cp, *cp);
    cp++;
  }

  int ints[] = { 1, 2, 3, 4, 5 };
  int *ip = ints;
  for (unsigned i = 0; i < sizeof(ints)/sizeof(ints[0]); i++) {
    printf("int pointer ip = %p, pointing at int %d\n", ip, *ip);
    ip++;
  }
  return 0;
}
