#include "tc.h"

/*	main for the tc interpreter. Their are two mains. The other, test.c
 * 	loads a set regression tests which can be run individually or collectively.
 */
extern char* defaultLibrary;
extern int optind;
extern char* optarg;
extern char* xendlib;
char* defaultStart="main";

void tcUsage() {
	printf("Usage: tc [-d] [-r code] [libfile...] appfile");
	printf("\n  No args prints this usage.");
	printf("\n  One arg loads the standard library, and the appfile.");
	printf("\n  Two or more args load libraries, then the appfile.");
	printf("\n  In that case the standard library is not included by");
	printf("\n  default but must be explicitly included in the list ");
	printf("\n  if required by the app.");
	printf("\nThe default start of the tinyC app is a main() function,");
	printf("\n  with no arguments. -r changes this default to code, which ");
	printf("\n  can be any function, args permitted, or even a compound");
	printf("\n  statement.");
	printf("\n -d enables the debugger, which has its own usage print.");
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
	strcpy(pr,defaultStart);
	epr = prused = pr+strlen(defaultStart);
    while ((opt = getopt(argc, argv, "dvr:")) != -1) {
        switch (opt) {
        case 'd': 
        	debug=1; 
        	break;
        case 'v': 
        	verbose[VL]=1; 
        	break;
        case 'r': 
        	strcpy(pr,optarg);
        	epr = prused = pr+strlen(optarg);
        	break;
	    case '?':
	        if (optopt == 'r')
	          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	        else if (isprint (optopt))
	          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	        else
	          fprintf (stderr,
	                   "Unknown option character `\\x%x'.\n",
	                   optopt);
	        return 1;
        default:
            fprintf(stderr, "Usage: %s [-d] [-v] [file...]\n", argv[0]);
            exit(1);
        }
    }

	cursor = pr;
	curglbl = fun;

	/* load default or specified libraries, epr has been set above */
	if(optind>=argc){
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

