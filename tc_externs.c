#include "tc.h"
char verbose[8];
int debug;
int quiet;
struct stackentry *stack;
int nxtstack, stacklen;
struct funentry *fun;
struct funentry *curglbl, *curfun, *efun;
struct var *vartab;
int nxtvar, vtablen;
char fcnName[VLEN+1];
char* pr;
char *lpr, *apr, *endapp, *prused, *EPR;
int error, leave, brake;
char* fname;
char* lname;
char* cursor;
char* stcurs;
char* errat;
int obsize, vclass, alen;
int traceMode;
char* ppsPath="./pps";
FILE* fileUnit[MAX_UNIT];
