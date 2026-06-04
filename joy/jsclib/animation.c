#include <assert.h>
#include "jsclib.h"
#include "../janimations.h"

#define ANIM_TAG "sprite_animation"

static void anim_finalize(js_State *J, void *data)
{
        (void)J;
        sprite_animation_p anim = (sprite_animation_p)data;
        if (anim) sprite_animation_destroy(anim);
}

static sprite_animation_p check_anim(js_State *J, int idx)
{
        return (sprite_animation_p)js_touserdata(J, idx, ANIM_TAG);
}

static void janim_create(js_State *J)
{
        SDL_Renderer *ren = (SDL_Renderer*)js_touserdata(J, 1, "SDL_Renderer*");
        if (!ren) { js_error(J, "animation.create: invalid renderer"); return; }
        sprite_animation_p anim = sprite_animation_create(ren);
        js_newuserdata(J, ANIM_TAG, anim, anim_finalize);
}

static void janim_destroy(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        if (anim) { sprite_animation_destroy(anim); anim = NULL; }
        js_pushnull(J);
}

static void janim_reset(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        if (anim) sprite_animation_reset(anim);
}

static void janim_addclip(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        if (!anim) return;
        const char *path = js_tostring(J, 2);
        float duration = js_tonumber(J, 3);
        SDL_FRect src;
        src.x = js_tonumber(J, 4);
        src.y = js_tonumber(J, 5);
        src.w = js_tonumber(J, 6);
        src.h = js_tonumber(J, 7);
        sprite_animation_add_clip(anim, path, duration, src);
}

static void janim_is_finished(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        js_pushboolean(J, anim && sprite_animation_is_finished(anim));
}

static void janim_set_position(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        if (anim) sprite_animation_set_position(anim, js_tonumber(J, 2), js_tonumber(J, 3));
}

static void janim_set_scale(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        if (anim) sprite_animation_set_scale(anim, js_tonumber(J, 2), js_tonumber(J, 3));
}

static void janim_set_rotation(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        if (anim) sprite_animation_set_rotation(anim, js_tonumber(J, 2));
}

static void janim_update(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        if (anim) sprite_animation_update(anim, js_tonumber(J, 2));
}

static void janim_draw(js_State *J)
{
        sprite_animation_p anim = check_anim(J, 1);
        if (!anim) return;
        SDL_FRect camera;
        camera.x = js_tonumber(J, 2);
        camera.y = js_tonumber(J, 3);
        camera.w = js_tonumber(J, 4);
        camera.h = js_tonumber(J, 5);
        sprite_animation_draw(anim, &camera);
}

void jsopen_animation(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, janim_create, "create", 1);          js_setproperty(J, -2, "create");
        js_newcfunction(J, janim_destroy, "destroy", 1);        js_setproperty(J, -2, "destroy");
        js_newcfunction(J, janim_reset, "reset", 1);            js_setproperty(J, -2, "reset");
        js_newcfunction(J, janim_addclip, "addclip", 7);        js_setproperty(J, -2, "addclip");
        js_newcfunction(J, janim_is_finished, "is_finished", 1); js_setproperty(J, -2, "is_finished");
        js_newcfunction(J, janim_set_position, "set_position", 3); js_setproperty(J, -2, "set_position");
        js_newcfunction(J, janim_set_scale, "set_scale", 3);    js_setproperty(J, -2, "set_scale");
        js_newcfunction(J, janim_set_rotation, "set_rotation", 2); js_setproperty(J, -2, "set_rotation");
        js_newcfunction(J, janim_update, "update", 2);          js_setproperty(J, -2, "update");
        js_newcfunction(J, janim_draw, "draw", 5);              js_setproperty(J, -2, "draw");
        js_setglobal(J, "animation");
}
