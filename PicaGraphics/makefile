# Compiler
CC = /usr/bin/gcc

# The tc object files
OBJPLOT = plot.o test.o objects.o

# All the header and c files
SRCS = plot.c star.c test.c objects.c
HDRS = plot.h

# Add -I to the dir the include files are in
#CFLAGS = -w -g -ansi -I /usr/lib/syslinux/com32/include/
CFLAGS = -g

PLOT = plot

all: $(PLOT)

#install:

# Build the executable file
$(PLOT): $(OBJPLOT)
	$(CC) $(CFLAGS) $(OBJPLOT) -o $(PLOT) -lm

# Separately compile each .c file
test.o: test.c plot.h
	$(CC) $(CFLAGS) -c test.c

plot.o: plot.c plot.h
	$(CC) $(CFLAGS) -c plot.c

star.o: star.c plot.h
	$(CC) $(CFLAGS) -c star.c

objects.o: objects.c plot.h
	$(CC) $(CFLAGS) -c objects.c

# Clean up
clean:
	rm -fv core* $(PLOT) $(OBJPLOT)

