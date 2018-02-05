#!/bin/bash

# Compiles tiny-C using tcc
tcc -o tc -lm FileRead.c getch.c kbhit.c machineCall.c debug.c tc.c tcMain.c
ls -lt tc

