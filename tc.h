#ifndef TC_H_INCLUDED
#define TC_H_INCLUDED

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

#define VLEN 15
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
extern char verbose[8];
extern int debug;
extern int quiet;

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
#define DIVERR		 30
#define EXIT         98
#define KILL         99

#define INT long    // sizeof(INT) must equal sizeof(char*)

/*	All tc data are ints or chars. Pointers are an int index into pr. 
 *	stuff is the typless holder of any value.
 */
union stuff { char uc; INT ui; void* up; };

typedef enum type {Err,Char,Int,CharStar} Type;

/* a stack entry */
struct stackentry { 
	int class; int lvalue; Type type; 
	union stuff value; 
};
/* the stack */
extern struct stackentry *stack;
extern int nxtstack, stacklen;

/* a fun entry */
struct funentry { 
	int fvar,lvar;
	char* prused;
};
/* fun table */
extern struct funentry *fun;
extern struct funentry *curglbl, *curfun, *efun;

struct var { 
	char name[VLEN+1]; int class; Type type; int len; int brkpt;
	union stuff value; 
};
/* variable table */
extern struct var *vartab;
extern int nxtvar, vtablen;

/* most recent function entered */
extern char fcnName[VLEN+1];
void saveName();

/* program space */
extern char* pr;
extern char *lpr, *apr, *endapp, *prused, *EPR;

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
extern int error, leave, brake;
extern char* fname;
extern char* lname;
extern char* cursor;
extern char* stcurs;
extern char* errat;
extern int obsize, vclass, alen;
extern int traceMode;
extern char* ppsPath;

extern FILE* fileUnit[MAX_UNIT];
int tcFopen(char* name, char* mode);
int tcFputs(char* str, int unit);
int tcFputc(char c, int unit);
int tcFgets(char* buff, int len, int unit);
int tcFclose();
void stbegin();
void fcn_enter();
void fcn_leave();
char escKey();
int iProperty(char* file, char* name, int *val, int _default);
int sProperty(char* file, char* name, char* val, int vlen, char* _default);
int typeToSize( int class, Type type );
int topdiff();
int toptoi();
struct stackentry* popst();
void pushst( int class, int lvalue, Type type, union stuff *value );
void pushk(int datum);
void pushone();
void pushzero();
void pushPtr(int datum);
void eset( int err );
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
void dumpFun();
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
void br_hit(struct var *v);
void pft(char *from, char *to );
  /* All these two do is prevent warnings from compiler */
int Mchrdy();
int Mgch(int,int*);
int _asgn();
int _reln();
int _expr();
int _term();
void _factor();
char* _canon(char* first, char* l, char* buff);
void _errToWords();
void allocStuff();
void _rem();
int _decl();
int _lit(char*);
void _varAlloc(Type type, union stuff *vpassed);
int _symName();
int charIn(char c, char *choices );
void pFmt(char *fmt, INT *args);
int _skip(char l, char r);
#endif
