#include "common.h"

/* ************************************************************
 *
 *
 * */

typedef struct {
    int             n;
    double          data[];
} ky_vector_t;

/**
 * @brief Create lua userdata (a pointer to ky_vector_t)
 * */
static void create_lua_vector(lua_State *lua, ky_vector_t *vect)
{
    ky_vector_t **pp = lua_newuserdata(lua, sizeof(void*));
    pp[0] = vect;
    luaL_getmetatable(lua, "ky_vector_v1");
    lua_setmetatable(lua, -2);
}

static ky_vector_t * calloc_raw_vector(int n)
{
    ky_vector_t *v;
    v = malloc(sizeof(ky_vector_t)+sizeof(double)*n);
    v->n = n;
    memset(v->data, 0, sizeof(double)*n);
    for(int i=0; i<n; i++)
        v->data[i] = i;
    return v;
}

static int v_vector(lua_State *lua)
{
    int n;
    ky_vector_t *v;
    n = luaL_checkinteger(lua, 1);
    v = calloc_raw_vector(n);
    create_lua_vector(lua, v);
    return 1;
}

ky_vector_t * vector_CheckVector(lua_State * lua, int n)
{
    return *(ky_vector_t **)luaL_checkudata(lua, n, "ky_vector_v1");
}

int vector_destroy(lua_State *lua)
{
    ky_vector_t *v = vector_CheckVector(lua, 1);
    free(v);
    return 0;
}

int vector_tostring(lua_State *lua)
{
    int i;
    ky_vector_t *v, **udata;
    udata = (ky_vector_t**)luaL_checkudata(lua, 1, "ky_vector_v1");
    v = *udata;
    if(v->n<5) {
        for(i=0; i<v->n; i++)
            printf("#>%3d %f\n", i, v->data[i]);
    }
    lua_pushfstring(lua, "#%s(%d)", __func__, v->n);
    return 1;
}

static int vector_add(lua_State *lua)
{
    ky_vector_t *v1, *v2, *v3;
    v1 = vector_CheckVector(lua, 1);
    v2 = vector_CheckVector(lua, 2); 
    v3 = calloc_raw_vector(v1->n); assert(v1->n==v2->n);
    for(int i=0; i<v1->n; i++) {
        v3->data[i] = v1->data[i]+v2->data[i];
        //printf("  #%2d %f\n", i, v1->data[i]+v2->data[i]);
    }
    //lua_pushnumber(lua, 100);
    create_lua_vector(lua, v3);
    return 1;
}

void vector_luaopen(lua_State *lua)
{
    luaL_Reg vector_func[] = {
        { "vector",     &v_vector},
        { NULL,         NULL }
    };
    luaL_Reg vector_meta[] = {
        { "__gc",       &vector_destroy },
        { "__add",      &vector_add },
        { "__tostring", &vector_tostring },
        { NULL, NULL }
    };
    //setup the meta table
    debug_top_info(lua, __func__, "before meta");
    luaL_newmetatable(lua, "ky_vector_v1");     //metatable = {}
    //lua_pushvalue(lua, -1);
    //lua_setfield(lua, -2, "__index");           //metatable.__index=metatable
    luaL_setfuncs(lua, vector_meta, 0);
    lua_pop(lua, 1);
    debug_top_info(lua, __func__, "after meta");

    //setup the methods
    luaL_newlib(lua, vector_func);              //setup the vector table
    lua_setglobal(lua, "v");
}

int main(int argc, char *argv[])
{
    lua_State * lua = luaL_newstate();

    luaL_openlibs(lua);
    vector_luaopen(lua);

    debug_top_info(lua, __func__, "b4 v0def");
    //lua_pushvalue(lua, 1);
    lua_pushnumber(lua, 1);
    lua_setglobal(lua, "_vector_v0_def");
    debug_top_info(lua, __func__, "af v0def");

    if(argc>1) luaL_dofile(lua, argv[1]);

    lua_close(lua);
    return 0;
}
