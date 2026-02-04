#include "luaclib.h"
#include "liv-physics.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "liv-log.h"

static int l_world2df_create(lua_State* L)
{
        world2df_p world;
        world = world2df_create();
        lua_pushlightuserdata(L, world);
        return 1;
}

static int l_world2df_destroy(lua_State* L)
{
        world2df_p world;
        world = lua_touserdata(L, 1);
        world2df_destroy(world);
        return 0;
}

static int l_world2df_checksum(lua_State* L)
{
        //char md5_str[64] = { 0 };
        char md5_str[LIVNET_MAX_BUFFER] = { 0 };
        int len;
        world2df_p world;
        world = lua_touserdata(L, 1);
        len = world2df_checksum(world, md5_str);
        lua_pushlstring(L, md5_str, len);
        return 1;
}

static int l_world2df_serialize(lua_State* L)
{
        char output[2048*2];
        int len;
        world2df_p world;
        world = lua_touserdata(L, 1);
        len = world2df_serialize(world, output);
        lua_pushlstring(L, output, len);
        return 1;
}

static int l_world2df_deserialize(lua_State* L)
{
        const char *input;
        size_t len;
        world2df_p world;
        world = lua_touserdata(L, 1);
        input = luaL_checklstring(L, 2, &len);
        world = world2df_create();
        world2df_deserialize(world, input, len);
        lua_pushlightuserdata(L, world);
        return 1;
}

static int l_world2df_create_rigidbody(lua_State* L)
{
        rigidbody_p body;
        world2df_p world;
        fp_t width, height;
        world = lua_touserdata(L, 1);
        width = luaL_checkinteger(L, 2);
        height = luaL_checkinteger(L, 3);
        /*{
                char output[LIVNET_MAX_BUFFER] = { 0 };
                int len = world2df_checksum(world, output);
                output[len] = 0;
                liv_log_debug("output:%s\n", output);
        }*/

        body = world2df_create_rigidbody(world, width, height);
       /* {
                char output[LIVNET_MAX_BUFFER] = { 0 };
                int len = world2df_checksum(world, output);
                output[len] = 0;
                liv_log_debug("output:%s\n", output);
        }*/
        //lua_pushinteger(L, body->id);
        lua_pushlightuserdata(L, body);
        return 1;
}

static int l_world2df_destroy_rigidbody(lua_State* L)
{
        world2df_p world;
        uint32_t entity_id;
        world = lua_touserdata(L, 1);
        entity_id = luaL_checkinteger(L, 2);
        world2df_destroy_rigidbody(world, entity_id);
        return 0;
}

static int l_world2df_get_rigidbody(lua_State* L)
{
        world2df_p world;
        int conv;
        rigidbody_p body;
        world = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        body = world2df_get_rigidbody(world, conv);
        if (body) {
                lua_pushlightuserdata(L, body);
        }
        else {
                lua_pushnil(L);
        }
        return 1;
}

static int l_world2df_move_rigidbody(lua_State* L)
{
        world2df_p world;
        uint32_t id;
        short keycode;
        world = lua_touserdata(L, 1);
        id = luaL_checkinteger(L, 2);
        keycode = (short)luaL_checkinteger(L, 3);
        world2df_move_rigidbody(world, id, keycode);
        return 0;
}

static int l_world2df_get_entity_id(lua_State* L)
{
        world2df_p world;
        uint32_t entity_id;
        bool ret;
        int conv;
        world = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        ret = world2df_get_entity_id(world, conv, &entity_id);
        lua_pushboolean(L, ret);
        lua_pushinteger(L, entity_id);
        return 2;
}

static int l_world2df_connections_count(lua_State* L)
{
        int num;
        world2df_p world = (world2df_p)lua_touserdata(L, 1);
        num = world2df_connections_count(world);
        lua_pushinteger(L, num);
        return 1;
}

static int l_world2df_add_connection(lua_State* L)
{
        world2df_p world;
        uint32_t entity_id;
        int conv;
        world = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        entity_id = luaL_checkinteger(L, 3);
        world2df_add_connection(world, conv, entity_id);
        return 0;
}

static int l_world2df_add_emeny(lua_State* L)
{
        world2df_p world;
        uint32_t entity_id;
        world = lua_touserdata(L, 1);
        entity_id = luaL_checkinteger(L, 2);
        world2df_add_emeny(world, entity_id);
        return 0;
}

static int l_world2df_remove_connection(lua_State* L)
{
        world2df_p world;
        int conv, ret;
        world = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        ret = world2df_remove_connection(world, conv);
        lua_pushinteger(L, ret);
        return 1;
}

static int l_world2df_update_connection(lua_State* L)
{
        world2df_p world;
        uint32_t entity_id;
        int conv;
        world = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        entity_id = luaL_checkinteger(L, 3);
        world2df_update_connection(world, conv, entity_id);
        return 0;
}

static int l_world2df_collide(lua_State* L)
{
        world2df_p world;
        uint32_t a_id, b_id;
        int result;
        contact2df_t contact;

        world = lua_touserdata(L, 1);
        a_id = luaL_checkinteger(L, 2);
        b_id = luaL_checkinteger(L, 3);
        result = world2df_collide(world, a_id, b_id, &contact);

        lua_pushinteger(L, result);
        if (result) {
                lua_newtable(L);

                lua_newtable(L);
                lua_pushinteger(L, contact.sp.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.sp.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "sp");

                lua_newtable(L);
                lua_pushinteger(L, contact.ep.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.ep.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "ep");

                lua_newtable(L);
                lua_pushinteger(L, contact.normal.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.normal.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "normal");

                lua_pushinteger(L, contact.depth);
                lua_setfield(L, -2, "depth");
        }
        else {
                lua_pushnil(L);
        }

        return 2;
}

static void _foreach_body(rigidbody_p body, void* userdata)
{
        lua_State* L = (lua_State*)userdata;
        int callback_ref = lua_tointeger(L, -1);
        lua_rawgeti(L, LUA_REGISTRYINDEX, callback_ref);
        lua_pushlightuserdata(L, body);

        if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                liv_log_error("Error in body callback: %s", lua_tostring(L, -1));
                lua_pop(L, 1); // 弹出错误信息
        }
}

static int l_world2df_foreach_body(lua_State* L)
{
        world2df_p world = (world2df_p)lua_touserdata(L, 1);
        luaL_checktype(L, 2, LUA_TFUNCTION);

        // 创建唯一的引用
        lua_pushvalue(L, 2); // 复制函数到栈顶
        int callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

        // 将引用作为userdata传递给回调
        lua_pushinteger(L, callback_ref);

        world2df_foreach_body(world, _foreach_body, L);

        // 清理引用
        luaL_unref(L, LUA_REGISTRYINDEX, callback_ref);

        return 0;
}


static void _foreach_connection(int conv, uint32_t entity_id, void* userdata)
{
        lua_State* L = (lua_State*)userdata;
        int callback_ref = lua_tointeger(L, -1);
        lua_rawgeti(L, LUA_REGISTRYINDEX, callback_ref);
        lua_pushinteger(L, conv);
        lua_pushinteger(L, entity_id);
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
                liv_log_error("Error in body callback: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
        }
}

static int l_world2df_foreach_connection(lua_State* L)
{
        world2df_p world = (world2df_p)lua_touserdata(L, 1);
        luaL_checktype(L, 2, LUA_TFUNCTION);
        lua_pushvalue(L, 2);
        int callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_pushinteger(L, callback_ref);
        world2df_foreach_connection(world, _foreach_connection, L);
        luaL_unref(L, LUA_REGISTRYINDEX, callback_ref);
        return 0;
}

static int l_world2df_enemies_count(lua_State* L)
{
        int num;
        world2df_p world = (world2df_p)lua_touserdata(L, 1);
        num = world2df_enemies_count(world);
        lua_pushinteger(L, num);
        return 1;
}

static int l_world2df_update_emeny(lua_State* L)
{
        vec2f_t map_size = { 0 };
        world2df_p world = (world2df_p)lua_touserdata(L, 1);
        map_size.x = luaL_checkinteger(L, 2);
        map_size.y = luaL_checkinteger(L, 3);
        world2df_update_emeny(world, map_size);
        return 0;
}

int luaopen_world2df(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create", l_world2df_create},
            {"destroy", l_world2df_destroy},
            {"checksum", l_world2df_checksum},
            {"serialize", l_world2df_serialize},
            {"deserialize", l_world2df_deserialize},
            {"create_rigidbody", l_world2df_create_rigidbody},
            {"destroy_rigidbody", l_world2df_destroy_rigidbody},
            {"get_rigidbody", l_world2df_get_rigidbody},
            {"move_rigidbody", l_world2df_move_rigidbody},
            {"get_entity_id", l_world2df_get_entity_id},
            {"connections_count", l_world2df_connections_count},
            {"add_connection", l_world2df_add_connection},
            {"remove_connection", l_world2df_remove_connection},
            {"update_connection", l_world2df_update_connection},
            {"collide", l_world2df_collide},
            {"foreach_body", l_world2df_foreach_body},
            {"foreach_connection", l_world2df_foreach_connection},
            {"enemies_count", l_world2df_enemies_count},
            {"add_emeny", l_world2df_add_emeny},
            {"update_emeny", l_world2df_update_emeny},
            {NULL, NULL},
        };
        luaL_newlib(L, l);
        return 1;
}
