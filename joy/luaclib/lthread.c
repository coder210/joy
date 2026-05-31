#include "luaclib.h"
#include <SDL3/SDL.h>
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jcore.h"
#include "../jluax.h"

/* =================================================================
 * Thread
 * ================================================================= */
#define THREAD_MT "Thread*"

typedef struct {
    SDL_Thread*    th;
    luax_p         worker;
    char*          error;
    int            done;
    SDL_AtomicInt  quit_requested;
    SDL_Mutex*     mutex;
} thread_obj_t;

static int thread_worker(void* ud) {
    thread_obj_t* to = (thread_obj_t*)ud;
    lua_State* L = luax_get_state(to->worker);

    /* 进入主循环：类似主线程，持续调用 joy.update() + 热更检查 */
    while (!SDL_GetAtomicInt(&to->quit_requested)) {
        luax_call_joy(to->worker, "update", 1, 1.0f / 60.0f);
        luax_poll_reload(to->worker);
        SDL_Delay(1);
    }

    /* 退出时调用 joy.quit() */
    luax_call_joy(to->worker, "quit", 0, 0);

    SDL_LockMutex(to->mutex); to->done = 1; SDL_UnlockMutex(to->mutex);
    luax_release(to->worker);
    return 0;
}

static int th_release(lua_State* L) {
    thread_obj_t* to = (thread_obj_t*)luaL_checkudata(L, 1, THREAD_MT);
    if (to->mutex) {
        SDL_DestroyMutex(to->mutex);
        to->mutex = NULL;
    }
    if (to->error) {
        SDL_free(to->error);
        to->error = NULL;
    }
    /* to 本身是 lua_newuserdata 分配的，Lua 自己管理 */
    return 0;
}

static int th_start(lua_State* L) {
    thread_obj_t* to = (thread_obj_t*)luaL_checkudata(L, 1, THREAD_MT);
    if (to->th) return luaL_error(L, "thread already started");

    to->th = SDL_CreateThread(thread_worker, "lua_thread", to);
    if (!to->th) {
        log_error("thread start failed: %s", SDL_GetError());
        lua_pushboolean(L, false); return 1;
    }
    SDL_DetachThread(to->th);
    lua_pushboolean(L, true); return 1;
}

static int th_getError(lua_State* L) {
    thread_obj_t* to = (thread_obj_t*)luaL_checkudata(L, 1, THREAD_MT);
    SDL_LockMutex(to->mutex);
    if (to->error) lua_pushstring(L, to->error); else lua_pushnil(L);
    SDL_UnlockMutex(to->mutex); return 1;
}

static int th_quit(lua_State* L) {
    thread_obj_t* to = (thread_obj_t*)luaL_checkudata(L, 1, THREAD_MT);
    SDL_SetAtomicInt(&to->quit_requested, 1);
    return 0;
}

static int th_isRunning(lua_State* L) {
    thread_obj_t* to = (thread_obj_t*)luaL_checkudata(L, 1, THREAD_MT);
    SDL_LockMutex(to->mutex); int done = to->done; SDL_UnlockMutex(to->mutex);
    lua_pushboolean(L, !done); return 1;
}

static const luaL_Reg th_methods[] = {
    {"start", th_start},
    {"quit", th_quit},
    {"release", th_release},
    {"getError", th_getError},
    {"isRunning", th_isRunning},
    {NULL, NULL}
};

/* =================================================================
 * 模块入口
 * ================================================================= */
static int l_newThread(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);

    /* 为 worker 创建独立的 luax 上下文 */
    luax_p w = luax_create();
    if (!w) return luaL_error(L, "luax_create failed");

    /* 用 luax_reload 加载 worker 文件（保留 state，支持热更新） */
    luax_init_state(w);
    if (!luax_reload(w, filename)) {
        luax_release(w);
        lua_pushnil(L);
        lua_pushfstring(L, "failed to load %s", filename);
        return 2;
    }

    /* 设置文件监控（worker 也可以热更新） */
    luax_set_watch(w, filename);
    /* 初始加载完成后调用 joy.load() */
    luax_call_joy(w, "load", 0, 0);

    thread_obj_t* to = (thread_obj_t*)lua_newuserdata(L, sizeof(thread_obj_t));
    SDL_zero(*to);
    to->worker = w;
    to->mutex  = SDL_CreateMutex();
    luaL_getmetatable(L, THREAD_MT); lua_setmetatable(L, -2);
    return 1;
}

int luaopen_thread(lua_State* L) {
    luaL_newmetatable(L, THREAD_MT);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, th_methods, 0);
    /* 不设置 __gc：子线程生命周期由主线程在 joy.quit() 中显式管理 */
    lua_pop(L, 1);

    luaL_checkversion(L);
    luaL_Reg l[] = {{"newThread", l_newThread}, {NULL, NULL}};
    luaL_newlib(L, l);
    return 1;
}
