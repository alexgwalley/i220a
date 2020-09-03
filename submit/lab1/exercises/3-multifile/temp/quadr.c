#include "quadr.h"

#include <math.h>

Roots
quadratic_roots(double a, double b, double c)
{
  double discriminant = b*b - 4*a*c;
  Roots roots;
  roots.root1 = (-b + sqrt(discriminant))/(2 * a);  
  roots.root2 = (-b - sqrt(discriminant))/(2 * a);  
  return roots;
}

