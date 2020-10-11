#ifndef _GEOM_H
#define _GEOM_H

typedef struct {
  double x, y;
} Point;

/** Return perimeter of rectangle with lower-left corner at p0 and
 *  upper-right corner at p1.
 */
double rect_perimeter(Point p0, Point p1);

/** Return area of rectangle with lower-left corner at p0 and
 *  upper-right corner at p1.
 */
double rect_area(Point p0, Point p1);

/** Return perimeter of circle with origin at center and radius r.
 */
double circ_perimeter(Point center, double r);

/** Return perimeter of circle with origin at center and radius r.
 */
double circ_area(Point center, double r);

#endif //ifndef _GEOM_H

