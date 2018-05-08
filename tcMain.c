#include "tc.h"

/*	main for the tc interpreter. Their are two mains. The other, test.c
 * 	loads a set regression tests which can be run individually or collectively.
 */
extern char* defaultLibrary;
extern int optind;
extern char* optarg;
extern char* xendlib;
char* startSeed="[main();]";

void tcUsage() {
//	printf("Usage: tc [-d] [-r code] [libfile...] appfile");
	printf("Usage: tc [-d] [-r code] appfile");
	printf("\nNOTE the changes, May, 2018 ...");
	printf("\nThe one arg loads the appfile and libraries as follows.");
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
	printf("\n-d enables the debugger, which has its own usage print.");
	printf("\nNo args prints this usage.");
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

/*	Process one one from the top of the app to see if it is a #include,
 *	and load the include file if so. Return negative on error, 1 on load
 *	done, false on not a #anything.
 */
int doOneInc(char* line, int lineno){
	char* filename;
	if(*line == '#'){
		if(!strncmp(line,"#include ",9)){
			filename = line+9;
			loadCode(filename);
			return 1;
		}
		else{
			fprintf(stderr,"\nunrecognized # line %d",lineno);
			fprintf(stderr,"\n%s",line);
			return -11;
		}
	}
	else return 0;
}

/*	open the app file, read and process its leading # lines, close 
 *	the file. Return negative on error, else a count of loaded files.
 */
int doIncludes(char* fname) {
	int unit,len,more,lineno,count=0;
	char buff[200];
	unit = tcFopen(fname,"r");
	if(unit<0)return unit;
	more=1;
	while(more){
		len = tcFgets(buff,sizeof(buff),unit);
		if( buff[len-1]=='\n' || buff[len-1]==0x0d ){
			buff[len-1]=NULL;
			if( buff[len-2]=='\n' || buff[len-2]==0x0d )buff[len-2]=NULL;
		}
		if(len<0)return len;
		more = doOneInc(buff,++lineno);
		if(more<0)return more;
	}
	tcFclose(unit);
	return count;
}

int main(int argc, char *argv[]) {
	int opt,numIncs;

    int prlen=PRLEN;
    int err = iProperty("pps/tc.prop", "PRLEN", &prlen, PRLEN);
    if(err){
    	fprintf(stderr,"pps/tc.prop err, running pr[%d]",PRLEN);
    }
    pr = malloc(prlen);
    EPR=pr+prlen;

	strcpy(pr,startSeed);
	lpr = epr = prused = pr+strlen(startSeed);
    while ((opt = getopt(argc, argv, "dvr:")) != -1) {
        switch (opt) {
        case 'd': 
        	debug=1; 
        	break;
        case 'v': 
        	verbose[VL]=1; 
        	break;
        case 'r': 
        	*pr = '[';
        	strcpy(pr+1,optarg);
        	lpr = epr = prused = pr+strlen(optarg)+3;
        	*(epr-2) = ']';
        	*(epr-1)='\n';
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

/* load app and default or specified libraries, epr has been set above */
	if(optind >= argc){
		tcUsage();
		exit(1);
	}

	numIncs = doIncludes(argv[argc-1]);
	if(numIncs==0)loadCode(defaultLibrary);
	else if(numIncs<0)exit(numIncs);

//	else while(optind<argc-1)loadCode(argv[optind++]);

	markEndlibrary();
	/* load the app */
printf("~154 %s", argv[argc-1]);
	loadCode(argv[argc-1]);

	error=0;
	prused = epr+10;  /* a little slack */
//printf("\n~107 lpr,apr,epr  %d %d %d\n",lpr-pr,apr-pr,epr-pr);
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

