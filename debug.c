#include "tc.h"

#define BTABSIZE 10
#define BUF_SIZE 80

void prbegin(){
	if(debug) {
		cmds();
	}
}
void prdone(){}
void tcexit(){}
void stbegin() {}

int running = 0;

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

void print_b(char* param) {
	struct var sym;
	struct var *found;
	canonThis( param, param+strlen(param)-1, &sym );
	found = addrval_all(sym.name);
	if(!found) printf("no such sym\n");
	else{
		dumpVar(found);
		printf("\n");
	}
}

void next_b(){
	
}

int deb_cmd() {
	printf("(db) ");
	fgets(buf, BUF_SIZE, stdin);
		int clen = ((int)strlen(buf))-1;
		buf[clen]=0;
	char cmd = buf[0];
	switch(cmd) {
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
		running = 1;
		return 'r';
		break;
/* print */
	case 'p':
		print_b(param());
		return 'p';
		break;
/* next */
	case 'n':
		next_b();
		return 'n';
		break;
/* exit */
	case 'x':
		return 'x';
		break;
	default:
		printf("???\n");
	}
}

void cmds() {
	while(1) {
		int cmd = deb_cmd();
		if(cmd=='x') exit(0);
		else if(cmd=='r') return;
	}
}

struct var* br_hit(struct var *v) {
	int lineno;
	if(cursor>apr){
		lineno = countch(apr,cursor,'\n');
		printf("\nbreak at line %d: %s\n",lineno,(*v).name);
		showLine(cursor);
		printf("\n");
		cmds();
	}
}


