#include "plot.h"

/* test, four lines */	
void test() {

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
	
 	Point b5 = { 40,75 };
	Point e5 = { 46,5 };
	line(b5,e5); 
	
}

