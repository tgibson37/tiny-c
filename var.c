#include "tc.h"

/* SITUATION: Function call parsed. 
	Open new var and value frames for the functions locals.
 */
void newfun() {
	if(++curfun>efun){
		eset(TMFUERR);
	} 
	else {
		(*curfun).fvar = nxtvar;
		(*curfun).lvar = nxtvar-1;
		(*curfun).prused = prused;
	}
	if(verbose[VV]){
		fflush(stdout);
		fprintf(stderr,"\nnewfun %s",fcnName);
	}
}

/* SITUATION: function is completed. 
 *	Close its var and value frames.
 */
void fundone() {
	nxtvar=(*curfun).fvar;
	prused=(*curfun).prused;
	--curfun;
	if(verbose[VV]){
		fflush(stdout);
		fprintf(stderr,"\nfundone %s",fcnName);
	}
}

/*********** var tools ****************/

/*	copy the argument value into the new local place.
 */
int _copyArgValue(struct var *v, int class, Type type, union stuff *passed ) {
	if(passed && class){   					/* passed pointer */
		(*v).value.up = (*passed).up;
	} else if( passed && !class ) {			/* passed datum */
		switch(type){
		case Int:
			put_int( (*v).value.up, (*passed).ui );
			break;
		case Char:
			put_char( (*v).value.up, (*passed).uc );
			break;
		default:
			eset(TYPEERR);
			return TYPEERR;
		}
	}
}

/* allocates memory for value of v, return 0 on success, else !0
 */
int _allocSpace(struct var *v, int amount){
	char* kf;
	kf = prused+1;
	(*v).value.up = prused+1;
	prused += amount;
	if( prused-EPR >=0 ) {
		eset(TMVLERR);
		return TMVLERR;
	}
	memset( kf, 0, prused-kf+1 ); /* zero the reserved space */
	return 0;
}

/* SITUATION: Declaration is parsed, and its descriptive data known.
 * 	Fill in the var with this data. Allocate value storage unless already
 *	allocated, i.e. pointer to passed data. Copy passed data into allocation.
 *	NOTE: signifantly refactored.
 */
void newvar( int class, Type type, int len, union stuff *passed ) {
	struct var *v = &vartab[nxtvar];
	canon(v);    /* sets the canon'd name into v */
	(*v).class = class;
	(*v).type = type;
	(*v).len = len;
	(*v).brkpt = 0;
	int obsize = typeToSize(class,type);
	if(_allocSpace(v,len*obsize)) return;  /* true is bad, eset done */
	if(passed) _copyArgValue( v, class, type, passed);
	if(curfun>=fun) (*curfun).lvar = nxtvar;
	if( ++nxtvar>VTABLEN )eset(TMVRERR);
	if(verbose[VV])dumpVar(v);
	return;
}

/* Canonicalizes the name bracket by f,l inclusive into buff, and returns buff.
	sizeOf buff must be at least VLEN+1.
 */

char* _canon(char* first, char* l, char* buff) {
	int i=0; 
	char* f=first;
	while(f<=l && i<VLEN-1) buff[i++]=*(f++);
	if(f<=l)buff[i++]=*l;
	buff[i]=0;
	return buff;
}

/* 	fname..lname is full name. Puts canonicalized name into v. If short
 *	enough same as name. If longer first VLEN-1 characters + last character.
 *	The last char has more info than middle chars.
 */
void canon(struct var *v) {
	_canon(fname,lname,(*v).name);
}
#if 0
	char* n=(*v).name;
	while( n < ((*v).name)+VLEN ) *n++ = 0;
	int len = lname-fname+1;
	len = len>VLEN ? VLEN : len;
	/* zap name field of v */
	strncpy( (*v).name, fname, len );  /* pads with nulls if short */
	(*v).name[8] = 0;     /* so long name canonicalized as a string */
	int length = lname-fname+1;
	if(length>VLEN) {
		(*v).name[VLEN-1] = *lname; 
	} 
#endif


/* 	looks up a symbol at one level
 */
struct var* _addrval(char *sym, struct funentry *level) {
	int first = (*level).fvar;
	int last  = (*level).lvar;
	int pvar;
	for(pvar=first; pvar<=last; ++pvar) {
		if( !strcmp(vartab[pvar].name, sym) ) {
			if( debug && (vartab[pvar]).brkpt==1 )br_hit(&vartab[pvar]);
			return &vartab[pvar];
		}
	}
	return 0;
}

/* 	looks up a symbol at three levels
 */
struct var* addrval_all(char *sym) {
	struct var *v;
	v = _addrval( sym, curfun );
	if(!v) v = _addrval( sym, curglbl );
	if(!v) v = _addrval( sym, fun ); 
	if(v)return v;
	return 0;	
}

/* 	looks up a symbol pointed to by fname,lname: 
 *	locals, globals, library levels in that order. First hit wins.
 */
struct var* addrval() {
	struct var sym;
	canon( &sym );
	return addrval_all(sym.name);
}

/*	prints a value given its description taken from a struct stackEntry */
void dumpVal(Type t, int class, union stuff *val, char lval){
	fprintf(stderr,"pr[%d]",(*val).up-(int)pr);
	if(class==1 && t==Char ){
		char sval[30];
		strncpy(sval, (char*)((*val).up), 30);
		fprintf(stderr,"->%s<-", sval);
	}
	else if(class==0 && lval!='A'){ 
		if(t==Char){
			char x = *(char*)((*val).up);
			if(x)fprintf(stderr,"->%c<-", x );
			else fprintf(stderr,"->NULL<-");
		}
//		else fprintf(stderr,"->%d<-", (*val).ui );
		else fprintf(stderr,"->%d<-", *(int*)((*val).up) );
	}
/*
	else if(t==Char) fprintf(stderr,"%c",(*val).uc);
	else fprintf(stderr,"%d",(*val).ui);
*/
}

void dumpFunEntry( int e ) {
	fprintf(stderr,"\n fun entry at %d:  %d %d %d", e,
		fun[e].fvar, fun[e].lvar, fun[e].prused-pr );
}

void dumpFun() {
	fprintf(stderr,"\nfun table: fvar, lvar, prused");
	int i;
	int num = curfun-fun;
	for(i=0;i<=num;++i) {
		dumpFunEntry(i);
	}
}

void dumpVar(struct var *v) {
//fprintf(stderr,"\n~200V");
	fprintf(stderr,"\n var %d: %s %d %s %d ", v-vartab,
		(*v).name, (*v).class, typeToWord((*v).type), (*v).len );
/*	if((*v).value.up) 
		fprintf(stderr," ref to pr[%d]", (char*)((*v).value.up)-pr);
*/
		dumpVal( (*v).type, (*v).class, &((*v).value), 0 );
}

void dumpVarTab() {
	int pos = 0;
	fprintf(stderr,"\nVar Table: name class type len (type)value");
	struct var *v = vartab-1;
	while(++v < &vartab[nxtvar]) {
//fprintf(stderr,"\n~213V");
		dumpVar(v);
		++pos;
	};
	if( !pos )fprintf(stderr," empty");
}
