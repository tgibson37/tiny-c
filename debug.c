#include "tc.h"

#define BTABSIZE 10
#define BUF_SIZE 80

int db_running = 0;
int db_next = 0;
int db_skiplib = 1;

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
		if( strcmp(sym,(*brktab[i].var).name ) ) return &brktab[i];
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
		printf("is already %d",num_b(b));
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
				iDatum = get_int(where+i);
				printf(" %d", iDatum);
			}
		}
		else if(type==Char){
			char* where=(*var).value.up;
			if(*where<32) {
				int i;
				for(i=0; i<10; ++i) printf(" 0x",*(where+i));
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
	if(!found) printf("no such sym\n");
	else{
		printf("%s: ",param);
		print_val(found);
		printf("\n");
	}
}

void gdb_b() {}

int db_cmd() {
	fgets(buf, BUF_SIZE, stdin);
		int clen = ((int)strlen(buf))-1;
		buf[clen]=0;
	char cmd = buf[0];
	switch(cmd) {
/* back to gdb, to enable this set a gdb watch on gdb_b */
	case 'g':
		gdb_b();
		break;
/* set breakpt */
	case 'b':
		set_b(param());
		break;
/* info, list breakpts */
	case 'i':
		inf_b();
		break;
/* run */
	case 'r':
		db_running = 1;
		return 'r';
		break;
/* print */
	case 'p':
		print_b(param());
		return 'p';
		break;
/* next */
	case 'n':
		db_next=1;	
		return 'n';
		break;
/* next */
	case 'c':
		db_next=0;
		return 'c';
		break;
/* exit */
	case 'x':
		return 'x';
		break;
	default:
		return '?';
	}
}

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


