#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua5.3/lua.h>
#include <lua5.3/lauxlib.h>
#include <lua5.3/lualib.h>

typedef struct {
    int             n;
    double          data[];
} ky_vector_t;

static int vector_new(lua_State *lua)
{
    int i, n;
    ky_vector_t *mat, **udata;

    n = luaL_checkinteger(lua, 1);
    mat = malloc(sizeof(ky_vector_t)+sizeof(double)*n);
    mat->n = n;
    memset(mat->data, 0, sizeof(double)*n);
    for(i=0; i<n; i++)
        mat->data[i] = i;

    udata = (ky_vector_t**)lua_newuserdata(lua, sizeof(ky_vector_t*));
    *udata = mat;

    luaL_getmetatable(lua, "ky_vector_v1");
    lua_setmetatable(lua, -2);
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
    for(i=0; i<v->n; i++)
        printf(" >%3d %f\n", i, v->data[i]);
    lua_pushfstring(lua, "(%f)", v->n);
    return 1;
}

static int vector_add(lua_State *lua)
{
    ky_vector_t *v1, *v2, **udata;
    v1 = vector_CheckVector(lua, 1);
    v2 = vector_CheckVector(lua, 2);
    int i;
    for(i=0; i<v1->n; i++) {
        printf("  #%2d %f\n", i, v1->data[i]+v2->data[i]);
    }
    lua_pushnumber(lua, 100);
    return 1;
}

int vector_luaopen(lua_State *lua)
{
    luaL_Reg vector_func[] =
    {
        { "new",    &vector_new },
        { NULL, NULL }
    };

    luaL_Reg vector_meta[] =
    {
        { "__gc",       &vector_destroy },
        { "__add",      &vector_add },
        { "__tostring", &vector_tostring },
        { NULL, NULL }
    };

    //setup the meta table
    luaL_newmetatable(lua, "ky_vector_v1"); //metatable = {}
    lua_pushvalue(lua, -1);
    lua_setfield(lua, -2, "__index");         //metatable.__index=metatable
    luaL_setfuncs(lua, vector_meta, 0);

    //setup the methods
    luaL_newlib(lua, vector_func);    //setup the vector table
    lua_setglobal(lua, "vector");
    return 1;
}

int main(int argc, char *argv[])
{
    lua_State * lua = luaL_newstate();
    luaL_openlibs(lua);

    vector_luaopen(lua);

    if(argc>1) luaL_dofile(lua, argv[1]);

    lua_close(lua);
    return 0;
}
