#ifndef PLOT_H
#define PLOT_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "pgPlot.h"

typedef struct { float x; float y; } Point;

/* problem space */
float min_x, min_y, max_x, max_y;
float range_x, range_y, inc_x, inc_y;
void palette(float minx, float miny, float maxx, float maxy );

/* problem stuff */
void point(float x, float y);
void line(Point, Point);
void circle(Point center, float radius, int npts);
void star(Point center, float radius, int npts);

#endif /* PLOT_H */

