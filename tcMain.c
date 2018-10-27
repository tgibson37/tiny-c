#include "tc.h"
#include <dlfcn.h>

/*	main for the tc interpreter. Their are two mains. The other, test.c
 * 	loads a set regression tests which can be run individually or collectively.
 */
extern char* defaultLibrary;
extern int optind;
extern char* optarg;
extern char* xendlib;
char* startSeed="[main();]";
char* ppsPath="./pps";

void tcUsage() {
	printf("Usage: tc [-q] [-d] [-r seed-code] appfile");
	printf("\n-q quiet mode (logo and done msg suppressed)");
	printf("\n-d enables the debugger, which has its own usage print.");
	printf("\n-r run specific seed-code");
	printf("\nThe arg loads the appfile and libraries as follows.");
	printf("\n  If the appfile has NO #include <path to libfile> lines");
	printf("\n  the standard library is also loaded.");
	printf("\n  If the appfile has #include <path to libfile> lines");
	printf("\n  they are loaded. In that case the standard library is not");
	printf("\n  included by default but must be explicitly included in the");
	printf("\n  list if required by the app.");
	printf("\nThe default start of the tinyC app is a main() function,");
	printf("\n  with no arguments. -r changes this default to code, which ");
	printf("\n  can be any function, args permitted, or even a compound");
	printf("\n  statement.");
	printf("\nNo args prints this usage.");
	printf("\n");
}

int loadMC(char* libName) {
	char fileName[1000];
	strcpy(fileName,"lib");
	strcat(fileName,libName);
	strcat(fileName,".so");
	printf("~34stub loading MC, file: %s\n",fileName);
// Open the dynamic library
    void* libMC = dlopen(fileName,  RTLD_LAZY | RTLD_GLOBAL);
    if (!libMC) {
        fprintf(stderr, "Could not open %s\n",fileName);
        exit(1);
    }

}

int loadCode(char* file) {
	int nread = fileRead(file, endapp, EPR-endapp);
	if(nread==0){
		fprintf(stderr,"No such file: %s\n",file);
		exit(1);
	}
	if(nread<0){
		fprintf(stderr,"Err reading file: %s\n",file);
		exit(1);
	}
	endapp += nread;
	return nread;
}

void markEndlibrary() {
	strcpy(endapp,xendlib);
	endapp+=10;
	apr=endapp;
}

/*	open the app file, read and process its leading # lines, close 
 *	the file. Return negative on error, else a count of loaded files.
 */
int doIncludes(char* fname) {
	int unit,len,lineno,libCount,loadCount=0;
	char buff[200];
	unit = tcFopen(fname,"r");
//	if(unit<0)return unit;
	if(unit<0){eset(APPERR);_errToWords();exit(unit);} // lrb
	while(1){
		len = tcFgets(buff,sizeof(buff),unit);
		if(len<0)return len;
		++lineno;
		if(len>0){
			if( buff[len-1]=='\n' || buff[len-1]==0x0d ){
				buff[len-1]=NULL;
				if(len>1){
					if( buff[len-2]=='\n' || buff[len-2]==0x0d )
						buff[len-2]=NULL;
				}
			}
		}
		if(!strncmp(buff,"#include ",9)) {
			loadCode(buff+9);  // exit(1) on failure
			++libCount;
			if(!quiet)printf("%s loaded\n",buff+9);
		}
		else if(!strncmp(buff,"#loadMC ",8)) {
			loadMC(buff+8);  // exit(1) on failure
			++loadCount;
			if(!quiet)printf("%s loaded\n",buff+8);
		}
		else{
			break;
		}
	}
	tcFclose(unit);
	return libCount;
}

int main(int argc, char *argv[]) {
	int opt,numIncs;

	allocStuff();
	strcpy(pr,startSeed);
	lpr = endapp = prused = pr+strlen(startSeed);
    while ((opt = getopt(argc, argv, "qdvr:")) != -1) {
        switch (opt) {
        case 'q':
        	quiet=1;
        	break;
        case 'd': 
        	debug=1; 
        	break;
        case 'v': 
        	verbose[VL]=1; 
        	break;
        case 'r': 
        	*pr = '[';
        	strcpy(pr+1,optarg);
        	lpr = endapp = prused = pr+strlen(optarg)+3;
        	*(endapp-2) = ']';
        	*(endapp-1)='\n';
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

/* load app and default or specified libraries, endapp has been set above */
	if(optind >= argc){
		tcUsage();
		exit(1);
	}

	numIncs = doIncludes(argv[argc-1]);
	if(numIncs<0)exit(numIncs);
	if(numIncs==0)loadCode(defaultLibrary);

	markEndlibrary();
	/* load the app */
	loadCode(argv[argc-1]);

	error=0;
	prused = endapp+10;  /* a little slack */
	nxtvar = 0;
	nxtstack = 0;
	efun = fun+FUNLEN;
	curfun = fun-1;   /* none */
	logo(); 
	tclink();
	cursor=pr;
	prbegin();
	st();   /* <<<== executes statement above, line 10 */
	prdone();
	whatHappened();
}

