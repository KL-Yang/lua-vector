#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

#ifndef FAIL_ON
#define FAIL_ON(c,msg)  do {if(c) {printf("%s", msg); abort();}} while(0);
#endif

static int debug_level;
static int64_t mem_use;

#define MUSE(x) do { mem_use+=x; if(debug_level>1) printf("%s: mem=%ld\n", __func__, mem_use);} while(0)
#define FREE(x) do { mem_use-=x; if(debug_level>1) printf("%s: mem=%ld\n", __func__, mem_use);} while(0) 

#define VECTOR_NUMBER       0
#define VECTOR_OBJECT       1

typedef struct ky_vector_t ky_vector_t;

struct ky_vector_t {
    int           n, flag;
    double        * p;
};

/**
 * Managed vector will be __gc by lua, and unmanged will not be __gc.
 */
#define KY_VECTOR_V1        	"ky_vector_v1"

/**
 * @brief Create a new vector as lua userdata with provided C array
 */
void v_new_vector(lua_State *lua, double *a, int n, int flag)
{
    ky_vector_t *obj;
    obj = lua_newuserdata(lua, sizeof(ky_vector_t));
    obj->n = n;
    obj->p = a;
    obj->flag = flag;
    luaL_getmetatable(lua, KY_VECTOR_V1);
    lua_setmetatable(lua, -2);
}

/**
 * @brief Get the vector data, make sure it is vector before use!
 * */
void * v_get_vector(lua_State *lua, int index, int *n)
{
    const ky_vector_t *obj = lua_topointer(lua, index);
    *n = obj->n;
    return obj->p;
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
    MUSE(n1*sizeof(double));
    v_new_vector(lua, vc, n1, 0);
    return 1;
}

static int v_add(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '+'); }
static int v_sub(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '-'); }
static int v_mul(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '*'); }
static int v_div(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '/'); }
static int v_pow(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '^'); }

static int v_gc(lua_State *lua)
{
    const ky_vector_t *obj = lua_topointer(lua, 1);
    FREE(obj->n*sizeof(double));
    free(obj->p);
    return 0;
}

static int v_len(lua_State *lua)
{
    const ky_vector_t *b = lua_topointer(lua, 1);
    int n = b->n;
    lua_pushnumber(lua, n);
    return 1;
}

/**
 * @brief set or toggle if report the memory allocation tracking
 * @param level : verbose level of memory debuger
 * */
static int v_memdebug(lua_State *lua)
{
    debug_level = luaL_checknumber(lua, 1);
    printf("MEM_DEBUG: memory use = %ld Bytes\n", mem_use);
    return 0;
}

static int v_index(lua_State *lua)
{
    int index = (int)luaL_checknumber(lua, 2)-1;
    const ky_vector_t *b = lua_topointer(lua, 1);
    double *p = b->p; assert(index<b->n && index>=0);
    lua_pushnumber(lua, p[index]);
    return 1;
}

static int v_newindex(lua_State *lua)
{
    int index = (int)luaL_checknumber(lua,2)-1;
    double value = luaL_checknumber(lua,3);
    const ky_vector_t *obj = lua_topointer(lua, 1);
    double *p = obj->p; 
    if(index>=obj->n || index<0) {
        printf("Access vector out of boundary!\n"
                "This does not allow to change length after vector creation!\n");
        abort();
    }
    p[index] = value;
    return 0;
}

/**
 * @brief Function v.vector(n), initiate array of length n with value v
 * v.vector(n)    : create new vector of length n, initiate as 0
 * v.vector(tab)  : create new vector from a table with same length
 * */
static int v_vector(lua_State *lua)
{
    int n, argc; double v, w, *a;
    if(lua_istable(lua, 1)) {
        n = (int)lua_rawlen(lua, 1);
        a = malloc(n*sizeof(double));
        for(int k=0; k<n; k++) {
            lua_rawgeti(lua, 1, k+1);
            a[k] = lua_tonumber(lua, -1);
            lua_pop(lua, 1);
        }
    } else {
        n = luaL_checknumber(lua, 1);
        a = malloc(n*sizeof(double));
        for(int i=0; i<n; i++)
            a[i] = 0;
    }
    MUSE(n*sizeof(double));
    v_new_vector(lua, a, n, 0);
    return 1;
}

/**
 * @brief Function v.matrix(m, n), n is the least significant dim
 * Matrix is implement as table of vectors, this is lazy
 * */
static int v_matrix(lua_State *lua)
{
    int m, n, argc; double *a;
    if((argc=lua_gettop(lua))==1)
        v_vector(lua);      // only 1 parameter
    else 
    {
        m = luaL_checknumber(lua, 1);
        n = luaL_checknumber(lua, 2);
        lua_createtable(lua, m, 0);
        for(int i=0; i<m; i++) {
            a = calloc(n, sizeof(double));
            MUSE(n*sizeof(double));
            v_new_vector(lua, a, n, 0);
            lua_rawseti(lua, -2, i+1);
        }
    }
    return 1;
}

void vLib_v1_init(lua_State *lua)
{
    static const luaL_Reg ky_vect_meta[] = {
        {"__gc",       &v_gc},
        {"__len",      &v_len},
        {"__index",    &v_index},
        {"__newindex", &v_newindex},
        {"__add",      &v_add},
        {"__sub",      &v_sub},
        {"__mul",      &v_mul},
        {"__div",      &v_div},
        {"__pow",      &v_pow},
        {NULL,         NULL}
    };
    luaL_newmetatable(lua, KY_VECTOR_V1);
    luaL_setfuncs(lua, ky_vect_meta, 0);

    static const luaL_Reg ky_vect_func[] = {
        {"vector",      &v_vector},
        {"matrix",      &v_matrix},
        {"isvector",    &v_isvector},
        {"debug",       &v_memdebug},
        {NULL,          NULL}
    };
    luaL_newlib(lua, ky_vect_func);
    lua_setglobal(lua, "v"); 
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
