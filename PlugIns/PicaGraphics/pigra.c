#include "../../tc.h"
#include "pigra.h"
#include <stdlib.h>
#include <stdio.h>
#define MCERR        24

/*x	STEP 1: Create a sub-directory for the plug in code.
 *	Copy this file into that directory with the same name 
 *	as your plugin and dot-c as extent. Edit the copy 
 *	following the STEPs 2 and up below.
 */

/*x	STEP 2: Code plug-in functions here: remove or change names
 *	of pi_X functions. Add more functions to form your complete 
 *	plug in library. Provide the Mxx functions with nargs,args
 *	MC calling sequence. These are your machine calls.
 */
char Space[80][40];  //actually 80x80, every pica has two cells.

/* sleep for N seconds */
int Msleep(int nargs, int* args) {
	sleep(args[0]);	
}

/* fill Space with spaces */
int Mblank(int nargs, int* args) {
	blank();
}
void blank() {
	int r,c;
	for(r=0; r<40; ++r) for(c=0; c<80; ++c) Space[c][r]=' ';
}

/* show the drawing */
int Mshow(int nargs, int* args) {
	show();
}
void show() {
	int r,c;
	for(r=0; r<40; ++r) {
		printf("\n");
		if(frame)printf("|");
		for(c=0; c<80; ++c) {
			printf("%c",Space[c][r]);
		}
		if(frame)printf("|");
	}
	printf("\n");
}

/* plot a small mark at raw position x,y */
int Mplot(int nargs, int* args) {
	int x,y;
	if(nargs<2){eset(MCERR); return -1; }
	x=args[0];
	y=args[1];
	plot(x,y);
}
void plot(int x, int y){
#if 0
	if(debug>1){
		if(lastLabel!=label){
			lastLabel=label;
			fprintf(stderr,"\n    plot: new Object %c",label);
			countdown=3;
		}
		if(countdown){
			fprintf(stderr,"\n    plot %d %d", x, y);
			if(!(--countdown))fprintf(stderr,"  ...");
		}
	}
#endif
 	int row,col;
	char mark, test;
	if( x<0 || x>=80 ) return;
	if( y<0 || y>=80 ) return;
	row=39-y/2; 
	col=x;
	if(label) {
		Space[col][row]=label;
		return;
	}
	if( Space[col][row]==':' ) return;    /* already double marked */
	test = (y%2) ? '.' : '\'' ;  /* use this mark */
	mark = (y%2) ? '\'' : '.' ;  /* but don't erase this */
	if( Space[col][row]==test ) {
		Space[col][row]=':';              /* double mark */
		return;
	}
	Space[col][row] = mark;
}

/* Draw a line from x to y */
int Mline(int nargs, int* args){
	int x0, y0, x1, y1;
	if(nargs<4){eset(MCERR); return -1; }
	x0 = (int)args[0];
	y0 = (int)args[1];
	x1 = (int)args[2];
	y1 = (int)args[3];
	line(x0,y0,x1,y1);
}
void line(int x0, int y0, int x1, int y1){
	if(debug>1)++label;
	else label=0;
	if(debug>1)fprintf(stderr,"\n  pgline: %d,%d -> %d,%d",x0,y0,x1,y1);
	int xr = x1-x0;
	int yr = y1-y0;
	int x,y;
	int little,big,inc;
	if( abs(xr) > abs(yr) ) {  /* near horizontal case */
		if(debug>1)fprintf(stderr,"\n  horizontal");
		float yinc, ypos;
		yinc = (float)yr/xr;  /*   -1 < yinc < +1   */
		if(x0<x1){
			little=x0;
			big=x1;
			ypos=y0;
		}
		else{
			little=x1;
			big=x0;
			ypos=y1;
		}
		if(debug>1)fprintf(stderr,
				"  little,big,yinc %d %d %f",little,big,yinc);
		for( x=little; x<=big; ++x ) { 
			plot(x,(int)ypos);
			ypos += yinc;
		}
	} else {       /* near vertical case */
		if(debug>1)fprintf(stderr,"\n  vertical");
		float xinc, xpos;
		xinc = (float)xr/yr;
		if(y0<y1){
			little=y0;
			big=y1;
			xpos=x0;
		}
		else{
			little=y1;
			big=y0;
			xpos=x1;
		}
		if(debug>1)fprintf(stderr,
				"  little,big,xinc %d %d %f",little,big,xinc);
		for( y=little; y<=big; ++y ) {
			plot((int)xpos,y);
			xpos += xinc;
		}
	}
//	if(movie){show();sleep(1);}
	return 0;
}

/* Define npts pgPoints in a circle, results in Pts */
void _pointCircle(int xCenter, int yCenter, 
		int radius, int xPts[], int yPts[], int npts) {
	int i;
	float angle = 2*M_PI/npts;
	for(i=0; i<npts; ++i) {
		xPts[i] = (int)(radius*cos(i*angle)+xCenter);
		yPts[i] = (int)(radius*sin(i*angle)+yCenter);
	}
}

int Mcircle(int nargs, int *args) {
	if(nargs<4){eset(MCERR); return -1; }
	int xCenter = args[0];
	int yCenter = args[1];
	int radius = args[2];
	int npts = args[3];
	circle(xCenter,yCenter,radius,npts);
}
void circle(int xCenter, int yCenter, int radius, int npts) {
	int xPts[npts],yPts[npts];
	_pointCircle(xCenter,yCenter,radius,xPts,yPts,npts);
	int i;
	for(i=0;i<npts-1;++i) {
		line(xPts[i],yPts[i],xPts[i+1],yPts[i+1]);
	}
	line(xPts[npts-1],yPts[npts-1],xPts[0],yPts[0]);
}

int Mstar(int nargs, int *args) {
	if(nargs<4){eset(MCERR); return -1; }
	int xCenter = args[0];
	int yCenter = args[1];
	int radius = args[2];
	int npts = args[3];
	star(xCenter,yCenter,radius,npts);
}
void star(int xCenter,int yCenter,int radius,int npts) {
	int xPts[npts],yPts[npts];
	_pointCircle(xCenter,yCenter,radius,xPts,yPts,npts);
	int i, to;
	for(i=0;i<npts;++i) {
		to = (i+npts/2)%npts;
		line(xPts[i],yPts[i],xPts[to],yPts[to]);
	}
}

int Mtext(int nargs, int *args) {
	if(nargs<3){eset(MCERR); return -1; }
	int x = args[0];
	int y = args[1];
	char* lbl = args[2];
	text(x,y,lbl);
}
void text(int x, int y, char* str){
	if( x<0 || x>=80 ) return;
	if( y<0 || y>=80 ) return;
	int row=39-y/2;
	int i;
	for(i=0;i<strlen(str);++i) Space[x+i][row]=str[i];
}

#if 0
/*x	STEP 3: Assure #if 1 above to turn on pure C code testing.
 *	Code a test main. Compile and test until your 
 *	function code is satisfactory. 
 */

// This eset is used only for step 3 testing. Leave it in place.
// When plugged into tc its eset will get linked for callback.
int eset(int err){
	fprintf(stderr,"MC error %d\n",err);
	exit(1);
}
int main(int argc, char **argv) {
	debug=frame=0;
	label=0x60;  /*Just before 'a'*/

	blank();
	plot(30,50);
	plot(48,28);
	text(50,28," <-- another");
	text(55,26,"mangled text");
	line(10,10, 70,70 );
	show();
	sleep(2);
	circle(40,40, 25, 29);
	show();
	sleep(2);
	star(40,40, 25, 7);
	text(32,50," <-- see the dot");
	text(29,12,"Always put text last");
	show();
}

#else

/*x	STEP 4: Assure #if 0 above to turn on plug-in 
 *	infrastructure. 
 */

/*	INFRASTRUCTURE...
 */

/*x	STEP 5: register your M functions in McList array. Placement 
 *	in the array determines the MC number starting with 1001.
 *	This is the only change that should be made in the code below.
 *	
 *x	STEP 6: Use buildPlugIn.sh to manufacture the shared object.
 *	Technically the plug-in is complete and plugged in. You 
 *	should see a dot-so file in your current working directory.
 *		
 *	STEP 7: Write a small test tc program that uses the plug-in
 *	functions. Don't forget the #loadMC <myPlug-in> at the top.
 *	
 *	STEP 8: Run you test tc program. Use the -l option to see
 *	that your dot-so file is loaded. You are now debugging the
 *	combo of your test tc program, and your plug-in.
 */

// define plug-in's eset to call real eset in tc
int (*callEset)(int);
int eset(int err){
	return (*callEset)(err);
}
// tcMain calls this with address of real eset
void register_eset( void (*eset)(int) )
{
    callEset = eset;
}

typedef int (*McList)(int,int*);
void naf(){
	eset(MCERR);
}
McList plugInList[] =            // do STEP 5 here...
	{ &Mblank, &Mshow, &Mplot, &Mline, &Mcircle 
		, &Mstar, &Mtext, &Msleep, &naf, &naf
	};

void plugInMC(int mcno, int nargs, int *args) {
	if(mcno<1 || mcno>(sizeof(plugInList)/sizeof(void*))) {
		eset(MCERR);
	}
	else {
	    int x = plugInList[mcno-1](nargs, args);
	}
}

#endif
