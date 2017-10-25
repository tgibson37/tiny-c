#include "tc.h"

/*	main for the tc interpreter. Their are two mains. The other, test.c
 * 	loads a set regression tests which can be run individually or collectively.
 */

int main(int argc, char *argv[]) {
	cold();
	for(;;){ 
		warm(); 
		hot(); 
	}
}

