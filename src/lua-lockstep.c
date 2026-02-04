#include "liv-lockstep.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"

static int l_locksetp_create(lua_State* L)
{
        lockstep_p lockstep;
        lockstep = lockstep_create();
        lua_pushlightuserdata(L, lockstep);
        return 1;
}

static int l_locksetp_destroy(lua_State* L)
{
        lockstep_p lockstep;
        lockstep = (lockstep_p)lua_touserdata(L, 1);
        lockstep_destroy(lockstep);
        return 0;
}

static int l_locksetp_add_command(lua_State* L)
{
        lockstep_p lockstep;
        int frameid;
        const char* cmd;
        size_t cmd_size;
        lockstep = (lockstep_p)lua_touserdata(L, 1);
        frameid = (int)luaL_checkinteger(L, 2);
        cmd = luaL_checklstring(L, 3, &cmd_size);
        lockstep_add_command(lockstep, frameid, cmd, cmd_size);
        return 0;
}


static int l_locksetp_add_world(lua_State* L)
{
        lockstep_p lockstep;
        int frameid;
        const char* world;
        size_t world_size;
        lockstep = (lockstep_p)lua_touserdata(L, 1);
        frameid = (int)luaL_checkinteger(L, 2);
        world = luaL_checklstring(L, 3, &world_size);
        lockstep_add_world(lockstep, frameid, world, world_size);
        return 0;
}

int luaopen_lockstep(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                    {"create", l_locksetp_create},
                    {"destroy", l_locksetp_destroy},
                    {"add_command", l_locksetp_add_command},
                    {"add_world", l_locksetp_add_world},
                    {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
