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

 int accum_time[200],i,j,buffer_len,fast_leg,slow_leg;
 FILE *fp;
 char file[30]={0},buffer[10000]={0},buff[1000]={0},*num_str;
 int average_whole,average_remainder,pace,fast,slow;
 char right_paren=41;
  
 printf("%c%cfile? ",10,10);fgets(file,30,stdin);
 file[strlen(file)-1]=0;

 if ((fp=fopen(file,"r"))==NULL) {printf("%cerror on file open",10);exit(1);}
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
  printf("%cmib %d %caccumulated tibs %d",10,i+1,9,accum_time[i]);
  ++i;
  }

 average_whole=accum_time[i-1];
 printf("%c%cAverage pace was %3.1f",10,10,(float)average_whole/i);

 fast=99;slow=0;

 for (j=0;j<i-1;++j) {
  pace=accum_time[j+1]-accum_time[j];
  if (pace>slow) {slow=pace;slow_leg=j+2;}
  if (pace<fast) {fast=pace;fast_leg=j+2;}
  }

 printf("%c%cFastest pace occurred on leg # %d and was %d tibs per mib",10,10,fast_leg,fast);
 printf("%cSlowest pace occurred on leg # %d and was %d tibs per mib%c",10,slow_leg,slow,10);

 }

