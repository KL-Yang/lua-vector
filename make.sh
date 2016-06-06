#!/bin/bash
gcc -o v1 my_vector.c -O3 -msse3 -ffast-math -ggdb -std=gnu99 lua-5.2/liblua.a -lm -ldl 
gcc -o v2 m2_vector.c -O3 -msse3 -ffast-math -ggdb -std=gnu99 lua-5.2/liblua.a -lm -ldl
