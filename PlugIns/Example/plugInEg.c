#include <stdlib.h>
#include <stdio.h>

#define MCERR        24
// prototypes for callback functions
void (*callEset)(int) = NULL;

// plug-in example function. Argument is char*.
int foo(int nargs, int *args) {
	char* str = (char*)args[0];
	printf("foo called with \"%s\", length is %d, ",str,strlen(str));
	return strlen(str);
}

// plug-in example function. Argument is int d.
int bar(int nargs, int *args) {
	int d = args[0];
	int x = 77;
	printf("bar called with %d, 77/%d is %d",d,d,x/d);
	return x/d;
}

/* test main, commented out before making the plug-in
int main(int argc, char **argv) {
	printf("foo returned %d\n", foo("hello plug-in") );
	printf("bar returned %d\n", bar(11));
	printf("should get (just the numbers) 12, 12, 11, 7/11, 7, 7\n");
}
*/

/*	INFRASTRUCTURE...
 *	Comment out your test main and uncomment this infrastructure.
 *	first in this list is MC 1001. Change the names to functions 
 *	written above. Enlarge the list for larger libraries.
 */
typedef int (*McList)(int,int*);

McList plugInList[] =
	{ &foo, &bar
	};

/*	code the MC above and register in McList array. Placement in the array
 *	determines the MC number starting with 1001.
 */
void plugInMC(int mcno, int nargs, int *args) {
	if(mcno<1 || mcno>(sizeof(plugInList)/sizeof(void*))) {
		callEset(MCERR);
	}
	else {
	    int x = plugInList[mcno-1](nargs, args);
	}
}
// register callbacks to specific tc functions...
//void register_function( (void(*eset)(int)) );
void register_eset( eset )
{
    callEset = eset;
}
