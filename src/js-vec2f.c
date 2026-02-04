#include "liv-math.h"
#include "jsclib.h"
#include "SDL3/SDL.h"

static void jvec2f_negate(js_State *J)
{
        vec2f_t a, r;

        SDL_assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_negate(a);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2f_dot(js_State *J)
{
        vec2f_t a = { 0 }, b = { 0 };
        fp_t r = { 0 };

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_dot(a, b);

        js_pushint64(J, r);
}


static void jvec2f_add(js_State *J)
{
        vec2f_t a, b, r = { 0 };

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_toint64(J, -1);
        js_pop(J, 1);
       
        r = vec2f_add(a, b);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}


static void jvec2f_sub(js_State *J)
{
        vec2f_t a, b, r;

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_sub(a, b);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2f_scale(js_State *J)
{
        vec2f_t a, r;
        fp_t s;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        s = js_toint64(J, 2);
        
        r = vec2f_scale(a, s);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2f_absx(js_State *J)
{
        vec2f_t a, r;
        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_abs(a);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2f_length_squared(js_State *J)
{
        vec2f_t a;
        fp_t r;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_length_squared(a);
        js_pushint64(J, r);
}

static void jvec2f_length(js_State *J)
{
        vec2f_t a;
        fp_t r;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_length(a);

        js_pushint64(J, r);
}

static void jvec2f_normalize(js_State *J)
{
        vec2f_t a, r;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_normalize(a);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2f_normal(js_State *J)
{
        vec2f_t a, r;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_normal(a);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2f_rotate(js_State *J)
{
        vec2f_t a, r;
        fp_t angle;

        SDL_assert(js_isobject(J, 1));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        angle = js_toint64(J, 2);

        r = vec2f_rotate(a, angle);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2f_lerp(js_State *J)
{
        vec2f_t a, b, r;
        fp_t t;

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_toint64(J, -1);
        js_pop(J, 1);

        t = js_toint64(J, 3);

        r = vec2f_lerp(a, b, t);

        js_newobject(J);
        js_pushint64(J, r.x);
        js_setproperty(J, -2, "x");
        js_pushint64(J, r.y);
        js_setproperty(J, -2, "y");
}

static void jvec2f_distance(js_State *J)
{
        vec2f_t a, b;
        fp_t r;

        SDL_assert(js_isobject(J, 1));
        SDL_assert(js_isobject(J, 2));

        js_getproperty(J, 1, "x");
        a.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 1, "y");
        a.y = js_toint64(J, -1);
        js_pop(J, 1);

        js_getproperty(J, 2, "x");
        b.x = js_toint64(J, -1);
        js_pop(J, 1);
        js_getproperty(J, 2, "y");
        b.y = js_toint64(J, -1);
        js_pop(J, 1);

        r = vec2f_distance(a, b);
        js_pushint64(J, r);
}

void jsopen_vec2f(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jvec2f_absx, "absx", 1);
        js_setproperty(J, -2, "absx");
        js_newcfunction(J, jvec2f_add, "add", 1);
        js_setproperty(J, -2, "add");
        js_newcfunction(J, jvec2f_distance, "distance", 1);
        js_setproperty(J, -2, "distance");
        js_newcfunction(J, jvec2f_dot, "dot", 1);
        js_setproperty(J, -2, "dot");
        js_newcfunction(J, jvec2f_length, "length", 1);
        js_setproperty(J, -2, "length");
        js_newcfunction(J, jvec2f_length_squared, "length_squared", 1);
        js_setproperty(J, -2, "length_squared");
        js_newcfunction(J, jvec2f_lerp, "lerp", 1);
        js_setproperty(J, -2, "lerp");
        js_newcfunction(J, jvec2f_negate, "negate", 1);
        js_setproperty(J, -2, "negate");
        js_newcfunction(J, jvec2f_normal, "noraml", 1);
        js_setproperty(J, -2, "noraml");
        js_newcfunction(J, jvec2f_normalize, "noramlize", 1);
        js_setproperty(J, -2, "noramlize");
        js_newcfunction(J, jvec2f_rotate, "rotate", 1);
        js_setproperty(J, -2, "rotate");
        js_newcfunction(J, jvec2f_scale, "scale", 1);
        js_setproperty(J, -2, "scale");
        js_newcfunction(J, jvec2f_sub, "sub", 1);
        js_setproperty(J, -2, "sub");
        js_setglobal(J, "vec2f");
}
