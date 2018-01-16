# Compiler
CC = /usr/bin/gcc

# The tc object files
OBJPLOT = plot.o star.o test.o

# All the header and c files
SRCS = plot.c star.c test.c
HDRS = plot.h

# Add -I to the dir the include files are in
#CFLAGS = -w -g -ansi -I /usr/lib/syslinux/com32/include/
CFLAGS = 

PLOT = plot

all: $(PLOT)

#install:

# Build the executable file
$(PLOT): $(OBJPLOT)
	$(CC) $(CFLAGS) $(OBJPLOT) -o $(PLOT) -lm

# Seperately compile each .c file
test.o: test.c plot.h
	$(CC) $(CFLAGS) -c test.c

plot.o: plot.c plot.h
	$(CC) $(CFLAGS) -c plot.c

star.o: star.c plot.h
	$(CC) $(CFLAGS) -c star.c

# Clean up
clean:
	rm -fv core* $(PLOT) $(OBJS)

