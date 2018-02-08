#include "tc.h"

#define BTABSIZE 10
#define BUF_SIZE 80

int db_running = 0;
int db_next = 0;
int db_skiplib = 1;

/* breakpoint this function to enable using BOTH gdb and tcdb */
void gdb_b() {}

void prbegin(){
	if(debug) {
		db_cmds();
	}
}
void prdone(){}
void tcexit(){}
void stbegin() {
	if(!debug)return;
	if(db_skiplib && cursor<apr)return;
	if(db_next){
		int lineno = countch(apr,cursor,'\n');
		char* lc = lchar(cursor);
		printf("line %d cursor(pr[%d])->%.10s\n", lineno,cursor,cursor);
		--db_next;
		db_cmds();
	}
}

struct brk {
	struct var* var;
	int enabled;
	int hits;
} brktab[BTABSIZE];
int nxtbrk = 0;

char buf[BUF_SIZE+1];
int sizeof_line = BUF_SIZE;

char *param() {
	return buf[1]==' ' ? &buf[2] : &buf[1];
}

struct brk *find_b(char *sym) {
	int i;
	for( i=0; i<nxtbrk; ++i ) {
		if( !strcmp(sym,(*brktab[i].var).name ) ) return &brktab[i];
	}
	return 0;
}

int num_b(struct brk *b) {
	return b-brktab+1;
}

void set_b(char *sym) {
	struct var *v;
	struct brk *b = find_b(sym);
	if(b){
		printf("is already %d\n",num_b(b));
	}
	else {
		if(nxtbrk>=BTABSIZE) printf("too many breaks, max BTABSIZE");
		else {
			v = _addrval(sym,curfun);
			if(!v)v = _addrval(sym,curglbl);
			if(!v)v = _addrval(sym,fun);
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

void inf_b() {
	int i;
	printf("num sym enabled hits\n");
	for(i=0;i<nxtbrk;i++){
		printf("%2d %8s  %c  %d \n",i+1, (*(brktab[i]).var).name, 
				(brktab[i]).enabled?'y':'n', (brktab[i]).hits );
	}
}

void canonThis(char* f, char* l, struct var *buf) {
	char* tf=fname;
	char* tl=lname;
	fname=f; lname=l; canon(buf);
	fname=tf;
	lname=tl;
}

void printVar(struct var *v) {
	printf("\n var %d: %s %d %s %d ", v-vartab,
		(*v).name, (*v).class, typeToWord((*v).type), (*v).len );
		print_val(v);
}

/* printf a value ref'd from a struct var, strings trunc'd at 32 */
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

void print_b(char* param) {
	struct var sym;
	struct var *found;
	canonThis( param, param+strlen(param)-1, &sym );
	found = addrval_all(sym.name);
	if(!found) printf("no such symbol\n");
	else{
		printf("%s: ",param);
		print_val(found);
		printf("\n");
	}
}

void dbUsage() {
	printf("	b <symbol>    set breakpoint\n");
	printf("	r,c           start or continue run to next breakpoint\n");
	printf("	i [b]         display breakpoints\n");
	printf("	n             finish current and display next statement");
	printf("	p <symbol>    print the value of symbol\n");
	printf("	g             enter your C debugger (see setup notes)\n");
	printf("	?             print this usage (default)\n");
	printf("	x,q           exit tiny-C\n");
}

void type_b(char* param) {
	struct var sym;
	struct var *found;
	canonThis( param, param+strlen(param)-1, &sym );
	found = addrval_all(sym.name);
	if(!found) printf("no such symbol\n");
	else {
		printVar(found);
		printf("\n");
	}
}

void verbose_clear() {
	int i;
	for(i=0; i<sizeof(verbose); ++i ) 
		verbose[i]=0;
}
void verbose_toggle(char* param) {
	int bit;
	int kase=*param;
	switch(kase) {
	case 'e': bit=VE; break;
	case 'l': bit=VL; break;
	case 's': bit=VS; break;
	case 't': bit=VT; break;
	default: printf("v needs e, l, s or t"); return;
	}
	verbose[bit] = 1-verbose[bit];
}

/* Does one command. Returns the command letter */
int db_cmd() {
	fgets(buf, BUF_SIZE, stdin);
		int clen = ((int)strlen(buf))-1;
		buf[clen]=0;
	char cmd = buf[0];
	switch(cmd) {
/*	back to gdb. To enable this 
 *		set a gdb breakpoint on gdb_b() (which does nothing).
 */
/* set breakpt */
	case 'b':
		set_b(param());
		break;
/* continue */
	case 'c':
		db_next=0;
		return 'c';
		break;
	case 'g':
		gdb_b();
		break;
/* info, list breakpts */
	case 'i':
		inf_b();
		break;
/* next */
	case 'n':
		db_next=1;	
		return 'n';
		break;
/* print */
	case 'p':
		print_b(param());
		return 'p';
		break;
/* run */
	case 'r':
		db_running = 1;
		return 'r';
		break;
/* symbol type */
	case 't':
		type_b(param());
		return 't';
		break;
/* toggle verbosity */
	case 'v':
		verbose_toggle(param());
		return 'v';
		break;
/* exit */
	case 'q':
	case 'x':
		return 'x';
		break;
	default:
		dbUsage();
		return '?';
	}
}

/* Does commands until either x (exit) or r,c,n (resume processing) */
void db_cmds() {
	while(1) {
		db_next=0;
		printf("(tc-db) ");
		int cmd = db_cmd();
		if(cmd=='\n')cmd=db_cmd();
		if(cmd=='x') exit(0);
		if(cmd=='r') return;
		if(cmd=='c') return;
		if(cmd=='n') return;
	}
}

struct var* br_hit(struct var *v) {
	int lineno;
	if(cursor>apr){
		lineno = countch(apr,cursor,'\n');
		printf("\nbreak at line %d cursor pr[%d]: %s\n",lineno,cursor-pr,(*v).name);
		showLine(cursor);
		printf("\n");
		db_cmds();
	}
}


