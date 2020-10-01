#include <stdio.h>

int main() {
  double sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += 0.1;
  }
  printf("%.20f %d\n", sum, sum == 1.0);
  return 0;
}
