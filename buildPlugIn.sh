#!/bin/bash

#buildPlugInMC.sh
# arg 1 is the root file name of the library, which will
# be the filename of the dot-so, dot-c and dot-o produced

if [ $0 != "./bpi" ]; then
	echo "recommend: ln -s buildPlugIn.sh bpi (quicker typing)"
fi	

if [ "$#" -eq 1 ]; then
    CODE_AT="./"
elif [ "$#" -eq 2 ]; then
    CODE_AT=$2
else
	echo "usage: buildPlugIn.sh  lib_name  path_to_lib_code_dir"
fi

gcc -w -c -fPIC   $CODE_AT/$1.c   -o $CODE_AT/$1.o
echo "object done: `ls $CODE_AT/$1.o`"  
gcc    -shared $CODE_AT/$1.o   -o lib$1.so
echo "so Lib  done: `ls lib$1.so`"
chmod 755 lib$1.so

ls -lt lib$1.so
