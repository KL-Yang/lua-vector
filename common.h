#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#ifdef USE_JIT
#include "LuaJIT-2.0.5/src/lua.h"
#include "LuaJIT-2.0.5/src/lualib.h"
#include "LuaJIT-2.0.5/src/lauxlib.h"
#define   lua_rawlen(a,b) lua_objlen(a,b)    //lua-5.2
#else
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
#endif

#ifndef FAIL_ON
#define FAIL_ON(c,msg)  do {if(c) {printf("%s", msg); abort();}} while(0)
#endif

void vector_op_vs(const double * restrict a, double b, int n, char op, double * restrict c); 
void vector_op_sv(double a, const double * restrict b, int n, char op, double * restrict c);
void vector_op_vv(const double * restrict a, const double * restrict b, int n, char op, double * restrict c);
