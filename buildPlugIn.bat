gcc -w -c -fPIC %1.c -o %1.o
dir %1.o
gcc -shared %1.o -o lib%1.so -lm
dir lib%1.so
