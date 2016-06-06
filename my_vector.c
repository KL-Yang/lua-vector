#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lua-5.2/lua.h"
#include "lua-5.2/lualib.h"
#include "lua-5.2/lauxlib.h"

#ifndef FAIL_ON
#define FAIL_ON(c,msg) do {if(c) {printf("%s", msg); abort();}} while(0);
#endif
#define KY_VECT_META_TABLE_V1  "ky_vmeta_table_v1"

/**
 * @brief convert table in lua stack[i] into native C array
 * */
double * 
vect_getfarray(lua_State *lua, int * restrict n, int i)
{
    int m; double *a;
    m = *n = (int)lua_rawlen(lua, i);
    a = malloc(m*sizeof(double));
    for(int k=0; k<m; k++) {
        lua_rawgeti(lua, i, k+1);
        a[k] = lua_tonumber(lua, -1);
        lua_pop(lua, 1);
    }
    return a;
}

/**
 * @brief put the native C double array on top of lua stack
 * */
void 
vect_putfarray(lua_State *lua, const double * restrict a, int n)
{
    lua_createtable(lua, n, 0);
    for(int k=0; k<n; k++) {
        lua_pushnumber(lua, a[k]);
        lua_rawseti(lua, -2, k+1);
    }
    luaL_getmetatable(lua, KY_VECT_META_TABLE_V1);
    lua_setmetatable(lua, -2);
}

static void 
vect_meta_vv(const double * restrict a, const double * restrict b, 
        int n, char op, double * restrict c) 
{
    switch(op) {
        case '+': for(int i=0; i<n; i++) {c[i] = a[i]+b[i];} break;
        case '-': for(int i=0; i<n; i++) {c[i] = a[i]-b[i];} break;
        case '*': for(int i=0; i<n; i++) {c[i] = a[i]*b[i];} break;
        case '/': for(int i=0; i<n; i++) {c[i] = a[i]/b[i];} break;
        case '^': for(int i=0; i<n; i++) {c[i] = pow(a[i],b[i]);}
    }
}
static void
vect_meta_vs(const double * restrict a, double b, int n, char op, 
        double * restrict c) 
{
    switch(op) {
        case '+': for(int i=0; i<n; i++) {c[i] = a[i]+b;} break;
        case '-': for(int i=0; i<n; i++) {c[i] = a[i]-b;} break;
        case '*': for(int i=0; i<n; i++) {c[i] = a[i]*b;} break;
        case '/': for(int i=0; i<n; i++) {c[i] = a[i]/b;} break;
        case '^': for(int i=0; i<n; i++) {c[i] = pow(a[i],b);}
    }
}
static void
vect_meta_sv(double a, const double * restrict b, int n, char op, 
        double * restrict c) 
{
    switch(op) {
        case '+': for(int i=0; i<n; i++) {c[i] = a+b[i];} break;
        case '-': for(int i=0; i<n; i++) {c[i] = a-b[i];} break;
        case '*': for(int i=0; i<n; i++) {c[i] = a*b[i];} break;
        case '/': for(int i=0; i<n; i++) {c[i] = a/b[i];} break;
        case '^': for(int i=0; i<n; i++) {c[i] = pow(a,b[i]);} 
    }
}

static int v_add_sub_mul_div_pow(lua_State *lua, char operator)
{
    double *vc=NULL; int m=0;
    if(lua_istable(lua, 1)) 
    {
        double *va = vect_getfarray(lua, &m, 1);
        vc = malloc(m*sizeof(double));
        if(lua_istable(lua, 2)) {
            int n=0;
            double *vb = vect_getfarray(lua, &n, 2);
            FAIL_ON((m!=n), "Error: operte on <vect> with differ length!");
            vect_meta_vv(va, vb, m, operator, vc);
            free(vb);
        } else {
            double b = luaL_checknumber(lua, 2);
            vect_meta_vs(va, b, m, operator, vc);
        }
        free(va);
    } else 
    {
        double a = luaL_checknumber(lua, 1);
        if(lua_istable(lua, 2)) {
            double *vb = vect_getfarray(lua, &m, 2);
            vc = malloc(m*sizeof(double));
            vect_meta_sv(a, vb, m, operator, vc);
            free(vb);
        } else { /* WTF? should never happen */ }
    }
    assert(vc!=NULL);
    vect_putfarray(lua, vc, m);
    free(vc);
    return 1;
}

static int v_add(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '+'); }
static int v_sub(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '-'); }
static int v_mul(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '*'); }
static int v_div(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '/'); }
static int v_pow(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '^'); }

/**
 * @brief Function v.new(n[, v]), initiate array of length n with value v
 * The second arguments is optional, there is several way to call it.
 * v.vector(n)    : create new vector of length n, initiate as 0
 * v.vector(n,v)  : create new vector of length n, initiate as v 
 * v.vector(tab)  : create new vector from a table with same length
 * v.vector(n,a,z): create new vector of length n, in range [a, z]
 * */
static int v_vector(lua_State *lua)
{
    int n, argc; double v, w, *a;
    if((argc=lua_gettop(lua))==2) 
    {
        n = luaL_checknumber(lua, 1);
        v = luaL_checknumber(lua, 2);
        a = malloc(n*sizeof(double));
        for(int i=0; i<n; i++)
            a[i] = v;
        vect_putfarray(lua, a, n);
        free(a);
    } else if (argc==3) {
        n = luaL_checknumber(lua, 1);
        v = luaL_checknumber(lua, 2);
        w = luaL_checknumber(lua, 3);
        a = malloc(n*sizeof(double));
        for(int i=0; i<n; i++)
            a[i] = v+((w-v)*i)/(n-1);
        vect_putfarray(lua, a, n);
        free(a);
    } 
    else if(lua_istable(lua, 1)) {
        lua_pushvalue(lua, -1);
        luaL_getmetatable(lua, KY_VECT_META_TABLE_V1);
        lua_setmetatable(lua, -2);
    } 
    else 
    {
        n = luaL_checknumber(lua, 1);
        a = malloc(n*sizeof(double));
        for(int i=0; i<n; i++)
            a[i] = 0;
        vect_putfarray(lua, a, n);
        free(a);
    }
    return 1;
}

static int v_isvector(lua_State *lua)
{
    int e = 0;
    if(lua_getmetatable(lua, 1)) {
        luaL_getmetatable(lua, KY_VECT_META_TABLE_V1);
        e = lua_rawequal(lua, -1, -2);
    }
    lua_pushboolean(lua, e);
    return 1;
}

void vect_ky_table_v1_init(lua_State *lua)
{
    static const luaL_Reg ky_vect_meta[] = {
        {"__add",      &v_add},
        {"__sub",      &v_sub},
        {"__mul",      &v_mul},
        {"__div",      &v_div},
        {"__pow",      &v_pow},
        {NULL,         NULL}
    };
    luaL_newmetatable(lua, KY_VECT_META_TABLE_V1);
    luaL_setfuncs(lua, ky_vect_meta, 0);

    static const luaL_Reg ky_vect_func[] = {
        {"add",         &v_add},
        {"sub",         &v_sub},
        {"mul",         &v_mul},
        {"div",         &v_div},
        {"pow",         &v_pow},
        {"vector",      &v_vector},
        {"isvector",    &v_isvector},
        {NULL,          NULL}
    };
    luaL_newlib(lua, ky_vect_func);
    lua_setglobal(lua, "v"); 
}

int main(int argc, char *argv[])
{
    lua_State * lua = luaL_newstate();
    luaL_openlibs(lua);

    vect_ky_table_v1_init(lua);

    if(argc>1) luaL_dofile(lua, argv[1]);

    lua_close(lua);
    return 0;
}
