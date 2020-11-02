#include <stdio.h>

/** Return instruction pointer for return */
static void *
get_rip(void)
{
  void *rip;
  __asm__("leaq (%%rip), %0": "=r"(rip));
  return rip;
}

int main() {
  printf("&get_rip() = %p; rip = %p\n", get_rip, get_rip());
  return 0;
}
