#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*************  Platform stuff ************/
/* INSTALLERS/PORTERS may need to redefine these functions.
	Current choices: Windows, else linux.
 */

#if defined(_WIN32)
// WINDOWS, uses its lib code except getch_
#include <conio.h>
void initTermios(int echo) {}
void resetTermios(void) {}
char getch_(int echo) {
    return (echo) ? getche() : getch();
}

#else
//LINUX (works on mint linux, a derivitave of ubuntu)
#include <termios.h>
/*	Detect a key hit is waiting in its buffer, return the char,
 *	leaving it in the buffer to be read. Used to detect ESC character
 *	to force a quit. Return 0 if no character waiting.
 */
int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return ch;
	}

	return 0;
}
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

/* Non blocking read 1 character. echo defines echo mode. */
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

#endif