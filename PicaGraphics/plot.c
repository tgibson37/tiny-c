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
void pallette(float minx, float miny, float maxx, float maxy ) {
	if(debug)fprintf(stderr,
			"\npallette(plot~65) %f %f %f %f",minx,miny,maxx,maxy);
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

/* Map x from the range min..max to 0..80 */
int map(float x, float min, float max) {
	float range = max-min;
	float a = 80/range;
	float b = -80*min/range;
	int p = (int)(a*x+b);
	return p;
}

/* plot a small mark at problem position x,y */
void plot(float x, float y) { 
	if(debug){
		if(lastLabel!=label){
			lastLabel=label;
			fprintf(stderr,"\nplot: new Object %c",label);
			countdown=5;
		}
		if(countdown){
			fprintf(stderr,"\nplot %f %f", x, y);
			if(!(--countdown))fprintf(stderr,"  ...");
		}
	}
	raw_plot(map(x,min_x,max_x), map(y,min_y,max_y)); 
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
/*set global defaults*/
	debug=frame=0;
	label=0x60;  /*Just before 'a'*/
	points=17;
	min_x=min_y=0;
	max_x=max_y=80;

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
	pallette(0,0,80,80);  /* default */
	if(debug){
		countdown=5;
	}
	if(strcmp(graphic,"test")==0)test();
	else if(strcmp(graphic,"star")==0)star();
	else if(strcmp(graphic,"circle")==0)circle();
	else {
		printf("no graphic called %s\n",graphic);
		exit(1);
	}
    show();
    if(movie)sleep(2);
}

