#include "tc.h"

/*	main for the tc interpreter. Their are two mains. The other, test.c
 * 	loads a set regression tests which can be run individually or collectively.
 */
extern int optind;
int main(int argc, char *argv[]) {
	int opt;
    while ((opt = getopt(argc, argv, "dv")) != -1) {
        switch (opt) {
        case 'd': debug=1; break;
        case 'v': verbose=1; break;
        default:
            fprintf(stderr, "Usage: %s [-dv] [file...]\n", argv[0]);
            exit(1);
        }
    }

	strcpy(pr,"[_MAIN();]");  /* very first statement */
	epr = prused = pr+10;     /* prereq's to readTheFiles */
	cursor = pr;
	curglbl = fun;

	readTheFiles(argc,argv,optind);  /* also sets epr, curglbl */
	error=0;
	prused = epr+10;  /* a little slack */
	nxtvar = 0;
	nxtstack = 0;
	efun = fun+FUNLEN;
	curfun = fun-1;   /* none */
	logo();
	link();
	cursor=pr;
	prbegin();
	st();   /* <<<== executes statement above, line 8 */
	prdone();
	whatHappened();
}

void usage() {
	printf("Usage: tc [-d] [file1 [file2]]");
	printf("\n  no args prints this usage");
	printf("\n  one arg loads the standard library, and file1 as app");
	printf("\n  two args loads file1 as library, and file2 as app");
	printf("\nThe app will be started at its main() function");
	printf("\n  (no arguments to this main()");
	printf("\n -d enables the debugger");
	printf("\n");
}
