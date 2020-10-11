#include <stdio.h>

int main() {

  while (1) {
    float d;
    scanf("%f", &d);
    float x;
    for (x = d; x <= d + 1000.0; x = x + 1000.0) {
    }
    printf("loop exited with x = %.14g\n", x);
  }
  return 0;
}
