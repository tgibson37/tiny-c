#ifndef PLOT_H
#define PLOT_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

typedef struct { float x; float y; } Point;
int debug;
int frame;
int movie;
char label;

/* problem space */
float min_x, min_y, max_x, max_y;
float range_x, range_y, inc_x, inc_y;
void pallette(float minx, float miny, float maxx, float maxy );

/* problem stuff */
void plot(float x, float y);
void line(Point, Point);

/* basic stuff */
typedef struct { int x; int y; } pgPoint;
void blank();
void show();
void basic_plot(int x, int y);

/* basic drawings */
void test();
void star();

#endif /* PLOT_H */

