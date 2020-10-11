#include "geom.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Read lines from stdin: if line starts with r, then expect rest of
 *  line to contain 4 doubles specifying (x0, y0), (x1, y1)
 *  coordinates of rect left lower corner and right upper corner; if
 *  line starts with c, then epect rest of line to contain 3 doubles
 *  (x0, y0) and r specifying coordinates of center of circle and its
 *  radius.  In both cases, compute and output perimeter and area of
 *  specified figure.
 */
int 
main()
{
  enum { LINE_MAX = 80 };
  char line[LINE_MAX];
  FILE *out = stdout;
  while (fgets(line, LINE_MAX, stdin) != NULL) {
    switch (line[0]) {
      case 'c': {
	Point center;
	double radius;
	int n = sscanf(&line[1], "%lg %lg %lg", 
		       &center.x, &center.y, &radius);
	if (n == 3) {
	  fprintf(out, "perimeter = %lg, area = %lg\n", 
		  circ_perimeter(center, radius),
		  circ_area(center, radius));
	}
	break;
      } //case 'c'
      case 'r': {
	Point p0, p1;
	int n = sscanf(&line[1], "%lg %lg %lg %lg", &p0.x, &p0.y, &p1.x, &p1.y);
	if (n == 4) {
	  fprintf(out, "perimeter = %lg, area = %lg\n", 
		  rect_perimeter(p0, p1), rect_area(p0, p1));
	}
	break;
      } //case 'c'
    }
  }
}
