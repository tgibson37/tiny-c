#include "tc.h"

#define testcases 53

char timeStamp[40];
extern struct stackentry poptop;
/* setup tools used by some of the tests */
void testWhole(char* filename){
	char *argv[3];
	argv[1]=filename;
/* modified clone of tcMain.c code...
 */
	strcpy(pr,"[_MAIN();]");  /* required sys main */
	epr = prused = pr+10;
	cursor = pr;
	curglbl = fun;
	readTheFiles(2,argv);  /* sets epr, curglbl */
	error=0;
	prused = epr+10;  /* a little slack */
	nxtvar = 0;
	nxtstack = 0;
	efun = fun+FUNLEN;
	curfun = fun-1;   /* none */
	link();
	cursor=pr;
	st();   /* <<<== executes statement above, line 8 */
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
	cursor = pr;
	curglbl = fun;
	if(lib) {
		len = FileRead("pps/library.tc",pr,PRLEN);
		cursor += len; 	/*  cursor->app */
		curglbl = fun+1;
	}
	len += FileRead(filename,pr+len,PRLEN-len);
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
	Called by main. Args evaluated here. No args runs all tests. 
	List of test numbers runs just those tests.
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
			doTest(testcase);
		}
		printf("\n");
	} 
	else {
/* do all tests */	
		printf("\nDONT FORGET: test 35 requires keyboard input A<cr>");
		ps("\ninitial state before any tests\n");
		nxtstack=nxtvar=0;
		cursor=prused=epr=pr;
		dumpState();
		pl("\nBEGIN TESTING ALL");
		for(testcase=1; testcase<=testcases; ++testcase) {
			printf("\n\nTEST %d %s", testcase, timeStamp);
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
	Result: 4 copies of  "var: foobaaaz 0 2 1 (i)1" for each test.
	The data is fake. Check the canonicalized name is proper. 
	Should get: foobaaaz
*/
		case 2: strcpy(pr,"foob5678"); 
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
/*	Should get: foob5678  */

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
			symname();
			pl("");dumpName();
			printf("\nfname,lname,cursor= %d %d %d",fname-pr,lname-pr,cursor-pr);
			break;
/* symname() defines fname, lname, and advances cursor. 
	Should get...
			symbol
			fname,lname,cursor= 5 10 11
*/
		case 6: nxtvar=0;
			/*         01234567890123456789 V 345       */
			strcpy(pr,"char symbol, 34567890123456789012345"); 
			prused = pr+20;
			cursor = pr+5;
        	vAlloc(Char,0);
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
			/* calling lit(xchar) twice */
			pl("cursor->");ps(cursor);
			/* first, match true, cursor -> space after char */
			int match = lit("char");  /* lit should match */
			printf("\nmatch,cursor= %d %d",match,cursor-pr);
			/* again, no match this time */
			pl("cursor->");ps(cursor);
			match = lit("char");
			printf("\nmatch,cursor= %d %d",match,cursor-pr);
			/* cursor -> s of symbol */
			pl("cursor->");ps(cursor);
			break;
/* lit(). Must match char. Two calls. First succeeds, 
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
			decl();   /*does the char */
			decl();   /*does the int  */
			dumpVarTab();
			break;
/*	decl(). Parses entire char int declarations. Six calls cover all cases.
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
		case 10:
			pl(" empty case");
			break;

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
			strcpy(pr, "{}   {}   {{}{{}}}   {    STRING-END");
			epr = pr + strlen(pr) -1;
		/*prused = pr+100;*/
			cursor = pr+1; 
			printf("\nat start cursor,epr,error = %d %d %d",cursor-pr,epr-pr,error);
			printf("\ncursor->%s",cursor);
			pl("4 skips: ");
			/* 4 calls to skip, 
			the last should fail with error,cursor= 2 D */
			lit("{");skip('{','}'); printf("\nerror,cursor = %d %s", error, cursor);
			lit("{");skip('{','}'); printf("\nerror,cursor = %d %s", error, cursor);
			lit("{");skip('{','}'); printf("\nerror,cursor = %d %s", error, cursor);
			lit("{");skip('{','}'); printf("\nerror,cursor = %d %s", error, cursor);
			break;
/* 
	skip() over nested braces.
	Should get...
				at start cursor,epr,error = 1 35 0
				cursor->}   {}   {{}{{}}}   {    STRING-END
				4 skips: 
				error,cursor = 0    {}   {{}{{}}}   {    STRING-END
				error,cursor = 0    {{}{{}}}   {    STRING-END
				error,cursor = 0    {    STRING-END
				error,cursor = 2 
				Test 13 done, error=2    <<-- intentional
*/
		case 14: 
			strcpy(pr, "   \n\n  /* a tiny-c style comment   \nnext-line  string-end");
			epr = pr + strlen(pr) -1;
			cursor = pr;
			printf("\nBEFORE rem()");
			printf("\ncursor %d-->%s", cursor-pr, cursor );
			rem();
			printf("\nAFTER rem()");
			printf("\ncursor %d-->%s", cursor-pr, cursor );
			break;
/* 
	rem(). Should get...
				BEFORE rem()
				cursor 0-->   

				  /* a tiny-c style comment   
				next-line  string-end
				AFTER rem()
				cursor 36-->next-line  string-end
*/
		case 15:
			strcpy(pr,"   17  \"hello\"  \'c\'  STRING-END");
			cursor = pr;
			epr = pr + strlen(pr) -1;
			printf("\n1 cursor->%s<-",cursor);
			printf("\nkonst returns %d ",konst()); dumpName();
			printf("\n2 cursor->%s<-",cursor);
			printf("\nkonst returns %d ",konst()); dumpName();
			printf("\n3 cursor->%s<-",cursor);
			printf("\nkonst returns %d ",konst()); dumpName();
			printf("\n4 cursor->%s<-",cursor);
			printf("\nkonst returns %d ",konst()); /*name unchanged*/
			printf("\n5 cursor->%s<-",cursor);  /*cursor to STRING-END */
			break;
/* 
	konst() (8080 name: const()). 4 cases.
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
			pl("3 factor()'s");
			factor(); 
			factor(); 
			factor(); 
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
	factor(). Three cases. Should get...
				pr->   17  "hello"  'c'  STRING-END
				3 factor()'s
				Stack (from top) class lvalue type stuff
				 stack entry at 2: 0 A 1 (c)->c<-
				 stack entry at 1: 1 A 3 (>)->8<-
				 stack entry at 0: 0 A 2 (i)->17<-

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
			int x = asgn();
			printf("\nasgn returned %d, %s is %d", x, pr, toptoi());
			testSetup("1+2+3");
			x = asgn();
			printf("\nasgn returned %d, %s is %d", x, pr, toptoi());
			testSetup("1*2");
			x = asgn();
			printf("\nasgn returned %d, %s is %d", x, pr, toptoi());
			testSetup("(1+2)*3");
			x = asgn();
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
				asgn(); 
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
				asgn(); 
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
			decl();
			printf("\nprused,obsize %d %d \n",prused-pr,obsize);
			for( i=15; i<len; ++i) printf("%x ",pr[i]);

			printf("\ndecl int x");
			decl();
			printf("\nprused,obsize %d %d \n",prused-pr,obsize);
			for( i=15; i<len; ++i) printf("%x ",pr[i]);

			printf("\ndecl char p(7)");
			decl();
			printf("\nprused,obsize %d %d \n",prused-pr,obsize);
			for( i=15; i<len; ++i) printf("%x ",pr[i]);

			printf("\nprused,obsize %d %d ",prused-pr,obsize);
			break;
/* 	vAlloc(), newvar(), does newvar zero the allocated space properly.
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
			epr=pr+15;  	/* points to the ; */
			prused=epr+2;  /* middle of the X's */
			len = strlen(pr); /* capture this now, before 0's go in there */

			printf("\nparsing int x;\n");
			st();   /* int x; */
			for( i=15; i<len; ++i) printf("%x ",pr[i]);
			dumpVarTab();

			printf("\nparsing x=77\n");
			st();
			for( i=15; i<len; ++i) printf("%x ",pr[i]);
			dumpStack();
			printf("\n x is %d", toptoi());

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
			 x is 0
NOTE:		      ^== because st() now pops the stack before returning.
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
				Test 30:
				/* test 30
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
			st();
			dumpVarTab();
			break;
/* 	Should get...
				<dump of tc code>

				applying st()... 
				321F0 
				Var Table: name class type len (type)value
 				var 0: i 0 Int 1  ref to pr[135]
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
			len=testSetupFile("./testFiles/34", 0);
			printf("\nTest 34 (%d):\n%s",len,pr);
			printf("\napplying link()...\n");
			link();
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
			printf("empty case");
			break;
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
			testSetupFile("./testFiles/43",1);
			link();
			curglbl = fun;  /* */
			st();
			break;
/* 	Should get... 
				Hello, test  43
				 testing pl,ps,pn,alpha,num,atoi
				 should be 1 1 0 -> 1 1 0
				num 77 has  2 digits
				  and has value 77, and (atoi call) -77 value -77
  */
		case 44:
		case 45:
		case 46:
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
 *					hello tc
 *					hello ps
 *					hello pl. Seventy seven -->  77
 */
		case 52:
			testWhole("./testFiles/52");
			break;
/* 	Should get... 
 *			<blank line>
 *				23456
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
 */
		case 55:
			testWhole("./testFiles/55");
			break;
/* 	Should get... 
 */
/***************  keyboard input tests, run manually *********/
		case 99:
			testSetup("");
			printf("\n");
			pushk('t');
			pushk(1);		/* MC number */
			machinecall(2);	/* number of args */
			printf("<--MC 1 printed, and returns %c",toptoi());

			pushk(202);
			pushk(2);
			printf("\n  getchar() test, enter one char<ret>");
			machinecall(2);
			printf(" returns %d",toptoi());

			break;
/* 	Should get... 
 *				t<--MC 1 printed, and returns t
 *				  getchar() test, enter one char<ret>A
 *				 returns 65
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
