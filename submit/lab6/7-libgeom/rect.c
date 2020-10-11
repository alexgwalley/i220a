#include "geom.h"

/** Return perimeter of rectangle with lower-left corner at p0 and
 *  upper-right corner at p1.
 */
double 
rect_perimeter(Point p0, Point p1)
{
  int width = p1.x - p0.x;
  int height = p1.y - p0.x;
  return 2 * (width + height);
}

/** Return area of rectangle with lower-left corner at p0 and
 *  upper-right corner at p1.
 */
double 
rect_area(Point p0, Point p1)
{
  int width = p1.x - p0.x;
  int height = p1.y - p0.x;
  return (width * height);
}



