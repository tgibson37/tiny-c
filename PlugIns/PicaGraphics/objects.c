#include "plot.h"

/* objects.c -- Library with for drawing lines, more */

/* Map x from the range min..max to 0..80 */

/* define problem space */
void palette(float minx, float miny, float maxx, float maxy ) {
	if(debug)fprintf(stderr,
			"\npalette(plot~65) %f %f %f %f",minx,miny,maxx,maxy);
	min_x=minx; min_y=miny; max_x=maxx; max_y=maxy;
	range_x=maxx-minx; range_y=maxy-miny;
	inc_x=range_x/80; inc_y=range_y/80;
	if(inc_x<=0 || inc_y<=0 ) {
		printf("\nInvalid space: min x,y %f %f  max x,y %f %f\n"
				,minx,miny,maxx,maxy);
		exit(1);
	}
	blank();
}

/* maps x(or y) position in palette to position in 80x80 grid */
int map(float x, float min, float max) {
	float range = max-min;
	float a = 80/range;
	float b = -80*min/range;
	int p = (int)(a*x+b);
	if(debug>1)fprintf(stderr,"\n  map %f %f %f -> %d",x,min,max,p );
	return p;
}

/* scales x to 80x80 grid offset in x direction */
int xScale(float x) {
	int offset = 80*(x/range_x);
	if(debug>1)fprintf(stderr,"\n  xScale: %f -> %d",x,offset);
	return offset;
}

/* scales y to 80x80 grid offset in y direction */
int yScale(float y) {
	int offset = 80*(y/range_y);
	if(debug>1)fprintf(stderr,"\n  yScale: %f -> %d",y,offset);
	return offset;
}

/* Draw a line from a to b */
void line( Point a, Point b ) {
	if(debug)fprintf(stderr,"\nLine a: %f %f  b: %f %f",a.x,a.y,b.x,b.y);
	pgPoint pa = {map(a.x,min_x,max_x), map(a.y,min_y,max_y)};
	pgPoint pb = {map(b.x,min_x,max_x), map(b.y,min_y,max_y)};
	pgLine(pa,pb);
}

void circle(Point c, float r, int npts) {
	if(debug)fprintf(stderr,"\nCircle center: %f %f radius: %f points: %d",
		c.x, c.y, r, npts);
	pgPoint center = { map(c.x,min_x,max_x), map(c.y,min_y,max_y) };
	pgCircle(center,xScale(r),npts);
}

void star(Point c, float r, int npts) {
	if(debug)fprintf(stderr,"\nStar center: %f %f radius: %f points: %d",
		c.x, c.y, r, npts);
	pgPoint center = { map(c.x,min_x,max_x), map(c.y,min_y,max_y) };
	pgStar(center,xScale(r),npts);
}

