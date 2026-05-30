#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy/jluax.h>

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

typedef struct {
    char* file_path;
    SDL_Time last_mtime;
    luax_p lua;
    Uint64 last_poll;
    Uint64 prev_ticks;
} app_state_t;

static bool load_script(app_state_t* s) {
    SDL_Log("--- loading: %s ---", s->file_path);
    bool ok = luax_reload(s->lua, s->file_path);
    SDL_Log("--- %s ---", ok ? "loaded OK" : "load FAILED");
    return ok;
}

static bool check_reload(app_state_t* s) {
    SDL_PathInfo info;
    if (!SDL_GetPathInfo(s->file_path, &info))
        return false;
    if (info.modify_time == s->last_mtime)
        return false;
    s->last_mtime = info.modify_time;
    SDL_Log("[CHANGED] %s", s->file_path);
    return load_script(s);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    app_state_t* s = (app_state_t*)SDL_calloc(1, sizeof(app_state_t));
    *appstate = s;
    s->file_path = SDL_strdup(argc > 1 ? argv[1] : "test.lua");

    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)))
            SDL_Log("cwd: %s", cwd);
    }

    s->lua = luax_create();
    if (!s->lua) {
        SDL_Log("luax_create() failed");
        return SDL_APP_FAILURE;
    }
    luax_init_state(s->lua);

    SDL_PathInfo info;
    if (SDL_GetPathInfo(s->file_path, &info))
        s->last_mtime = info.modify_time;

    load_script(s);
    luax_call_joy(s->lua, "load", 0, 0);  // 仅启动时调用一次

    s->last_poll = SDL_GetTicks();
    s->prev_ticks = SDL_GetTicks();
    SDL_Log("joy_beat - watching: %s", s->file_path);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    app_state_t* s = (app_state_t*)appstate;

    Uint64 now = SDL_GetTicks();
    float dt = (now - s->prev_ticks) / 1000.0f;
    s->prev_ticks = now;

    luax_call_joy(s->lua, "update", 1, dt);
    luax_call_joy(s->lua, "draw", 0, 0);

    if (now - s->last_poll >= 500) {
        s->last_poll = now;
        check_reload(s);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    (void)appstate;
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    app_state_t* s = (app_state_t*)appstate;
    if (s) {
        if (s->lua) luax_release(s->lua);
        SDL_free(s->file_path);
        SDL_free(s);
    }
    (void)result;
}
