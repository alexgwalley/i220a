#include <stdio.h>

static int v[] = {
  211,  333, 319, 411, 911, 1001,
};

static int compute(int a, int b, int c) {
  int s = v[2];
  if (v[0] < a && a < v[1]) {
    if (b < v[2]) { 
      s += v[2];
      if (c < v[3]) {
	s += v[4];
      }
      else {
	s += v[3];
      }
    }
    else {
      s += v[1];
      if (c < v[1]) {
	s += v[4];
      }
      else {
	s += v[2];
      }
    }
  }
  else {
    if (b < v[3]) {
      if (c < v[4]) {
	s += v[5];
      }
      else {
	s += v[1];
      }
    }
    else {
      if (c < v[2]) {
	s += v[1];
      }
      else {
	s += v[3];
      }
    }
  }
  return s;
}

int 
main()
{
  int a, b, c;
  while (scanf("%d %d %d", &a, &b, &c) == 3) {
    printf("compute(%d, %d, %d) = %d\n", a, b, c, compute(a, b, c));
  }
  return 0;
}
