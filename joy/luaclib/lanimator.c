#include <SDL3/SDL.h>
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../joy.h"
#include "luaclib.h"

static int l_janimator_create(lua_State* L)
{
        jr2d_p r2d = (jr2d_p)lua_touserdata(L, 1);
        if (!r2d) { lua_pushnil(L); return 1; }
        janimator_p anim = janimator_create(r2d);
        lua_pushlightuserdata(L, anim);
        return 1;
}

static int l_janimator_destroy(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        janimator_destroy(anim);
        return 0;
}

static int l_janimator_set_float(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_set_float(anim, luaL_checkstring(L, 2), (float)luaL_checknumber(L, 3));
        return 0;
}

static int l_janimator_set_int(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_set_int(anim, luaL_checkstring(L, 2), (int)luaL_checkinteger(L, 3));
        return 0;
}

static int l_janimator_set_bool(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_set_bool(anim, luaL_checkstring(L, 2), lua_toboolean(L, 3));
        return 0;
}

static int l_janimator_set_trigger(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_set_trigger(anim, luaL_checkstring(L, 2));
        return 0;
}

static int l_janimator_get_float(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        lua_pushnumber(L, anim ? janimator_get_float(anim, luaL_checkstring(L, 2)) : 0);
        return 1;
}

static int l_janimator_get_int(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        lua_pushinteger(L, anim ? janimator_get_int(anim, luaL_checkstring(L, 2)) : 0);
        return 1;
}

static int l_janimator_get_bool(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        lua_pushboolean(L, anim ? janimator_get_bool(anim, luaL_checkstring(L, 2)) : 0);
        return 1;
}

static int l_janimator_add_state(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_add_state(anim, luaL_checkstring(L, 2));
        return 0;
}

static int l_janimator_set_default_state(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_set_default_state(anim, luaL_checkstring(L, 2));
        return 0;
}

/* read rect from arg[i]: supports table {x,y,w,h} or 4 numbers x,y,w,h */
static jrect_t read_rect(lua_State* L, int i)
{
        jrect_t r = { 0,0,0,0 };
        if (lua_istable(L, i)) {
                lua_getfield(L, i, "x"); r.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, i, "y"); r.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, i, "w"); r.w = (float)lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, i, "h"); r.h = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        }
        else {
                r.x = (float)luaL_checknumber(L, i);
                r.y = (float)luaL_checknumber(L, i + 1);
                r.w = (float)luaL_checknumber(L, i + 2);
                r.h = (float)luaL_checknumber(L, i + 3);
        }
        return r;
}

static int l_janimator_state_add_clip(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (!anim) return 0;
        const char* state_name = luaL_checkstring(L, 2);
        const char* image_path = luaL_checkstring(L, 3);
        float duration = (float)luaL_checknumber(L, 4);
        jrect_t src = read_rect(L, 5);
        janimator_state_add_clip(anim, state_name, image_path, duration, src);
        return 0;
}

static int l_janimator_state_add_color(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (!anim) return 0;
        const char* state_name = luaL_checkstring(L, 2);
        float duration = (float)luaL_checknumber(L, 3);
        int w = (int)luaL_checkinteger(L, 4);
        int h = (int)luaL_checkinteger(L, 5);
        int r = (int)luaL_checkinteger(L, 6);
        int g = (int)luaL_checkinteger(L, 7);
        int b = (int)luaL_checkinteger(L, 8);
        int a = (int)luaL_checkinteger(L, 9);
        janimator_state_add_color(anim, state_name, duration, w, h, r, g, b, a);
        return 0;
}

static int l_janimator_state_set_loop(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_state_set_loop(anim, luaL_checkstring(L, 2), lua_toboolean(L, 3));
        return 0;
}

static int l_janimator_state_set_speed(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_state_set_speed(anim, luaL_checkstring(L, 2), (float)luaL_checknumber(L, 3));
        return 0;
}

static int l_janimator_state_set_blend(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_state_set_blend(anim, luaL_checkstring(L, 2), luaL_checkstring(L, 3));
        return 0;
}

static int l_janimator_state_add_motion(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (!anim) return 0;
        const char* state_name = luaL_checkstring(L, 2);
        const char* image_path = luaL_checkstring(L, 3);
        float duration = (float)luaL_checknumber(L, 4);
        int rect_arg = 5;
        jrect_t src;
        if (lua_istable(L, 5)) {
                src = read_rect(L, 5);
                rect_arg = 6;
        }
        else {
                src.x = (float)luaL_checknumber(L, 5);
                src.y = (float)luaL_checknumber(L, 6);
                src.w = (float)luaL_checknumber(L, 7);
                src.h = (float)luaL_checknumber(L, 8);
                rect_arg = 9;
        }
        float threshold = (float)luaL_checknumber(L, rect_arg);
        janimator_state_add_motion(anim, state_name, image_path, duration, src, threshold);
        return 0;
}

static int l_janimator_add_transition(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (!anim) { lua_pushinteger(L, -1); return 1; }
        int idx = janimator_add_transition(anim, luaL_checkstring(L, 2), luaL_checkstring(L, 3));
        lua_pushinteger(L, idx);
        return 1;
}

static int l_janimator_add_any_transition(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (!anim) { lua_pushinteger(L, -1); return 1; }
        int idx = janimator_add_any_transition(anim, luaL_checkstring(L, 2));
        lua_pushinteger(L, idx);
        return 1;
}

static int l_janimator_transition_add_cond(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) {
                int trans_idx = (int)luaL_checkinteger(L, 2);
                const char* param_name = luaL_checkstring(L, 3);
                int mode = (int)luaL_checkinteger(L, 4);
                float threshold = (float)luaL_checknumber(L, 5);
                janimator_transition_add_cond(anim, trans_idx, param_name, mode, threshold);
        }
        return 0;
}

static int l_janimator_set_exit_time(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_transition_set_exit_time(anim, (int)luaL_checkinteger(L, 2), (float)luaL_checknumber(L, 3));
        return 0;
}

static int l_janimator_set_flip_h(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_set_flip_h(anim, lua_toboolean(L, 2));
        return 0;
}

static int l_janimator_update(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_update(anim, (float)luaL_checknumber(L, 2));
        return 0;
}

static int l_janimator_draw(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        if (anim) janimator_draw(anim, (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
        return 0;
}

static int l_janimator_current_state(lua_State* L)
{
        janimator_p anim = (janimator_p)lua_touserdata(L, 1);
        lua_pushstring(L, anim ? janimator_current_state(anim) : "");
        return 1;
}

int luaopen_janimator(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create", l_janimator_create},
            {"destroy", l_janimator_destroy},
            {"set_float", l_janimator_set_float},
            {"set_int", l_janimator_set_int},
            {"set_bool", l_janimator_set_bool},
            {"set_trigger", l_janimator_set_trigger},
            {"get_float", l_janimator_get_float},
            {"get_int", l_janimator_get_int},
            {"get_bool", l_janimator_get_bool},
            {"add_state", l_janimator_add_state},
            {"set_default_state", l_janimator_set_default_state},
            {"state_add_clip", l_janimator_state_add_clip},
            {"state_add_color", l_janimator_state_add_color},
            {"state_set_loop", l_janimator_state_set_loop},
            {"state_set_speed", l_janimator_state_set_speed},
            {"state_set_blend", l_janimator_state_set_blend},
            {"state_add_motion", l_janimator_state_add_motion},
            {"add_transition", l_janimator_add_transition},
            {"add_any_transition", l_janimator_add_any_transition},
            {"transition_add_cond", l_janimator_transition_add_cond},
            {"transition_set_exit_time", l_janimator_set_exit_time},
            {"update", l_janimator_update},
            {"draw", l_janimator_draw},
            {"set_flip_h", l_janimator_set_flip_h},
            {"current_state", l_janimator_current_state},
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
