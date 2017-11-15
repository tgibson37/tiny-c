#include "tc.h"

/* tc debugger 
 */

struct brk {
	struct var* var;
	int enabled;
};
int brktab[10];
int nxtbrk = 0;
int running = 0;

char buf[80];
int sizeof_line = 80;

char *param() {
	return buf[1]==' ' ? &buf[2] : &buf[1];
}

void tcDebug() {
	if(running) {
		if(hit()) {
			showLine();
			cmds();
		}
	}
	else {
		cmds();
	}
}

void cmds() {
	while(1) {
		int cmd = do_cmd();
		if(cmd=='X') exit(0);
		else if(cmd=='r') return;
	}
}

int do_cmd() {
	printf("\n(db) ");
	fgets(buf, sizeof(buf), stdin);
	char cmd = buf[0];
	switch(cmd) {
/* set breakpt */
	case 'b':
		printf("%s\n",param());
		break;
/* run */
	case 'r':
		running = 1;
		return 'r';
		break;
/* exit */
	case 'X':
		return 'X';
		break;
	default:
		printf("???\n");
	}
}

struct var* hit() {
	
}
	
