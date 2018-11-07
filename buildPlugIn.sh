#!/bin/bash

#buildPlugInMC.sh
# arg 1 is the root file name of the library, which will
# be the filename of the dot-so, dot-c and dot-o produced

if [ "$#" -eq 1 ]; then
    CODE_AT="./"
elif [ "$#" -eq 2 ]; then
    CODE_AT=$2
else
	echo "usage: buildPlugIn.sh  lib_name  path_to_lib_code_dir"
fi

echo code at $CODE_AT

rm -f $CODE_AT/$1.o
gcc -w -c -fPIC   $CODE_AT/$1.c   -o $CODE_AT/$1.o
if [ $? -ne 0 ]; then exit; fi
echo "object done: `ls $CODE_AT/$1.o`"  

gcc -shared $CODE_AT/$1.o   -o lib$1.so -lm
if [ $? -ne 0 ]; then exit; fi
echo "so Lib  done: `ls lib$1.so`"

ls -lt lib$1.so
