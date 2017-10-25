#include <time.h>
#include <stdio.h>
#include <string.h>

#define SIZE 256

void time_now(char *buff) {
  time_t curtime;
  struct tm *loctime;
  curtime = time (NULL);
  loctime = localtime (&curtime);
  strcpy(buff,asctime (loctime));
    strftime (buff, 40, " %R", loctime);

}

/*int main (void)
{
  char timeStamp[40];
  time_now(timeStamp);
  fputs (timeStamp, stdout);
}
*/