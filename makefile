# Compiler
CC = /usr/bin/gcc
# alternative for _WIN32
#CC = /usr/bin/gcc.exe

# Name of development files/targets
TC = tc
LIB = ./pps/library.tc
TEST = test
RUN = ./tc
DOTEST = ./test 2>dump_results >test_results
TEST_RESULTS = test_results
DIFF1 = diff test_results testFiles/good_t_results
KEEP1 = cp test_results testFiles/good_t_results
DIFF2 = diff dump_results testFiles/good_d_results
KEEP2 = cp dump_results testFiles/good_d_results

# Linix install dirs, note these are file names
INSTALLTC = /usr/local/bin/tinyc
INSTALLLIB = /usr/local/share/tinyC/library.tc
LATEST = ls -lt $(TC) $(INSTALLTC) $(LIB) $(INSTALLLIB)

OBJMOST = tc.o FileRead.o debug.o machineCall.o platform.o var.o stack.o dialog.o

OBJALL = $(OBJMOST) tcTestMain.o tcMain.o test.o

# The tc object files
OBJTC = $(OBJMOST) tcMain.o

# The test object files
OBJTEST = $(OBJMOST) test.o tcTestMain.o

# All the header and c files
SRCS = test.c tc.c machineCall.c var.c stack.c tcTestMain.c tcMain.c \
platform.c dialog.c
HDRS = tc.h

# Add -I to the dir the include files are in
#CFLAGS = -Wall -g        #<< still a flood
CFLAGS = -w -g

.PHONY: all run difft diffd keep dotest install latest
all: $(TC) $(TEST)

run:
	$(RUN)

difft: 
	$(DIFF1)

diffd:
	$(DIFF2)

keep: $(TEST_RESULTS)
	$(KEEP1)
	$(KEEP2)

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

.PHONY: clean cleanobj cleanexe
clean : cleanobj
	rm -fv test_results dump_results tc.h.gch

cleanobj :
	rm -fv *.o

cleanexe :
	rm -fv tc test
