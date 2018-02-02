# Compiler
CC = /usr/bin/gcc

# Name of development files/targets
TC = tc
LIB = ./pps/library.tc
TEST = test
RUN = ./tc
DOTEST = ./test > test_results
TEST_RESULTS = test_results
DIFF = diff test_results good_results
KEEP = cp test_results good_results

# Linix install dirs, note these are file names
INSTALLTC = /usr/local/bin/tinyc
INSTALLLIB = /usr/local/share/tinyC/library.tc
LATEST = ls -lt $(TC) $(INSTALLTC) $(LIB) $(INSTALLLIB)

# The tc object files
OBJTC = tc.o FileRead.o time.o getch.o kbhit.o debug.o machineCall.o tcMain.o

# The test object files
OBJTEST = tc.o test.o FileRead.o time.o getch.o kbhit.o debug.o machineCall.o tcTestMain.o

# All the header and c files gcc-able with CFLAGS
SRCS = test.c tc.c machineCall.c tcTestMain.c tcMain.c 
HDRS = tc.h

# Add -I to the dir the include files are in
CFLAGS = -w -g -ansi -I /usr/lib/syslinux/com32/include/
#CFLAGS = -Wall -m32 -g        #<< still a flood

all: $(TC) $(TEST)

run:
	$(RUN)

diff: 
	$(DIFF)

keep: $(TEST_RESULTS)
	$(KEEP)

dotest:
	$(DOTEST)
	ls -lt *_results

# Linux only, requires privilage to write install dirs. 
install: tc
	cp tc $(INSTALLTC)
	cp pps/library.tc $(INSTALLLIB)

latest:
	$(LATEST)

# Build (link) the executable files
$(TC): $(OBJTC)
	$(CC) $(CFLAGS) $(OBJTC) -o $(TC)

$(TEST): $(OBJTEST)
	$(CC) $(CFLAGS) $(OBJTEST) -o $(TEST)

# Seperately compile each .c file
test.o: test.c tc.h
	$(CC) $(CFLAGS) -c test.c

tc.o: tc.c tc.h
	$(CC) $(CFLAGS) -c tc.c

# Clean up
clean:
	rm -fv core* $(TC) $(OBJS)

# separately: gcc -c FileRead.c,...  WITHOUT cflags

debug.o: debug.c tc.h
	$(CC) -c -w -g debug.c

FileRead.o: FileRead.c tc.h
	$(CC) -c -w FileRead.c

getch.o: getch.c tc.h
	$(CC) -c -w getch.c tc.h

kbhit.o: kbhit.c tc.h
	$(CC) -c -w kbhit.c tc.h

time.o: time.c tc.h
	$(CC) -c -w time.c tc.h

