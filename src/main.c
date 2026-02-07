#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "joy.h"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        app_p app;
        const char* config_file;
        if (!SDL_Init(SDL_INIT_EVENTS)) {
                log_error("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        //config_file = argc > 1 ? argv[1] : "scripts/config4.lua";
        config_file = argc > 1 ? argv[1] : "scripts/server_config.lua";
        //config_file = argc > 1 ? argv[1] : "scripts/server_config.lua";
        app = joy_create(config_file);
        if (app) {
                *appstate = app;
                return SDL_APP_CONTINUE;
        }
        else {
                return SDL_APP_FAILURE;
        }
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        app_p app;
        app = (app_p)appstate;
        joy_event(app, event);
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        app_p app;
        app = (app_p)appstate;
        joy_update(app);
        if (joy_running(app)) {
                return SDL_APP_CONTINUE;
        }
        else {
                return SDL_APP_SUCCESS;
        }
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        app_p app;
        app = (app_p)appstate;
        if (result == SDL_APP_SUCCESS) {
                joy_destroy(app);
        }
        log_debug("good bye.");
}