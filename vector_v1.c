#include "common.h"

#define TYPE_VECTOR  "vector_v1"

/**
 * @brief put the native C double array on top of lua stack
 * */
static void v_set_vector(lua_State *lua, const double * restrict a, int n)
{
    lua_createtable(lua, n, 0);
    for(int k=0; k<n; k++) {
        lua_pushnumber(lua, a[k]);
        lua_rawseti(lua, -2, k+1);
    }
    luaL_getmetatable(lua, TYPE_VECTOR);
    lua_setmetatable(lua, -2);
}

/**
 * @brief convert table in lua stack[i] into native C array
 * */
static double * v_get_vector(lua_State *lua, int * restrict n, int index)
{
    int m; double *a;
#ifdef USE_JIT
    m = *n = (int)lua_rawlen(lua, index);
#else
    m = *n = (int)lua_rawlen(lua, index);
#endif
    a = malloc(m*sizeof(double));
    for(int k=0; k<m; k++) {
        lua_rawgeti(lua, index, k+1);
        a[k] = lua_tonumber(lua, -1);
        lua_pop(lua, 1);
    }
    return a;
}

/**
 * @Brief return 1 for vector and 2 for matrix and 0 for None!
 * */
static int vL_isvector(lua_State *lua, int index) 
{
    int d=0;
    if(lua_istable(lua, index)) 
        if(lua_getmetatable(lua, 1)) {
            luaL_getmetatable(lua, TYPE_VECTOR);
            d = lua_rawequal(lua, -1, -2);
            lua_pop(lua, 2);
        }
    return d;
}

static int vL_dim(lua_State *lua, int index)
{
    int shape=0;
    if(lua_istable(lua, index)) {
        lua_rawgeti(lua, index, 1);
        shape = (vL_isvector(lua, -1))?(2):(1);
        lua_pop(lua, 1);
    } else {
        shape=(vL_isvector(lua, index))?(1):(0);
    }
    return shape;
}

static int v_isvector(lua_State *lua)
{
    int shape = vL_dim(lua, 1);
    lua_pushnumber(lua, shape);
    return 1;
}

static int v_add_sub_mul_div_pow(lua_State *lua, char operator)
{
    double *vc=NULL; int m=0;
    if(lua_istable(lua, 1)) 
    {
        double *va = v_get_vector(lua, &m, 1);
        vc = malloc(m*sizeof(double));
        if(lua_istable(lua, 2)) {
            int n=0;
            double *vb = v_get_vector(lua, &n, 2);
            FAIL_ON((m!=n), "Error: operte on <vect> with differ length!");
            vector_op_vv(va, vb, m, operator, vc);
            free(vb);
        } else {
            double b = luaL_checknumber(lua, 2);
            vector_op_vs(va, b, m, operator, vc);
        }
        free(va);
    } else 
    {
        double a = luaL_checknumber(lua, 1);
        if(lua_istable(lua, 2)) {
            double *vb = v_get_vector(lua, &m, 2);
            vc = malloc(m*sizeof(double));
            vector_op_sv(a, vb, m, operator, vc);
            free(vb);
        } else { /* WTF? should never happen */ }
    }
    assert(vc!=NULL);
    v_set_vector(lua, vc, m);
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
 * v.vector(n)    : create new vector of length n, initiate as 0
 * v.vector(tab)  : create new vector from a table with same length
 * */
static int v_vector(lua_State *lua)
{
    int n; double *a;
    if(lua_istable(lua, 1)) {
        lua_pushvalue(lua, -1);
        luaL_getmetatable(lua, TYPE_VECTOR);
        lua_setmetatable(lua, -2);
    } else {
        n = luaL_checknumber(lua, 1);
        a = calloc(n, sizeof(double));
        v_set_vector(lua, a, n);
        free(a);
    }
    return 1;
}

/**
 * @brief Function v.matrix(m, n), n is the least significant dim
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
            v_set_vector(lua, a, n);
            free(a);
            lua_rawseti(lua, -2, i+1);
        }
    }
    return 1;
}

/**
 * dummy function, do nothing
 * */
static int v_memdebug(lua_State __attribute__((unused)) *lua)
{
    return 0;
}

/**
 * dummy, vector is implement as table, raw alias is itself!
 * */
static int v_raw_alias(lua_State *lua)
{
    lua_pushvalue(lua, 1);
    return 1;
}

void vLib_v1_init(lua_State *lua)
{
    static const luaL_Reg ky_vect_meta[] = {
        {"__add",      &v_add},
        {"__sub",      &v_sub},
        {"__mul",      &v_mul},
        {"__div",      &v_div},
        {"__pow",      &v_pow},
        {NULL,         NULL}
    };
    static const luaL_Reg ky_vect_func[] = {
        {"vector",      &v_vector},
        {"matrix",      &v_matrix},
        {"dim",         &v_isvector},
        {"raw",         &v_raw_alias},
        {"debug",       &v_memdebug},
        {NULL,          NULL}
    };

    luaL_newmetatable(lua, TYPE_VECTOR);
#ifdef USE_JIT
    luaL_register(lua, NULL, ky_vect_meta);
    luaL_register(lua, "v", ky_vect_func);
#else
    luaL_setfuncs(lua, ky_vect_meta, 0);
    luaL_newlib(lua, ky_vect_func);
    lua_setglobal(lua, "v"); 
#endif
}

int main(int argc, char *argv[])
{
    lua_State * lua = luaL_newstate();
    luaL_openlibs(lua);

    vLib_v1_init(lua);

    if(argc>1) {
        int a = luaL_dofile(lua, argv[1]);
        assert(a==0);
    }

    lua_close(lua);
    return 0;
}
