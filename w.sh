#!/bin/bash
# 
# Convenience script to show warnings for whatever files are recompiled.
# 
# recompiles at least one object file assumming make has warnings turned on.
# greps just the warning lines for quick reference.
# leaves full warnings text in the file warnings.txt
#   Usage: ./w.sh filename
#       extent is optional, and if given is changed to dot-o
#       therefore filename.o will be recompiled.

# Robust against wrong extent, an easy human error.
#   i.e arg is tc.c instead of tc.o
name=$(echo "$1" | cut -f 1 -d '.')
ext=".o"
obj=$name$ext
#echo $1
#echo $name
echo deleting $obj
rm $obj
make 2> warnings.txt
echo
grep warning warnings.txt
