#include "tc.h"

#define BTABSIZE 10
#define BUF_SIZE 80

/********* globals *****************/
int db_running = 0;
int db_next = 0;
int db_skiplib = 1;
int db_rundepth = 1;
struct brk {
	struct var* var;
	int enabled;
	int hits;
} brktab[BTABSIZE];
int nxtbrk = 0;
char buf[BUF_SIZE+1];
int sizeof_line = BUF_SIZE;

/********* tools *******************/
/* look up a symbol without a hit callback */
struct var* addrval_nohit(char* sym) {
	struct var *v;
	debug = 0;
	v = addrval_all(sym);
	debug = 1;
	return v;
}

/* printf a value ref'd from a struct var, strings truncated at 32 */
void print_val(struct var *var) {
	int class=(*var).class;
	int type =(*var).type;
	if(class==0) {
		if(type==Int){
			int iDatum = get_int((*var).value.up);
			printf(" %d", iDatum);
		}
		else if(type==Char){
			char cDatum = get_char((*var).value.up);
			printf(" (%x)%c", cDatum,cDatum);
		}
	}
	else if(class==1) {
		if(type==Int){
			int* where=(int*)((*var).value.up);
			int len=(*var).len;
			int iDatum, i;
			for(i=0;i<len;++i){
				iDatum = get_int((char*)(where+i));
				printf(" %d", iDatum);
			}
		}
		else if(type==Char){
			char* where=(*var).value.up;
			if(*where<32) {
				int i;
				for(i=0; i<30; ++i) {
					char c = *(where+i);
					if(!c || c == 0x0d || c==0x0a ) break;
					printf(" 0x%x",c);
					
				}
			}
			else printf(" %s", where);
		}
	}
	else {
		char* name=(*var).name;
		printf("%s is a function\n",name);
	}
}

int num_b(struct brk *b) {
	return b-brktab+1;
}

/* canonicalize param symbol (f,l) into buf. */
void canonThis(char* f, char* l, struct var *buf) {
	char* tf=fname;
	char* tl=lname;
	fname=f; lname=l; canon(buf);
	fname=tf;
	lname=tl;
}

/* return brk entry for sym, or false if none */
struct brk *find_b(char *sym) {
	int i;
	for( i=0; i<nxtbrk; ++i ) {
		if( !strcmp(sym,(*brktab[i].var).name ) ) return &brktab[i];
	}
	return 0;
}

/* print description and value of variable v */
void printVar(struct var *v) {
	printf("\n4~88\n var %d: %s %d %s %d ", v-vartab,
		(*v).name, (*v).class, typeToWord((*v).type), (*v).len );
		print_val(v);
}

/********* command code *****************/
/* note: one or two liners are implemented in db_cmd */

/* b <sym> */
void db_brkset(char *sym) {
	struct var *v;
	struct brk *b = find_b(sym);
	if(b){
		printf("is already %d\n",num_b(b));
	}
	else {
		if(nxtbrk>=BTABSIZE) printf("too many breaks, max BTABSIZE");
		else {
			v = addrval_nohit(sym);
			if(!v){
				printf("%s ? no such symbol\n",sym);
				return;
			}
			b = &brktab[nxtbrk++];
			(*b).var = v;
			(*b).hits = 0;
			(*b).enabled = 1;
			(*v).brkpt = 1;
		}
	}
}

/* d */
void db_dump(char* param) {
	int kase=*param;
	switch(kase){
	case 'f': dumpFun(); printf("\n"); break;
	case 'v': dumpVarTab(); printf("\n"); break;
	default: printf("d needs f (fcn table) or v (var table) parameter");
	break;
	}
}

/* i */
void db_info() {
	int i;
	if(nxtbrk<=0){
		printf("no breakpoints set\n");
		return;
	}
	printf("num sym enabled hits\n");
	for(i=0;i<nxtbrk;i++){
		printf("%2d %8s  %c  %d \n",i+1, (*(brktab[i]).var).name, 
				(brktab[i]).enabled?'y':'n', (brktab[i]).hits );
	}
}

/* p <sym>  */
void db_print(char* param) {
	if(*param) {
		struct var *found;
		_canon( param, param+strlen(param)-1, buf );
		found = addrval_nohit(buf);
		if(!found) printf("no such symbol\n");
		else{
			printf("%s: ",param);
			print_val(found);
			printf("\n");
		}
	} else {
		dumpLine();
		printf("\n");
	}
}

/* t <sym> */
void db_type(char* param) {
	struct var *found;
	_canon( param, param+strlen(param)-1, buf );
	found = addrval_nohit(buf);
	if(!found) printf("no such symbol\n");
	else {
		printVar(found);
		printf("\n");
	}
}

/* <default> e.g. ? */
void db_usage() {
	printf("	b <symbol>    set breakpoint\n");
	printf("	r,c           start or continue run to next breakpoint\n");
	printf("	i [b]         display breakpoints\n");
	printf("	n             next statement in this function\n");
	printf("	s             step into the function\n");
	printf("	p <symbol>    print the value of symbol\n");
	printf("	t <symbol>    print the type of symbol\n");
	printf("	g             enter your C debugger (see setup notes)\n");
	printf("	v [e|p|s|v]     toggle verbose mode for one of:\n");
	printf("	                e assignment, p parsed symbol,\n");
	printf("	                s stack push/pops, v variables\n");
	printf("	d [f|v]       dump function or variable table\n");
	printf("	default       print this usage\n");
	printf("	x,q           exit tiny-C\n");
}

/* v - */
void verbose_clear() {
	int i;
	for(i=0; i<sizeof(verbose); ++i ) 
		verbose[i]=0;
}
/* v <param> */
void db_verbose(char* param) {
	int bit;
	int kase=*param;
	switch(kase) {
	case '-': verbose_clear(); return;
	case 'e': bit=VE; break;
	case 'l': bit=VL; break;
	case 's': bit=VS; break;
	case 'p': bit=VP; break;
	case 'v': bit=VV; break;
	default: printf("v needs e, l, s, p, or v parameter"); return;
	}
	verbose[bit] = 1-verbose[bit];
}

/********* command parser/dispatcher *****************/
char *param() {
	return buf[1]==' ' ? &buf[2] : &buf[1];
}

/* breakpoint this function to enable using BOTH gdb and tcdb */
void gdb_b() {}

/* Does one command. Returns the command letter */
int db_cmd() {
	fgets(buf, BUF_SIZE, stdin);
		int clen = ((int)strlen(buf))-1;
		buf[clen]=0;
	char cmd = buf[0];
	switch(cmd) {
/* set breakpt */
	case 'b':
		db_brkset(param());
		break;
/* continue */
	case 'c':
		db_next=0;
		return 'c';
		break;
/* dump */
	case 'd':
		db_dump(param());
		return 'd';
		break;
/*	back to gdb (or other debugger). To enable this set a gdb breakpoint on 
 *	gdb_b(), a function which does nothing. Typing g<ret> from this 
 *	debugger sends you into the external debugger where you can step through
 *	the interpreter code.
 */
	case 'g':
		gdb_b();
		break;
/* info, list breakpts */
	case 'i':
		db_info();
		break;
/* next */
	case 'n':
		db_next=db_rundepth;
		return 'n';
		break;
/* print */
	case 'p':
		db_print(param());
		return 'p';
		break;
/* run */
	case 'r':
		db_running = 1;
		return 'r';
		break;
/* step into */
	case 's':
		db_next=db_rundepth+1;
		return 's';
		break;
/* symbol type */
	case 't':
		db_type(param());
		return 't';
		break;
/* toggle verbosity */
	case 'v':
		db_verbose(param());
		return 'v';
		break;
/* exit */
	case 'q':
	case 'x':
		return 'x';
		break;
	default:
		db_usage();
		return '?';
	}
}

/********* Command Loop *****************/
/* Does commands until either x (exit) or r,c,n (resume processing) */
void _dbCommands() {
	while(1) {
		db_next=0;
		printf("(tc-db) ");
		int cmd = db_cmd();
		if(cmd=='\n')cmd=db_cmd();
		if(cmd=='x') exit(0);
		if(cmd=='r') return;
		if(cmd=='c') return;
		if(cmd=='n') return;
		if(cmd=='s') return;
	}
}

/********* BUGOUTs from interpreter *****************/
/* Four standard bugouts, even the 1977 8080 code had these... */

/* after link, before interpreter loop */
void prbegin(){
	if(debug) {
		_dbCommands();
	}
}

/* end of whatHappened() */
void prdone(){}

/* before exiting tc (and old pps) */
void tcexit(){}

/* breakpoint appstbegin to see JUST app statments. */
int firstAppStmt = 1;
void appstbegin(){
//printf("\n~326 nx %d lev %d",db_next,db_rundepth);
	if(firstAppStmt){
		firstAppStmt = 0;
		_dbCommands();
	}
	if(db_next && (db_next >= db_rundepth) ){
		int lineno = countch(apr,cursor,'\n');
		char* lc = lchar(cursor);
		printf("line %d cursor(pr[%d])->%.10s\n", lineno,cursor-pr,cursor);
		db_next=0;
		_dbCommands();
	}
}

/* beginning of each statement */
void stbegin() {
	if(!debug)return;
	if(db_skiplib && cursor<apr)return;
	if(cursor<lpr)return;
	appstbegin();
}

/* called from tc.c ~490, symbol lookup if breakpoint set */
struct var* br_hit(struct var *v) {
	int lineno;
	if(cursor>apr){
		lineno = countch(apr,cursor,'\n');
		printf("\nbreak at line %d cursor pr[%d]: %s\n",lineno,cursor-pr,(*v).name);
		showLine(cursor);
		printf("\n");
		_dbCommands();
	}
}

/* called from enter when entering/leaving functions */
void fcn_enter() {
	++db_rundepth;
}
void fcn_leave() {
	--db_rundepth;
}

