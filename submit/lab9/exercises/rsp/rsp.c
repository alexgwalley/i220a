#include <stdio.h>

/** Return value of %rsp register just before return */
static void *
get_rsp(void)
{
  void *rsp;
  //@TODO: inline assembly to set rsp C var to value of %rsp register
  __asm__("leaq (%%rsp), %0": "=r"(rsp));
  return rsp;
}

int main() {
  int stkVar;
  printf("&stkVar = %p; rsp = %p\n", &stkVar, get_rsp());
  return 0;
}
