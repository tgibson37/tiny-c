#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define ECHO 1
#define VLEN 8
#define VTABLEN 200
#define PRLEN 20000
#define STACKLEN 100
#define FUNLEN 100

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
#define KILL         99

/* globals */
int debug, verbose;

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
struct stackentry stack[STACKLEN];
int nxtstack;

/* a fun entry */
struct funentry { 
	int fvar,lvar;
	char* prused;
};
/* fun table */
struct funentry fun[FUNLEN];
struct funentry *curglbl, *curfun, *efun;

struct var { 
	char name[VLEN+1]; int class; Type type; int len; int brkpt;
	union stuff value; 
};
/* variable table */
struct var vartab[VTABLEN];
int nxtvar;

/* program space */
char pr[PRLEN];
char *apr, *epr, *prused;
#define EPR (pr+PRLEN-1)
/* EPR is end of program SPACE. 
 *	pr+10 is start of system program.
 *	apr is start of application program text, 
 *	epr is end of ALL program text, 
 *	prused includes value space.
 *	EPR is pointer to last byte of pr array.
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

void stbegin();
void tcDebug();
struct var* hit();
int typeToSize( int class, Type type );
int eq();
int topdiff();
int toptoi();
struct stackentry* popst();
void pushst( int class, int lvalue, Type type, union stuff *value );
void pushk(int datum);
void pushone();
void pushzero();
int eset( int err );
void ps(char* s);
void pl(char* s);
int  pn(int n);
void pc(char c);
int lit(char *s);
int skip(char l, char r);
int symname();
int symnameis(char* name);
char* find( char *from, char *upto, char c);
char* mustfind( char *from, char *upto, char c, int err );
char* findEOS( char* x );
Type konst();
void rem();
void newvar( int class, Type type, int len, union stuff *passed );
int allocSpace(struct var *v, int amount);
int copyArgValue(struct var *v, 
		int class, Type type, union stuff *passed );
struct var* addrval();
struct var* _addrval();
void canon(struct var *v);
int asgn();
int reln();
int expr();
int term();
void factor();
int ATOI();
void skipst();
void vAlloc(Type type, union stuff *vpassed);
int quit();
void st();
int decl();
void enter();
void setarg( Type type, struct stackentry *arg );
void machinecall();
char* typeToWord(Type t);
void PrVal(Type t, int class, union stuff *val, char lval);
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
void put_int(char *where, int datum);
int  get_int(char *where);
void put_char(char *where, char datum);
char get_char(char *where);
void put_ptr(char *where, void* datum);
char* get_ptr(char *where);
int FileRead(char*filename, char* buffer, int bufflen);
void readTheFiles(int argc, char *argv[], int optind);
void db_cmds();
int countch(char*,char*,char);
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
void errToWords();
void tcUsage();
struct var* br_hit(struct var *v);
void pft(char *from, char *to );
  /* unfortunately #include ctypes.h causes gcc link problems, so... */
int isalpha(int);
int isalnum(int);  
  /* All these two do is prevent warnings for tcc compile */

