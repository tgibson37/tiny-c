#include "pigra.h"

/*	This version is all int and no structs.
 */
int points;
//extern int countdown;
/* test, four lines */	
void test() {
	circle(28,43, 25, 11);
	star  (28,43, 25, 11);
	line(5,5, 75,15);
	line(5,8, 75,12);
	line(75,5, 65,70);
	line(72,5, 67,70);
}

void teststar() {
	star(40,40, 35, points);
}

void testcircle() {
	circle(40,40, 35, points);
}

void testboth() {
	teststar(); testcircle();
}

void usage() {
	printf("Usage: plot -gNAME [OPTIONS]\n");
	printf("Plot a 17 Point star, or some test lines\n");
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
        case 'D': debug=2; break;  /* Deep, includes  level */
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
//		countdown=3;
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

