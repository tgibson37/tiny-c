#include "plot.h"

/* objects.c -- Library with for drawing lines, more */

/* Draw a line from a to b */
void line( Point a, Point b ) {
	if(debug)++label;
	else label=0;
	if(debug)fprintf(stderr,"\nLine: %f,%f -> %f,%f",a.x,a.y,b.x,b.y);

	float xr = b.x-a.x;  /* .25 */
	float yr = b.y-a.y;  /* -.15 */
	float x,y;
	float little,big,inc;
	if( fabs(xr) > fabs(yr) ) {  /* near horizontal case */
		if(debug)fprintf(stderr,"\nhorizontal");
		float yinc, ypos;
		yinc = (yr/xr)*inc_x;   /* (-.15/.25)*.0125 = -.0075 */
		if(a.x<b.x){
			little=a.x;
			big=b.x;
			ypos=a.y+yinc/2;
		}
		else{
			little=b.x;
			big=a.x;
			ypos=b.y+yinc/2;
		}
		if(debug)fprintf(stderr,
				"  little,big,inc_x %f %f %f",little,big,inc_x);
		for( x=little+inc_x/2; x<=big; x+=inc_x ) { 
							/* control from global inc_x */
			plot(x,ypos);
			ypos += yinc;	/* lesser yinc calculated above */
		}
	} else {       /* near vertical case */
		if(debug)fprintf(stderr,"\nvertical");
		float xinc, xpos;
		xinc = (xr/yr)*inc_y;
		if(a.y<b.y){
			little=a.y;
			big=b.y;
			xpos=a.x+xinc/2;
		}
		else{
			little=b.y;
			big=a.y;
			xpos=b.x+xinc/2;
		}
		if(debug)fprintf(stderr,
				"  little,big,inc_y %f %f %f",little,big,inc_y);
		for( y=little+inc_y/2; y<=big; y+=inc_y ) {
			plot(xpos,y);
			xpos += xinc;
		}
	}
}

void pointCircle(Point center, float radius, Point pts[], int npts ) {
if(debug)fprintf(stderr,"\nobject~60 pointCircle");
	int i;
	float angle = 2*M_PI/npts;
	for(i=0; i<npts; ++i) {
		pts[i].x = radius*cos(i*angle)+center.x;
		pts[i].y = radius*sin(i*angle)+center.y;
	}
}

void circle() {
if(debug)fprintf(stderr,"\nobject~69 circle");
	int npts=points;
	Point pts[npts];
	Point center = {40.0,40.0};
	float radius=30.0;
	pointCircle(center,radius,pts,npts);
	int i;
	for(i=0;i<npts-1;++i) {
		line(pts[i],pts[i+1]);
	}
	line(pts[npts-1],pts[0]);
}

