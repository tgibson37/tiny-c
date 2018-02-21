#include <stdio.h>
#include <stdlib.h>
/*
#include "tc.h"
*/

/* read the named file into buffer pr. Return amount read
	or zero on 'no such file' or -1 on read error. */
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

