#include "tc.h"

/*	main for the tc interpreter. Their are two mains. The other, test.c
 * 	loads a set regression tests which can be run individually or collectively.
 */

int main(int argc, char *argv[]) {
	strcpy(pr,"[_MAIN();]");  /* very first statement */
	epr = prused = pr+10;
	cursor = pr;
	curglbl = fun;

	if(argc==1){
		usage();
		exit(0);
	}
	readTheFiles(argc,argv);  /* sets epr, curglbl */
	error=0;
	prused = epr+10;  /* a little slack */
	nxtvar = 0;
	nxtstack = 0;
	efun = fun+FUNLEN;
	curfun = fun-1;   /* none */
	logo();
	link();
	cursor=pr;
	st();   /* <<<== executes statement above, line 8 */
	whatHappened();
}

void usage() {
	printf("Usage: tc [file1 [file2]]");
	printf("\n  no args prints this usage");
	printf("\n  one arg loads the standard library, and file1 as app");
	printf("\n  two args loads file1 as library, and file2 as app");
	printf("\nThe app will be started at its main() function");
	printf("\n  (no arguments to this main()");
	printf("\n");
}

