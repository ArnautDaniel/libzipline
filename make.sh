#!/bin/bash

gcc -Wall -fPIC -c main.c
gcc -Wall -fPIC -c extern/zip.c
gcc -shared -o libzipline.so main.o zip.o -Wl,-rpath='${ORIGIN}' 
gcc -o test test.c -Wl,-rpath='${ORIGIN}' -L. -l:libzipline.so
