#include <assert.h>
#include "jsclib.h"
#include "../jconfig.h"
#include <SDL3/SDL.h>

#define TIMER_TAG "timer"

typedef struct js_timer {
        uint64_t last_time;
        int interval;
        bool active;
} js_timer_t;

static void timer_finalize(js_State *J, void *p)
{
        (void)J; SDL_free(p);
}

static void jtimer_create(js_State *J)
{
        int interval = (int)js_tonumber(J, 1);
        js_timer_t *t = (js_timer_t*)SDL_malloc(sizeof(js_timer_t));
        assert(t);
        t->last_time = SDL_GetTicks();
        t->interval = interval;
        t->active = true;
        js_newuserdata(J, TIMER_TAG, t, timer_finalize);
}

static void jtimer_set_interval(js_State *J)
{
        js_timer_t *t = js_touserdata(J, 1, TIMER_TAG);
        t->interval = (int)js_tonumber(J, 2);
}

static void jtimer_set_active(js_State *J)
{
        js_timer_t *t = js_touserdata(J, 1, TIMER_TAG);
        t->active = js_toboolean(J, 2);
        if (t->active) t->last_time = SDL_GetTicks();
}

static void jtimer_reset(js_State *J)
{
        js_timer_t *t = js_touserdata(J, 1, TIMER_TAG);
        t->last_time = SDL_GetTicks();
}

static void jtimer_get_elapsed(js_State *J)
{
        js_timer_t *t = js_touserdata(J, 1, TIMER_TAG);
        js_pushnumber(J, (double)(SDL_GetTicks() - t->last_time));
}

static void jtimer_trigger(js_State *J)
{
        js_timer_t *t = js_touserdata(J, 1, TIMER_TAG);
        if (!t || !t->active) { js_pushboolean(J, 0); return; }
        uint64_t now = SDL_GetTicks();
        if (now - t->last_time >= (uint64_t)t->interval) {
                js_pushnumber(J, (double)(now - t->last_time));
                js_pushnumber(J, t->interval);
                t->last_time += t->interval;
                while (now - t->last_time >= (uint64_t)t->interval)
                        t->last_time += t->interval;
        } else {
                js_pushundefined(J);
        }
}

void jsopen_timer(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jtimer_create, "create", 1);            js_setproperty(J, -2, "create");
        js_newcfunction(J, jtimer_set_interval, "setInterval", 2); js_setproperty(J, -2, "setInterval");
        js_newcfunction(J, jtimer_set_active, "setActive", 2);     js_setproperty(J, -2, "setActive");
        js_newcfunction(J, jtimer_reset, "reset", 1);              js_setproperty(J, -2, "reset");
        js_newcfunction(J, jtimer_get_elapsed, "getElapsed", 1);   js_setproperty(J, -2, "getElapsed");
        js_newcfunction(J, jtimer_trigger, "trigger", 1);          js_setproperty(J, -2, "trigger");
        js_setglobal(J, "timer");
}
