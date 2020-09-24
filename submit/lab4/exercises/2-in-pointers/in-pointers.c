#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  enum { N = 5 };
  enum { MIN_OK_RUN = 3 };

  int ints[N];
  int nOk = 0;
  int nErr = 0;
  int nRun = 0;
  printf("Given the base address of an int[] array you need to\n"
         "provide the address of a specified element.\n"
         "An int occupies %zu bytes.\n\n"
         "Please provide addresses in hex without any leading 0x\n"
         "You will need %d correct responses in succession in order\n"
         "to complete this exercise.\n", sizeof(int), MIN_OK_RUN);
  while (nRun < MIN_OK_RUN) {
    int index = rand()%N;
    printf("base of ints[] is at %p\n", ints);
    printf("enter pointer to ints[%d]: ", index);
    fflush(stdout);
    int *p;
    if (scanf("%p", &p) != 1) {
      printf("bad input: enter hex pointer value without leading 0x or 0X\n");
    }
    else {
      bool isOk = (p == &ints[index]);
      int val __attribute__((unused)) = *p;
      printf("%s\n", isOk ? "ok" : "not ok");
      if (isOk) {
        nOk++;
        nRun++;
      }
      else {
        nErr++;
        nRun = 0;
      }
    }
    int c = getchar();
    while (c != '\n' && c != EOF) { //waste line
      c = getchar();
    }
  }
  printf("# ok = %d; # errors = %d\n", nOk, nErr);
  return 0;
}
