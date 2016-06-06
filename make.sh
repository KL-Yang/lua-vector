#!/bin/bash
gcc -o v1 vector_v1.c -O3 -msse3 -ffast-math -ggdb -std=gnu99 -llua5.3 -lm -ldl 
gcc -o v2 vector_v2.c -O3 -msse3 -ffast-math -ggdb -std=gnu99 -llua5.3 -lm -ldl
