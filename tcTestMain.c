#include "tc.h"
void testing(int argc, char *argv[]);

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


/*	allocate four major areas
 */
void allocStuff() {
    int prlen=PRLEN, err;
    err = iProperty("pps/tc.prop", "PRLEN", &prlen, PRLEN);
    if(err){
    	fprintf(stderr,"pps/tc.prop err, running pr[%d]",PRLEN);
    }
    pr = malloc(prlen);
    EPR=pr+prlen;

    int funlen=FUNLEN,size;
    err = iProperty("pps/tc.prop", "FUNLEN", &funlen, FUNLEN);
    if(err){
    	fprintf(stderr,"pps/tc.prop err, running fun[%d]",FUNLEN);
    }
    size = sizeof(struct funentry);
    fun = malloc(funlen*size);
    efun=fun+funlen*size;

    stacklen=STACKLEN;
    err = iProperty("pps/tc.prop", "STACKLEN", &stacklen, STACKLEN);
    if(err){
    	fprintf(stderr,"pps/tc.prop err, running stack[%d]",STACKLEN);
    }
    stack = malloc(stacklen*sizeof(struct stackentry));

    vtablen=VTABLEN;
    err = iProperty("pps/tc.prop", "VTABLEN", &vtablen, VTABLEN);
    if(err){
    	fprintf(stderr,"pps/tc.prop err, running var[%d]",VTABLEN);
    }
    vartab = malloc(vtablen*sizeof(struct var));
//fprintf(stderr,"~1035TC: sizes of pr %d fun %d stack %d var %d\n", 
//    			prlen, funlen, stacklen, vtablen);
}
