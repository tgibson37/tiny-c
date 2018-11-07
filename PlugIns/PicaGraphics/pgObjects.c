#include "pgPlot.h"

/*	Pica plot routine in 80x80 space. 
 */
char Space[80][40];

/* fill Space with spaces */
void blank() {
	int r,c;
	for(r=0; r<40; ++r) for(c=0; c<80; ++c) Space[c][r]=' ';
}

/* show the drawing */
void show() {
	int r,c;
	for(r=0; r<40; ++r) {
		printf("\n");
		if(frame)printf("|");
		for(c=0; c<80; ++c) printf("%c",Space[c][r]);
		if(frame)printf("|");
	}
	printf("\n");
}

char lastLabel; 
int countdown=0;

/* plot a small mark at raw position x,y */
void pgPlot(int x, int y) {
	if(debug>1){
		if(lastLabel!=label){
			lastLabel=label;
			fprintf(stderr,"\n    pgPlot: new Object %c",label);
			countdown=3;
		}
		if(countdown){
			fprintf(stderr,"\n    pgPlot %d %d", x, y);
			if(!(--countdown))fprintf(stderr,"  ...");
		}
	}
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

/* Draw a line from a to b */
void pgLine(int x0, int y0, int x1, int y1) {
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
			pgPlot(x,(int)ypos);
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
			pgPlot((int)xpos,y);
			xpos += xinc;
		}
	}
	if(movie){show();sleep(1);}
}

/* Define npts pgPoints in a circle, results in pts */
void pgPointCircle(int xCenter, int yCenter, 
		int radius, int xPts[], int yPts[], int npts) {
	int i;
	float angle = 2*M_PI/npts;
	for(i=0; i<npts; ++i) {
		xPts[i] = (int)(radius*cos(i*angle)+xCenter);
		yPts[i] = (int)(radius*sin(i*angle)+yCenter);
	}
}

void pgCircle(int xCenter, int yCenter, int radius, int npts) {
	int xPts[npts],yPts[npts];
	pgPointCircle(xCenter,yCenter,radius,xPts,yPts,npts);
	int i;
	for(i=0;i<npts-1;++i) {
		pgLine(xPts[i],yPts[i],xPts[i+1],yPts[i+1]);
	}
	pgLine(xPts[npts-1],yPts[npts-1],xPts[0],yPts[0]);
}

void pgStar(int xCenter, int yCenter, int radius, int npts) {
	int xPts[npts],yPts[npts];
	pgPointCircle(xCenter,yCenter,radius,xPts,yPts,npts);
	int i, to;
	for(i=0;i<npts;++i) {
		to = (i+npts/2)%npts;
		pgLine(xPts[i],yPts[i],xPts[to],yPts[to]);
	}
}

