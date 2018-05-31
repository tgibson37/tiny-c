#include "tc.h"

/* 	tcTestMain.c for test.c, the regression tests of the parts as they were built.
 *	There are two mains. The other, tcMain.c, is the main for the interpreter
 *	without the component tests. Normal use is ./tc.c. Test use is ./test
 */

void tcUsage() {
	printf("Usage: test [TESTNUM..]");
	printf("\n  no args runs all regression test.");
	printf("\n  one or more number args runs just those tests");
	printf("\n");
}

extern int optind;
int main(int argc, char *argv[])
{
	allocStuff();
	traceMode=0;
	optind=1;
	if( argc>1 && !strcmp(argv[1],"-t")) {
		traceMode=1;
		printf("trace mode on");
		++optind;
		testing(--argc, argv+1);
	} else {
//		pr=malloc(PRLEN);
//		EPR=pr+PRLEN;
		testing(argc,argv);
	}
	return 0;
}

