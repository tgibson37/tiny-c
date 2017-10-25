#include "tc.h"

/* 	todo: MC's 7,8,9,13
 *	Propose to deprecate 3,4,5,6 and replace with 
 *	102,103. readFile,writeFile.

		MC7 ;move a block up or down. Args are first,last,K. If K
		  negative, block is moved down |k| bytes, if positive
		  then up K bytes.
		
		MC8 ;count # instances of character CH in a block. Args are
		 first,last,CH.
		
		MC9 ;scan for nth occurance of CH in a block. Args are
		  first,last,CH,cnt address. Return pointer to nth
		  occurance,if it exists, otherwise to last. Also
		  cnt is reduced by one for every CH found.
 */

typedef int (*McList)(int,int*);

/*	return true if c exists in choices, false otherwise. AND the true 
 *	value is the integer index plus one of c in choices
 */
int charIn(char c, char *choices ){
	char *x = choices;
	do {
		if(*(x++) == c) return x-choices;
	}while(*x);
	return 0;
}

/* print a block of ram from from to to inclusive 
 */
void pft(char *from, char *to ) {
/*
printf("\n36: pr,from-pr,to-pr %d %d %d\n",pr,from-pr,to-pr);
exit(77);
*/
	while(from <= to) printf("%c",*(from++));
}

/*	used by printf,ps,pl. Prints one token of format string; 
 *	either a %<char> or a block of chars excluding %. Recursive
 *	until whole fmt string consumed.
 */
void pFmt(char *fmt, int *args) {
	char *x;
	int datum, fmtchar;
	if(!(*fmt))return;
	if(*fmt=='%'){
		x = ++fmt;
		if( fmtchar=charIn(*x,"dcsx") ){
			++fmt;
			datum = *(args++);
		}
		switch(fmtchar) {
		case 0: printf("format char %c not supported",fmtchar);
		break;
		case 1: printf("%d",datum);
		break;
		case 2: printf("%c",datum);
		break;
		case 3: printf("%s",datum);
		break;
		case 4: printf("%x",datum);
		break;
		}
	}
	else if( x=find(fmt, fmt+strlen(fmt), '%') ) {
		pft(fmt,x-1);	/* block print */
		fmt = x;
	}
	else {
		ps(fmt);
		return;  /* all done */
	}
	pFmt(fmt, args); /* do the rest */
	return;
}

/* new MC's with this implementation. A bit of modernization. */
int MprF(int nargs, int *args)
{
/*
printf("\n\n63: MprF: nargs %d args[0..3] %d %d %d %d",
			nargs,args[0],args[1],args[2],args[3]);
*/
	pFmt(*args,(args+1));  /* fmt, args */
}

/* original MC's, sparcely implemented */
int Mpc(int nargs, int *args)
{
/*printf("\n88: args,*args %x %x",args,*args);
*/
    printf("%c", *args);
}

int Mpn(int nargs, int *args)
{
    printf("%d", *args);
}

int Mgch(int nargs, int *args)
{
    return getchar();
}

int Mpft(int nargs, int *args) {
	char *from = *args;
	char *to = *(args+1);
/*printf("\nMC 109: from to %d %d\n", from-pr, to-pr );*/
	for(;from<=to;++from) printf("%c",*from);
		/*putchar(*from);*/
}

int bar(int nargs, int *args)
{
	printf("\nPhantom MC");
    return 2;
}

/* args: a,b,dist. Block is [a..b] inclusive, distance is dist */
int MmvBl(int nargs, int *args) 
{
	char *a, *b; int dist;
	a=args[0]; b=args[1]; dist=args[3];

	char *dest = a;
	char *src = a+dist;
	int n = dest-src;
/*
printf( "117: dest,src,n %d %d %d", dest,src,n );
*/
/*	memmove(char *dest, char *src, int n);
*/
}

/* first in this list is MC 1 */
McList origList[] = 
	{ &Mpc, &Mgch, &bar, &bar, &bar
	, &bar, &MmvBl, &bar, &bar, &bar
	, &bar, &bar, &Mpft, &Mpn, &bar
};
/* first in this list is MC 101 */
McList newList[] = 
	{ &MprF, &bar, &bar, &bar, &bar
	, &bar, &bar, &bar, &bar, &bar
	, &bar, &bar, &bar, &bar, &bar
};
/* first in this list is MC 201 */
McList userList[] = 
	{ &bar, &bar, &bar, &bar, &bar
	, &bar, &bar, &bar, &bar, &bar
	, &bar, &bar, &bar, &bar, &bar
};

/*	code the MC above and register in McList array. Placement in the array determines
 *	the MC number starting with 1.
 */

void origMC(int mcno, int nargs, int *args) {
	if(mcno<1 || mcno>(sizeof(origList)/8)) {
		pushk(0); eset(MCERR);
	}
	else {
	    int x = origList[mcno-1](nargs, args);
	    pushk(x);
	}
}

void newMC(int mcno, int nargs, int *args) {
	if(mcno<1 || mcno>(sizeof(newList)/8)) {
		pushk(0); eset(MCERR);
	}
	else {
	    int x = newList[mcno-1](nargs, args); /* mcno minus one, index base shift */
	    pushk(x);
	}
}

void userMC(int mcno, int nargs, int *args) {
	pushk(0); eset(MCERR);
}

void machinecall( int nargs ) {
/*printf("\n183: machinecall");
dumpStack();
*/
	int i, args[nargs-1];
	int mcno = toptoi();
	--nargs;
	for(i=0; i<nargs; ++i){
		int x=toptoi();
		args[nargs-1-i]=x;
	}
/*
printf("193: mcno %d,nargs %d, args[0..3] %x %d %x %d ",
mcno,nargs,args[0],args[1],args[2],args[3]);
*/
	if(mcno<100)origMC(mcno, nargs, args);
	else if(mcno<200)newMC(mcno-100, nargs, args);
	else userMC(mcno-200, nargs, args);
	if(error)printf("\nMC %d not defined",mcno);
}
