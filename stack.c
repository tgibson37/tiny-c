#include "tc.h"

void dumpStackEntry(int e){
	fflush(stdout);
	if( 0<=e && e<=nxtstack ) {
		fprintf(stderr,"\n stack entry at %d: %d %c %d ", e, stack[e].class, 
			stack[e].lvalue, stack[e].type );
		if(verbose[VS])dumpVal(stack[e].type, stack[e].class, 
				&stack[e].value,stack[e].lvalue);
	}
	else {
		fprintf(stderr,"no stack entry at %d", e);
	}
}
void dumpStack(){
	int e;
	fflush(stdout);
	fprintf(stderr,"\nStack (from top) class lvalue type stuff");
	for( e=nxtstack-1; e>=0; --e) {
		dumpStackEntry(e); 
	}
	fprintf(stderr,"\n");
}
/* dumps the just popped stack entry */
void dumpPopTop() {
	dumpStackEntry(nxtstack);
}
/* dumps the top stack entry */
void dumpTop() {
	dumpStackEntry(nxtstack-1);
}

void stuffCopy( union stuff *to, union stuff *from ) {
	memcpy( to, from, sizeof(*to));
}

/* basic pusher */
void pushst( int class, int lvalue, Type type, union stuff *value ) {
/*if(nxtstack>5)fprintf(stderr,"pushst: %d\n",nxtstack);*/
	if( nxtstack > STACKLEN) { error = PUSHERR; return; }
	stack[nxtstack].class = class;
	stack[nxtstack].lvalue = lvalue;
	stack[nxtstack].type = type;
	stuffCopy( &stack[nxtstack].value, value);
	++nxtstack;
	if(verbose[VS]){
		fprintf(stderr,"\nstack push: ");
		dumpStackEntry(nxtstack-1);
	}
}

/* basic popper, entry stays accessible until pushed over */
struct stackentry* popst() {
	if( nxtstack-1 < 0 ) { error = POPERR; return; }
	if(verbose[VS]){
		fprintf(stderr,"\nstack pop: ");
		dumpStackEntry(nxtstack-1);
	}
	--nxtstack;
	return &stack[nxtstack];
}

/************ derived convenient pushers and poppers ************/

int topdiff() {
	int b = toptoi();
	int a = toptoi();
	return ( a-b );
}

/* pop the stack returning its int value, pointer 
	resolved and cast to int if necessary. */
int toptoi() {
	int datum;
	union stuff *ptr;
	if(verbose[VS]){
		fprintf(stderr,"\ntoptoi pop: ");
		dumpStackEntry(nxtstack-1);
	}

	struct stackentry *top = &stack[--nxtstack];
	if( (*top).class==1 ) {
		if((*top).lvalue == 'L') {
			ptr = (union stuff *)((*top).value.up);
			datum=(int)((*ptr).up);
		}
		else datum=(int)((*top).value.up);
	}
	else if((*top).lvalue == 'L') {
		if((*top).type==Int ) datum = *((int*)((*top).value.up));
		else if((*top).type==Char) datum = *((char*)((*top).value.up));
		else eset(TYPEERR);
	}
	else if((*top).lvalue == 'A') {
		if((*top).type==Char) datum  = (char)((*top).value.uc);
		else if((*top).type==Int) datum  = ((*top).value.ui);
		else eset(TYPEERR);
	}
	else { eset(LVALERR); }
	return datum;
}

/* push an int */
void pushk(int datum) {
	union stuff d;
	d.ui = datum;
	pushst( 0, 'A', Int, &d );
}

/* push an int as a class 1 */
void pushPtr(int datum) {
	union stuff d;
	d.up = (void*)datum;
	pushst( 1, 'A', Int, &d );
}

/* these two used by RELN */
void pushone() {
	pushk(1);
}
void pushzero() {
	pushk(0);
}
