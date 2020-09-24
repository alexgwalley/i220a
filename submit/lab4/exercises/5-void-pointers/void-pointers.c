#include <stdio.h>

int main() {
  char chars[] = { 'a', 'b', 'c', 'd', 'e' };
  int ints[] = { 1, 2, 3, 4, 5 };
  void *p = chars;
  for (unsigned i = 0; i < sizeof(chars)/sizeof(chars[0]); i++) {
    printf("void pointer p = %p, pointing at char %c\n", p, *(char *)p);
    p = (char *)p + 1;
  }
  p = ints;
  for (unsigned i = 0; i < sizeof(ints)/sizeof(ints[0]); i++) {
    printf("void pointer p = %p, pointing at int %d\n", p, *(int *)p);
    p = (int *)p + 1;
  }
  return 0;
}
