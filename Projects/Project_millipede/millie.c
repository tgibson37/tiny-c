#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// millie.c - 6/28/18 - tct

char *gp,*end;

int find_char(char c) {
 while (*gp!=c) {
  ++gp;
  if (gp==end) return 0;
  }
  return 1;
 }

void main() {

 printf("%cmillie.c - 6/28/18 - tct",10);

 int accum_time[30],i,j,buffer_len;
 char file[30]={0},buffer[10000]={0},buff[1000]={0},*num_str;
 int average_whole,average_remainder,pace,fast,slow;
 char right_paren=41;
  
 printf("%c%cfile? ",10,10);fgets(file,30,stdin);
 file[strlen(file)-1]=0;

 printf("%c",10);
 FILE *fp;
 fp=fopen(file,"r");
 while (fgets(buff,1000,fp)!=NULL) strcat(buffer,buff);
 fclose(fp);
 buffer_len=strlen(buffer);

 gp=buffer;
 end=gp+buffer_len;

// get past the text that precedes the data

 find_char(right_paren);++gp;
 find_char(right_paren);++gp;

 i=0;

 while (1) {

  if (find_char(',')==0) break;
  ++gp;
  num_str=gp;
  find_char('\n');
  *gp=0;
  accum_time[i]=atoi(num_str);
  printf("%c%d %d",10,i+1,accum_time[i]);
  ++i;

  }

 average_whole=accum_time[i-1]/i;
 average_remainder=accum_time[i-1]%i;

 printf("%c%cAverage pace : ",10,10);printf("%d",average_whole);
 printf(" %d/%d tibs per mib",average_remainder,i);

 fast=99;slow=0;j=0;

 while (j<i-1) {

  pace=accum_time[j+1]-accum_time[j];
  if (pace>slow) slow=pace;
  if (pace<fast) fast=pace;
  j=j+1;

  }

 printf("%cFastest pace : %d tibs per mib",10,fast);
 printf("%cSlowtest pace : %d tibs per mib%c",10,slow,10);

 }

