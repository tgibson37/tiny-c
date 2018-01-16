#ifndef PLOT_H
#define PLOT_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

typedef struct { int x; int y; } Point;
int debug;
int frame;
int movie;
char label;
int points;

/* problem space */
float min_x;
float min_y;
float max_x;
float max_y;

/* basic stuff */
void blank();
void show();
void basic_plot(int x, int y);

/* problem stuff */
void plot(float x, float y);

/* basic drawings */
void test();
void star();
void line(Point, Point);

#endif /* PLOT_H */

