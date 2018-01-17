#include "plot.h"

void star() {
	Point pt[points];
	int i;
	/* Define the points of the star */
	if(debug)fprintf(stderr,"\npoints");
	float angle = 0.0;
	float inc = 2*M_PI/points;
	for(i=0;i<points;++i) {
		angle = i*inc;
/* *35 and +40 maps problem to basic */
		pt[i].x = sin(angle)*35 + 40;
		pt[i].y = cos(angle)*35 + 40;
		if(debug)fprintf(stderr,"\n%f %f %f",angle, pt[i].x, pt[i].y);
	}
	/* Draw the lines */
	if(debug)fprintf(stderr,"\nlines");
	for(i=0;i<points;++i) {
		int j;
		j=(i+points/2)%points;
		if(debug)fprintf(stderr,"\n\nline %d to %d",i,j);
		line(pt[i],pt[j]);
		if(movie){show();sleep(1);}
	}
}


