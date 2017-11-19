#include "tc.h"

#define BTABSIZE 10
#define BUF_SIZE 80

void prbegin(){};
void prdone(){};
void tcexit(){};
void stbegin() {
	if(debug) {
		tcDebug();
	}
}

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
	struct brk *b = find_b(sym);
	if(b){
		printf("is already %d",num_b(b));
	}
	else {
		if(nxtbrk>=BTABSIZE) printf("too many breaks, max BTABSIZE");
		else {
			b = &brktab[nxtbrk++];
			(*b).var = _addrval(sym,curglbl);
			(*b).hits = 0;
			(*b).enabled = 1;
		}
	}
}

void inf_b() {
	int i;
	for(i=0;i<nxtbrk;i++){
		printf("\n%2d %8s  %c  %d ",i+1, (*(brktab[i]).var).name, 
				(brktab[i]).enabled?'y':'n', (brktab[i]).hits );
	}
}

int deb_cmd() {
	printf("\n(db) ");
	fgets(buf, BUF_SIZE, stdin);
	char cmd = buf[0];
	switch(cmd) {
/* set breakpt */
	case 'b':
		printf("%s\n",param());
		set_b(param());
		break;
/* set breakpt */
	case 'i':
		printf("%s\n",param());
		inf_b();
		break;
/* run */
	case 'r':
		running = 1;
		return 'r';
		break;
/* exit */
	case 'X':
		return 'X';
		break;
	default:
		printf("???\n");
	}
}

void cmds() {
	while(1) {
		int cmd = deb_cmd();
		if(cmd=='X') exit(0);
		else if(cmd=='r') return;
	}
}

struct var* hit() {
	
}

void tcDebug() {
	if(running) {
		if(hit()) {
			showLine();
			cmds();
		}
	}
	else {
		cmds();
	}
}


