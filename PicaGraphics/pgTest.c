#include "pgPlot.h"

int points;
extern int countdown;
/* test, four lines */	
void test() {
	pgPoint b1 = { 5,5 };
	pgPoint e1 = { 75,35 };
	pgLine(b1,e1); 

	pgPoint b2 = { 5,7 };
	pgPoint e2 = { 75,30 };
	pgLine(b2,e2); 

 	pgPoint b3 = { 75,7 };
	pgPoint e3 = { 65,75 };
	pgLine(b3,e3); 
	
 	pgPoint b4 = { 65,7 };
	pgPoint e4 = { 75,75 };
	pgLine(b4,e4); 
	
 	pgPoint b5 = { 20,60 };
	pgCircle(b5, 15, 30);
	pgStar(b5, 15, 5);
	
}

void star() {
 	pgPoint b5 = { 40,40 };
	pgStar(b5, 35, points);
}

void circle() {
 	pgPoint b5 = { 40,40 };
	pgCircle(b5, 35, points);
}

void both() {
	star(); circle();
}

void usage() {
	printf("Usage: plot -gNAME [OPTIONS]\n");
	printf("Plot a 23 pgPoint star, or some test lines\n");
	printf("Options:\n");
	printf("  -d		Debug mode.\n");
	printf("  -f		Add a frame\n");
	printf("  -gNAME	(required) name is either 'star' or 'test'\n");
	printf("  -m 		Movie mode\n");
	printf("  -pNUM	Number of pgPoints on the star.\n");
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
    while ((opt = getopt(argc, argv, "dfmp:g:")) != -1) {
    	usageNeeded=0;
        switch (opt) {
        case 'd': debug=1; break;
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
	else if(strcmp(graphic,"star")==0)star();
	else if(strcmp(graphic,"circle")==0)circle();
	else if(strcmp(graphic,"both")==0)both();
	else {
		printf("no graphic called %s\n",graphic);
		exit(1);
	}
    show();
    if(movie)sleep(2);
}

