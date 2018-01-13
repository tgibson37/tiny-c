#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*	Typewriter plot routine in 80x80 space, Space. 
 */
int points = 17;
int debug, frame;
char label = 'a';
char Space[80][40];
typedef struct { int x; int y; } Point;

/* fill Space with spaces */
void blank() {
	int r,c;
	for(r=0; r<40; ++r) for(c=0; c<80; ++c) Space[c][r]=' ';
}

/* show the drawing */
void show() {
	int r,c;
	for(r=0; r<40; ++r) {
		printf("\n");
		if(frame)printf("|");
		for(c=0; c<80; ++c) printf("%c",Space[c][r]);
		if(frame)printf("|");
	}
	printf("\n");
}

/* plot a small mark at position x,y */
void plot(int x, int y, char label) {
	if(debug)fprintf(stderr,"\n %d %d",x,y);
	int r,c;
	char mark, test;
	if( x<0 || x>=80 ) return;
	if( y<0 || y>=80 ) return;
	
	r=39-y/2; 
	c=x;
	if(label) {
		Space[c][r]=label;
		return;
	}
	if( Space[c][r]==':' ) return;    /* already double marked */
	test = (y%2) ? '.' : '\'' ;  /* use this mark */
	mark = (y%2) ? '\'' : '.' ;  /* but don't erase this */
	if( Space[c][r]==test ) {
		Space[c][r]=':';              /* double mark */
		return;
	}
	Space[c][r] = mark;
}

/* Draw a line from a to b */
void line( Point a, Point b ) {
	if(debug)++label;
	else label=0;
	if(debug)fprintf(stderr,"\nLine: %d,%d; %d,%d",a.x,a.y,b.x,b.y);
	int xr = b.x-a.x;
	int yr = b.y-a.y;
	int x,y;
	int biginc;
	if( abs(xr) > abs(yr) ) {  /* near horizontal case */
		float yinc, ypos;
		int xinc = b.x>a.x ? 1 : -1;
		yinc = ((float)yr)/xr;  /*  -1 < yinc < +1  */
		ypos = a.y + yinc/2;
		if(debug)fprintf(stderr," line near horizonal %d %d %d", a.x, b.x, xinc);
		for( x = a.x; x<=b.x; x+=xinc ) {
			y = (int)ypos;
			plot(x,y,label);
			ypos += yinc;
		}
	} else {       /* near vertical case */
		float xinc, xpos;
		int yinc = b.y>a.y ? 1 : -1;
		xinc = ((float)xr)/yr;
		xpos = a.x + xinc/2;
		if(debug)fprintf(stderr," line near vertical %d %d %d", a.y, b.y, yinc); 
		for( y=a.y; y<=b.y; y+=yinc ) {
			x = (int)xpos;
			plot(x,y,label);
			xpos += xinc;
		}
	}
}

/* test */	
void test() {
	blank();

	Point b1 = { 5,5 };
	Point e1 = { 75,35 };
	line(b1,e1); 

	Point b2 = { 5,7 };
	Point e2 = { 75,30 };
	line(b2,e2); 

 	Point b3 = { 75,7 };
	Point e3 = { 65,75 };
	line(b3,e3); 
	
 	Point b4 = { 65,7 };
	Point e4 = { 75,75 };
	line(b4,e4); 
}

void star() {
	blank();
	int i;
	Point pt[points];
	float angle = 0.0;
	float inc = 2*M_PI/points;
	/* Define the points of the star */
	if(debug)fprintf(stderr,"\npoints");
	for(i=0;i<points;++i) {
		angle = i*inc;
		pt[i].x = sin(angle)*35 + 40;
		pt[i].y = cos(angle)*35 + 40;
		if(debug)fprintf(stderr,"\n%f %d %d",angle, pt[i].x, pt[i].y);
	}
	/* Draw the lines */
	if(debug)fprintf(stderr,"\nlines");
	for(i=0;i<points;++i) {
		int j;
		j=(i+points/2)%points;
		line(pt[i],pt[j]);
		j=(j+1)%points;
		if(debug)fprintf(stderr,"\n %d %d",i,j);
		line(pt[i],pt[j]);
	}
}

int main(int argc, char* argv) {
	int opt;
	char* optarg;
    while ((opt = getopt(argc, argv, "p:df")) != -1) {
        switch (opt) {
        case 'd': debug=1; break;
        case 'f': frame=1; break;
        case 'p':
        	printf("%s point star",optarg);
        	points = atoi(optarg);
        break;
        }
    }
/*	test();*/
	star();
    show();
}


