#include <termios.h>
#include <stdio.h>

/* ref:
 * https://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux
   Returns 0x1b (esc) followed by two characters for keys such as up arrow.
 */


static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}

/* Let's test it out
int main(void) {
  char c;
  printf("(getche example) please type a letter: ");
  c = getche();
  printf("\nYou typed: %c\n", c);
  while(1){
	  printf("(getch example) please type a letter...");
	  c = getch();
	  printf("\nYou typed: %c\n", c);
  }
  return 0;
} 
 */
