#include "common.h"

static int have_jit;
static int debug_level;
static int64_t mem_use;
typedef struct { int n; double * p; } vector_t;

#define TYPE_VECTOR "vector_v4"
#define NAME_VECFFI "vecffi_v4"

#define MUSE(x) do { mem_use+=x; if(debug_level>1) printf("%s: mem=%ld\n", __func__, mem_use);} while(0)
#define FREE(x) do { mem_use-=x; if(debug_level>1) printf("%s: mem=%ld\n", __func__, mem_use);} while(0) 

/**
 * @brief Create a new vector as lua userdata with provided C array
 */
static void v_set_vector(lua_State *lua, double *a, int n)
{
    vector_t *obj;
    obj = lua_newuserdata(lua, sizeof(vector_t));
    obj->n = n;
    obj->p = a;
    luaL_getmetatable(lua, TYPE_VECTOR);
    lua_setmetatable(lua, -2);
}

/**
 * @brief Get the vector data, make sure it is vector before use!
 * */
static void * v_get_vector(lua_State *lua, int index, int *n)
{
    const vector_t *obj = lua_topointer(lua, index);
    *n = obj->n;
    return obj->p;
}

/**
 * @Brief return 1 for vector and 2 for matrix and 0 for None!
 */
static int vL_isvector(lua_State *lua, int index)
{
    int d=0;
    if(lua_isuserdata(lua, index)) 
        if(lua_getmetatable(lua, index)) {
            luaL_getmetatable(lua, TYPE_VECTOR);
            d=lua_rawequal(lua, -1, -2);
            lua_pop(lua, 2);
        }
    return d;
}

static int vL_dim(lua_State *lua, int index)
{
    int shape=0;
    if(lua_istable(lua, index)) {
        lua_rawgeti(lua, index, 1); //only check the first one
        shape = (vL_isvector(lua, -1))?(2):(1);
        lua_pop(lua, 1);
    } else {
        shape = (vL_isvector(lua, index))?(1):(0);
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
    double *vc=NULL; int n1=0, n2=0;
    if(lua_isnumber(lua, 1))
    {
        double a = lua_tonumber(lua, 1);
        double *vb = v_get_vector(lua, 2, &n1);
        vc = malloc(n1*sizeof(double));
        vector_op_sv(a, vb, n1, operator, vc);
    } 
    else 
    {
	    double *va = v_get_vector(lua, 1, &n1);
        vc = malloc(n1*sizeof(double));
        if(lua_isnumber(lua, 2)) {
            double b = lua_tonumber(lua, 2);
            vector_op_vs(va, b, n1, operator, vc);
        } else {
            double *vb = v_get_vector(lua, 2, &n2);
            FAIL_ON((n1!=n2), 
                "Error: operte on <vect> with differ length!");
            vector_op_vv(va, vb, n1, operator, vc);
        } 
    }
    MUSE(n1*sizeof(double));
    v_set_vector(lua, vc, n1);
    return 1;
}

static int v_add(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '+'); }
static int v_sub(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '-'); }
static int v_mul(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '*'); }
static int v_div(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '/'); }
static int v_pow(lua_State *lua) {return v_add_sub_mul_div_pow(lua, '^'); }

static int v_gc(lua_State *lua)
{
    const vector_t *obj = lua_topointer(lua, 1);
    FREE(obj->n*sizeof(double));
    free(obj->p);
    return 0;
}

static int v_len(lua_State *lua)
{
    const vector_t *b = lua_topointer(lua, 1);
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

/**
 * @brief add raw method!
 * */
static int v_index(lua_State *lua)
{
    const vector_t *b; int i;
    b = lua_topointer(lua, 1);
    i = (int)luaL_checknumber(lua, 2);
    assert(i<=b->n && i>=0);
    lua_pushnumber(lua, b->p[i]);
    return 1;
}

static int v_newindex(lua_State *lua)
{
    int index = (int)luaL_checknumber(lua,2);
    double value = luaL_checknumber(lua,3);
    const vector_t *obj = lua_topointer(lua, 1);
    double *p = obj->p; 
    if(index>obj->n || index<0) {
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
 * allocation 1 more element for index compatiblilty with LuaJit.ffi
 * */
static int v_vector(lua_State *lua)
{
    int n; double *a;
    if(lua_istable(lua, 1)) {
        n = (int)lua_rawlen(lua, 1);
        a = malloc((n+1)*sizeof(double));
        for(int k=0; k<n; k++) {
            lua_rawgeti(lua, 1, k+1);
            a[k+1] = lua_tonumber(lua, -1);
            lua_pop(lua, 1);
        }
    } else {
        n = luaL_checknumber(lua, 1);
        a = malloc((n+1)*sizeof(double));
        for(int i=0; i<n; i++)
            a[i+1] = 0;
    }
    MUSE((n+1)*sizeof(double));
    v_set_vector(lua, a, n);
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
            a = calloc(n+1, sizeof(double));
            MUSE((n+1)*sizeof(double));
            v_set_vector(lua, a, n);
            lua_rawseti(lua, -2, i+1);
        }
    }
    return 1;
}

/**
 * @brief handle=require("ffi"), TODO: handle without jit!
 * */
static void jit_ffi_init(lua_State *lua)
{
    lua_getglobal(lua, "jit");
    if(lua_istable(lua, -1)) {
        have_jit = 1;
        lua_getglobal(lua, "require");
        lua_pushstring(lua, "ffi");
        lua_call(lua, 1, 1);
        lua_setglobal(lua, NAME_VECFFI);
        printf("Initiate FFI\n"); fflush(0);
    } else {
        printf("No Jit, without FFI\n"); fflush(0);
    }
}

/**
 * @brief c wrap around the ffi.cast() luajit function
 * */
static int jit_ffi_cast(lua_State *lua, const char *type, void *pt)
{
    lua_getglobal(lua, NAME_VECFFI);    //ffi.
    lua_getfield(lua, -1, "cast");      //cast
    lua_pushstring(lua, type);          //double*
    lua_pushlightuserdata(lua, pt);     //pt
    lua_call(lua, 2, 1);
    lua_remove(lua, -2);                //remove ffi
    return 1;   //1 result left on stack
}

/**
 * @brief get the raw pointer(*) or (**) for vector or matrix
 * v.raw_internal() for use with luajit ffi
 * should be a lua wrapper to provide compatibility without luajit
 * */
static int v_raw_alias(lua_State *lua)
{
    int i, m, n, dim; double *p;
    dim = vL_dim(lua, 1);
    if(have_jit && dim!=0) {
        if(dim==1) {
            p = v_get_vector(lua, 1, &n);
            jit_ffi_cast(lua, "double*", p);
        } else {
            m = lua_rawlen(lua, 1);
            lua_createtable(lua, m, 0);
            for(i=0; i<m; i++) {
                lua_rawgeti(lua, 1, i+1);
                p = v_get_vector(lua, -1, &n);
                //printf("Matrix[%d] n=%d @%p top=%d\n", i, n, p, lua_gettop(lua));
                lua_pop(lua, 1);
                jit_ffi_cast(lua, "double*", p);
                lua_rawseti(lua, -2, i+1);
            }
        } //table of lightuser point
    } else 
        lua_pushvalue(lua, 1);
    return 1;
}

void vLib_v4_init(lua_State *lua)
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

    //TODO: need first test if LuaJit is available!
    jit_ffi_init(lua);
    vLib_v4_init(lua);

    if(argc>1) {
        int a = luaL_dofile(lua, argv[1]);
        assert(a==0);
    }

    lua_close(lua);
    return 0;
}
