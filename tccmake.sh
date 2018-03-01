#!/bin/bash

# Compiles tiny-C using tcc
tcc -o tc -lm platform.c debug.c FileRead.c tc.c var.c dialog.c machineCall.c stack.c tcMain.c
ls -lt tc

