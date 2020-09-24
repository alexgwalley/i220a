#include <stdio.h>

typedef struct {
  const char *exp;
  void *p;
} ExpPointer;

int main() {

  char chars[] = { 'a', 'b', 'c', 'd', 'e' };
  int ints[] = { 1, 2, 3, 4, 5 };
  ExpPointer questions[] = {
    { "ints[2]", &ints[2] },
    { "chars[3]", &chars[3] },
    { "ints[4]", &ints[4] },
    { "chars[1]", &chars[1] },
    { "chars[4]", &chars[4] },
    { "ints[1]", &ints[1] },
  };
  for (unsigned i = 0;
       !feof(stdin) && i < sizeof(questions)/sizeof(questions[0]);
       i++) {
    ExpPointer q = questions[i];
    int isOk = 0;
    do {
      printf("chars[] is at %p; ints[] ints at %p\n", chars, ints);
      printf("enter pointer to %s: ", q.exp);
      fflush(stdout);
      void *p;
      int nOk = scanf("%p", &p);
      if (nOk == 1) {
        isOk = (p == q.p);
        printf("%s\n", isOk ? "ok" : "not ok");
      }
      else if (nOk == EOF) {
        break;
      }
      else {
        printf("bad input: enter hex pointer value\n");
      }
      int c = getchar();
      while (c != '\n' && c != EOF) { //waste line
        c = getchar();
      }
    } while (!isOk);
  }
  return 0;
}
