#include "tc.h"

#if defined(_WIN32)
        char* defaultLibrary = "pps\\library.tc";
#else
        char* defaultLibrary = "/usr/local/share/tinyC/library.tc";
#endif

/*	BUGOUTS: Use these to gather stats, debug, whatever.
	prbegin/prdone called just before/after application level.
	tcexit called before exiting the interpreter.
	NOTE: moved to debug.c
void prbegin(){};
void prdone(){};
void tcexit(){};
void stbegin() {};
 */
 
 /* debugging aid, see ~177 for sample use */
 int watchme=0;

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
char* xcmnt2 = "//";
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

/* stored size of one datum */
int typeToSize( int class, Type type ) {
	if(type==Char)return 1;
	else if(type==Int)return sizeof(int);
	else eset(TYPEERR);
	return 0; /* has to be one of the above */
}

/* SITUATION: Parsed an assignment expression. Two stack entries, lvalue, datam.
 *	Effects the assignment. 
 */
int _eq() {
	int  iDatum;  /* memcpy into these from pr using val.stuff */
	char cDatum;  /*  and val.size, giving needed cast */
	void* pDatum;

	struct stackentry *val = &stack[nxtstack-1]; /* value (on top) */
	struct stackentry *lval = &stack[nxtstack-2]; /* where to put it */
	if(verbose[VE]){
		fprintf(stderr,"\neq: lval");
		dumpStackEntry(nxtstack-2);
		fprintf(stderr,"\neq: val");
		dumpStackEntry(nxtstack-1);
	}
/*	popst();popst();  */
	void* where = &((*lval).value.up);
	int class = (*lval).class;
	int type = (*lval).type;
	int whereSize = typeToSize(class,type);  /* of the lvalue */

	if((*lval).lvalue != 'L') { 
		eset(LVALERR); 
		return; 
	}
	
	if(class==1 && (*val).class==1) {
		pDatum = (*val).value.up;
		if( (*val).lvalue=='L' ){
			pDatum = (char*)(*(int*)pDatum);   /* now its 'A' */
		}
		char **where = (*lval).value.up;
		*where = (char*)pDatum;
		pushst(class, 'A', type, &(*val).value);
	}
	else if(class==1 && (*val).class==0) {  /* ptr = int */
		if( (*val).type != Int ){
			eset(EQERR);
			return;
		}
		if( (*val).lvalue=='L' ) {
			iDatum = get_int((*val).value.up);
		}
		else {
			iDatum = (*val).value.ui;
		}
		pDatum = (void*)iDatum;
		char **where = (*lval).value.up;
		*where = (char*)pDatum;
		pushst(class, 'A', type, &(*val).value);
	}
	else if(class==0 && (*val).class==1) {  /* int = ptr */
		if(type!=Int){
			eset(EQERR);
			return;
		}
		pDatum = (*val).value.up;
		if( (*val).lvalue=='L' ){
			pDatum = (char*)(*(int*)pDatum);   /* now its 'A' */
		}
		iDatum = (int)pDatum;
		put_int( (*lval).value.up, iDatum);
		pushk(iDatum);
	}
	else if(class==0 && (*val).class==0) {
		if(type==Int){
			if( (*val).lvalue=='L' ) {
				iDatum = get_int((*val).value.up);
			}
			else {
				iDatum = (*val).value.ui;
			}
			if((*val).type==Char) iDatum = iDatum&0xff;
			put_int( (*lval).value.up, iDatum);
			pushk(iDatum);
		}
		else if(type==Char){
			if( (*val).lvalue=='L' ) {
				cDatum = get_char((*val).value.up);
			}
			else {
				cDatum = (*val).value.uc;
			}
			put_char( (*lval).value.up, cDatum );
			pushk(cDatum);
		}
	}
	popst();popst();  
}

/******* set error unless already set, capture cursor in errat *******/
int eset( int err ){
	if(!error){error=err;errat=cursor;}
	return error;
}

/* Bump cursor over whitespace. Then return true on match and advance
   cursor beyond the literal else false and do not advance cursor
 */
int _lit(char *s){
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
int _skip(char l, char r) {
	int counter = 1;
	 while( counter>0 && cursor<epr ) {
		if(*cursor==l)++counter;
		if(*cursor==r)--counter;
		++cursor;
	};
	if( counter ) { eset(CURSERR); return error; }
	return 0;
}

/* Parse a symbol defining fname, lname. ret: true if symbol.
 *	Advances the cursor to but not over the symbol,
 */
int _symName() {
	char* temp;
	while( *cursor == ' ' || *cursor == '\t' ) ++cursor;
	temp=cursor;
	if( isalpha(*temp) || *temp=='_') {
		fname = temp;
	}
	else return 0;  /* not a symbol */
	while( isalnum(*temp) || *temp=='_'){
		++temp; 
	}
	lname = temp-1;
	if(verbose[VP]){
		fprintf(stderr,"\nparsed ");
		dumpft(fname,lname);
	}
	return lname-fname+1;  /* good, fname and lname defined */
}

/****************** some C helper functions **************/

/* return true if symname matches arg, no state change */
int _symNameIs(char* name){
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
char* _mustFind( char *from, char *upto, char c, int err ) {
	char* x = find(from, upto, c);
	if( x ) return x;
	else { eset(err); return 0; }
}

/* special find for end of string */
char* _findEOS( char* x ) {
	while( x<epr) {
		if( *x==0 || *x==0x22 ) return x;
		++x;
	}
	eset(CURSERR);
	return 0;
}

/* skip over comments and/or empty lines in any order, new version
	tolerates 0x0d's, and implements // as well as old /* comments.
 */
void _rem() {
	for(;;) {
		while(    *cursor==0x0a
				||*cursor==0x0d
				||*cursor==' '
				||*cursor=='\t'
			  )++cursor;
		if( !(_lit(xcmnt)||_lit(xcmnt2)) ) return;
		while( *cursor != 0x0a && *cursor != 0x0d && cursor<epr )
			++cursor;
	}
}

/* 	SITUATION: int or char is parsed.
 *	Parses one variable. Makes allocation and symbol entry.
 */
void _varAlloc(Type type, union stuff *vpassed) {
	if( !_symName() ) {		/*defines fname,lname. True is match.*/
		eset(SYMERR);
		return;
	}
	cursor=lname+1;
	if( _lit("(") ){
		vclass = 1;		/* array or pointer */
		char* fn=fname; /* localize globals that _asgn() may change */
		char* ln=lname;
		if( _asgn() ) alen=toptoi()+1;  /* dimension */
		fname=fn; 		/* restore the globals */
		lname=ln;
		char* x = _mustFind(cursor,cursor+5,')',RPARERR);
		if(x)cursor = x+1;
	} else {
		vclass = 0;
		alen = 1;
	}
	newvar(vclass, type, alen, vpassed);
}

/* Situation: parsing argument declarations, passed values are on the stack.
 * arg points into stack to an argument of type. 
 * Gets actual value of arg, calls valloc which parses and sets
 * up local with the passed value.
 */ 
void _setArg( Type type, struct stackentry *arg ) {
	union stuff vpassed  = (*arg).value;
	char* where;
	int class = (*arg).class;
	int lvalue = (*arg).lvalue;
	int stacktype = (*arg).type;
	if( lvalue=='L') {
		where = (char*)vpassed.up;
		if( class==1 ) { 
			vpassed.up = *((char**)(*arg).value.up);
		}
		else if( stacktype==Int ) vpassed.ui = get_int(where);
		else if( stacktype==Char) vpassed.ui = get_char(where);
			/* ui to clear high order byte */
	}
	_varAlloc( type, &vpassed);
}

/*	SITUATION: Just parsed symbol with class 'E', or special symbol MC.
 *	Parses the args putting values are on the stack, arg pointing to the first 
 *	of them.
 *	Sets the cursor to the called function's 'where'. Parses arg decl's
 *	giving them values from the stack. Executes the function body.
 *	Pops the locals (vars and values). Restores the caller's stcurs and 
 *	cursor.
 */

void saveName() {
	_canon(fname,lname,fcnName);
/*	int j=0;
	char* i;
	for(i=fname;i<=lname;++i) fcnName[j++] = *i;
	fcnName[j]=0;
*/
}

void _enter( char* where) {
	int arg=nxtstack, nargs=0;

	if(where)saveName();
	_lit(xlpar); /* optional (   */
	int haveArgs = ! (  _lit(xrpar)
					 || *cursor==*xlb
					 || *cursor==*xrb
					 || *cursor==*xsemi
					 || *cursor==*xnl
					 || *cursor==0x0d
					 || *cursor==*xslash
					);
	if ( haveArgs ) {
		do {
			if(error)return;
			if( _asgn()) ++nargs;
			else break;  /* break on error */
		} while( _lit(xcomma) );
	}
	if(error)return;
	_lit(xrpar);   /* optional )   */
	_rem();
	if(!where) {
		if(nxtstack) machinecall( nargs );
		else eset(MCERR);
		return;
	}
	else {   /* ABOVE parses the call args, BELOW parses the called's arg decls */
		char *localstcurs=stcurs, *localcurs=cursor;
		cursor = where;
		newfun();  
		for(;;) {	  
			_rem();
			if(_lit(xint)) { 
				do {
					_setArg(Int, &stack[arg]);
					arg++;
				} while(_lit(xcomma));
				_lit(xsemi); /* optional */
			} 
			else if ( _lit(xchar)) {
				do {
					_setArg(Char, &stack[arg]);
					arg++;
				} while(_lit(xcomma));
				_lit(xsemi);
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

/********** expression parser *****************/

/* converts fname..lname inclusive to integer
 */
int _atoi() {
	char* x = fname;
	int val = 0;
	int sign = *x=='-' ? -1 : 1;
	if( *x=='-' || *x=='+' ) ++x;
	do{ 
		val = 10*val+*x-'0'; 
	} while( ++x <= lname );
	return sign*val;
}

/*	parses constant defining fname..lname which brackets trimmed constant. 
 *	Cursor moved just beyond constant. Returns Type: 
 */
Type _konst() {
	char* x;
	while(*cursor==' ')++cursor;
	char c = *cursor;
	if( c=='+' || c=='-' || (c>='0'&&c<='9') ) {
		fname = cursor;
		do{
			++cursor; c=*cursor;
		} while(c>='0'&&c<='9');
		lname=cursor-1;
		if(verbose[VP]){
			fprintf(stderr,"\nparsed ");
			dumpft(fname,lname);
		}
		return Int;

	} else if(_lit("\"")) {
		fname=cursor;
		/* set lname = last char, cursor = lname+2 (past the quote) */
		if( x = _findEOS(fname) ) {
			lname = x-1; /*before the quote */
			cursor = x+1; /*after the quote */
			*x = 0;
		}
		else { eset(CURSERR); return Err; }
		if(verbose[VP]){
			fprintf(stderr,"\nparsed ");
			dumpft(fname,lname);
		}
		return CharStar;

	} else if(_lit("\'")) {
		fname=cursor;
		/* lname = last char, cursor = lname+2 (past the quote) */
		if( x=_mustFind(fname+1,fname+2,'\'',CURSERR) ) {
			lname = x-1; 
			cursor = x+1;
		}
		else { eset(CURSERR); return -1; }
		if(verbose[VP]){
			fprintf(stderr,"\nparsed ");
			dumpft(fname,lname);
		}
		return Char;
	
	} else return Err;  /* no match, Err==0 */
}

/* An asgn is a reln or an lvalue = asgn. Note that
   reln can match an lvalue.
 */
int _asgn(){ 
	if(_reln()){
		if(_lit(xeq)){
			_asgn();
			if(!error)_eq();
		}
	}
	return error? 0: 1;
}

/* a RELN is an expr or a comparison of exprs
 */
int _reln(){
	if(_expr()){
		if(_lit(xle)){
			if(_expr()){
				if(topdiff()<=0)pushone();
				else pushzero();
			}
		}
		else if(_lit(xge)){
			if(_expr()){
				if(topdiff()>=0)pushone();
				else pushzero();
			}
		}
		else if(_lit(xeqeq)){
			if(_expr()){
				if(topdiff()==0)pushone();
				else pushzero();
			}
		}
		else if(_lit(xnoteq)){
			if(_expr()){
				if(topdiff()!=0)pushone();
				else pushzero();
			}
		}
		else if(_lit(xgt)){
			if(_expr()){
				if(topdiff()>0)pushone();
				else pushzero();
			}
		}
		else if(_lit(xlt)){
			if(_expr()){
				if(topdiff()<0)pushone();
				else pushzero();
			}
		}
		else return 1;  /* just expr is a reln */
	}
	return 0;   /* not an expr is not a reln */
}

/* ;an EXPR is a term or sum (diff) of terms.
 */
int _expr(){
	if(_lit(xminus)){    /* unary minus */
		_term();
		pushk(-toptoi());
	}
	else if(_lit(xplus)){
		_term();
		pushk(toptoi());
	}
	else _term();
	while(!error){    /* rest of the terms */
		int leftclass = stack[nxtstack-1].class;
		int rightclass;
		if(_lit(xminus)){
			_term();
			rightclass = stack[nxtstack-1].class;
			int b=toptoi();
			int a=toptoi();
			if( rightclass || leftclass) pushPtr(a-b);
			else pushk(a-b);
		}
		else if(_lit(xplus)){
			_term();
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

/* ;a term is a factor or a product of factors.
 */
int _term() {
	_factor();
	while(!error) {
		if(_lit(xstar)){
			_factor();
			if(!error)pushk(toptoi()*toptoi());
		}
		else if(_lit(xslash)){
			if(*cursor=='*') {
				--cursor;    /* opps, its a comment */
				return 1;
			}
			_factor();
			int denom = toptoi();
			int numer = toptoi();
			int div = numer/denom;
			if(!error)pushk(div);
		}
		else if(_lit(xpcnt)){
			_factor();
			int b=toptoi();
			int a=toptoi();
			int pct = a%b;
			if(!error)pushk(pct);
		}
		else return 1;  /* no more factors */
	}
	return 0;
}

/* a FACTOR is a ( asgn ), or a constant, or a variable reference, or a function
    reference. NOTE: factor must succeed or it esets SYNXERR. Callers test error
    instead of a returned true/false. This varies from the rest of the expression 
    stack.
 */
void _factor() {
	union stuff foo;
	Type type;
	char* x;
	if(_lit(xlpar)) {
		_asgn();
		if( x=_mustFind( cursor, cursor+5, ')' , RPARERR ) ) {
			cursor = x+1; /*after the paren */
		}
	} 
	else if( type=_konst() ) {
	/* Defines fname,lname. Returns Type. 
	   void pushst( int class, int lvalue, Type type, void* stuff );
	*/
		switch(type){
		case Int: 
			pushk( _atoi() );  /* integer, use private _atoi */
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
	else if( _symName() ) {
		cursor = lname+1;
		int where, len, class, obsize, stuff;
		if( _symNameIs("MC") ) { 
			_enter(0); return;
		} else {
			struct var *v = addrval();  /* looks up symbol */
			if( !v ){ eset(SYMERR); return; } /* no decl */
		  	char* where = (*v).value.up;
		  	int integer =  (*v).value.ui; 
		  	int character = (*v).value.uc;
		  	int class=(*v).class; 
	  		int type=(*v).type; 
	  		int obsize = typeToSize(class,type);
	  		int len=(*v).len;
		  	if( class=='E' ) _enter(where);  /* fcn call */
			else {   /* is var name */
				if( _lit(xlpar) ) {		       /* is dimensioned */
			  		if( !class ) {   /* must be class>0 */
						eset(CLASERR);
			  		} else {  /* dereference the lvalue */
			  			/* reduce the class and recompute obsize */
	  					obsize = typeToSize(--class,type);
			  			/* increment where by subscript*obsize */
		        		_asgn(); if( error )return;
		        		_lit(xrpar);
			      		int subscript = toptoi();
						if(len-1)if( subscript<0 || subscript>=len )eset(RANGERR); 
						where += subscript * obsize;
						foo.up = where;
						pushst( class, 'L', type, &foo);
						return;
					}
				} else {	
				/* is simple. Must push as 'L', &storagePlace. */
					if(class==1){
						foo.up = &((*v).value.up);
					}
					else{
						foo.up = where;
					}
			  		pushst( class, 'L', type, &foo);
				}
			}
		}
	}
	else {
		eset(SYNXERR);
	}
}

/************ scan tools ******************/

/*	skip a possibly compound statement. Shortcoming is brackets
 *	in comments, they must be balanced.
 */
void _skipSt() {
	_rem();
	if( _lit(xlb) ) {		/* compound */
		_skip('[',']');
		_rem();
		return;
	}
	else if( _lit(xif)||_lit(xwhile) ) {
		_lit(xlpar);			/* optional left paren */
		_skip('(',')');
		_skipSt();
		_rem();
		if(_lit(xelse))_skipSt();
		_rem();
		return;
	}
	else {					/* simple statement, eol or semi ends */
		while(++cursor<epr) {
			if( (*cursor==0x0d)||(*cursor=='\n')||(*cursor==';') )break;
		}
		++cursor;
		_rem();
	}
}

/* Returns true if user signals quit, or any other error.
 *	NOTE: MC 2 esets KILL on ESC, but test here for hard loop
 */
int quit() {
	int foo[]={0};  /* to avoid tcc error */
	if(kbhit()==0x1b){
		getch_(0);
		return escKey();
	}
	return 0; 
}

/* Match char or int, else do nothing. If match parse
 *  all comma separated declarations of that particular type
 *	making var table entries and allocating value storage. Returns false
 *	if not a declaration statement, true if it is. Leaves cursor just past
 *  optional semi. 
 */
int _decl() { 
	Type t;
	if( _lit(xchar) ) {
		do {
			_varAlloc( Char, 0 );  /* 2nd arg is vpassed */
		} while( _lit(xcomma) );
	} else if( _lit(xint) ) {
		do {
			_varAlloc( Int, 0 );  /* 2nd arg is vpassed */
		} while( _lit(xcomma) );
	} else {
		return 0;  /* not decl */
	}
	_lit(xsemi);    /* is decl */
	return 1;
}

/* st(): interprets a possibly compound statement
 */
void st() {
	char *whstcurs, *whcurs, *objt, *agin ;
	brake=0;

	if(quit())return;
	_rem();
	stbegin();
	stcurs = cursor;
	if(_decl()){
		_rem();
		return;
	}
	else if( _lit(xlb) ){     /* compound statement */
		for(;;){
			_rem();
			if(leave||brake||error)return;
			if(_lit(xrb)){
				_rem();
				return;
			}
			st();
		}
	}
	else if(_lit(xif)) {
		if(_asgn()) {
			if(toptoi()) {
				st();
				if(_lit(xelse)) {
					_skipSt();
				}
			} 
			else {
				_skipSt();
				if(_lit(xelse)) {
					st();
				}
			}
			_rem();
			return;
		}
	}
	else if(_lit(xwhile)) {
		_lit(xlpar);    /* optional left paren */
		if( !_asgn() )return;   /* error */
		_lit(xrpar);
		int condition = toptoi();
		if( condition ) {
/* prepare for repeating/skipping while (stcurs) 
	or object */ 
			agin = stcurs;
			objt = cursor;
			st();

			if(brake) {
				cursor = objt;	/* break: done with the while */
				_skipSt();		/* skip over the object */
				brake = 0;
				return;
			}
			else {
				cursor = agin;	/* no break: do the entire while again */
				return;
			}
		}
		else {
			_skipSt();
		}
	}
	else if(_lit(xsemi)) {
		_rem();
	}
	else if(_lit(xreturn)) {
		int eos = ( _lit(xrpar)
					 || *cursor==*xlb
					 || *cursor==*xrb
					 || *cursor==*xsemi
					 || *cursor==*xnl
					 || *cursor==0x0d
					 || *cursor==*xslash
					);
		if ( eos ) {
			pushzero(); /* default return value */
		}
		else {
			_asgn();  /* specified return value */
		}
		leave=1;		/* signal st() to leave the compound 
						statement containing this return */
		return;
	}
	else if(_lit(xbreak)) {
		brake=1;
		return;
	}
	else if( _asgn() ) {      /* if expression discard its value */
		toptoi();
        _lit(xsemi);
	}
	else {
		eset(STATERR);
	}
}

/*
 *	Checks for balanced brackets, cursor to epr inclusive. 
 */
void checkBrackets() {
	int count;
	while(*(cursor++) != '[' && cursor<=epr) ;
	if(_skip('[',']'))eset(RBRCERR);
}

/*********** a variety of dumps for debugging **********/
char* typeToWord(Type t){
	switch(t) {
		case Char:     return "Char";
		case Int:      return "Int";
		default:       return " NOT A TYPE ";
	}
}

void dumpHex( void* where, int len ) {
	char* w=where;
	fflush(stdout);
	fprintf(stderr,"\n%x: ",w);
	int i;
	for( i=0; i<len; ++i )fprintf(stderr," %x",w[i]);
}

/* dump from..to inclusive  */
void dumpft(char *from, char *to ) {
	fflush(stdout);
	while(from <= to) fprintf(stderr,"%c",*(from++));
}

/* dump the line cursor is in from cursor to nl */
void dumpLine() {
	fflush(stdout);
	char* begin = cursor;
	char* end = cursor;
	while (*end!=0x0a && *end!=0x0d && end<epr){  /* find end of line */
		++end;
	}
	while(begin<end){
		fprintf(stderr,"%c",*begin);
		++begin;
	}
}

int stateCallNumber=0;
void dumpState() {
	fflush(stdout);
	if(stateCallNumber==0){
		fprintf(stderr,"\nADDRESSES (hex)");
		fprintf(stderr,"\npr:     %x",pr);
		fprintf(stderr,"\nstack:  %x",stack);
		fprintf(stderr,"\nvartab: %x",vartab);	
	}

	fprintf(stderr,"\n----\nSTATE %d\nparsing: ",stateCallNumber++);
	dumpLine();
	fprintf(stderr,"<--end of line--");
	dumpVarTab();
	dumpStack();
}

/* dump a just parsed piece of pr, typically a name */
void dumpName() {
	fflush(stdout);
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

/*	reads two files using command line args for one or both.
 *		./tc                      Usage
 *		./tc [libfile] appfile    Load and go
 *	  Default libfile is /usr/local/share/tinyC/library.tc
 */
void readTheFiles(int argc, char *argv[], int optind) {
	int nread;
	int optcount = optind-1;
	if(argc-optcount==2){
		/* sys */
#if defined(_WIN32)
        char* sysfile = "pps\\library.tc";
#else
        char* sysfile = "/usr/local/share/tinyC/library.tc";
#endif
		nread = fileRead(sysfile,epr,EPR-epr);
		if(nread == -1) {
			fprintf(stderr,"tc: file read error: %s",sysfile);
			exit(1);
		}
		else if(nread == 0) {
			fprintf(stderr,"tc-lib: no such file: %s\n",sysfile);
			exit(1);
		}
		apr = epr += nread;
		/* app */
		nread = fileRead( argv[optcount+1],epr,EPR-epr);
		if(nread == -1) {
			fprintf(stderr,"tc: file read error: %s\n",argv[optcount+1]);
			exit(1);
		}
		else if(nread == 0) {
			fprintf(stderr,"tc-app: no such file: %s\n",argv[optcount+1]);
			exit(1);
		}
		epr += nread;
		curglbl = fun+1;
	}
	else if(argc-optcount==3){
		/* sys, e.g. lib */
		nread = fileRead(argv[optcount+1],epr,EPR-epr);
		if(nread == -1) {
			fprintf(stderr,"file read error: %s\n",argv[optcount+1]);
			exit(1);
		}
		else if(nread == 0) {
			fprintf(stderr,"no such lib file: %s\n",argv[optcount+1]);
			exit(1);
		}
		apr = epr += nread;
		/* app */
		nread = fileRead(argv[optcount+2],epr,EPR-epr);
		if(nread == -1) {
			fprintf(stderr,"file read error: %s\n",argv[optcount+2]);
			exit(1);
		}
		else if(nread == 0) {
			fprintf(stderr,"no such app file: %s\n",argv[optcount+2]);
			exit(1);
		}
		epr += nread;
		curglbl = fun+1;
	}
	else {
		tcUsage();
		exit(1);
	}
}

/*
 *	Saves and (and later restores) cursor, then
 *	scans program from pr to progend and allocates all externals 
 * 	in next fctn layer. An "endlibrary" line causes a new fctn layer
 * 	to be opened and globals done there.
 */
void tclink() {
	char* x;
	char* savedCursor=cursor;
	checkBrackets();
	if(error)return;
	cursor=pr;
	newfun();
	while(cursor<epr && !error){
		char* lastcur = cursor;
		_rem();
		if(_lit(xlb)) _skip('[',']');
		else if(_decl()) ;
		else if(_lit(xendlib)){
			if(curfun==fun) {   /* 1st endlib, assume globals follow */
				newfun();
				curglbl=curfun;
			}
			else {        /* subsequent endlib, 
			                 move assummed globals to frame 0 */
				fun[0].lvar = nxtvar-1;     /* moved */
				fun[1].fvar = nxtvar;      /* globals now empty */
			}
		}
		else if(_symName()) {     /* fctn decl */
			union stuff kursor;
			kursor.up = cursor = lname+1;
			newvar('E',2,1,&kursor);
			if(x=_mustFind(cursor, epr, '[',LBRCERR)) {
				cursor=x+1;
				_skip('[',']');
			}
		}
		if(cursor==lastcur)eset(LINKERR);
	}
	cursor = savedCursor;
	if(verbose[VL])dumpVarTab();
}

/* returns pointer to first character of the current line
 */
char* fchar(char* k){
	do{
		if(*k==0x0a||*k==0x0d)break;
	} while( --k > apr);
	return k+1;
}
/* returns pointer to last character of the current line
 */
char* lchar(char* k){
	do{
		if(*k==0x0a||*k==0x0d)break;
	} while( ++k < epr);
	return k-1;
}
