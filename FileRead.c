#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "tc.h"

int nxtUnit=0;

/* read the named file into buffer pr. Return amount read on success,
	or zero on 'no such file', or -1 on read error. */
int fileRead(char* name, char* buff, int bufflen){
	int readlen,err;
	FILE *fp = fopen( name, "rb");
	if (fp != NULL) {
	    readlen = fread(buff, 1, bufflen, fp);  /* FREAD */
	    if ( err=ferror( fp ) != 0 ) {
	        return -1;
	    } else {
	        buff[readlen] = '\0'; /* Just to be safe. */
	    }
	    fclose(fp);
	}
	else {
		return 0;
	}
	return readlen;
}

/* write the named file from buffer pr. Return amount written
 *	or -1 on write error. If the named file exists its contents are
 *	truncated to zero before the write. If it doesn't exist it is 
 *	created.
 */
int fileWrite(char* name, char* buff, int bufflen){
	int wrlen,err;
	FILE *fp = fopen( name, "wb");
	if (fp != NULL) {
	    wrlen = fwrite(buff, 1, bufflen, fp);  /* FWRITE */
	    if ( err=ferror( fp ) != 0 ) {
	        return -1;
	    }
	    fclose(fp);
	}
	else {
		return 0;
	}
	return wrlen;
}

/*	open a file returning unit (0..MAX_UNIT), 
 *	else -9, too many, -1 fopen error
 */
int tcFopen(char* name, char* mode){
	if(nxtUnit>MAX_UNIT)return -9;
	FILE* file = fopen(name,mode);
	if(file==NULL)return -1;
	fileUnit[nxtUnit] = file;
	return nxtUnit++;
}
/*	put a string from unit returning the length,
 *	else -9 unit not open, -8 bad unit, -2 fputs error. 
 */
int tcFputs(char* str, int unit) {
	if(unit<0 + unit>MAX_UNIT)return -8;
	if(fileUnit[unit]==NULL)return -9;
 	if( fputs(str,fileUnit[unit]) != NULL){
 		return strlen(str);
 	}
 	return -2;
 }
/*	put a character from unit returning 1 ok,
 *	-9 unit not open, -8 bad unit, -3 fputc error. 
 */
int tcFputc(char c, int unit) {
	if(unit<0 + unit>MAX_UNIT)return -8;
	if(fileUnit[unit]==NULL)return -9;
 	if( fputc(c,fileUnit[unit]) != NULL){
 		return 1;
 	}
 	return -3;
 }
/*	get a line from unit returning its length including the newline,
 *	else -9 unit not open, -8 bad unit, -4 fgets error
 */
int tcFgets(char* buff, int len, int unit) {
	if(unit<0 + unit>MAX_UNIT)return -8;
	if(fileUnit[unit]==NULL)return -9;
 	if( fgets(buff,len,fileUnit[unit]) != NULL){
 		return strlen(buff);
 	}
 	return -4;
 }
/*	close the file -8 bad unit, 
 */
 int tcFclose(unit) {
	if(unit<0 + unit>MAX_UNIT)return -8;
	if(fileUnit[unit]==NULL)return -9;
 	fclose(fileUnit[unit]);
 	fileUnit[unit]=0;
 }

/*	set *val to default unless optionally overridden in property file.
 *	Syntax each line: name whiteSpace value newline.
 */
int iProperty(char* file, char* name, int *val, int _default) {
	char buff[256];
	*val = _default;
	char* next;
	int lname = strlen(name);
	char* endptr;
	FILE* fp = fopen(file,"r");
	if(fp==NULL){
		return -1;
	}
	while(1){
		next=fgets(buff,256,fp);
		if( next==NULL ) break;
		if( !strncmp(buff,name,lname)) {
			*val = strtol(next+lname,&endptr,10);
			break;
		}
	}
	fclose(fp);
	return 0;
}
/***ISSUE 5/22/2018 Above could call sProperty for the search */


/*	set *val to default unless optionally overridden in property file.
 *	Syntax each line: name whiteSpace value newline.
 */
int sProperty(char* file, char* name, char* val, int vlen, char* _default) {
	char buff[256];
	char* next;
	char* v;
	int lv;
	int lname = strlen(name);
	strncpy(val,_default,vlen);
	FILE* fp = fopen(file,"r");
	if(fp==NULL){
		return -1;
	}
	while(1){
		next=fgets(buff,256,fp);
		if( next==NULL ) break;
		if( !strncmp(next,name,lname)) {
			v = buff+lname;
			while(*v==' '||*v=='\t'||*v=='=') ++v;
			lv = strlen(v);
			if(v[lv-1]=='\n')v[lv-1]=0;
			strncpy(val,v,vlen);
			break;
		}
	}
	fclose(fp);
	return next;
}
