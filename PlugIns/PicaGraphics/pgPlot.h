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
void blank();
void show();
void pgPlot(int x, int y);
void pgLine(int x0, int y0, int x1, int y1);
void pgPointCircle(int xCenter, int yCenter, 
		int radius, int xPts[], int yPts[], int npts);
void pgCircle(int xCenter, int yCenter, int radius, int npts);
void pgStar  (int xCenter, int yCenter, int radius, int npts);

/* basic drawings */
void pgTest();

#endif /* PGPLOT_H */

