#include "tc.h"

int countch(char *f, char *t, char c){
	int k=1;   /* start on line 1 */
	while( f++ <= t) if(*f==c) ++k;
	return k;
}

void _errToWords(){
	char *x;
	switch(error){
		case 2: x="CURSERR, cursor out of range"; break;
		case 3: x="SYMERR, decl needed"; break;
		case 1: x="STATERR"; break;
		case 5: x="RPARERR, ) missing"; break;
		case 6: x="RANGERR, subscript out of range"; break;
		case 7: x="CLASERR"; break;
		case 8: x="TYPEERR"; break;
		case 9: x="SYNXERR"; break;
		case 14: x="LVALERR, not assignable"; break;
		case 15: x="POPERR, nothing to pop"; break;
		case 16: x="PUSHERR, overflowed stack area"; break;
		case 17: x="TMFUERR, overflowed function table"; break;
		case 18: x="TMVRERR, overflowed variable table"; break;
		case 19: x="TMVLERR, overflowed available space for values"; break;
		case 20: x="LINKERR"; break;
		case 21: x="ARGSERR, args don't match"; break;
		case 22: x="LBRCERR, [ required"; break;
		case 23: x="RBRCERR, ] required somewhere"; break;
		case 24: x="MCERR, no such MC"; break;
		case 26: x="SYMERRA, decl needed"; break;
		case 27: x="EQERR, illegal assign"; break;
		case 28: x="PTRERR"; break;
		case 29: x="APPERR, app not found";break; // lrb
		case 98: x="EXIT, stopped by exit call"; break;
		case 99: x="KILL, stopped by user"; break;
		case 1023: x="RBRCERR, ] required in -r "; break;
		case 2023: x="RBRCERR, ] required in library"; break;
		case 3023: x="RBRCERR, ] required in app code"; break;
	}
	printf("%s\n",x);
}

/* returns pointer to first character of the current line
 */
char* fchar(char* k){
	do{
		if(*k==0x0a||*k==0x0d)break;
	} while( --k >= pr);
	return k==pr?pr:k+1;
}
/* returns pointer to last character of the current line
 */
char* lchar(char* k){
	do{
		if(*k==0x0a||*k==0x0d)break;
	} while( ++k < endapp);
	return k-1;
}

/*	Prints end of program message, "done" if no error, else code and 
 *	line with error and carot under.
 */
void whatHappened() {
	if(error==KILL) _errToWords();
	else if(error){
		char *fc, *lc;
		int firstSignif=0, blanks, lineno;
		if(*errat==0x0a||*errat==0x0d)--errat;
		if(errat<lpr){
			printf("\nseed ");
			lineno=0;
		}
		else if(errat<apr){
			lineno = countch(lpr,errat,0x0a);
			if(!lineno)lineno = countch(pr,errat,0x0d);
			printf("\nlib ");
		}
		else {
			lineno = countch(apr,errat,0x0a);
			if(!lineno)lineno = countch(apr,errat,0x0d);
			printf("\napp ");
		}
		printf("line %d (cursor pr[%d])", lineno,errat-pr); _errToWords();
		fc=fchar(errat);
		while((*(fc+firstSignif))==' ' ||(*(fc+firstSignif))=='\t' )
			 ++firstSignif;
		lc=lchar(errat);
		pft(fc,lc);
		printf("\n");
		pft(fc,fc+firstSignif-1);        /* leading whitespace */
		blanks=errat-fc-firstSignif-1;   /* blanks to carot */
		while(--blanks >= 0) printf(" ");
		printf("^\n");
	}
	else {
		if(!quiet)printf("\ndone");
		printf("\n");
	}
}

void showLine(char *line) {
		char *fc, *lc;
		fc=fchar(line);
		lc=lchar(line);
		pft(fc,lc);
}

/************ simple prints ******************/
void ps(char* s) {printf("%s",s);}
void pl(char* s) {printf("\n%s",s);}
int  pn(int n)   {printf("%d", n);return n;}
void pc(char c)  {printf("%c", c);}

void logo() {
	if(quiet)return;
	printf(
"***  TINY-C VERSION 1.0,  COPYRIGHT 1977, T A GIBSON  ***\n"
		);
	printf(
"        This C version copyright 2017, T A Gibson\n"
		);
}

