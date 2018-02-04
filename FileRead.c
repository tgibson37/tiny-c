#include <stdio.h>
#include <stdlib.h>
/*
#include "tc.h"
*/

/* read the named file into buffer pr. Return amount read
	or zero on 'no such file' or -1 on read error. */
int FileRead(char* name, char* buff, int bufflen){
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

/*int main(int argc,char* argv[]){
	if(argc<2) {
		printf("Usage: testFileRead fileName\n");
		exit(1);
	}
	int len = testFileRead(argv[1]);
	switch(len){
		default:
			printf("\nlen %d\n-->%s<--\n",len,pr);
		break;
		case 0:
			printf("no such file\n");
		break;
		case -1:
	        printf("Error reading file %d", error);
		break;
	}
}
*/
