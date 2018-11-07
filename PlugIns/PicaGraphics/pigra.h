#ifndef PGPLOT_H
#define PGPLOT_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

int debug;
int frame;
int movie;
char label;

/* basic stuff */
//typedef struct { int x; int y; } pgPoint;
void blank();
void show();
void pgPlot(int x, int y);
//void pgLine(pgPoint,pgPoint);
//void pgPointCircle(pgPoint center, int radius, pgPoint pts[], int npts);
//void pgCircle(pgPoint center, int radius, int npts);
//void pgStar(pgPoint center, int radius, int npts);

/* basic drawings */
void pgTest();

#endif /* PGPLOT_H */

