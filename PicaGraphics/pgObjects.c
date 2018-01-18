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
	if(debug){
		if(lastLabel!=label){
			lastLabel=label;
			fprintf(stderr,"\nplot: new Object %c",label);
			countdown=3;
		}
		if(countdown){
			fprintf(stderr,"\nplot %d %d", x, y);
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
void pgLine( pgPoint a, pgPoint b ) {
	if(debug)++label;
	else label=0;
	if(debug)fprintf(stderr,"\nLine: %d,%d -> %d,%d",a.x,a.y,b.x,b.y);

	int xr = b.x-a.x;
	int yr = b.y-a.y;
	int x,y;
	int little,big,inc;
	if( abs(xr) > abs(yr) ) {  /* near horizontal case */
		if(debug)fprintf(stderr,"\nhorizontal");
		float yinc, ypos;
		yinc = (float)yr/xr;  /*   -1 < yinc < +1   */
		if(a.x<b.x){
			little=a.x;
			big=b.x;
			ypos=a.y;
		}
		else{
			little=b.x;
			big=a.x;
			ypos=b.y;
		}
		if(debug)fprintf(stderr,
				"  little,big,yinc %d %d %f",little,big,yinc);
		for( x=little; x<=big; ++x ) { 
			pgPlot(x,(int)ypos);
			ypos += yinc;
		}
	} else {       /* near vertical case */
		if(debug)fprintf(stderr,"\nvertical");
		float xinc, xpos;
		xinc = (float)xr/yr;
		if(a.y<b.y){
			little=a.y;
			big=b.y;
			xpos=a.x;
		}
		else{
			little=b.y;
			big=a.y;
			xpos=b.x;
		}
		if(debug)fprintf(stderr,
				"  little,big,xinc %d %d %f",little,big,xinc);
		for( y=little; y<=big; ++y ) {
			pgPlot((int)xpos,y);
			xpos += xinc;
		}
	}
	if(movie){show();sleep(1);}
}

/* Define npts pgPoints in a circle, results in pts */
void pgPointCircle(pgPoint center, int radius, pgPoint pts[], int npts ) {
	if(debug)fprintf(stderr,"\npgObject~117 pgPointCircle");
	int i;
	float angle = 2*M_PI/npts;
	for(i=0; i<npts; ++i) {
		pts[i].x = (int)(radius*cos(i*angle)+center.x);
		pts[i].y = (int)(radius*sin(i*angle)+center.y);
	}
}

void pgCircle(pgPoint center, int radius, int npts) {
	if(debug)fprintf(stderr,"\npgObject~127 circle");
	pgPoint pts[npts];
	pgPointCircle(center,radius,pts,npts);
	int i;
	for(i=0;i<npts-1;++i) {
		pgLine(pts[i],pts[i+1]);
	}
	pgLine(pts[npts-1],pts[0]);
}

void pgStar(pgPoint center, int radius, int npts) {
	if(debug)fprintf(stderr,"\npgObject~127 circle");
	pgPoint pts[npts];
	pgPointCircle(center,radius,pts,npts);
	int i, to;
	for(i=0;i<npts;++i) {
		to = (i+npts/2)%npts;
		pgLine(pts[i],pts[to]);
	}
}

