#include "plot.h"

int points;
extern int countdown;
/* test, four lines */	
void test() {
	palette(-1,-1,1,1);
/*	Point a = {0.25,0.25};
	Point b = {0.75,0.75};
	line(a,b);

	Point b1 = { .5,.5 };
	Point e1 = { .75,.35 };
	line(b1,e1); 

	Point b2 = { .5,.7 };
	Point e2 = { .75,.30 };
	line(b2,e2); 

 	Point b3 = { .75,.7 };
	Point e3 = { .65,.75 };
	line(b3,e3); 
	
 	Point b4 = { .65,.7 };
	Point e4 = { .75,.75 };
	line(b4,e4); 
*/
 	Point b5 = { 0,0 };
/*	star(b5, .65, 5);*/
	circle(b5, .65, 25);

}

void teststar() {
 	pgPoint b5 = { 40,40 };
	pgStar(b5, 35, points);
}

void testcircle() {
 	pgPoint b5 = { 40,40 };
	pgCircle(b5, 35, points);
}

void testboth() {
	teststar(); testcircle();
}

void usage() {
	printf("Usage: plot -gNAME [OPTIONS]\n");
	printf("Plot a 17 pgPoint star, or some test lines\n");
	printf("Options:\n");
	printf("  -d		Debug mode high level\n");
	printf("  -D		Debug mode Deep\n");
	printf("  -f		Add a frame\n");
	printf("  -gNAME	(required)\n");
	printf("  -m 		Movie mode\n");
	printf("  -pNUM		points\n");
	printf("  NUM is points on the star, or line segments for circle (default 17)\n");
	printf("  NAME is one of test, circle, star, both.\n");
}

int main(int argc, char* argv[]) {
	int opt;
	char* graphic = "star";
/*set global defaults*/
	debug=frame=0;
	label=0x60;  /*Just before 'a'*/
	points=17;

   	int usageNeeded=1;
    while ((opt = getopt(argc, argv, "dDfmp:g:")) != -1) {
    	usageNeeded=0;
        switch (opt) {
        case 'd': debug=1; break;  /* just palette level */
        case 'D': debug=2; break;  /* Deep, includes pg level */
        case 'f': frame=1; break;
        case 'm': movie=1; break;
        case 'g': graphic=optarg; break;
        case 'p': points=atoi(optarg); break;
        case '?': usageNeeded=1; break;
        }
    }
    if(usageNeeded){
    	usage();
    	exit(0);
    }
	if(debug){
		countdown=3;
		label='a';
	}
	blank();
	if(strcmp(graphic,"test")==0)test();
	else if(strcmp(graphic,"star")==0)teststar();
	else if(strcmp(graphic,"circle")==0)testcircle();
	else if(strcmp(graphic,"both")==0)testboth();
	else {
		printf("no graphic called %s\n",graphic);
		exit(1);
	}
    show();
    if(movie)sleep(2);
}

