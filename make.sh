#!/bin/bash

LUA_FLAG="-W -Wall -O3 -msse3 -ffast-math -std=gnu99"
LUA_LINK="lua-5.3.4/src/liblua.a"

JIT_FLAG="-W -Wall -O3 -msse3 -ffast-math -std=gnu99 -DUSE_JIT"
JIT_LINK="LuaJIT-2.0.5/src/libluajit.a"

gcc -c $LUA_FLAG common.c -O3
gcc -o v1 $LUA_FLAG vector_v1.c common.o $LUA_LINK -lm -ldl 
gcc -o v4 $LUA_FLAG vector_v4.c common.o $LUA_LINK -lm -ldl

gcc -c $JIT_FLAG common.c -O3
gcc -o v1_jit $JIT_FLAG vector_v1.c common.o $JIT_LINK -lm -ldl 
gcc -o v4_jit $JIT_FLAG vector_v4.c common.o $JIT_LINK -lm -ldl
