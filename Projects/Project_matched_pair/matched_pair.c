
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// matched_pair.c - 6/27/18 - tct

// globals

char *gp,lbr,rbr;int gctr,verbose;

void pft (char *from,char *to) {
 while (from <= to) printf("%c",*(from++));
 }

void find_matching_right_bracket() {
 if (verbose) printf("%d",gctr);
 while (gctr) {
  if (*gp==lbr) {++gctr;if (verbose) printf("%d",gctr);}
  if (*gp==rbr) {--gctr;if (verbose&&gctr!=0) printf("%d",gctr);}
  if (gctr!=0) ++gp;
  }
 }

void usage() {
 printf("%cUsage: matched_pair [-l left_bracket_character]",10);
 printf(" [-r right_bracket_character] [-v] [-h]%c",10);
 }

int main(int argc,char *argv[]) {

// defaults

 lbr=91;rbr=93;verbose=0;

 int buffer_len,i;
 char file[30]={0},*p_lbr,*p_beg,*p_end;
 char buffer[10000]={0},buff[1000]={0};

 printf("%cmatched_pair.c, tct, 6/27/18%c",10,10);

 for (i=1;i<argc;++i) {
  if (strcmp(argv[i],"-l")==0) {++i;lbr=*argv[i];}
  if (strcmp(argv[i],"-r")==0) {++i;rbr=*argv[i];}
  if (strcmp(argv[i],"-v")==0) {verbose=1;}
  if (strcmp(argv[i],"-h")==0) {usage();}
  }

 printf("%cfile? ",10);fgets(file,30,stdin);
 file[strlen(file)-1]=0;
 printf("%c",10);
 FILE *fp;
 fp=fopen(file,"r");
 while (fgets(buff,1000,fp)!=NULL) strcat(buffer,buff);
 fclose(fp);
 buffer_len=strlen(buffer);
 gp=buffer;

 while (1) {

  // find first left bracket

  while (*gp!=lbr) {
   if (gp==buffer+buffer_len) {printf("%c",10);exit(1);}
   ++gp;
   }

  p_lbr=p_beg=p_end=gp;

  // find the beginning and the ending of the line containing the first left bracket

  while (*p_beg!=10) --p_beg;
  ++p_beg;
  while (*p_end!=10) ++p_end;

  // print out the line containing the function

  pft(p_beg,p_end);

  ++gp; // get past the first left bracket

  gctr=1;

  find_matching_right_bracket();

  ++gp; // get past the matching right bracket

  printf("%c",10);

  }

 }

