#include "tc.h"

int  (*piMC )(int,int,int*) = NULL;
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
	char pct[9], *nxtpct;
	int datum, fmtchar;
	if(!(*fmt))return;
//printf("\n~69 %s<<--\n",fmt);
	if(*fmt=='%'){
		datum = *(args++);
		int i=0;
		while(i<5){
			pct[i++]=*(fmt++);
			pct[i]=NULL;
			if(charIn(*fmt,"dscx")){
				pct[i++]=*(fmt++);
				pct[i]=NULL;
				break;
			}
			else if(!isdigit(*fmt))break;
		}
//printf("\n  ~82 %s<<--\n",pct);
		if(i>=5)printf("\nBAD FMT, max 3 digits, then one of dscx\n");
		else printf(pct,datum);
	}
	else if( (nxtpct=find(fmt,fmt+strlen(fmt),'%')) ) {
		pft(fmt,nxtpct-1);	/* block print */
		fmt = nxtpct;
	}
	else {
		ps(fmt);
		return;      /* one item done */
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

#if defined(_WIN32)
int Mgch(int nargs, int *args)  // mod's lrb
{
 int loop=0;
 int x;
 while (!loop) {
  if (kbhit()) {
   loop=1;
   x = getch_(ECHO);
   if (x == ESCAPE) {
    eset(KILL);
    }
   }
  }
 if (x == CTRLC) exit(0);
 return x;
}
#else
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
#endif

int Mpft(int nargs, int *args) {
	char *from = (char*)*args;
	char *to = (char*)*(args+1);
/*printf("\nMC 109: from to %d %d\n", from-pr, to-pr );*/
	for(;from<=to;++from) printf("%c",*from);
		/*putchar(*from);*/
}

int naf(int nargs, int *args)
{
	fprintf(stderr,"\nMC: no such number");
	eset(MCERR);
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

int Mcountch(int nargs, int *args) // lrb
{
	char *from, *to; int ch;
	char *c;
    from=(char*)args[0];to=(char*)args[1];ch=args[2];
	int i;
	for (i=0,c = from; c <= to; c++)
		i += (*c == ch);
	return i;
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
	if(nargs<3){ eset(ARGSERR); return -1; }
	char *name = (char*)argsv[0];
	char *buff = (char*)argsv[1];
	int bufflen = argsv[2];
	return fileRead(name, buff, bufflen);
}

int Mfilwt(int nargs, int *argsv) {
	if(nargs<3){ eset(ARGSERR); return -1; }
	char *name = (char*)argsv[0];
	char *buff = (char*)argsv[1];
	int bufflen = argsv[2];
	return fileWrite(name, buff, bufflen);
}

int Mstrlen(int nargs,int *argsv) {
	if(nargs<1){ eset(ARGSERR); return -1; }
	char* s=(char*)argsv[0];
	return strlen(s);
}

int Mstrcat(int nargs, int *argsv) {
	if(nargs<2){ eset(ARGSERR); return -1; }
	char* a=(char*)argsv[0];
	char* b=(char*)argsv[1];
	return (int)strcat(a,b);
}

int Mstrcpy(int nargs, int *argsv) {
	if(nargs<2){ eset(ARGSERR); return -1; }
	char* a=(char*)argsv[0];
	char* b=(char*)argsv[1];
	int x = (int)strcpy(a,b);
	return x;
}

int Mfopen(int nargs, int *args) {
	if(nargs<2){ eset(ARGSERR); return -1; }
	char *filename = (char*)args[0];
	char *mode   = (char*)args[1];
	return tcFopen(filename,mode);
}

int Mfgets(int nargs, int *args) {
	if(nargs<3){ eset(ARGSERR); return -1; }
	char* buff = (char*)args[0];
	int len = args[1];
	int unit = args[2];
	return tcFgets(buff,len,unit);
}

int Mfputs(int nargs, int *args) {
	if(nargs<2){ eset(ARGSERR); return -1; }
	char* str = (char*)args[0];
	int unit = args[1];
	return tcFputs(str,unit);
}

int Mfputc(int nargs, int *args) {
	if(nargs<2){ eset(ARGSERR); return -1; }
	char c = (char)args[0];
	int unit = args[1];
	return tcFputc(c,unit);
}

int Mfclose(int nargs, int *args) {
	if(nargs<1){ eset(ARGSERR); return -1; }
	int unit = args[0];
	return tcFclose(unit);
}

int Mexit(int nargs, int *args) {
	eset(EXIT);
}

int Mexitq (int nargs, int *args) { // lrb
	exit(0);
}

/*	get value from property file returning in supplied buff.
 */
int Mgetprop(int nargs, int *args) {
	char* file = (char*)args[0];
	char* name = (char*)args[1];
	char* buff = (char*)args[2];
	int bufflen = args[3];
	char* defawlt = (char*)args[4];
	return sProperty(file,name,buff,bufflen,defawlt);
}

// load current date and time into supplied buff
int Mcdate(int nargs, int *argsv) {
	if(nargs<1){ eset(ARGSERR); return -1; }
	char *buff = (char*)argsv[0];
	time_t rawtime;
	struct tm *info;
	time( &rawtime );
	info = localtime( &rawtime );
	sprintf(buff,"%04d-%02d-%02d %02d:%02d:%02d", \
		info->tm_year-100+2000,info->tm_mon+1, \
		info->tm_mday,info->tm_hour,info->tm_min, \
		info->tm_sec);
	return buff;
}

// execute another process, hangs until process ends
int Msystem(int nargs, int *argsv) {
	if(nargs<1){ eset(ARGSERR); return -1; }
	char *cmd = (char*)argsv[0];
	return system(cmd);
}
// Like pn, but to an open file
int Mfpn(int nargs, int *argsv) {
	if(nargs<2){ eset(ARGSERR); return -1; }
	int x = argsv[0];
	int unit = argsv[1];
	char buf[12];

	if(unit<0 + unit>MAX_UNIT)return -8;
	if(fileUnit[unit]==NULL)return -9;
// itoa...
	sprintf(buf, "%d", x);
 	if( fputs(buf,fileUnit[unit]) >= 0){
 		return strlen(buf);
 	}
 	return -2;
}

/* first in this list is MC 1 */
McList origList[] =
	{ &Mpc, &Mgch, &naf, &naf, &naf
	, &naf, &MmvBl, &Mcountch, &Mscann, &naf // lrb
	, &naf, &Mchrdy, &Mpft, &Mpn, &naf
};

/* first in this list is MC 101 */
McList newList[] =
	{ &MprF, &Msleep, &Mfilrd, &Mstrlen, &Mstrcat
	, &Mstrcpy, &Mfilwt, &Mexit, &Mexitq, &Mcdate
	, &Mfopen, &Mfputs, &Mfputc, &Mfgets, &Mfclose
	, &Mgetprop, &Msystem, &Mfpn, &naf, &naf
	, &naf, &naf, &naf, &naf, &naf
};

/* first in this list is MC 201 */
McList userList[] =
	{ &naf, &naf, &naf, &naf, &naf  // lrb
	, &naf, &naf, &naf, &naf, &naf
};

/*	code the MC above and register in McList array. Placement in the array
 *	determines the MC number starting with 1, 101, 201.
 */

void origMC(int mcno, int nargs, int *args) {
	if(mcno<1 || mcno>(sizeof(origList)/sizeof(void*))) {
		pushk(0); eset(ARGSERR);
	}
	else {
	    int x = origList[mcno-1](nargs, args);
	    pushk(x);
	}
}

void newMC(int mcno, int nargs, int *args) {
	if(mcno<1 || mcno>(sizeof(newList)/sizeof(void*))) {
		pushk(0); eset(ARGSERR);
	}
	else {
	    int x = newList[mcno-1](nargs, args);
	    pushk(x);
	}
}

void userMC(int mcno, int nargs, int *args) { // lrb
	if(mcno<1 || mcno>(sizeof(userList)/sizeof(void*))) {
		pushk(0); eset(ARGSERR);
	}
	else {
	    int x = userList[mcno-1](nargs, args);
	    pushk(x);
	}
}

int plugInMC(int mcno, int nargs, int *args) {
//fprintf(stderr,"~355mc %d\n",piMC);
	if(piMC==NULL) eset(ARGSERR);
	else return (*piMC)(mcno, nargs, args);
}

void machinecall( int nargs ) {
//	int i, args[nargs-1];
	int i, args[10]; // lrb tcc complains ... wants a constant expression
	int mcno = toptoi();
	--nargs;
	for(i=0; i<nargs; ++i){
		int x=toptoi();
		args[nargs-1-i]=x;
	}
	if(mcno<100)origMC(mcno, nargs, args);
	else if(mcno<200) newMC(mcno-100, nargs, args);
	else if(mcno<300) userMC(mcno-200, nargs, args);
	else {
		int rval;
		rval = plugInMC(mcno-1000, nargs, args);
		pushk(rval);
	}
	if(error==KILL)return;
	if(error==EXIT)return;
	if(error)printf("\nMC %d not defined",mcno);
}
