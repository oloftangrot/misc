#!/bin/bash
#
gcc -c -I/$HOME/include example.c >& compiler.txt
if [ $? -ne 0 ]; then
  echo "Errors compiling example.c"
  exit
fi
rm compiler.txt
#
gcc example.o ~/libc/gnuplot_i.o -o example -lm
if [ $? -ne 0 ]; then
  echo "Errors linking and loading example.o"
  exit
fi
rm example.o
#
#
echo "Executable installed as ~/binc/$ARCH/example"
