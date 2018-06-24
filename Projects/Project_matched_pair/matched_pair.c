
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// matched_pair.c - 6/24/18 - tct

// globals
char *gp,lbr,rbr;int gctr;

void pft (char *from,char *to) {
 while (from <= to) printf("%c",*(from++));
 }

void find_matching_right_bracket() {
 while (gctr) {
  if (*gp==lbr) ++gctr;
  if (*gp==rbr) --gctr;
  if (gctr!=0) ++gp;
  }
 }

void main() {
 int buffer_len;
 char file[20],buff[1000],*p_lbr,*p_beg,*p_end;
 char buffer[10000]={0}; // important!
 lbr=91;rbr=93;
 printf("%cmatched_pair.c, tct, 6/24/18%c",10,10);
 printf("%cfile? ",10);gets(file);
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
   if (gp==buffer+buffer_len) exit(1);
   ++gp;
   }
  p_lbr=p_beg=gp;
  gctr=1;
  // find beginning and ending of line containing the first left bracket
  while (*p_beg!=10) --p_beg;
  ++p_beg;
  p_end=p_lbr;
  while (*p_end!=10) ++p_end;
  // print out the line containing the function
  pft(p_beg,p_end);
  ++gp; // get past first left bracket
  find_matching_right_bracket();
  ++gp; // get past matching right bracket
  printf("%c",10);
  }
 }

