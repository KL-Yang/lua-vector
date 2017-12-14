#!/bin/bash

FLAG="-W -Wall -ffast-math -std=gnu99"

gcc -c $FLAG common.c -O3
gcc -o v1 -W vector_v1.c common.o -O3 -msse3 -ffast-math -ggdb -std=gnu99 -llua5.3 -lm -ldl 
gcc -o v2 -W vector_v2.c -O3 -msse3 -ffast-math -ggdb -std=gnu99 -llua5.3 -lm -ldl
gcc -o v3 -W vector_v3.c -O3 -msse3 -ffast-math -ggdb -std=gnu99 -llua5.3 -lm -ldl
gcc -o v4 -W vector_v4.c common.o -O3 -msse3 -ffast-math -ggdb -std=gnu99 -llua5.3 -lm -ldl
