#include "liv-math.h"
#include "jsclib.h"
#include "SDL3/SDL.h"

static void jvec2_negate(js_State *J)
{
        vec2_t a, r;

        SDL_assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_negate(a);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2_dot(js_State *J)
{
        vec2_t a = { 0 }, b = { 0 };
        ft_t r = { 0 };

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_dot(a, b);

        js_pushnumber(J, r);
}


static void jvec2_add(js_State *J)
{
        vec2_t a, b, r = { 0 };

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_tonumber(J, -1);
        js_pop(J, 1);
       
        r = vec2_add(a, b);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}


static void jvec2_sub(js_State *J)
{
        vec2_t a, b, r;

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_sub(a, b);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2_scale(js_State *J)
{
        vec2_t a, r;
        ft_t s;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        s = js_tonumber(J, 2);
        
        r = vec2_scale(a, s);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2_absx(js_State *J)
{
        vec2_t a, r;
        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_abs(a);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2_length_squared(js_State *J)
{
        vec2_t a;
        ft_t r;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_length_squared(a);
        js_pushnumber(J, r);
}

static void jvec2_length(js_State *J)
{
        vec2_t a;
        ft_t r;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_length(a);

        js_pushnumber(J, r);
}

static void jvec2_normalize(js_State *J)
{
        vec2_t a, r;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_normalize(a);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2_normal(js_State *J)
{
        vec2_t a, r;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_normal(a);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2_rotate(js_State *J)
{
        vec2_t a, r;
        ft_t angle;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        angle = js_tonumber(J, 2);

        r = vec2_rotate(a, angle);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2_lerp(js_State *J)
{
        vec2_t a, b, r;
        ft_t t;

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_tonumber(J, -1);
        js_pop(J, 1);

        t = js_tonumber(J, 3);

        r = vec2_lerp(a, b, t);

        js_newobject(J);
        js_pushnumber(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2_distance(js_State *J)
{
        vec2_t a, b;
        ft_t r;

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_tonumber(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_tonumber(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_tonumber(J, -1);
        js_pop(J, 1);

        r = vec2_distance(a, b);
        js_pushnumber(J, r);
}

void jsopen_vec2(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jvec2_absx, "absx", 1);
        js_setproperty(J, -2, "absx");
        js_newcfunction(J, jvec2_add, "add", 1);
        js_setproperty(J, -2, "add");
        js_newcfunction(J, jvec2_distance, "distance", 1);
        js_setproperty(J, -2, "distance");
        js_newcfunction(J, jvec2_dot, "dot", 1);
        js_setproperty(J, -2, "dot");
        js_newcfunction(J, jvec2_length, "length", 1);
        js_setproperty(J, -2, "length");
        js_newcfunction(J, jvec2_length_squared, "length_squared", 1);
        js_setproperty(J, -2, "length_squared");
        js_newcfunction(J, jvec2_lerp, "lerp", 1);
        js_setproperty(J, -2, "lerp");
        js_newcfunction(J, jvec2_negate, "negate", 1);
        js_setproperty(J, -2, "negate");
        js_newcfunction(J, jvec2_normal, "noraml", 1);
        js_setproperty(J, -2, "noraml");
        js_newcfunction(J, jvec2_normalize, "noramlize", 1);
        js_setproperty(J, -2, "noramlize");
        js_newcfunction(J, jvec2_rotate, "rotate", 1);
        js_setproperty(J, -2, "rotate");
        js_newcfunction(J, jvec2_scale, "scale", 1);
        js_setproperty(J, -2, "scale");
        js_newcfunction(J, jvec2_sub, "sub", 1);
        js_setproperty(J, -2, "sub");
        js_setglobal(J, "vec2f");
}
