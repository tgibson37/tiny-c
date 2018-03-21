#include "tc.h"

/*	main for the tc interpreter. Their are two mains. The other, test.c
 * 	loads a set regression tests which can be run individually or collectively.
 */
extern char* defaultLibrary;
extern int optind;
extern char* xendlib;

void tcUsage() {
	printf("Usage: tc [-d] [libfile...] appfile");
	printf("\n  No args prints this usage.");
	printf("\n  One arg loads the standard library, and the appfile.");
	printf("\n  Two or more args load libraries, then the appfile.");
	printf("\n  In that case the standard library is not included by");
	printf("\n  default but must be explicitly included in the list ");
	printf("\n  if required by the app.");
	printf("\nThe tinyC app will be started at its main() function");
	printf("\n  (no arguments to this main()");
	printf("\n -d enables the debugger");
	printf("\n");
}

int loadCode(char* file) {
	int nread = fileRead(file, epr, EPR-epr);
	if(nread==0){
		fprintf(stderr,"No such file: %s\n",file);
		exit(1);
	}
	if(nread<0){
		fprintf(stderr,"Err reading file: %s\n",file);
		exit(1);
	}
	epr += nread;
	return nread;
}

void markEndlibrary() {
	strcpy(epr,xendlib);
	epr+=10;
	apr=epr;
}

int main(int argc, char *argv[]) {
	int opt;
    while ((opt = getopt(argc, argv, "dv")) != -1) {
        switch (opt) {
        case 'd': debug=1; break;
        case 'v': verbose[VL]=1; break;
        default:
            fprintf(stderr, "Usage: %s [-d] [-v] [file...]\n", argv[0]);
            exit(1);
        }
    }

	strcpy(pr,"[_MAIN();]");  /* very first statement */
	epr = prused = pr+10;     /* prereq's to readTheFiles */
	cursor = pr;
	curglbl = fun;

	/* load default or specified libraries, epr has been set above */
	if(optind<=argc){
		tcUsage();
		exit(1);
	}
	if(optind==argc-1)loadCode(defaultLibrary);
	else while(optind<argc-1)loadCode(argv[optind++]);
	markEndlibrary();
	/* load the app */
	loadCode(argv[argc-1]);

	error=0;
	prused = epr+10;  /* a little slack */
	nxtvar = 0;
	nxtstack = 0;
	efun = fun+FUNLEN;
	curfun = fun-1;   /* none */
	logo(); 
	tclink();
	cursor=pr;
	prbegin();
	st();   /* <<<== executes statement above, line 19 */
	prdone();
	whatHappened();
}

