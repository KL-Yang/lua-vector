#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

#ifndef FAIL_ON
#define FAIL_ON(c,msg) do {if(c) {printf("%s", msg); abort();}} while(0);
#endif

#define KY_FLAG_MANAGED		(0x1<<1)

static int allocation_h;

typedef struct {
    int         n, flag;
    double    * p;
} ky_vector_t;

/**
 * Managed vector will be __gc by lua, and unmanged will not be __gc.
 */
#define KY_VECTOR_V1        	"ky_vector_v1"

/**
 * @brief Export a C pointer as our Lua light user data Vector
 */
void v_exp_vector(lua_State *lua, double *a, int n, int flag)
{
    ky_vector_t **c, *b;
    c = (ky_vector_t**)lua_newuserdata(lua, sizeof(void*));     allocation_h++;
    b = *c = calloc(1, sizeof(ky_vector_t));
    b->n = n;
    b->p = a;
    b->flag = flag;
    luaL_getmetatable(lua, KY_VECTOR_V1);
    lua_setmetatable(lua, -2);
}

/**
 * @brief Verify lua_islightuserdata before use
 * */
void * v_get_vector(lua_State *lua, int index, int *n)
{
    ky_vector_t **c, *b;
    c = (ky_vector_t**)lua_touserdata(lua, index);
    b = *c; *n = b->n;
    return b->p;
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

/**
 * @Brief return 1 for vector and 2 for matrix and 0 for None!
 */
static int vL_isvector(lua_State *lua, int index)
{
    int d=0;
    if(lua_isuserdata(lua, index)) 
        if(lua_getmetatable(lua, index)) {
            luaL_getmetatable(lua, KY_VECTOR_V1);
            d=lua_rawequal(lua, -1, -2);
            lua_pop(lua, 2);
        }
    return d;	//1 if index is a vector, 2 matrix
}

static int v_isvector(lua_State *lua)
{
    int e = vL_isvector(lua, 1);
    lua_pushboolean(lua, (e==1));
    return 1;
}

static int v_ismatrix(lua_State *lua)
{
    int e = vL_isvector(lua, 1);
    lua_pushboolean(lua, (e==2));
    return 1;
}

static int v_add_sub_mul_div_pow(lua_State *lua, char operator)
{
    double *vc=NULL; int n1=0, n2=0;
    if(lua_isnumber(lua, 1))
    {
        double a = lua_tonumber(lua, 1);
        double *vb = v_get_vector(lua, 2, &n1);
        vc = malloc(n1*sizeof(double));
        vect_meta_sv(a, vb, n1, operator, vc);
    } 
    else 
    {
	    double *va = v_get_vector(lua, 1, &n1);
        vc = malloc(n1*sizeof(double));
        if(lua_isnumber(lua, 2)) {
            double b = lua_tonumber(lua, 2);
            vect_meta_vs(va, b, n1, operator, vc);
        } else {
            double *vb = v_get_vector(lua, 2, &n2);
            FAIL_ON((n1!=n2), 
                "Error: operte on <vect> with differ length!");
            vect_meta_vv(va, vb, n1, operator, vc);
        } 
    }

    assert(vc!=NULL);
    v_exp_vector(lua, vc, n1, KY_FLAG_MANAGED);
    return 1;
}

static int v_add(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '+'); }
static int v_sub(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '-'); }
static int v_mul(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '*'); }
static int v_div(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '/'); }
static int v_pow(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '^'); }

static int v_gc(lua_State *lua)
{
    int argc = lua_gettop(lua);
    ky_vector_t **c, *b;
    c = (ky_vector_t**)lua_touserdata(lua, 1); b = *c;  
    if(b->flag & KY_FLAG_MANAGED)
        free(b->p);
    free(b); allocation_h--;
    return 0;
}

static int v_len(lua_State *lua)
{
    int n=0;
    ky_vector_t **c, *b;
    c = (ky_vector_t**)lua_touserdata(lua, 1);
    b = *c; n = b->n;
    lua_pushnumber(lua, n);
    return 1;
}

static int v_report(lua_State *lua)
{
    printf("memory allocated %d\n", allocation_h);
    return 0;
}

static int v_index(lua_State *lua)
{
    int index = (int)luaL_checknumber(lua, 2)-1;
    ky_vector_t **c, *b;
    c = (ky_vector_t**)lua_touserdata(lua, 1);
    b = *c; double *p = b->p; 
    assert(index<b->n && index>=0);
    lua_pushnumber(lua, p[index]);
    return 1;
}

static int v_newindex(lua_State *lua)
{
    int index = (int)luaL_checknumber(lua,2)-1;
    double value = luaL_checknumber(lua,3);
    ky_vector_t **c, *b;
    c = (ky_vector_t**)lua_touserdata(lua, 1);
    b = *c; double *p = b->p; 
    assert(index<b->n && index>=0);
    p[index] = value;
    return 0;
}

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
    } else if (argc==3) {
        n = luaL_checknumber(lua, 1);
        v = luaL_checknumber(lua, 2);
        w = luaL_checknumber(lua, 3);
        a = malloc(n*sizeof(double));
        for(int i=0; i<n; i++)
            a[i] = v+((w-v)*i)/(n-1);
    } 
    else if(lua_istable(lua, 1)) {
        n = (int)lua_rawlen(lua, 1);
        a = malloc(n*sizeof(double));
        for(int k=0; k<n; k++) {
            lua_rawgeti(lua, 1, k+1);
            a[k] = lua_tonumber(lua, -1);
            lua_pop(lua, 1);
        }
    } 
    else 
    {
        n = luaL_checknumber(lua, 1);
        a = malloc(n*sizeof(double));
        for(int i=0; i<n; i++)
            a[i] = 0;
    }
    v_exp_vector(lua, a, n, KY_FLAG_MANAGED);
    return 1;
}

void vLib_v1_init(lua_State *lua)
{
    static const luaL_Reg ky_vect_meta[] = {
        {"__gc",       &v_gc},
        {"__add",      &v_add},
        {"__sub",      &v_sub},
        {"__mul",      &v_mul},
        {"__div",      &v_div},
        {"__pow",      &v_pow},
        {"__len",      &v_len},
        {"__index",    &v_index},
        {"__newindex", &v_newindex},
        {NULL,         NULL}
    };
    luaL_newmetatable(lua, KY_VECTOR_V1);
    luaL_setfuncs(lua, ky_vect_meta, 0);

    static const luaL_Reg ky_vect_func[] = {
        {"vector",      &v_vector},
        {"isvector",    &v_isvector},
        {"report",      &v_report},
        {NULL,          NULL}
    };
    luaL_newlib(lua, ky_vect_func);
    lua_setglobal(lua, "v"); 
}

void mLib_v1_init(lua_State *lua)
{
}

int main(int argc, char *argv[])
{
    lua_State * lua = luaL_newstate();
    luaL_openlibs(lua);

    vLib_v1_init(lua);

    if(argc>1) luaL_dofile(lua, argv[1]);

    lua_close(lua);
    return 0;
}
