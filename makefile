# Compiler
ifeq ($(OS),Windows_NT)
    CC = gcc.exe
    TC = tc.exe
    TEST = test.exe
    RM = del /q
    LS = dir /o-d
    RUN = tc
    RUNTEST = test
else
    CC = /usr/bin/gcc
    TC = tc
    TEST = test
    RM = rm -fv
    LS = ls -lt
    RUN = ./tc
    RUNTEST = ./test
endif

# Name of development files/targets

VERSIONPROP = version.prop
LIB = ./pps/library.tc
DOTEST = $(RUNTEST) 2>dump_results >test_results
TEST_RESULTS = test_results
DIFF1 = diff test_results testFiles/good_t_results
KEEP1 = cp test_results testFiles/good_t_results
DIFF2 = diff dump_results testFiles/good_d_results
KEEP2 = cp dump_results testFiles/good_d_results
DIFF3 = cat diff_*.txt

# Linux install dirs, note these are file names
INSTALLTC = /usr/local/bin/tinyc
INSTALLLIB = /usr/local/share/tinyC/library.tc
LATEST = ls -lt $(TC) $(INSTALLTC) $(LIB) $(INSTALLLIB)

OBJMOST = tc_externs.o tc.o FileRead.o debug.o machineCall.o platform.o var.o stack.o dialog.o

OBJALL = $(OBJMOST) tcTestMain.o tcMain.o test.o

# The tc object files
OBJTC = $(OBJMOST) tcMain.o

# The test object files
OBJTEST = $(OBJMOST) test.o tcTestMain.o

# All the header and c files
SRCS = tc_externs.c test.c tc.c machineCall.c var.c stack.c tcTestMain.c tcMain.c \
platform.c dialog.c
HDRS = tc.h

# Add -I to the dir the include files are in
#CFLAGS = -g -Wall -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast \
-Wno-unused-variable -Wno-unused-but-set-variable
#CLIBS = -L/usr/lib/x86_64-linux-gnu -ldl -lm
CLIBS =  -ldl -lm
CFLAGS = -w -g -m32 
#-fcommon
#CFLAGS = 
.PHONY: all run difft diffd keep dotest install latest
all: $(TC) $(TEST)
# $(VERSIONPROP)

run:
	$(RUN)

difft: 
	$(DIFF1)

diffd:
	$(DIFF2)

diffp:
	$(DIFF3)

keep: $(TEST_RESULTS)
	$(KEEP1)
	$(KEEP2)

dotest:
	$(DOTEST)
	$(LS) *_results

# Linux only, requires privilage to write install dirs. 
install: tc
	cp tc $(INSTALLTC)
	cp pps/library.tc $(INSTALLLIB)

latest:
	$(LATEST)

# Build (link) the executable files
$(TC): $(OBJTC)
	$(CC) $(CFLAGS) $(OBJTC) -o $(TC) $(CLIBS)

$(TEST): $(OBJTEST)
	$(CC) $(CFLAGS) $(OBJTEST) -o $(TEST) $(CLIBS)
	
#$(VERSIONPROP):
#	./version.sh
# alternative for _WIN32
# manually launch the following .bat file
#	version.bat	
	

$(VERSIONPROP): $(TC)
	./version.sh

.PHONY: clean cleanobj cleanexe
clean : cleanobj
	$(RM) test_results dump_results tc.h.gch

cleanobj :
	$(RM) *.o

cleanexe :
	$(RM) $(TC) $(TEST)
