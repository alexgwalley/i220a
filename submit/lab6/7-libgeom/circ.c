#include "geom.h"

#include <math.h>

/** Return perimeter of circle with origin at center and radius r.
 */
double 
circ_perimeter(Point center, double r)
{
  double pi = acos(-1.0);
  return 2.0 * pi * r;
}

/** Return perimeter of circle with origin at center and radius r.
 */
double 
circ_area(Point center, double r)
{
  double pi = acos(-1.0);
  return pi * r * r;
}


