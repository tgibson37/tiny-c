#include "tc.h"

/* 	todo: MC's 7,8,9,13
 *	Propose to deprecate 3,4,5,6 and replace with 
 *	102,103. readFile,writeFile.

		MC7 ;move a block up or down. Args are first,last,K. If K
		  negative, block is moved down |k| bytes, if positive
		  then up K bytes.
		
		MC8 ;count # instances of character CH in a block. Args are
		 first,last,CH.
*/
		
typedef int (*McList)(int,int*);

/*		MC9 ;scan for nth occurance of CH in a block. Args are
		  first,last,CH,cnt address. Return pointer to nth
		  occurance,if it exists, otherwise to last. Also
		  cnt is reduced by one for every CH found.
 */
int scann( char *from, char *to, char c, int *n ) {
	char *f = from;
	for(;f<=to;++f) {
		if(*f == c) {
			--(*n);
			if(*n<=0) break;
		}
	}
	return f-from;
}
int Mscann(int nargs, int *args) {
	char *from = (char*)args[0];
	char *to   = (char*)args[1];
	char c     = args[2];
	int *starN = (int*)args[3];
	int n = *starN;
	int offset = scann(from,to,c,&n);
	*starN = n;
	return offset;
}


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

/* print from..to inclusive 
 */
void pft(char *from, char *to ) {
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
		fmtchar=charIn(*x,"dcsx");
		if(fmtchar){
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
		case 3: printf("%s",(char*)datum);
		break;
		case 4: printf("%x",datum);
		break;
		}
	}
	else if( (x=find(fmt,fmt+strlen(fmt),'%')) ) {
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
	pFmt((char*)*args,(args+1));  /* fmt, args */
}

/* original MC's */
int Mpc(int nargs, int *args)
{
    printf("%c", *args);
}

int Mpn(int nargs, int *args)
{
    printf("%d", *args);
}

/* MC1 (Mgch) has input an ESC key (via getch_), process that key */ 
char escKey() {
	if( kbhit()=='[' ){
		getch_(0);   /* toss the [ */
		int code = getch_(0);
		if( 'A'<= code <= 'D' )return -code;
	}
	eset(KILL);
}

int Mgch(int nargs, int *args)
{
	int x = getch_(ECHO);
	if(x==0x1b)return escKey();
	if(x==0x1b)eset(KILL);
	if(x==0x03)exit(0);
	return x;
}

int Mpft(int nargs, int *args) {
	char *from = (char*)*args;
	char *to = (char*)*(args+1);
/*printf("\nMC 109: from to %d %d\n", from-pr, to-pr );*/
	for(;from<=to;++from) printf("%c",*from);
		/*putchar(*from);*/
}

int bar(int nargs, int *args)
{
	fprintf(stderr,"\nPhantom MC");
    return 2;
}

/* args: a,b,dist. Block is [a..b] inclusive, distance is [+|-]dist */
int MmvBl(int nargs, int *args) 
{
	char *a, *b; int dist;
	a=(char*)args[0]; b=(char*)args[1]; dist=args[2];
	char *src = a;
	char *dest = a+dist;
	int n = b-a+1;
	memmove(dest, src, n);

}

/* test if keyboard char ready, return copy if so, else 0 */
int Mchrdy() 
{
	return kbhit();
}

/* sleep for N seconds */
void Msleep(int nargs, int *argsv) 
{
	int N = *argsv;
	sleep(N);
}

int Mfilrd(int nargs, int *argsv) {
	if(nargs<3){ eset(MCERR); return -1; }
	char *name = (char*)argsv[0];
	char *buff = (char*)argsv[1];
	int bufflen = argsv[2];
	return fileRead(name, buff, bufflen);
}

int Mfilwt(int nargs, int *argsv) {
	if(nargs<3){ eset(MCERR); return -1; }
	char *name = (char*)argsv[0];
	char *buff = (char*)argsv[1];
	int bufflen = argsv[2];
	return fileWrite(name, buff, bufflen);
}

int Mstrlen(int nargs,int *argsv) {
	if(nargs<1){ eset(MCERR); return -1; }
	char* s=(char*)argsv[0];
	return strlen(s);
}

int Mstrcat(int nargs, int *argsv) {
	if(nargs<2){ eset(MCERR); return -1; }
	char* a=(char*)argsv[0];
	char* b=(char*)argsv[1];
	return (int)strcat(a,b);	
}

int Mstrcpy(int nargs, int *argsv) {
	if(nargs<2){ eset(MCERR); return -1; }
	char* a=(char*)argsv[0];
	char* b=(char*)argsv[1];
	return (int)strcpy(a,b);
}

int Mfopen(int nargs, int *args) {
	char *filename = (char*)args[0];
	char *mode   = (char*)args[1];
	return tcFopen(filename,mode);
}
int Mfgets(int nargs, int *args) {
	char* buff = (char*)args[0];
	int len = args[1];
	int unit = args[2];
	return tcFgets(buff,len,unit);
}
int Mfputs(int nargs, int *args) {
	char* str = (char*)args[0];
	int unit = args[1];
	return tcFputs(str,unit);
}
int Mfputc(int nargs, int *args) {
	char c = (char)args[0];
	int unit = args[1];
	return tcFputc(c,unit);
}
int Mfclose(int nargs, int *args) {
	int unit = args[0];
	return tcFclose(unit);
}

/* first in this list is MC 1 */
McList origList[] = 
	{ &Mpc, &Mgch, &bar, &bar, &bar
	, &bar, &MmvBl, &bar, &Mscann, &bar
	, &bar, &Mchrdy, &Mpft, &Mpn, &bar
};
/* first in this list is MC 101 */
McList newList[] = 
	{ &MprF, &Msleep, &Mfilrd, &Mstrlen, &Mstrcat
	, &Mstrcpy, &Mfilwt, &bar, &bar, &bar
	, &Mfopen, &Mfputs, &Mfputc, &Mfgets, &Mfclose
};
/* first in this list is MC 201 */
McList userList[] = 
	{ &bar, &bar, &bar, &bar, &bar
	, &bar, &bar, &bar, &bar, &bar
	, &bar, &bar, &bar, &bar, &bar
};

/*	code the MC above and register in McList array. Placement in the array
 *	determines the MC number starting with 1, 101, 201.
 */

void origMC(int mcno, int nargs, int *args) {
	if(mcno<1 || mcno>(sizeof(origList)/sizeof(void*))) {
		pushk(0); eset(MCERR);
	}
	else {
	    int x = origList[mcno-1](nargs, args);
	    pushk(x);
	}
}

void newMC(int mcno, int nargs, int *args) {
	if(mcno<1 || mcno>(sizeof(newList)/sizeof(void*))) {
		pushk(0); eset(MCERR);
	}
	else {
	    int x = newList[mcno-1](nargs, args);
	    pushk(x);
	}
}

void userMC(int mcno, int nargs, int *args) {
	pushk(0); eset(MCERR);
}

void machinecall( int nargs ) {
/*printf("\nMC ~211, nargs %d",nargs);*/
	int i, args[nargs-1];
	int mcno = toptoi();
	--nargs;
	for(i=0; i<nargs; ++i){
		int x=toptoi();
		args[nargs-1-i]=x;
	}
	if(mcno<100)origMC(mcno, nargs, args);
	else if(mcno<200)newMC(mcno-100, nargs, args);
	else userMC(mcno-200, nargs, args);
	if(error==KILL)return;
	if(error)printf("\nMC %d not defined",mcno);
}
