#include <stdbool.h>
#include <SDL3/SDL.h>
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jconfig.h"
#include "../janimations.h"
#include "luaclib.h"

static int l_sprite_animation_create(lua_State* L)
{
        SDL_Renderer* renderer;
        sprite_animation_p anim;
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        anim = sprite_animation_create(renderer);
        lua_pushlightuserdata(L, anim);
        return 1;
}

static int l_sprite_animation_destroy(lua_State* L)
{
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
	sprite_animation_destroy(anim);
        return 0;
}

static int l_sprite_animation_reset(lua_State* L)
{
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
        sprite_animation_reset(anim);
        return 0;
}

static int l_sprite_animation_addclip(lua_State* L)
{
        const char* image_path;
        float duration;
        SDL_FRect src_rect;
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
        image_path = luaL_checkstring(L, 2);
        duration = luaL_checknumber(L, 3);

        src_rect.x = luaL_checknumber(L, 4);
        src_rect.y = luaL_checknumber(L, 5);
        src_rect.w = luaL_checknumber(L, 6);
        src_rect.h = luaL_checknumber(L, 7);

        sprite_animation_add_clip(anim, image_path, duration, src_rect);
        return 0;
}

static int l_sprite_animation_is_finished(lua_State* L)
{
        bool result;
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
        result = sprite_animation_is_finished(anim);
        lua_pushboolean(L, result);
        return 1;
}

static int l_sprite_animation_set_position(lua_State* L)
{
        float x, y;
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
        x = (float)luaL_checknumber(L, 2);
        y = (float)luaL_checknumber(L, 3);
        sprite_animation_set_position(anim, x, y);
        return 0;
}

static int l_sprite_animation_set_scale(lua_State* L)
{
        float x, y;
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
        x = (float)luaL_checknumber(L, 2);
        y = (float)luaL_checknumber(L, 3);
        sprite_animation_set_scale(anim, x, y);
        return 0;
}

static int l_sprite_animation_set_rotation(lua_State* L)
{
        float rotation;
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
        rotation = (float)luaL_checknumber(L, 2);
        sprite_animation_set_rotation(anim, rotation);
        return 0;
}

static int l_sprite_animation_update(lua_State* L)
{
        float dt;
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
        dt = (float)luaL_checknumber(L, 2);
        sprite_animation_update(anim, dt);
        return 0;
}

static int l_sprite_animation_draw(lua_State* L)
{
        SDL_FRect camera;
        sprite_animation_p anim;
        anim = lua_touserdata(L, 1);
        camera.x = (float)luaL_checknumber(L, 2);
        camera.y = (float)luaL_checknumber(L, 3);
        camera.w = (float)luaL_checknumber(L, 4);
        camera.h = (float)luaL_checknumber(L, 5);
        sprite_animation_draw(anim, &camera);
        return 0;
}


int luaopen_animation(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create", l_sprite_animation_create},
            {"destroy", l_sprite_animation_destroy},
            {"reset", l_sprite_animation_reset},
            {"addclip", l_sprite_animation_addclip},
            {"is_finished", l_sprite_animation_is_finished},
            {"set_position", l_sprite_animation_set_position},
            {"set_scale", l_sprite_animation_set_scale},
            {"set_rotation", l_sprite_animation_set_rotation},
            {"update", l_sprite_animation_update},
            {"draw", l_sprite_animation_draw},
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}