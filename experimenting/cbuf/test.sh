#!/bin/sh

rm -f cachegrind* callgrind* mmap naive
files="mmap.c naive.c"
for file in $files; do
    out=${file%".c"}
    gcc -static -Ofast -fno-builtin -std=c99 -march=native $file -o "$out"
    valgrind --dsymutil=yes --tool=callgrind ./$out
    valgrind --dsymutil=yes --tool=cachegrind ./$out
    callgrind_annotate callgrind*
done
