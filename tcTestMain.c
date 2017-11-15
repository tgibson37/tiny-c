#include "tc.h"

/* 	main for test.c, the regression tests of the parts as they were built.
 *	There are two mains. The other, tc.c, is the main for the interpreter
 *	without the component tests. Normal use is tc.c.
 */

extern int optind;
int main(int argc, char *argv[])
{
	traceMode=0;
	optind=1;
	if( argc>1 && !strcmp(argv[1],"-t")) {
		traceMode=1;
		printf("trace mode on");
		++optind;
		testing(--argc, argv+1);
	} else {
		testing(argc,argv);
	}
	return 0;
}

