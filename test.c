#include "tc.h"

#define testcases 70

char timeStamp[40];
extern struct stackentry poptop;
extern char* defaultLibrary;
extern char* xendlib;

void time_now(char *buff) {
  time_t curtime;
  struct tm *loctime;
  curtime = time (NULL);
  loctime = localtime (&curtime);
  strcpy(buff,asctime (loctime));
  strftime (buff, 40, " %R", loctime);
}

int loadCode(char* file) {
	int nread = fileRead(file, epr, EPR-epr);
	if(nread==0){
		fprintf(stderr,"No such file: %s",file);
		exit(1);
	}
	if(nread<0){
		fprintf(stderr,"Err reading file: %s",file);
		exit(1);
	}
	epr += nread;
	return nread;
}

void markEndlibrary() {
	strcpy(epr,xendlib);
	epr+=10;
	apr=epr;
}

/* setup tools used by some of the tests */
void testWhole(char* filename){
/* modified clone of tcMain.c code...
 */
	strcpy(pr,"[_MAIN();]");  /* required sys main */
	lpr = epr = prused = pr+10;
	cursor = pr;
	curglbl = fun;

	/* load the test subject code */
	loadCode(defaultLibrary);
	markEndlibrary();
	loadCode(filename);

	error=0;
	prused = epr+10;  /* a little slack */
	nxtvar = 0;
	nxtstack = 0;
	efun = fun+FUNLEN;
	curfun = fun-1;   /* none */
	tclink();
	cursor=pr;
	st();   /* <<<== executes statement above, line 13 */
	whatHappened();
}
void testSetup(char* code) {
	strcpy( pr, code);
	error=0;
	cursor = pr;
	epr = pr + strlen(pr) -1;
	prused = epr+10;  /* a little slack */
	nxtvar = 0;
	nxtstack = 0;
	curfun = fun-1;
	efun = fun+FUNLEN;
}

int testSetupFile(char* filename, int lib) {
	int len=0;
	curglbl = fun;
	strcpy(pr,"[]");
	lpr = pr+2;
	cursor = pr+2;
	if(lib) {
		len = fileRead("pps/library.tc",lpr,PRLEN);
		cursor += len; 	/*  cursor->app */
		curglbl = fun+1;
	}
	apr = cursor;
	len += fileRead(filename,lpr+len,PRLEN-len);
	error=0;
	epr = pr+len;
	prused = epr+10;  /* a little slack */
	nxtvar = 0;
	nxtstack = 0;
	efun = fun+FUNLEN;
	curfun = fun-1;
	return len;
}

/*
 *	Called by main. Args evaluated here. No args runs all tests. 
 *	List of test numbers runs just those tests.
 */
void testing(int argc, char *argv[]) {
	int testcase;

  	time_now(timeStamp);
	printf("%s",timeStamp);

	if(argc>1){
		int t;
		for( t=1; t<argc; ++t ) {
			testcase = atoi(argv[t]);
			printf("\n------------\nBEGIN TEST %d %s", testcase, timeStamp);
			fflush(stdout);
			doTest(testcase);
		}
		printf("\n");
	} 
	else {
/* do all tests */	
		nxtstack=nxtvar=0;
		cursor=prused=epr=pr;
		fprintf(stderr,"\ninitial state before any tests\n");
		dumpState();
		pl("\nBEGIN TESTING ALL");
		for(testcase=1; testcase<=testcases; ++testcase) {
			printf("\n\nTEST %d %s", testcase, timeStamp);
			fprintf(stderr,"\n\nTEST %d %s", testcase, timeStamp);
			doTest(testcase);
		}
		pl("");
	}
}

void doTest(int testcase) {
/* BIG switch wants all decls up here */
	char* rel_case[] = {
		"1<2", "1>2", "1==2", "1!=2", "1>=2", "1<=2",
		"2<2", "2>2", "2==2", "2!=2", "2>=2", "2<=2",
		"2<1", "2>1", "2==1", "2!=1", "2>=1", "2<=1"
		};
	int rel_cases = 18;
	int i, len;
	char* ops_case[] = {
		"7+2", "7-2","7*2", "7/2", "7%2",  "", "", "", "", "", "", "", 
	};
	int ops_cases = 5;

	union stuff Stuff;
	int err;
	int isInt; 
	char isChar; 
	char* isPtr;
	int iDatum;
	char cDatum;
	char buff[80];
	int ibuff[10];

	error=0;
	int array1[4];
	testSetup("");
	switch(testcase) {
		case 1: strcpy(pr,"foobaaarwxyz"); 
			fname = &pr[0];	
			lname = &pr[strlen(pr)-1];
			nxtvar = 0;
			prused = pr+50;
			newvar(0,Int,1,0);
			newvar(0,2,1,0);
			newvar(0,2,1,0);
			newvar(0,2,1,0);
			pl("name in pr ->");ps(pr);ps("<-");
			dumpVarTab();
			break;
/* canon() four tests. Partial vartab(). 
 *	Result: 4 copies of  "var: foobaaaz 0 2 1 (i)1" for each test.
 *	The data is fake. Check the canonicalized name is proper. 
 *	Should get: foobaaaz
 */
		case 2: strcpy(pr,"foob5678x"); 
			fname = &pr[0];	
			lname = &pr[strlen(pr)-1];
			nxtvar = 0;
			prused = pr+20;
			newvar(0,Int,1,0);
			--lname;
			newvar(0,2,1,0);
			--lname;
			newvar(0,2,1,0);
			--lname;
			newvar(0,2,1,0);
			pl("name in pr ->");ps(pr);ps("<-");fflush(stdout);
			dumpVarTab();
			break;
/*	Should get: foob567x, foob5678, f00b567, foob56  */

		case 3: strcpy(pr,"foob"); 
			fname = &pr[0];	lname = &pr[strlen(pr)-1];
			nxtvar = 0;
			prused = pr+20;
			newvar(0,Int,1,0);
			newvar(0,2,1,0);
			newvar(0,2,1,0);
			newvar(0,2,1,0);
			pl("name in pr ->");ps(pr);ps("<-");
			dumpVarTab();
			break;
/*	Should get: foob */

		case 4: strcpy(pr,"foo(baaarwxyz"); 
			fname = &pr[0];	lname = &pr[2];
			nxtvar = 0;
			prused = pr+20;
			newvar(0,Int,1,0);
			newvar(0,2,1,0);
			newvar(0,2,1,0);
			newvar(0,2,1,0);
			pl("name in pr ->");ps(pr);ps("<-");
			dumpVarTab();
			break;
/*	Should get: foo  */

		case 5:
			strcpy(pr,"char symbol, more stuff"); 
			prused = pr+100;
			cursor = pr+5;
			_symName();
			cursor = lname+1;
			pl("");dumpName();
			printf("\nfname,lname,cursor= %d %d %d",fname-pr,lname-pr,cursor-pr);
			break;
/* _symName() defines fname, lname, and advances cursor. 
	Should get...
			symbol
			fname,lname,cursor= 5 10 11
*/
		case 6: nxtvar=0;
			/*         01234567890123456789 V 345       */
			strcpy(pr,"char symbol, 34567890123456789012345"); 
			prused = pr+20;
			cursor = pr+5;
        	_varAlloc(Char,0);
        	dumpVarTab();
			printf("\npr ->%s<-",pr);		/* stops at the NULL */
			printf("\npr ->%s<-",pr+22);	/* resumes after the NULL */
			break;
/* vAlloc puts single NULL at the V. 
	Should get...
					Var Table: name class type len (type)value
					 var 0: symbol 0 Char 1 pr[21]-><-
					pr ->char symbol, 34567890<-
					pr ->23456789012345<-
*/
		case 7: cursor = pr;
			strcpy(pr,"char symbol, more stuff"); 
			prused = pr+100;
			cursor = pr;
			/* calling _lit(xchar) twice */
			pl("cursor->");ps(cursor);
			/* first, match true, cursor -> space after char */
			int match = _lit("char");  /* lit should match */
			printf("\nmatch,cursor= %d %d",match,cursor-pr);
			/* again, no match this time */
			pl("cursor->");ps(cursor);
			match = _lit("char");
			printf("\nmatch,cursor= %d %d",match,cursor-pr);
			/* cursor -> s of symbol */
			pl("cursor->");ps(cursor);
			break;
/* _lit(). Must match char. Two calls. First succeeds, 
	second fails returning 0, cursor advanced
	over white space. 	
	Should get...
			cursor->char symbol, more stuff
			match,cursor= 1 4
			cursor-> symbol, more stuff
			match,cursor= 0 5
			cursor->symbol, more stuff
*/

		case 8:
			cursor = pr+5;
			nxtvar = 0;
			strcpy(pr,"char a,b(9),c; int d,e(9),f;");
			cursor = pr;
			prused = pr+100;
			epr = prused+100;
			/* 2 decl's push 6 vars into vartable */
			_decl();   /*does the char */
			_decl();   /*does the int  */
			dumpVarTab();
			break;
/*	_decl(). Parses entire char int declarations. Six calls cover all cases.
	Should get...
			Var Table: name class obsize len (type)stuff
				 var 0: a 0 Char 1  ref to pr[101]
				 var 1: b 1 Char 10  ref to pr[102]
				 var 2: c 0 Char 1  ref to pr[112]
				 var 3: d 0 Int 1  ref to pr[113]
				 var 4: e 1 Int 10  ref to pr[117]
				 var 5: f 0 Int 1  ref to pr[157]
*/
		case 9:
			err = iProperty("testFiles/test.prop","alpha",&isInt,99);
			printf("\nalpha is %d",isInt);
			err = iProperty("testFiles/test.prop","beta",&isInt,99);
			printf("\nbeta is %d",isInt);
			err = iProperty("testFiles/test.prop","gamma",&isInt,99);
			printf("\ngamma is %d",isInt);
			err = iProperty("testFiles/test.prop","kappa",&isInt,99);
			printf("\nkappa is %d",isInt);fflush(stdout);

			err = sProperty("testFiles/test.prop","delta",&buff,80,"default");
			printf("\ndelta is ->%s<-",buff);fflush(stdout);
			err = sProperty("testFiles/test.prop","epsilon",&buff,80,"default");
			printf("\nepsilong is ->%s<-",buff);fflush(stdout);
			err = sProperty("testFiles/test.prop","zeta",&buff,80,"default");
			printf("\nzeta is ->%s<-",buff);fflush(stdout);
			err = sProperty("testFiles/test.prop","theta",&buff,80,"default");
			printf("\ntheta is ->%s<-",buff);fflush(stdout);
			err = sProperty("testFiles/test.prop","eta",&buff,80,"default");
			printf("\neta is ->%s<-",buff);fflush(stdout);
			break;
/*	property file. Should get...
				alpha is 10
				beta is -20
				gamma is 30
				epsilon is 99
 */
		case 10:
			pl(" empty case");
			break;
/*	Should get...
 */

/* stack tests */
		case 11: 
			/*pushone();*/
			pushk( 7 );
			dumpStack();
			int val = toptoi();
			printf("\ntest 11, stack, popped val is %d", val);
			break;
/* 
	toptoi(). Should get...
				Stack (from top) class lvalue type stuff
				 stack entry at 0: 0 A 2 (i)->7<-
				test 11, stack, popped val is 7
*/
		case 12:
			nxtstack = 0;
			array1[0]=11; array1[1]=22; array1[2]=33; array1[3]=44;
			int *array2;
			/* four pushes, string, char, int, int[] */
			/* void pushst( char class, char lvalue, int size, void* stuff ) */
			printf("\nnxtstack %d", nxtstack);
			Stuff.up = pr+555;
			pushst( 1, 'A', 1, &Stuff );
			Stuff.uc = 'c';
			pushst( 0, 'A', 1, &Stuff );
			Stuff.ui = 77;
			pushst( 0, 'A', 2, &Stuff );
			Stuff.up = pr+111;
			pushst( 1, 'A', 2, &Stuff );
			/* five pops, values in global poptop, last should throw an error */
			printf("\nafter 4 pushes nxtstack %d", nxtstack);
			dumpStack();
			/* should be:
					nxtstack 0
					after 4 pushes nxtstack 4
					Stack (from top) class lvalue type stuff
					 stack entry at 3: 1 A 2 pr[111]
					 stack entry at 2: 0 A 2 77
					 stack entry at 1: 0 A 1 c
					 stack entry at 0: 1 A 1 pr[555]


			*/
			pl("begin 5 pops");
			popst();  /* int array */
			pl("\nfirst pop done:");
			dumpPopTop();
			int* array = stack[nxtstack].value.up;
			printf("\narray has %d %d %d %d ", 
				array[0], array[1], array[2], array[3] );

			popst();  /* int */
			dumpPopTop();
			val = stack[nxtstack].value.ui; 
			printf("\nval = %d",val);

			popst();  /* char */
			dumpPopTop();
			val = stack[nxtstack].value.uc; 
			printf("\nval = %c",val);

			popst();  /* char array */
			dumpPopTop();

			printf("\nbefore 5th pop err: %d",error);
			printf("\nnxtstack %d\n", nxtstack);
			popst();  /* one too many pops, on purpose */
			printf("\nafter 5th pop err: %d",error);
			dumpPopTop();
			dumpStack();
			break;
/* 
	push() pop(). 
	Should get...
				nxtstack 0
				after 4 pushes nxtstack 4
				Stack (from top) class lvalue type stuff
				 stack entry at 3: 1 A 2 pr[111] 608a4f 6089e0
				 stack entry at 2: 0 A 2 77
				 stack entry at 1: 0 A 1 c
				 stack entry at 0: 1 A 1 pr[555] 608c0b 6089e0

				begin 5 pops

				first pop done:
				 stack entry at 3: 1 A 2 pr[111] 608a4f 6089e0
				array has 0 0 0 0 
				 stack entry at 2: 0 A 2 77
				val = 77
				 stack entry at 1: 0 A 1 c
				val = c
				 stack entry at 0: 1 A 1 pr[555] 608c0b 6089e0
				before 5th pop err: 0
				nxtstack 0

				after 5th pop err: 15
				 stack entry at 0: 1 A 1 pr[555] 608c0b 6089e0
				Stack (from top) class lvalue type stuff

				Test 12 done, error=15  <<-- intentional
*/
		case 13: nxtvar=0;
			int err;
			strcpy(pr, "{}   {}   {{}{{}}}   {    STRING-END");
			epr = pr + strlen(pr) -1;
		/*prused = pr+100;*/
			cursor = pr+1; 
			printf("\nat start cursor,epr,error = %d %d %d",cursor-pr,epr-pr,error);
			printf("\ncursor->%s",cursor);
			pl("4 skips: ");
			/* 4 calls to skip, 
			the last should fail with error,cursor= 2 D */
			_lit("{");err=_skip('{','}'); printf("\nerr,cursor = %d %s", err, cursor);
			_lit("{");err=_skip('{','}'); printf("\nerr,cursor = %d %s", err, cursor);
			_lit("{");err=_skip('{','}'); printf("\nerr,cursor = %d %s", err, cursor);
			_lit("{");err=_skip('{','}'); printf("\nerr,cursor = %d %s", err, cursor);
			break;
/* NOTE(3/28/2017) _skip no longer sets an error, but returns the 
	count of unmatched ['s. Calling routine decides the treatment.
	_skip() over nested braces.
	Should get...
				at start cursor,epr,error = 1 35 0
				cursor->}   {}   {{}{{}}}   {    STRING-END
				4 skips: 
				err,cursor = 0    {}   {{}{{}}}   {    STRING-END
				err,cursor = 0    {{}{{}}}   {    STRING-END
				err,cursor = 0    {    STRING-END
				err,cursor = 1 D 
				Test 13 done, error=0    <<-- intentional
*/
		case 14: 
			strcpy(pr, "   \n\n  /* a tiny-c style comment   \nnext-line  string-end");
			epr = pr + strlen(pr) -1;
			cursor = pr;
			printf("\nBEFORE _rem()");
			printf("\ncursor %d-->%s", cursor-pr, cursor );
			_rem();
			printf("\nAFTER _rem()");
			printf("\ncursor %d-->%s", cursor-pr, cursor );
			break;
/* 
	_rem(). Should get...
				BEFORE _rem()
				cursor 0-->   

				  /* a tiny-c style comment   
				next-line  string-end
				AFTER _rem()
				cursor 36-->next-line  string-end
*/
		case 15:
			strcpy(pr,"   17  \"hello\"  \'c\'  STRING-END");
			cursor = pr;
			epr = pr + strlen(pr) -1;
			printf("\n1 cursor->%s<-",cursor);
			printf("\nkonst returns %d ",_konst()); dumpName();
			printf("\n2 cursor->%s<-",cursor);
			printf("\nkonst returns %d ",_konst()); dumpName();
			printf("\n3 cursor->%s<-",cursor);
			printf("\nkonst returns %d ",_konst()); dumpName();
			printf("\n4 cursor->%s<-",cursor);
			printf("\nkonst returns %d ",_konst()); /*name unchanged*/
			printf("\n5 cursor->%s<-",cursor);  /*cursor to STRING-END */
			break;
/* 
	_konst() (8080 name: const()). 4 cases.
	Should get...
				1 cursor->   17  "hello"  'c'  STRING-END<-
				konst returns 1 17
				2 cursor->  "hello"  'c'  STRING-END<-
				konst returns 2 hello
				3 cursor->  'c'  STRING-END<-
				konst returns 3 c
				4 cursor->  STRING-END<-
				konst returns 0 
				5 cursor->STRING-END<-

*/
		case 16:
			testSetup("   17  \"hello\"  \'c\'  STRING-END");
			printf("\npr->%s", pr);
			pl("3 _factor()'s");
			_factor(); 
			_factor(); 
			_factor(); 
			dumpStack();
			pl("3 popst()'s");
			popst();  /* char */
			printf("\n char %c", stack[nxtstack].value );
			popst();
			printf("\n string %s", stack[nxtstack].value );
			popst();
			printf("\n int %d", stack[nxtstack].value );
			break;
/* 
	_factor(). Three cases. Should get...
				pr->   17  "hello"  'c'  STRING-END
				3 _factor()'s
				Stack (from top) class lvalue type stuff
				 stack entry at 2: 0 A 1 
				 stack entry at 1: 1 A 1 
				 stack entry at 0: 0 A 2 

				3 popst()'s
				 char c
				 string hello
				 int 17
*/
		case 17:
			strcpy(pr,"foobaaarwxyz"); 
			fname = &pr[0];	lname = &pr[strlen(pr)-1];
			nxtvar = 0;
			newvar(11,1,13,0);
			pl("name in pr ->");ps(pr);ps("<-");

			strcpy(pr,"foob5678"); 
			fname = &pr[0];	lname = &pr[strlen(pr)-1];
			newvar(21,2,23,0);
			pl("name in pr ->");ps(pr);ps("<-");

			strcpy(pr,"foob"); 
			fname = &pr[0];	lname = &pr[strlen(pr)-1];
			newvar(31,2,33,0);
			pl("name in pr ->");ps(pr);ps("<-");

			strcpy(pr,"foo(baaarwxyz"); 
			fname = &pr[0];	lname = &pr[2];
			newvar(41,1,43,0);
			pl("name in pr ->");ps(pr);ps("<-");
			
			dumpVarTab();
			/*                   11111111112222222
			           012345678901234567890123456                */
			strcpy(pr,"foo foob5678 foobaaarwxyz ");
			fun[0].fvar = 0;
			fun[0].lvar = 3;
			curglbl = fun;
			curfun = fun;

			pl("look up foo");
			fname = &pr[0];	lname = &pr[2];
			struct val* v=addrval();
			if(v) dumpVar(v);
			else pl("addrval returns 0");

			pl("look up foob5678");
			fname = &pr[4];	lname = &pr[11];
			v=addrval();
			if(v) dumpVar(v);
			else pl("addrval returns 0");
			
			pl("look up foobaaaz");
			fname = &pr[13];	lname = &pr[24];
			v=addrval();
			if(v) dumpVar(v);
			else pl("addrval returns 0");
			

			break;
/* 
	newvar(), addrval(): Same cases as canon (test 1). 
	Should get...
				name in pr ->foobaaarwx<-
				name in pr ->foob5678<-
				name in pr ->foob<-
				name in pr ->foo(baaarwxyz<-
				Var Table: name class type len (type)value
				 var 0: foobaaaz 11 Char 13 pr[10]->xyz<-
				 var 1: foob5678 21 Int 23 pr[23]-><-
				 var 2: foob 31 Int 33 pr[115]-><-
				 var 3: foo 41 Char 43 pr[247]-><-
				look up foo
				 var 3: foo 41 Char 43 pr[247]-><-
				look up foob5678
				 var 1: foob5678 21 Int 23 pr[23]->yz <-
				look up foobaaaz
				 var 0: foobaaaz 11 Char 13 pr[10]->78 foobaaarwxyz <-
				Test 17 done, error=0
		NOTE: ref to indexes may vary, data is garbage, 
		lookup names is the test. 
*/
		case 18:
			testSetup("1+2");
			int x = _asgn();
			printf("\nasgn returned %d, %s is %d", x, pr, toptoi());
			testSetup("1+2+3");
			x = _asgn();
			printf("\nasgn returned %d, %s is %d", x, pr, toptoi());
			testSetup("1*2");
			x = _asgn();
			printf("\nasgn returned %d, %s is %d", x, pr, toptoi());
			testSetup("(1+2)*3");
			x = _asgn();
			printf("\nasgn returned %d, %s is %d", x, pr, toptoi());
			break;
/* 
	Simple expressions, Should get...
				asgn returned 1, 1+2 is 3
				asgn returned 1, 1+2+3 is 6
				asgn returned 1, 1*2 is 2
				asgn returned 1, (1+2)*3 is 9
*/
		case 19:
			for(i=0; i<rel_cases; ++i) {
				testSetup( rel_case[i] );
				_asgn(); 
				printf("\n%s is %d ", pr, toptoi());
			}
			break;
/* 
	Relations, Should get...
					1<2 is 1 
					1>2 is 0 
					1==2 is 0 
					1!=2 is 1 
					1>=2 is 0 
					1<=2 is 1 
					2<2 is 0 
					2>2 is 0 
					2==2 is 1 
					2!=2 is 0 
					2>=2 is 1 
					2<=2 is 1
					2<1 is 0 
					2>1 is 1 
					2==1 is 0 
					2!=1 is 1 
					2>=1 is 1 
					2<=1 is 0 
*/
	
		case 20:
			for(i=0; i<ops_cases; ++i) {
				testSetup( ops_case[i] );
				_asgn(); 
				printf("\n%s is %d ", pr, toptoi());
			}
			break;
/* 
	Math ops, Should get...
				7+2 is 9 
				7-2 is 5 
				7*2 is 14 
				7/2 is 3 
				7%2 is 1 
*/

		case 21:
			testSetup("char a; a='A'; ");
			printf("\npr: %s",pr);
			st();
			dumpVarTab();
			st();
			dumpVarTab();
			break;
/* 
	Should get...
				Var Table: name class type len (type)value
				 var: a 0 Char 1  ref to pr[23]
				Var Table: name class type len (type)value
				 var: a 0 Char 1  ref to pr[23]
 */

		case 22:
		/*             012345678901234567890123456789   */
			testSetup(" char a; int x; char p(7); xXXXXXXXXXXXXXXXXXXXXx");
			printf("\npr: %s",pr);
			epr=pr+27;  	/* points to the ; */
			prused=epr+2;  /* middle of the X's */
			len = strlen(pr); /* capture this now, before 0's go in there */

			printf("\ndecl char a");
			_decl();
			printf("\nprused,obsize %d %d \n",prused-pr,obsize);
			for( i=15; i<len; ++i) printf("%x ",pr[i]);

			printf("\ndecl int x");
			_decl();
			printf("\nprused,obsize %d %d \n",prused-pr,obsize);
			for( i=15; i<len; ++i) printf("%x ",pr[i]);

			printf("\ndecl char p(7)");
			_decl();
			printf("\nprused,obsize %d %d \n",prused-pr,obsize);
			for( i=15; i<len; ++i) printf("%x ",pr[i]);

			printf("\nprused,obsize %d %d ",prused-pr,obsize);
			break;
/* 	_varAlloc(), newvar(), does newvar zero the allocated space properly.
	And does put|get_int|char use allocated space properly.
	Should get... 
		prused,obsize 6316529 1 78 58 0 58 58 58 58 58 58 58 58 58 58 78 
		prused,obsize 6316533 4 78 58 0 0 0 0 0 58 58 58 58 58 58 78 
		prused,obsize 6316533 4       ^---LOOK CLOSELY, 1 then 5 zero's,
					then 13 zero's in third line.
*/
		case 23:
		/*             012345678901234567890123456789   */
			testSetup(" int x; x=77;  xXXXXXXXXXXXXXXXXXXXXx");
			printf("\npr: %s",pr);
			epr=pr+12;  	/* points to the ; */
			prused=epr+5;  /* middle of the X's */
			len = strlen(pr); /* capture this now, before 0's go in there */

			printf("\nparsing int x;\n");
			st();   /* int x; */
			for( i=15; i<len; ++i) printf("%x ",pr[i]);
			dumpVarTab();

			printf("\nparsing x=77\n");
			st();
			for( i=15; i<len; ++i) printf("%x ",pr[i]);
			dumpStack();
			break;
/* 	Should get...
			parsing int x;
			78 58 58 0 0 0 0 58 58 58 58 58 ... 58 78
			Var Table: name class type len (type)value
			 var: x 0 Int 1  has value or ref
			parsing x=77
			78 58 58 4d 0 0 0 58 58 58 58 58 ... 58 78
			Stack (from top) class lvalue type stuff
NOTE:			<blank line, empty stack>
*/

		case 24:
		/*             0123456789012345678901234567890123456789   */
			testSetup(" int x(2); x(1)=77; xXX...XXXXXXXXXXXX...XXXXx");
			printf("\npr: %s",pr);
			epr=pr+20;	 	/* points to the ; +1 */
			prused=epr+5;	/* middle X's */
			len = strlen(pr); /* capture this now, before 0's go in there */

			printf("\nparsing int x(3);\n");
			st();   /* int x(2); */
		/* int x(2) allocates 3 ints, uses 12 bytes of pr, 
		inside the ...'s should be 12 zeroes */
			for( i=20; i<len; ++i) printf("%x ",pr[i]);
			dumpVarTab();
		 	printf("\nparsing x(1)=77;\n");
			st();
			for( i=20; i<len; ++i) printf("%x ",pr[i]);
			break;
/*	Should get...
				parsing int x(3);
				78 58 58 2e 2e 2e 0 0 0 0 0 0 0 0 0 0 0 0 2e 2e 2e 58 58 ...
				Var Table: name class type len (type)value
				 var: x 1 Int 3  ref in pr 26
				parsing x(1)=77;
				78 58 58 2e 2e 2e 0 0 0 0 4d 0 0 0 0 0 0 0 2e 2e 2e 58 58...
NOTE: 12 zero's. Then 4d is right here----^^
*/

		case 25:
			testSetup("MC('M',1);");
			printf("\n->");
			st();
			printf("<- Back from MC('M',1);");
			dumpStack();
			break;
/*	Should get...
				->M<- Back from MC('M',1);
				Stack (from top) class lvalue type stuff

NOTE: Stack is empty (blank line) because st() pops (discards) one entry.
*/
		case 26:
			testSetup("if(1)MC('T',1);else MC('F',1);");
			printf("\n");
			st();
			/*         0123456789012345678901234567890 */
			testSetup("if(0)MC('T',1);else MC('F',1);");
			st();
			break;
/* 	Should get...
				   TF
*/
		case 27:
		case 28:
			pl(" empty case");
			break;
/*	Should get...
*/
		case 29:
			testSetup("MC('o',1);");
			printf("\n");
			st();
			break;
/* 	Should get...
				o
*/
		case 30:
			len=testSetupFile("./testFiles/30", 0);
			len = strlen(pr);
			printf("\nTest 30 (%d):\n%s",len,pr);
			printf("\napplying st()...\n");
			st();
			break;
/* 	Should get...
				Test 30 (99):
				[]/* test 30
				[
				MC...  <NOTE: 3 lines of MC's
				]
				applying st()...
				hello tc
*/
		case 31:
			len=testSetupFile("./testFiles/31", 0);
			len = strlen(pr);
			printf("\nTest 31 (%d):\n%s",len,pr);
			printf("\napplying st()...\n");
			st(); fflush(stdout);
			dumpVarTab();
			break;
/* 	Should get...
				<dump of tc code>

				applying st()... 
				321F0 
				Var Table: name class type len (type)value
 				var 0: i 0 Int 1  ref to pr[136]
*/
		case 32:
		/*             01234567890123456789012345678901234567890
						 | |        cursor---------|        |
					     v v----fname,lname        v        v--epr=pr+37  */
			testSetup("[ foo int bar[ MC(bar,14)];  foo(77); ]   4.........5.........");

			cursor = pr + 6;
			fname =  pr + 2;
			lname =  pr + 4;
			union stuff cur; 
			cur.up = cursor;
			newvar('E', 2, 1, &cur);
			dumpVarTab();  /* that's all link does */
			cursor=pr+28;  /* ready for call */
			printf("\nparsing foo(77);\n");
			st();      /* or st(); */
			break;
/* should get...
	var tab with foo 'E' (69) etc.
	parsing foo(77);
	77
*/	
		case 33:
			len=testSetupFile("./testFiles/33", 0);
			printf("\nTest 33 (%d):\n%s",len,pr);
			printf("\napplying st()...\n");
			st();
			dumpVarTab();
			break;
/* 	Should get... 
				dump of test code
				applying st()...
				Var Table: name class type len (type)value
				 var 0: a 0 Int 1 pr[57]->0<-
				 var 1: b 1 Int 1 pr[61]-><-
				 var 2: c 0 Int 1 pr[65]->0<-
				 var 3: d 0 Char 1 pr[69]-><-
				 var 4: e 1 Char 1 pr[70]-><-
				 var 5: f 0 Char 1 pr[71]-><-
 */
		case 34:
/* test 34, link, must set lpr=2, apr-->pr+366, epr=nread */
			len=testSetupFile("./testFiles/34", 0);
			printf("\nTest 34 (%d):\n%s",len,pr);
			printf("\napplying link()...\n");
			lpr=pr+2;
			apr=pr+366;
			tclink();
			dumpFun();
			dumpVarTab();
			break;
/* 	Should get... 
				<dump of code, 35 lines with endlibrary>
				fun table: fvar, lvar, prused
				 fun entry at 0:  0 3 394
				 fun entry at 1:  4 11 426
 				Var Table: name class type len (type)value
				 var 0: putchar 69 Int 1  ref to pr[51]
				 var 1: pft 69 Int 1  ref to pr[136]
				 var 2: ps 69 Int 1  ref to pr[201]
				 var 3: scann 69 Int 1  ref to pr[275]
				 var 4: a 0 Int 1  ref to pr[396]
				 var 5: b 1 Int 1  ref to pr[400]
				 var 6: c 0 Int 1  ref to pr[404]
				 var 7: d 0 Char 1  ref to pr[408]
				 var 8: e 1 Char 1  ref to pr[409]
				 var 9: f 0 Char 1  ref to pr[410]
				 var 10: main 69 Int 1  ref to pr[369]
				 var 11: foobar 69 Int 1  ref to pr[381]
*/
		case 35:
			testSetup("");
			printf("\n");
			pushk('t');
			pushk(1);		/* MC number */
			machinecall(2);	/* number of args */
			printf("<--MC 1 echoed, and returns %c",toptoi());
			break;
/* 	Should get... 
				t<--MC 1 echoed, and returns t
 */
 		case 36:
			printf("\ncharIn a abc is %d",charIn('a',"abc"));
			printf("\ncharIn b abc is %d",charIn('b',"abc"));
			printf("\ncharIn c abc is %d",charIn('c',"abc"));
			printf("\ncharIn x abc is %d",charIn('x',"abc"));
			break;
/* 	Should get... 
 *				charIn a abc is 1
 *				charIn b abc is 2
 *				charIn c abc is 3
 *				charIn x abc is 0
 */
		
		case 37:
			testSetup("");
			ibuff[0]=77;
			pFmt("\nSeventy seven %d", ibuff);
			break;
/* 	Should get... 
 *			Seventy seven 77
 */
		case 38:
			testSetup("[ MC(\"%d %s %d\",11,\"abc\",33,101) ]  ");
			printf("\npr: -->%s<--\n calling st();\n",pr);
			st();
			break;
/* 	Should get... 
 *				pr: -->[ MC("%d %s %d",11,"abc",33,101) ]  <--
 *				 calling st();
 *				11 abc 33
 */
 		case 39:
 			printf("\nempty case");
 			break;
/******** Library tests, 40 up, 43 is the only one   *********/
		case 40:
		case 41:
		case 42:
			pl(" empty case");
			break;
		case 43:
/* test 43 -- compound statment WITH lib calls 
	 link, must set lpr=pr+2, apr-->line 28, epr=nread */	
	 		testSetupFile("./testFiles/43",1);
			tclink();
			curglbl = fun;  /* */
			st();
			break;
/* 	Should get... 
 *				Hello, test  43
 *				 testing pl,ps,pn,alpha,num,atoi
 *				 should be 1 1 0 -> 1 1 0
 *				num 77 has  2 digits
 *				  and has value 77, and (atoi call) -77 value -77
 */
		case 44:
			testWhole("./testFiles/44");
			break;
/* 	Should get... 
				 testing ceqn, index, move
				cat in hat
 */
		case 45:
			testWhole("./testFiles/45");
			break;
/* 	Should get...
 
				whatHappened, INTENTIONAL ERROR...
				line 6 SYMERRA, decl needed 
						  Kcount = 30;   /* <<== tab-tab-space-space K...
 */
		case 46:
			testWhole("./testFiles/46");
			break;
/* 	Should get...
		fopen returned 0
		fputs returned 6
		fputc returned 1
		fputs returned 6
		fputc returned 1
		fputs returned 15
		fputc returned 1
		fclose returned 0
		READ TEST
		fopen returned 1
		fgets returned 7
		line 1

		fopen returned 1
		fgets returned 7
		line 2

		fopen returned 1
		fgets returned 16
		line 3 and last

		the blank lines are the newlines fputc put in the file
 */
		case 47:
		case 48:
		case 49:
			pl(" empty case");
			break;
/******** Whole tc (prog with main) tests, 50 up *********/
		case 50:
			testWhole("./testFiles/50");
			break;
/* 	Should get... 
 *			<blank line from putchar 10>
 *			Hello, Application main
 */
		case 51:
			testWhole("./testFiles/51");
			break;
/* 	Should get... 
 *			hello tc
 *			hello ps
 *			hello pl. Seventy seven -->  77
 */
		case 52:
			testWhole("./testFiles/52");
			break;
/* 	Should get... 
 *			<blank line>
 *			23456
 */
		case 53:
			testWhole("./testFiles/53");
			break;
/* 	Should get... 
 *			<blank line>
 *    			b(3) is 123, b(1) is 123 
 */
		case 54:
			testWhole("./testFiles/54");
			break;
/* 	Should get... 
			ceqn cat cat, then cat dog
			 1 0 should be 1 0
			foo-bar-and-hello-to-you
			foo-bar-and-hello-to-me
			ceqn 20:  1 should be 1
			ceqn 23:  0 should be 0
			scann h:  12 0 should be 12 0
			scann g:  11 0 should be 11 0
 */
		case 55:
			testWhole("./testFiles/55");
			break;
/* 	Should get... 
 *				line 3 SYMERRA, decl needed 
 *					error = on + purpose
 *					     ^
 */
		case 56:
			testWhole("./testFiles/56");
			break;
/* 	Should get... 
 *				test 56: index
 *				in ->the cat in the hat<- find cat:    5 should be 5
 *				in ->the cat in the hat<- find dog:    0 should be 0
 */
		case 57:
			testWhole("./testFiles/57");
			break;
/* 	Should get... 
 *				cat in the hat .......
 *				t57: 143  17
 *				cat in the hat ..cat..
 */
		case 58:
			testWhole("./testFiles/58");
			break;
/* 	Should get... 
 *				health(2) should be 2, is 2
 */
		case 59:
			testWhole("./testFiles/59");
			break;
/* 	Should get... 
 *				numread 490
 *				First 100 chars...
 *				/* test of lib function readfile, reads and prints itself
 *				main[
 *					char buff(1000)
 *					char name(0)
 *					int n<<--
 *
 *				numread 27
 *				 should be 27 including linefeed
 *				abcdefghijklmnopqrstuvwxyz
 *				<<--
 */
		case 60:
			testWhole("./testFiles/60");
			break;
/* 	Should get... 
 *				abcdefg 7
 */
		case 61:
			testWhole("./testFiles/61");
			break;
/* 	Should get... 
 *				try i = p; good, now p = i; almost there; 
 *				foobar; is that foobar? GOOD
 */
		case 62:
			testWhole("./testFiles/62");
			break;
/* 	Should get... 
 *				before: The cat in the the hat? or the cathouse?
 *				after: The cat in the the hat? or the cathouse?
 */

		case 63:
			testWhole("./testFiles/63");
			break;
		case 64:
			testWhole("./testFiles/64");
			break;
		case 65:
			testWhole("./testFiles/65");
			break;
/* 	Should get for each: 63,64,65... 
 *				double.tg - lrb - 2/5/18
 *				<type of eol's>
 *				hello
 *				hello 1
 *				world
 *	Test test linux, dos, and macintosh eol's
 */
		case 66:
			testWhole("./testFiles/66");
			break;
/* 	Should get... 
 *				writing to test64tempfile: This is test 64, fwrite and fread
 *				reading test64tempfile: 
 *				amount read:  33
 *				content read: This is test 64, fwrite and fread
 */
		case 67:
			testWhole("./testFiles/67");
			break;
/* 	Should get... 
 *			calling foo 
 *				back from foo
 *				foo returns bar
 */
		case 68:
			testWhole("./testFiles/68");
			break;
/* 	Should get... 
 *			calling foo 
 *				back from foo
 *				foo returns bar
 */
		case 69:
			testWhole("./testFiles/69");
			break;
/* 	Should get... 
 *	lib line 249 (cursor pr[4499])EXIT, stopped by exit call
 *	] 
 *	^
 *	Test 69 done, error=98
 */
		case 70:
			testWhole("./testFiles/70");
			break;
/* 	Should get... 
 *			YES!!   IT WORKS  x=77 y=88
 *			Now lets do a newline
 *			Now lets be sure varargs is false. To do that force an args error.
 *			good call...
 *			Arg to foo: BAR, (what did you expect?)
 *			bad call INTENTIONAL ARGSERR ...
 *			app line 19 (cursor pr[5060])ARGSERR, args don't match
 */

/***************  keyboard input tests, run manually *********/
/*	Test 90 MUST be run using ./test 90. It is NOT a tc program.
 *	Tests 91 up may be run using ./test 9x, or ./tc testFiles/9x. They
 *	are tc programs. These tests if put into the regression (with numbers in
 *	the regression range) would not show their prompts. So they are
 *	numbered in their own range, and run manually. Check them by examining
 *	the "Should get..." comments.
 */


		case 90:
			testSetup("");
			pushk(2);
			printf("\n  MC 2 test, enter one key then <ret> :");
			machinecall(1);
			printf(" entered hex %x",toptoi());

			break;
/* 	Should get... 
 *				  MC 2 test, enter one key then <ret> :A
 *				 entered hex 41
 */
		case 91:
			testSetup("");
			printf("\nkbhit and getchar, ctrl-C to stop\n");
			while(1) {
				while(!kbhit()) {
					sleep(1);
					printf("Press a key... ");
				}
				printf("You pressed '%c'!\n", getchar());
			}
			break;
/* 	Should get (typical)... 
 *				kbhit and getchar, ctrl-C to stop
 *				Press a key... Press a key... ^[[APress a key... You pressed ''!
 *				You pressed '['!
 *				You pressed 'A'!
 *				Press a key... Press a key... pPress a key... You pressed 'p'!
 *				Press a key... qPress a key... You pressed 'q'!
 *				Press a key... Press a key... ^C
 */
		case 92:
			testWhole("./testFiles/92");
			break;
/* 	Clone of 91 using lib functions. Should get (typical)... 
 *				Press a key... 
 *				Press a key... ^A
 *				Press a key... You pressed 
 *				Press a key... 
 *				Press a key... ^[[A
 *				Press a key... You pressed You pressed [You pressed A
 *				Press a key... 
 *				Press a key... p
 *				Press a key... You pressed p
 *				Press a key... 
 *				Press a key... q
 *				Press a key... You pressed q^C
 */
		case 93:
			testWhole("./testFiles/93");
			break;
/* 	Should get (typical)... 
				getchar test; enter one character, then <enter> :abcdef
				getchar: a
				gc test; enter one character, then <enter> :
				gc: b
				gs test; enter a string, then <enter> :-->cdef<--
				gn test; enter a number, then <enter> :77
				gn:  77
 */
		case 94:
			testWhole("./testFiles/94");
			break;
/* 			RUN USING ./tc testFiles/94 -d
			set break on symbol 'brk', then r, then p each...
			   iArray, cArray, cString, iDatum, cDatum
	Should get... 
			break at line 24 cursor pr[4369]: brk
				brk=0
			(tc-db) p cArray
			cArray:  ABCDEFGHIJ
			(tc-db) p cString
			cString: 
			(tc-db) p iDatum
			iDatum:  5
			(tc-db) p cDatum
			cDatum:  (45)E
			(tc-db) x
 */
		case 95:
			testWhole("./testFiles/95");
			break;
/* 	ESC test. Run under either tc or test. Hit ESC
	Should get... 
		lots of dots, then KILL, stopped by user
 */
		case 99:
			testWhole("./testFiles/99");
			break;
/* 	Should get... 
 *			no such file: ./testFiles/99
 */

		default:
			ps("invalid test case "); pn(testcase); pl(""); exit(1);
			break;
	

	}  /* end of switch */
	printf("\nTest %d done, error=%d",testcase, error);
/*	if(error){
		printf("\n  errat=%d\n", (int)errat+pr );
		pft(errat-40,errat-1);
		printf("\n***cursor-->");pft(errat,errat+20);
	}
*/
}
