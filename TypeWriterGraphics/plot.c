#include "plot.h"

/*	Typewriter plot routine in 80x80 space, Space. 
 */
char Space[80][40];

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

char lastLabel; 
int countdown=0;

/* plot a small mark at raw position x,y */
void raw_plot(int x, int y) {
	if(debug){
		if(lastLabel!=label){
			lastLabel=label;
			fprintf(stderr,"\nplot: new Object %c",label);
			countdown = 3;
		}
		if(countdown){
			fprintf(stderr,"\nplot %d %d", x, y);
			if(!(--countdown))fprintf(stderr,"  ...");
		}
	}
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

/* define problem space */
void space(float minx, float miny, float maxx, float maxy ) {
	min_x=minx; min_y=miny; max_x=maxx; max_y=maxy;
}

/* plot a small mark at problem position x,y */
void plot(float x, float y) { raw_plot((int)x,(int)y); }


/* Draw a line from a to b */
void line( Point a, Point b ) {
	if(debug)++label;
	else label=0;
	if(debug)fprintf(stderr,"\nLine: %d,%d -> %d,%d",a.x,a.y,b.x,b.y);

	int xr = b.x-a.x;
	int yr = b.y-a.y;
	int x,y;
	int little,big;
	if( abs(xr) > abs(yr) ) {  /* near horizontal case */
		float yinc, ypos;
		yinc = ((float)yr)/xr;  /*  -1 < yinc < +1  */
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
		for( x = little; x<=big; ++x ) {
			y = (int)ypos;
			plot(x,y);
			ypos += yinc;
		}
	} else {       /* near vertical case */
		float xinc, xpos;
		xinc = ((float)xr)/yr;
		if(a.y<b.y){
			little=a.y;
			big=b.y;
			xpos=a.x+xinc;
		}
		else{
			little=b.y;
			big=a.y;
			xpos=b.x+xinc/2;
		}
		for( y=little; y<=big; ++y ) {
			x = (int)xpos;
			plot(x,y);
			xpos += xinc;
		}
	}
}

void usage() {
	printf("Usage: plot -gNAME [OPTIONS]\n");
	printf("Plot a 23 point star, or some test lines\n");
	printf("Options:\n");
	printf("  -d		Debug mode.\n");
	printf("  -f		Add a frame\n");
	printf("  -gNAME	(required) name is either 'star' or 'test'\n");
	printf("  -m 		Movie mode\n");
	printf("  -pNUM	Number of points on the star.\n");
}

int main(int argc, char* argv[]) {
	int opt;
	char* graphic = "star";
/*globals*/
	debug=frame=0;
	label='a';
	points=17;
   	int usageNeeded=1;
    while ((opt = getopt(argc, argv, "dfmp:g:")) != -1) {
    	usageNeeded=0;
        switch (opt) {
        case 'd': debug=1; break;
        case 'f': frame=1; break;
        case 'm': movie=1; break;
        case 'p':
        	printf(">>> %s point star",optarg);
        	points = atoi(optarg);
        	break;
        case 'g': graphic=optarg; break;
        case '?': usageNeeded=1; break;
        }
    }
    if(usageNeeded){
    	usage();
    	exit(0);
    }
	blank();
	if(strcmp(graphic,"test")==0)test();
	else if(strcmp(graphic,"star")==0)star();
	else {
		printf("no graphic called %s\n",graphic);
		exit(1);
	}
    show();
    if(movie)sleep(2);
}

