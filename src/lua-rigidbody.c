#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include <SDL3/SDL.h>
#include "liv-physics.h"
#include "liv-log.h"
#include "luaclib.h"

// rigidbody 的 metatable 名称
#define RIGIDBODY_MT "rigidbody"

// 创建 rigidbody userdata
static int l_rigidbody_new(lua_State* L)
{
        // 从参数获取 rigidbody 指针
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        if (!body) {
                return luaL_error(L, "Invalid rigidbody pointer");
        }

        // 创建 userdata 并复制数据（或存储指针）
        rigidbody_p ud = (rigidbody_p)lua_newuserdata(L, sizeof(rigidbody_t));
        SDL_memcpy(ud, body, sizeof(rigidbody_t));

        // 设置 metatable
        luaL_getmetatable(L, RIGIDBODY_MT);
        lua_setmetatable(L, -2);

        return 1;
}

static int l_rigidbody_get_id(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushinteger(L, body->id);
        return 1;
}

static int l_rigidbody_get_is_static(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushboolean(L, body->is_static);
        return 1;
}

static int l_rigidbody_get_position(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushnumber(L, FPTF(body->position.x));
        lua_pushnumber(L, FPTF(body->position.y));
        return 2;
}

static int l_rigidbody_set_position(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        body->position.x = luaL_checkinteger(L, 2);
        body->position.y = luaL_checkinteger(L, 3);
        return 0;
}

static int l_rigidbody_get_linear_velocity(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushnumber(L, FPTF(body->linear_velocity.x));
        lua_pushnumber(L, FPTF(body->linear_velocity.y));
        return 2;
}

static int l_rigidbody_set_linear_velocity(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        body->linear_velocity.x = luaL_checkinteger(L, 2);
        body->linear_velocity.y = luaL_checkinteger(L, 3);
        return 0;
}

static int l_rigidbody_get_angular_velocity(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushnumber(L, FPTF(body->angular_velocity));
        return 1;
}

static int l_rigidbody_get_rotation(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushnumber(L, FPTF(body->rotation));
        return 1;
}

static int l_rigidbody_get_width(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushnumber(L, FPTF(body->width));
        return 1;
}

static int l_rigidbody_get_height(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushnumber(L, FPTF(body->height));
        return 1;
}

static int l_rigidbody_tostring(lua_State* L)
{
        rigidbody_p body = (rigidbody_p)lua_touserdata(L, 1);
        lua_pushfstring(L, "rigidbody[%d]: pos=(%f, %f), vel=(%f, %f), rot=%f, w=%f, h=%f",
                body->id, FPTF(body->position.x), FPTF(body->position.y),
                FPTF(body->linear_velocity.x), FPTF(body->linear_velocity.y), FPTF(body->rotation), FPTF(body->width), FPTF(body->height));
        return 1;
}

int luaopen_rigidbody(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"get_id", l_rigidbody_get_id},
                {"get_is_static", l_rigidbody_get_is_static},
                {"get_position", l_rigidbody_get_position},
                {"set_position", l_rigidbody_set_position},
                {"get_linear_velocity", l_rigidbody_get_linear_velocity},
                {"set_linear_velocity", l_rigidbody_set_linear_velocity},
                {"get_angular_velocity", l_rigidbody_get_angular_velocity},
                {"get_rotation", l_rigidbody_get_rotation},
                {"get_width", l_rigidbody_get_width},
                {"get_height", l_rigidbody_get_height},
                {"tostring", l_rigidbody_tostring},
                {NULL, NULL},
        };
        luaL_newlib(L, l);
        return 1;
}
