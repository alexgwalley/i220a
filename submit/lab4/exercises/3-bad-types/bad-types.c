#include <stdio.h>

int main() {
  char cs[] = { 'a', 'b', 'c', 'd', 'e' };
  int ints[] = { 1, 2, 3, 4, 5 };
  char *cp = ints;  //expect warning
  int *ip = cs;     //expect warning
  for (unsigned i = 0; i < sizeof(cs)/sizeof(cs[0]); i++) {
    printf("char pointer cp = %p, pointing at char '%c' (0x%x)\n",
           cp, *cp, *cp);
    cp++;
  }
  for (unsigned i = 0; i < sizeof(ints)/sizeof(ints[0]); i++) {
    printf("int pointer ip = %p, pointing at int %d (0x%x)\n", ip, *ip, *ip);
    ip++;
  }
  return 0;
}
