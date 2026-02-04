#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "SDL3/SDL.h"
#include "liv-ecs.h"
#include "luaclib.h"

typedef struct lua_ecs_data {
        lua_State* L;
        int func_ref;
}lua_ecs_data_t, * lua_ecs_data_p;

static int lecs_sizeofloat(lua_State* L)
{
        lua_pushinteger(L, sizeof(float));
        return 1;
}

static int lecs_sizeofinteger(lua_State* L)
{
        lua_pushinteger(L, sizeof(int));
        return 1;
}

static int lecs_sizeofshort(lua_State* L)
{
        lua_pushinteger(L, sizeof(short));
        return 1;
}

static int lecs_create(lua_State* L)
{
        ecs_world_p world;
        world = liv_ecs_create();
        lua_pushlightuserdata(L, world);
        return 1;
}

static int lecs_destroy(lua_State* L)
{
        ecs_world_p world;
        world = (ecs_world_p)lua_touserdata(L, 1);
        liv_ecs_destroy(world);
        return 0;
}

static int lecs_process(lua_State* L)
{
        float dt;
        ecs_world_p world;
        world = (ecs_world_p)lua_touserdata(L, 1);
        dt = (float)luaL_checknumber(L, 2);
        liv_ecs_process(world, dt);
        return 0;
}

static int lecs_spawn(lua_State* L)
{
        ecs_id_t id;
        ecs_world_p world;
        world = (ecs_world_p)lua_touserdata(L, 1);
        id = liv_ecs_spawn(world);
        lua_pushinteger(L, id);
        return 1;
}

static int lecs_kill(lua_State* L)
{
        ecs_id_t id;
        ecs_world_p world;
        world = (ecs_world_p)lua_touserdata(L, 1);
        id = (ecs_id_t)luaL_checkinteger(L, 2);
        liv_ecs_kill(world, id);
        return 1;
}

static int lecs_define(lua_State* L)
{
        const char* name;
        ecs_world_p world;
        size_t size;
        world = (ecs_world_p)lua_touserdata(L, 1);
        name = luaL_checkstring(L, 2);
        size = (size_t)luaL_checkinteger(L, 3);
        liv_ecs_define(world, name, size);
        return 0;
}

static int lecs_set(lua_State* L)
{
        ecs_id_t id;
        size_t len;
        const char* name, *ptr;
        ecs_world_p world;
        world = (ecs_world_p)lua_touserdata(L, 1);
        id = (ecs_id_t)luaL_checkinteger(L, 2);
        name = luaL_checkstring(L, 3);
        ptr = luaL_checklstring(L, 4, &len);
        liv_ecs_set(world, id, name, ptr, len);
        return 0;
}

static int lecs_get(lua_State* L)
{
        ecs_id_t id;
        void* ptr;
        size_t size;
        const char* name;
        ecs_world_p world;
        world = (ecs_world_p)lua_touserdata(L, 1);
        id = (ecs_id_t)luaL_checkinteger(L, 2);
        name = luaL_checkstring(L, 3);
        size = liv_ecs_get(world, id, name, &ptr);
        if (size <= 0) {
                lua_pushnil(L);
        }
        else {
		lua_pushlstring(L, (const char*)ptr, size);
        }
        return 1;
}

static int lecs_remove(lua_State* L)
{
        ecs_id_t id;
        const char* name;
        ecs_world_p world;
        world = (ecs_world_p)lua_touserdata(L, 1);
        id = (ecs_id_t)luaL_checkinteger(L, 2);
        name = luaL_checkstring(L, 3);
        liv_ecs_remove(world, id, name);
        return 0;
}

static int lecs_query(lua_State* L)
{
        liv_vector_t(kecs) id_arr;
        ecs_world_p world;
        int args_count;
        char** component_names;
        int components_count;

        components_count = 0;
        world = (ecs_world_p)lua_touserdata(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        args_count = luaL_len(L, 2);
        component_names = SDL_malloc(sizeof(char*) * args_count);
        for (int i = 1; i <= args_count; i++) {
                lua_rawgeti(L, 2, i); 
                component_names[i - 1] = luaL_checkstring(L, -1);
                components_count++;
                lua_pop(L, 1);
        }
        id_arr = liv_ecs_queryx(world, components_count, component_names);
        args_count = liv_vector_size(kecs, &id_arr);
        if (args_count > 1) {
                int kkk = 0;
        }
        lua_createtable(L, args_count, 0);
        for (int i = 0; i < args_count; i++) {
                lua_pushinteger(L, liv_vector_a(kecs, &id_arr, i));
                lua_rawseti(L, -2, i + 1); 
        }
        liv_vector_destroy(kecs, &id_arr);
        SDL_free(component_names);
        return 1;
}

static void _system_cb(ecs_world_p world, void *arg, float dt)
{
        lua_State* L;
        lua_ecs_data_p data;
        data = (lua_ecs_data_p)arg;
        L = data->L;
        if (data->func_ref == LUA_NOREF) 
                return;

        lua_rawgeti(L, LUA_REGISTRYINDEX, data->func_ref);
        lua_pushlightuserdata(L, world);
        lua_pushnumber(L, dt);
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
                SDL_Log("Error in system: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
        }
}

static int lecs_register(lua_State* L)
{
        ecs_world_p world;
        lua_ecs_data_p data;
        ecs_system_t system;
        world = (ecs_world_p)lua_touserdata(L, 1);
        data = (lua_ecs_data_p)SDL_malloc(sizeof(lua_ecs_data_t));
        data->func_ref = luaL_ref(L, LUA_REGISTRYINDEX);
        data->L = L;
        system.cb = _system_cb;
        system.arg = data;
        liv_ecs_register(world, system);
        return 0;
}

int luaopen_ecs(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                    {"sizeoffloat", lecs_sizeofloat},
                    {"sizeofinteger", lecs_sizeofinteger},
                    {"sizeofshort", lecs_sizeofshort},
                    {"create", lecs_create},
                    {"destroy", lecs_destroy},
                    {"process", lecs_process},
                    {"spawn", lecs_spawn},
                    {"kill", lecs_kill},
                    {"define", lecs_define},
                    {"set", lecs_set},
                    {"get", lecs_get},
                    {"remove", lecs_remove},
                    {"query", lecs_query},
                    {"register", lecs_register},
                    {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
