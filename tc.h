#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define ECHO 1
#define ESCAPE 27 // lrb
#define CTRLC 3 // lrb
#define UPARROW -65
#define DOWNARROW -66
#define RIGHTARROW -67
#define LEFTARROW -68

#define VLEN 8
#define VTABLEN 300
#define PRLEN 30000
#define STACKLEN 100
#define FUNLEN 100
#define MAX_UNIT 10

/* debug and verbosity tags */
#define VE 0
#define VL 1
#define VS 2
#define VP 3
#define VV 4
char verbose[8];
int debug;
int quiet;

/* error tags */
#define STATERR      1
#define CURSERR      2
#define SYMERR       3
#define RPARERR      5
#define RANGERR      6
#define CLASERR      7
#define TYPEERR      8
#define SYNXERR      9
#define LVALERR      14
#define POPERR       15
#define PUSHERR      16
#define TMFUERR      17
#define TMVRERR      18
#define TMVLERR      19
#define LINKERR      20
#define ARGSERR      21
#define LBRCERR      22
#define RBRCERR      23
#define MCERR        24
#define SYMERRA      26
#define EQERR		 27
#define PTRERR		 28
#define APPERR	     29 // lrb
#define EXIT         98
#define KILL         99

/*	All tc data are ints or chars. Pointers are an int index into pr. 
 *	stuff is the typless holder of any value.
 */
union stuff { char uc; int ui; void* up; };

typedef enum type {Err,Char,Int,CharStar} Type;

/* a stack entry */
struct stackentry { 
	int class; int lvalue; Type type; 
	union stuff value; 
};
/* the stack */
struct stackentry *stack;
int nxtstack, stacklen;

/* a fun entry */
struct funentry { 
	int fvar,lvar;
	char* prused;
};
/* fun table */
struct funentry *fun;
struct funentry *curglbl, *curfun, *efun;

struct var { 
	char name[VLEN+1]; int class; Type type; int len; int brkpt;
	union stuff value; 
};
/* variable table */
struct var *vartab;
int nxtvar, vtablen;

/* most recent function entered */
char fcnName[VLEN+1];
void saveName();

/* program space */
char* pr;
char *lpr, *apr, *endapp, *prused, *EPR;

/* EPR is end of program SPACE. 
 *	pr starts with startSeed, then libs, then app, then values
 *	lpr is start of libraries
 *	apr is start of application program
 *	endapp is end of ALL program text, 
 *	endapp+10 start of value space
 *	prused includes values, moves up/down with fcn entry/leaving
 *	EPR is pointer to last byte of pr array
 */

/************ Globals **************/
int error, leave, brake;
char* fname;
char* lname;
char* cursor;
char* stcurs;
char* errat;
int obsize, vclass, alen;
int traceMode;
extern char* ppsPath;

FILE* fileUnit[MAX_UNIT];
int tcFopen(char* name, char* mode);
int tcFputs(char* str, int unit);
int tcFputc(char c, int unit);
int tcFgets(char* buff, int len, int unit);
int tcFclose();
void stbegin();
int typeToSize( int class, Type type );
int topdiff();
int toptoi();
struct stackentry* popst();
void pushst( int class, int lvalue, Type type, union stuff *value );
void pushk(int datum);
void pushone();
void pushzero();
void pushPtr(int datum);
int eset( int err );
void ps(char* s);
void pl(char* s);
int  pn(int n);
void pc(char c);
/*int _lit(char *s);
int _skip(char l, char r);*/
char* find( char *from, char *upto, char c);
/*void _rem();*/
void newfun();
void fundone();
void newvar( int class, Type type, int len, union stuff *passed );
struct var* addrval();
void canon(struct var *v);
int quit();
void st();
void machinecall();
char* typeToWord(Type t);
void dumpVal(Type t, int class, union stuff *val, char lval);
void dumpLine();
void dumpStuff(int v, int class, Type type);
void dumpStackEntry(int e);
void dumpStack();
void dumpPopTop();
void dumpTop();
void dumpVar(struct var *v);
void dumpVarTab();
void dumpHex( void* where, int len );
void dumpState();
void dumpName();
void dumpft(char*,char*);
void put_int(char *where, int datum);
int  get_int(char *where);
void put_char(char *where, char datum);
char get_char(char *where);
int fileRead(char*filename, char* buffer, int bufflen);
int fileWrite(char*filename, char* buffer, int bufflen);
void readTheFiles(int argc, char *argv[], int optind);
int countch(char*,char*,char);
char* fchar(char*);
char* lchar(char*);
struct var* addrval_all(char*);
void showLine(char*);
char getch_(int echo);
int kbhit(void);
void logo();
void tclink();
void prbegin();
void prdone();
void whatHappened();
void tcUsage();
struct var* br_hit(struct var *v);
void pft(char *from, char *to );
  /* All these two do is prevent warnings from compiler */
int Mchrdy();
int Mgch(int,int*);
int _asgn();
int _reln();
int _expr();
int _term();
void _factor();

