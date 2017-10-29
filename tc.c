#include "tc.h"

/* called by st(), does nothing, used for stats, debugging, whatever. 
 */
int traceCount=0;
int traceMode=0;

void stbegin() {
	if(traceMode) {
		printf("---- %d\n",traceCount++);
		dumpLine();
	}
	debug();
}
/* so gdb can 'b debug', and brkpt comes AFTER the dumpLine
 */
void debug(){}
/*	valid exit, last chance for summary stats, whatever. 
	prbegin/prdone called just before/after application level.
	tcexit called before exiting the interpreter.
 */
void prbegin(){};
void prdone(){};
void tcexit(){};

/* chunk 1: literals
/************** literals **************/
char* xif = "if";
char* xelse = "else";
char* xint = "int";
char* xchar = "char";
char* xwhile = "while";
char* xreturn = "return";
char* xbreak = "break";
char* xendlib = "endlibrary";
char* xr = "r";
char* xg = "g";
char* xlb = "[";
char* xrb = "]";
char* xlpar = "(";
char* xrpar = ")";
char* xcomma = ",";
char* newline = "\n";
char* xcmnt = "/*";
char* xstar = "*";
char* xsemi = ";";
char* xpcnt = "%";
char* xslash = "/";
char* xplus = "+";
char* xminus = "-";
char* xlt = "<";
char* xgt = ">";
char* xnoteq = "!=";
char* xeqeq = "==";
char* xeq = "=";
char* xge = ">=";
char* xle = "<=";
char* xnl = "\n";

/* chunk 2: eq, topdiff, toptoi, pop, pone, pzero, pushk, pushst, eset, ps(etc)
 */

/* stored size */
int typeToSize( int class, Type type ) {
	if(type==Char)return 1;
	else if(type==Int)return sizeof(int);
	else eset(TYPEERR);
	return 0; /* has to be one of the above */
}

/* SITUATION: Parsed an assignment expression. Two stack entries, lvalue, datam.
 *	Effects the assignment. 
 */
int eq() {
	int  iDatum;  /* memcpy into these from pr using val.stuff */
	char cDatum;  /*  and val.size, giving needed cast */
	void* pDatum;

	struct stackentry *val = &stack[nxtstack-1]; /* value (on top) */
	struct stackentry *lval = &stack[nxtstack-2]; /* where to put it */
	popst();popst();  
	void* where = &((*lval).value.up);
	int class = (*lval).class;
	int type = (*lval).type;
	int whereSize = typeToSize(class,type);  /* of the lvalue */

	if((*lval).lvalue != 'L') { eset(LVALERR); return; }
	
	if(class==1 && (*val).class==1) {
		pDatum = (*val).value.up;
		if( (*val).lvalue=='L' ){
			pDatum = *(int*)pDatum;   /* now its 'A' */
		}
		char **where = (*lval).value.up;
		*where = (char*)pDatum;
		pushst(class, 'A', type, &(*val).value);
	}
	else if(class==1 && (*val).class==0) {
		eset(EQERR);
	}
	else if(class==0 && (*val).class==1) {
		eset(EQERR);
	}
	else if(class==0 && (*val).class==0) {
		if(type=Int){
			if( (*val).lvalue=='L' ) 
				iDatum = get_int((*val).value.up);
			else 
				iDatum = (*val).value.ui;
			put_int( (*lval).value.up, iDatum);
			pushk(iDatum);
		}
		else if(type==Char){
			cDatum = (*val).value.uc;
			put_char( (*lval).value.up, cDatum );
			pushk(cDatum);
		}
	}
	
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
	struct stackentry *top = &stack[--nxtstack];
	if( (*top).class==1 ) {
		if((*top).lvalue == 'L') {
			ptr = (union stuff *)((*top).value.up);
			datum=(*ptr).up;
		}
		else datum=(char*)((*top).value.up);
	}
	else if((*top).lvalue == 'L') {
		if((*top).type==Int ) datum = *((int*)((*top).value.up));
		else if((*top).type==Char) datum = *((char*)((*top).value.up));
		else eset(TYPEERR);
	}
	else if((*top).lvalue == 'A') {
		if((*top).type==Char) datum  = (char)((*top).value.uc);
		else if((*top).type==Int) datum  = (char)((*top).value.ui);
		else eset(TYPEERR);
	}
	else { eset(LVALERR); }
/* ISSUE what about other types for 'L' ANSWER class 1 */
	return datum;
}
/* basic popper, entry stays accessible until popped over */
struct stackentry* popst() {
	if( nxtstack-1 < 0 ) { error = POPERR; return; }
	--nxtstack;
	return &stack[nxtstack];
}

void stuffCopy( union stuff *to, union stuff *from ) {
	memcpy( to, from, sizeof(to));
}

/* basic pusher */
void pushst( int class, int lvalue, Type type, union stuff *value ) {
	if( nxtstack > STACKLEN) { error = PUSHERR; return; }
	stack[nxtstack].class = class;
	stack[nxtstack].lvalue = lvalue;
	stack[nxtstack].type = type;
	stuffCopy( &stack[nxtstack].value, value);
	++nxtstack;
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
/******* set error unless already set, capture cursor in errat *******/
int eset( int err ){
	if(!error){error=err;errat=cursor;}
	return error;
}
/************ simple prints ******************/
void ps(char* s) {printf("%s",s);}
void pl(char* s) {printf("\n%s",s);}
int  pn(int n)   {printf("%d", n);return n;}
void pc(char c)  {printf("%c", c);}

/* chunk 3: lit, skip, alnum, alpha, symname, konst, rem
 */

/* Bump cursor over whitespace. Then return true on match and advance
   cursor beyond the literal else false and do not advance cursor
 */
int lit(char *s){
	while( *cursor == ' ' 
		|| *cursor == '\t' ) ++cursor;
	int match = !strncmp(s,cursor, strlen(s));
	if( match ) {
		cursor += strlen(s);
		return 1;
	}
	return 0; /* no match */
}

/* skips balance l-r assuming left is matched. 
 *	Returns 0 on OK, else curserr.
 */
int skip(char l, char r) {
	int counter = 1;
	 while( counter>0 && cursor<epr ) {
		if(*cursor==l)++counter;
		if(*cursor==r)--counter;
		++cursor;
	};
	if( cursor > epr ) { eset(CURSERR); return error; }
	return 0;
}


/* Parse a symbol defining fname, lname. ret: true if symbol */
int symname() {
	while( *cursor == ' ' ) ++cursor;
	if( isalpha(*cursor) || *cursor=='_') {
		fname = cursor;
	}
	else return 0;  /* not a symbol */
	while( isalnum(*cursor) || *cursor=='_'){
		++cursor; 
	}
	lname = cursor-1;
	return 1;  /* good, fname and lname defined */
}

/* some C helper functions */
/* return true if symname matches arg, no state change (new) */
int symnameis(char* name){
	int x = strncmp(fname, name, lname-fname+1);
	return( !x );
}
/* State is not changed by find or mustFind. Returned value is
sole purpose of find. That plus setting err for mustFind. */
char* find( char* from, char* upto, char c) {
	char* x = from;
	while( *x != c && x<upto) {
		++x;
	}
	return x<upto ? x : 0;
}
/* same as find but sets err on no match */
char* mustfind( char *from, char *upto, char c, int err ) {
	char* x = find(from, upto, c);
	if( x ) return x;
	else { eset(err); return 0; }
}

/* special find for end of string */
char* findEOS( char* x ) {
	while( x<epr) {
		if( *x==0 || *x==0x22 ) return x;
		++x;
	}
	eset(CURSERR);
	return 0;
}

/*	parses constant defining fname..lname which brackets trimmed constant. 
 *	Cursor moved just beyond constant. Returns Type: 
 */
Type konst() {
	char* x;
	while(*cursor==' ')++cursor;
	char c = *cursor;
	if( c=='+' || c=='-' || (c>='0'&&c<='9') ) {
		fname = cursor;
		do{
			++cursor; c=*cursor;
		} while(c>='0'&&c<='9');
		lname=cursor-1;
		return Int;

	} else if(lit("\"")) {
		fname=cursor;
		/* set lname = last char, cursor = lname+2 (past the quote) */
		if( x = findEOS(fname) ) {
			lname = x-1; /*before the quote */
			cursor = x+1; /*after the quote */
			*x = 0;
		}
		else { eset(CURSERR); return Err; }
		return CharStar;

	} else if(lit("\'")) {
		fname=cursor;
		/* lname = last char, cursor = lname+2 (past the quote) */
		if( x=mustfind(fname+1,epr,'\'',CURSERR) ) {
			lname = x-1; 
			cursor = x+1;
		}
		else { eset(CURSERR); return -1; }
		return Char;
	
	} else return Err;  /* no match, Err==0 */
}

/* skip over comments and/or empty lines in any order
 */
void rem() {
	for(;;) {
		while(lit(xnl));
		if( !lit(xcmnt) ) return;
		while( *cursor != '\n' && cursor<epr ) ++cursor;
		++cursor;
	}
}

/* chunk 4: newfun, fundone, newvar, addrval, canon
 */

/* SITUATION: Function call parsed. 
	Open new var and value frames for the functions locals.
 */
void newfun() {
	if(++curfun>efun){
		eset(TMFUERR);
	} 
	else {
		(*curfun).fvar = (*curfun).lvar = nxtvar;
		(*curfun).prused = prused;
	}
}

/* SITUATION: function is completed. 
 *	Close its var and value frames.
 */
void fundone() {
	nxtvar=(*curfun).fvar;
	prused=(*curfun).prused;
	--curfun;
}

/*********** var tools ****************/

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
	int obsize = typeToSize(class,type);
	if(allocSpace(v,len*obsize)) return;  /* true is bad, eset done */
	if(passed) copyArgValue( v, class, type, passed);
	if(curfun>=fun) (*curfun).lvar = nxtvar;
	if( ++nxtvar>VTABLEN )eset(TMVRERR);
	return;
}

/* allocates memory for value of v, return 0 on success, else !0
 */
int allocSpace(struct var *v, int amount){
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

/*	copy the argument value into the new local place.
 */
int copyArgValue(struct var *v, int class, Type type, union stuff *passed ) {
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

/* 	looks up a symbol pointed to by fname,lname.
 *	This version looks from 0 to nxtvar-1.
 *	ISSUE	Ultimate version looks through 3 regions, locals,globals,library.
 */
struct var* addrval() {
	struct funentry *sfun = curfun;
	struct var temp;
	canon( &temp );  /* temp.name is string version of fname..lname */
	int area;
	for(area=0; area<3; ++area){
		int first = (*sfun).fvar;
		int last  = (*sfun).lvar;
		int pvar;
		for(pvar=first; pvar<=last; ++pvar) {
			if( !strcmp(vartab[pvar].name, temp.name) ) return &vartab[pvar];
		}
		switch(area){
		case 0:
			sfun = curglbl;		/* locals done, switch to globals */
			break;
		case 1:
			sfun = fun;			/* globals done, switch to library */
			break;
		case 2:
			eset(SYMERRA);
			return 0;   		/* not found */
		}
	}
}

/* 	fname..lname is full name. Puts canonicalized name into v. If short
 *	enough same as name. If longer first VLEN-1 characters + last character.
 *	The last char has more info than middle chars.
 */
void canon(struct var *v) {
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
/*		*((*v).name+VLEN-1) = *lname;  */
/* above 2 lines are equivalent!! But which is more readable? */
	} 
}


/* chunk 5: asgn, .., factor. Classical LR1 grammer.
 */

/* SITUATION: Cursor is positioned where an expression must exist. It may or 
 *	may not be an assignment. 
 *	Classical LR parsing is used to parse the full expression, evaluating as 
 *	it is parsed, and leaving one value on the stack. Returns true on success.
 * (This comment applies to functions asgn,reln...,factor and konst).
 */
int asgn(){ 
	if(reln()){
		if(lit(xeq)){
			asgn();
			if(!error)eq();
		}
	}
	return error? 0: 1;
}

int reln(){
	if(expr()){
		if(lit(xle)){
			if(expr()){
				if(topdiff()<=0)pushone();
				else pushzero();
			}
		}
		else if(lit(xge)){
			if(expr()){
				if(topdiff()>=0)pushone();
				else pushzero();
			}
		}
		else if(lit(xeqeq)){
			if(expr()){
				if(topdiff()==0)pushone();
				else pushzero();
			}
		}
		else if(lit(xnoteq)){
			if(expr()){
				if(topdiff()!=0)pushone();
				else pushzero();
			}
		}
		else if(lit(xgt)){
			if(expr()){
				if(topdiff()>0)pushone();
				else pushzero();
			}
		}
		else if(lit(xlt)){
			if(expr()){
				if(topdiff()<0)pushone();
				else pushzero();
			}
		}
		else return 1;  /* just expr is a reln */
	}
	return 0;   /* not an expr is not a reln */
}
int expr(){
	if(lit(xminus)){    /* unary minus */
		term();
		pushk(-toptoi());
	}
	else if(lit(xplus)){
		term();
		pushk(toptoi());
	}
	else term();
	while(!error){    /* rest of the terms */
		int leftclass = stack[nxtstack-1].class;
		int rightclass;
		if(lit(xminus)){
			term();
			rightclass = stack[nxtstack-1].class;
			int b=toptoi();
			int a=toptoi();
			if( rightclass || leftclass) pushPtr(a-b);
			else pushk(a-b);
		}
		else if(lit(xplus)){
			term();
			rightclass = stack[nxtstack-1].class;
			int b=toptoi();
			int a=toptoi();
			if( rightclass || leftclass) pushPtr(a+b);
			else pushk(a+b);
		}
		else return 1;   /* is expression, all terms done */
	}
	return 0;   /* error, set down deep */
}

int term() {
	factor();
	if(error)return 0;
	if(lit(xstar)){
		factor();
		if(!error)pushk(toptoi()*toptoi());
	}
	else if(lit(xslash)){
		factor();
		int denom = toptoi();
		if(!error)pushk(toptoi()/denom);
	}
	else if(lit(xpcnt)){
		factor();
		int b=toptoi();
		int a=toptoi();
		int pct = a%b;
		if(!error)pushk(a%b);
	}
	return 1;  /* factor is a term */
}

/*	a FACTOR is a ( asgn ), or a constant, or a variable
 *	reference, or a function reference.
*/
void factor() {
	union stuff foo;
	Type type;
	char* x;
	if(lit(xlpar)) {
		asgn();
		if( x=mustfind( cursor, epr, ')' , RPARERR ) ) {
			cursor = x+1; /*after the paren */
		}
	} 
	else if( type=konst() ) {
	/* Defines fname,lname. Returns Type. 
	   void pushst( int class, int lvalue, Type type, void* stuff );
	*/
		switch(type){
		case Int: 
			pushk( ATOI() );  /* integer, use our ATOI */
			break;
		case Char:
			foo.uc = *fname;
			pushst( 0, 'A', type, &foo );
			break;
		case CharStar:		/* special type used ONLY here */
			foo.up = fname;
			pushst( 1, 'A', Char, &foo );
		case Err:
			return;
		}
	}
	else if( symname() ) {
		int where, len, class, obsize, stuff;
		if( symnameis("MC") ) { 
			enter(0); return;
		} else {
			struct var *v = addrval();  /* looks up symbol */
			if( !v ){ eset(SYMERR); return; } /* typo, or no decl */
		  	char* where = (*v).value.up;
		  	int integer =  (*v).value.ui; 
		  	int character = (*v).value.uc;
		  	int class=(*v).class; 
	  		int type=(*v).type; 
	  		int obsize = typeToSize(class,type);
	  		int len=(*v).len;
		  	if( class=='E' ) enter(where);  /* fcn call */
			else {   /* is var name */
				if( lit(xlpar) ) {		       /* is dimensioned */
			  		if( !class ) {   /* must be class>0 */
						eset(CLASERR);
			  		} else {  /* dereference the lvalue */
			  			/* reduce the class and recompute obsize */
	  					obsize = typeToSize(--class,type);
			  			/* increment where by subscript*obsize */
		        		asgn(); if( error )return;
		        		lit(xrpar);
			      		int subscript = toptoi();
						if(len-1)if( subscript<0 || subscript>=len )eset(RANGERR); 
						where += subscript * obsize;
						foo.up = where;
						pushst( class, 'L', type, &foo);
						return;
					}
				} else {	
				/* is simple. Must push as 'L', &storagePlace. */
					if(class==1)foo.up = &((*v).value.up);
					else foo.up = where;
			  		pushst( class, 'L', type, &foo);
				}
			}
		}
	}
	else {
		eset(SYNXERR);
	}
}

/* converts fname..lname inclusive to integer
 */
int ATOI() {
	char* x = fname;
	int val = 0;
	int sign = *x=='-' ? -1 : 1;
	if( *x=='-' || *x=='+' ) ++x;
	do{ 
		val = 10*val+*x-'0'; 
	} while( ++x <= lname );
	return sign*val;
}

/* chunk 6:  skipst, vAlloc
 */

/************ scan tools ******************/

/*	skip a possibly compound statement. Shortcoming is brackets
 *	in comments, they must be balanced.
 */
void skipst() {
	rem();
	if( lit(xlb) ) {		/* compound */
		skip('[',']');
		rem();
		return;
	}
	else if( lit(xif)||lit(xwhile) ) {
		lit(xlpar);			/* optional left paren */
		skip('(',')');
		rem();
		return;
	}
	else {					/* simple statement, newline or semi ends */
		while(++cursor<epr) {
			if( (*cursor=='\n') || (*cursor==';') )break;
		}
		++cursor;
		rem();
	}
}

/* 	SITUATION: int or char is parsed.
 *	Parses one variable. Makes allocation and symbol entry.
 */
void vAlloc(Type type, union stuff *vpassed) {
	if( !symname() ) {		/*defines fname,lname. True is match.*/
		eset(SYMERR);
		return;
	}
	if( lit("(") ){
		vclass = 1;		/* array or pointer */
		char* fn=fname; /* localize globals that asgn() may change */
		char* ln=lname;
		if( asgn() ) alen=toptoi()+1;  /* dimension */
		fname=fn; 		/* restore the globals */
		lname=ln;
		char* x = mustfind(cursor,epr,')',RPARERR);
		if(x)cursor = x+1;
	} else {
		vclass = 0;
		alen = 1;
	}
	newvar(vclass, type, alen, vpassed);
}

/* chunk 7: st, decl, quit
 */

/* Returns true if user signals quit.
 *	STUBBED for now. ISSUE how to set the quit flag from keyboard.
 */
int quit() { return 0; }

/* st(): interprets a possibly compound statement
 */
void st() {
	char *whstcurs, *whcurs, *objt, *agin ;
	int brake=0;

	if(quit())return;
	rem();
	stbegin();
	stcurs = cursor;
	if(decl()){
		rem();
		return;
	}
	else if( lit(xlb) ){     /* compound statement */
		for(;;){
			rem();
			if(leave||brake||error)return;
			if(lit(xrb)){
				/*rem();*/
				return;
			}
			st();
		}
	}
	else if(lit(xif)) {
		if(asgn()) {
			if(toptoi()) {
				st();
				if(lit(xelse)) {
					skipst();
				}
			} 
			else {
				skipst();
				if(lit(xelse)) {
					st();
				}
			}
			return;
		}
	}
	else if(lit(xwhile)) {
		lit(xlpar);    /* optional left paren */
		if( !asgn() )return;   /* error */
		lit(xrpar);
		int condition = toptoi();
		if( condition ) {
/* prepare for repeating/skipping while (stcurs) 
	or object */ 
			agin = stcurs;
			objt = cursor;
			st();

			if(brake) {
				cursor = objt;	/* break: done with the while */
				skipst();		/* skip over the object */
				brake = 0;
				return;
			}
			else {
				cursor = agin;	/* no break: do the entire while again */
				return;
			}
		}
		else {
			skipst();
		}
	}
	else if(lit(xsemi)) {
		rem();
	}
	else if(lit(xreturn)) {
		if(lit(xsemi)||lit(xnl)) {
			pushzero(); /* default return value */
		}
		else {
			asgn();  /* specified return value */
		}
		leave=1;		/* signal st() to leave the compound 
						statement containing this return */
		return;
	}
	else if(lit(xbreak)) {
		brake=1;
		return;
	}
	else if( asgn() ) {      /* if expression discard its value */
		toptoi();
        lit(xsemi);
	}
	else {
		eset(STATERR);
	}
}

/* Match char or int, else do nothing. If match parse
 *  all comma separated declarations of that particular type
 *	making var table entries and allocating value storage. Returns false
 *	if not a declaration statement, true if it is. Leaves cursor just past
 *  optional semi. 
 */
int decl() { 
	Type t;
	if( lit(xchar) ) {
		do {
			vAlloc( Char, 0 );  /* 2nd arg is vpassed */
		} while( lit(xcomma) );
	} else if( lit(xint) ) {
		do {
			vAlloc( Int, 0 );  /* 2nd arg is vpassed */
		} while( lit(xcomma) );
	} else {
		return 0;  /* not decl */
	}
	lit(xsemi);    /* is decl */
	return 1;
}

/* chunk 9: cold, warm, hot, loader, logo
 *  This chunk is replaced by command line args, see tcMain.c. 
  *	logo() remains here, though.
 */
void logo() {
	printf(
"***  TINY-C VERSION 1.0,  COPYRIGHT 1977, T A GIBSON  ***\n"
		);
	printf(
"        This C version copyright 2017, T A Gibson\n"
		);
}

/* chunk 8: enter, setarg, link
 */

/*	SITUATION: Just parsed symbol with class 'E', or special symbol MC, and
 *	the args. Their values are on the stack, arg pointing to the first 
 *	of them.
 *	Sets the cursor to the called function's 'where'. Parses arg decl's
 *	giving them values from the stack. Executes the function body.
 *	Pops the locals (vars and values). Restores the caller's stcurs and 
 *	cursor.
 */
void enter( char* where) {
	int arg=nxtstack, nargs=0;

	lit(xlpar); /* optional (   */
	int haveArgs = ! (  lit(xrpar)
					 || lit(xlb)
					 || lit(xsemi)
					 || lit(xnl) 
					 || lit(xslash) 
					);
	if ( haveArgs ) {
		do {
			if(error)return;
			if( asgn()) ++nargs;
			else break;  /* break on error */
		} while( lit(xcomma) );
	}
	if(error)return;
	lit(xrpar);   /* optional )   */
	if(!where) {
		if(nxtstack) machinecall( nargs );
		else eset(MCERR);
		return;
	}
	else {   /* ABOVE parses the call, BELOW parses the callee's arg decls */
		char *localstcurs=stcurs, *localcurs=cursor;
		cursor = where;
		newfun();  
		for(;;) {	  
			rem();
			if(lit(xint)) { 
				do {
					setarg(Int, &stack[arg]);
					arg++;
				} while(lit(xcomma));
				lit(xsemi); /* optional */
			} 
			else if ( lit(xchar)) {
				do {
					setarg(Char, &stack[arg]);
					arg++;
				} while(lit(xcomma));
				lit(xsemi);
			}
			else {
				break;		/* and the only way outa here */
			}
		}
		if(arg != nxtstack) {
			cursor=localcurs;
			stcurs=localstcurs;
			eset(ARGSERR);
		}
		while(nargs>0){
			popst();
			--nargs;
		}
		if(!error)st();     /*  <<-- HERE is where we do it */
		if(!leave)pushzero();
		leave=0;
		cursor=localcurs;
		stcurs=localstcurs;
		fundone();
	}
}

/* Situation: parsing argument declarations, passed values are on the stack.
 * arg points into stack to an argument of type. 
 * Gets actual value of arg, calls valloc which parses and sets
 * up local with the passed value.
 */ 
void setarg( Type type, struct stackentry *arg ) {
	union stuff vpassed  = (*arg).value;
	char* where;
	int class = (*arg).class;
	int lvalue = (*arg).lvalue;
	/*int size = (*arg).size; */
	if( lvalue=='L') {
		where = (char*)vpassed.up;
		if( type==Int ) vpassed.ui = get_int(where);
		else if( type==Char) vpassed.uc = get_char(where);
		else if( class==1 ) { 
			(*arg).value.up = *((char*)(*arg).value.up);
		}
	}
	vAlloc( type, &vpassed);
}

/*
 *	scans program from cursor to progend and allocates all externals 
 * 	in next fctn layer. An "endlibrary" line causes a new fctn layer
 * 	to be opened and globals done there.
 */
void link() {
	char* x;
	char* problemCursor=cursor;
	cursor=pr;
	newfun();
	while(cursor<epr && !error){
		char* lastcur = cursor;
		rem();
		if(lit(xlb)) skip('[',']');
		else if(decl()) ;
		else if(lit(xendlib))newfun();
		else if(symname()) {     /* fctn decl */
			union stuff kursor;
			kursor.up = cursor;
			newvar('E',2,1,&kursor);
			if(x=mustfind(cursor, epr, '\[',LBRCERR)) {
				cursor=x+1;
				skip('\[','\]');
			}
		}
		if(cursor==lastcur)eset(LINKERR);
	}
	cursor = problemCursor;
}

/* chunk 11: MC's. 
 *	A handful of very primitive machine (and compiler) dependant functions.
 *	This is the portability layer. 
 * 	Moved to machineCall.c
 */

/* chunk 12(new): dumps
 */

/*********** a variety of dumps for debugging **********/
char* typeToWord(Type t){
	switch(t) {
		case Char:     return "Char";
		case Int:      return "Int";
		default:       return " NOT A TYPE ";
	}
}


/* dump the line cursor is in from cursor to nl */
void dumpLine() {
	char* begin = cursor;
	char* end = cursor;
	while (*end!='\n' && end<epr){  /* find end of line */
		++end;
	}
	while(begin<end){
		printf("%c",*begin);
		++begin;
	}
}

void PrVal(Type t, int class, union stuff *val, char lval){
	printf("pr[%d]",(*val).up-(int)pr);
	if(class==1 && t==Char ){
		printf("->%s<-", (char*)((*val).up) );
	}
	else if(class==0 && lval!='A'){ 
		if(t==Char){
			char x = *(char*)((*val).up);
			if(x)printf("->%c<-", x );
			else printf("->NULL<-");
		}
		else printf("->%d<-", *(int*)((*val).up) );
	}
/*
	else if(t==Char) printf("%c",(*val).uc);
	else printf("%d",(*val).ui);
*/
}

void dumpStackEntry(int e){
	if( 0<=e && e<=nxtstack ) {
		printf("\n stack entry at %d: %d %c %d ", e, stack[e].class, 
			stack[e].lvalue, stack[e].type );
		PrVal(stack[e].type, stack[e].class, 
				&stack[e].value,stack[e].lvalue);
	}
	else {
		printf("no stack entry at %d", e);
	}
}
void dumpStack(){
	int e;
	pl("Stack (from top) class lvalue type stuff");
	for( e=nxtstack-1; e>=0; --e) {
		dumpStackEntry(e); 
	}
	printf("\n");
}
/* dumps the just popped stack entry */
void dumpPopTop() {
	dumpStackEntry(nxtstack);
	}

/* dumps the top stack entry */
void dumpTop() {
	dumpStackEntry(nxtstack-1);
}

void dumpFunEntry( int e ) {
	printf("\n fun entry at %d:  %d %d %d", e,
		fun[e].fvar, fun[e].lvar, fun[e].prused-pr );
}

void dumpFun() {
	printf("\nfun table: fvar, lvar, prused");
	int i;
	int num = curfun-fun;
	for(i=0;i<=num;++i) {
		dumpFunEntry(i);
	}
}

void dumpVar(struct var *v) {
	printf("\n var %d: %s %d %s %d ", v-vartab,
		(*v).name, (*v).class, typeToWord((*v).type), (*v).len );
/*	if((*v).value.up) 
		printf(" ref to pr[%d]", (char*)((*v).value.up)-pr);
*/
		PrVal( (*v).type, (*v).class, &((*v).value), 0 );
}

void dumpVarTab() {
	int pos = 0;
	ps("\nVar Table: name class type len (type)value");
	struct var *v = vartab-1;
	while(++v < &vartab[nxtvar]) {
		dumpVar(v);
		++pos;
	};
	if( !pos )pl(" empty");
}

void dumpHex( void* where, int len ) {
	char* w=where;
	printf("\n%x: ",w);
	int i;
	for( i=0; i<len; ++i )printf(" %x",w[i]);
}

int stateCallNumber=0;
void dumpState() {
	if(stateCallNumber++==0){
		printf("\nADDRESSES (hex)");
		printf("\npr:     %x",pr);
		printf("\nstack:  %x",stack);
		printf("\nvartab: %x",vartab);	
	}

	printf("\n----\nSTATE %d\nparsing: ",stateCallNumber);
	dumpLine();
	ps("<--end of line--");
	dumpVarTab();
	dumpStack();
}

/* dump a just parsed piece of pr, typically a name */
void dumpName() {
	char *c = fname;
	while( c <= lname ) { 
			pc(*(c++));
	}
}

/****  C tools to deal with typeless storage ****/

	void put_int(char *where, int datum) {
		memcpy( where, &datum, sizeof(datum) );
	}
	int get_int(char *where) {
		int datum;
		memcpy( &datum, where, sizeof(datum));
		return datum;
	}
	void put_char(char *where, char datum) {
		memcpy(where, &datum, sizeof(datum));
	}
	char get_char(char *where) {
		char datum;
		memcpy( &datum, where, sizeof(datum));
		return datum;
	}

/* reads two files using command line args for one or both.
 */
void readTheFiles(int argc, char *argv[]) {
	int nread;
	if(argc==2){
		nread = FileRead("pps/library.tc",epr,EPR-epr);
		if(nread == -1) {
			printf("file read error: pps/library.tc\n");
			exit(1);
		}
		else if(nread == 0) {
			printf("no such file: argv[1]\n");
			exit(1);
		}
		epr += nread;
		nread = FileRead( argv[1],epr,EPR-epr);
		if(nread == -1) {
			printf("file read error: %s\n",argv[1]);
			exit(1);
		}
		else if(nread == 0) {
			printf("no such file: %s\n",argv[1]);
			exit(1);
		}
		epr += nread;
		curglbl = fun+1;
	}
	else if(argc==3){
		nread = FileRead(argv[1],epr,EPR-epr);
		if(nread == -1) {
			printf("file read error: %s\n",argv[1]);
			exit(1);
		}
		else if(nread == 0) {
			printf("no such file: %s\n",argv[1]);
			exit(1);
		}
		epr += nread;
		nread = FileRead(argv[2],epr,EPR-epr);
		if(nread == -1) {
			printf("file read error: %s\n",argv[2]);
			exit(1);
		}
		else if(nread == 0) {
			printf("no such file: %s\n",argv[2]);
			exit(1);
		}
		epr += nread;
		curglbl = 1;
	}
}