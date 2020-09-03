#ifndef _QUADR_H
#define _QUADR_H

typedef struct {
  double root1;
  double root2;
} Roots;

Roots quadratic_roots(double a, double b, double c);

#endif
