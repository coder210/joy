#define SDL_MAIN_USE_CALLBACKS 1
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "./external/lua/lua.h"
#include "./external/lua/lualib.h"
#include "./external/lua/lauxlib.h"
#include "liv.h"
#include "liv-sys.h"
#include "liv-config.h"
#include "liv-log.h"
#include "liv-physics.h"

static void liv_print_title()
{
        liv_log_info("-------------------------------------------------------");
        liv_log_info("  ##     00  ##       ##  #######   #######  ########  ");
        liv_log_info("  ##     ##   ##     ##   ##   ##   #           ##     ");
        liv_log_info("  ##     ##    ##   ##    ##   ##   ######      ##     ");
        liv_log_info("  ##     ##     ## ##     ##   ##   #           ##     ");
        liv_log_info("  #####  ##      ###      ##   ##   #######     ##     ");
        liv_log_info("-------------------------------------------------------");
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        app_p app;
        const char* config_file;
        char* file_data;
        size_t data_size;

        if (!SDL_Init(SDL_INIT_EVENTS)) {
                liv_log_error("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        liv_print_title();
        liv_log_debug(SDL_GetBasePath());
        //config_file = argc > 1 ? argv[1] : "scripts/example/config4.lua";
        config_file = argc > 1 ? argv[1] : "scripts/adventure/client_config.lua";
        //config_file = argc > 1 ? argv[1] : "scripts/adventure/server_config.lua";
        file_data = SDL_LoadFile(config_file, &data_size);
        if (!file_data) {
                return SDL_APP_FAILURE;
        }
	SDL_Log("%s", "=======================2");
        app = liv_create(file_data);
	SDL_Log("%s", "=======================3");
        *appstate = app;
        SDL_free(file_data);
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        app_p app;
        app = (app_p)appstate;
        liv_event(app, event);
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        app_p app;
        app = (app_p)appstate;
        liv_update(app);
        if (liv_running(app)) {
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
                liv_destroy(app);
        }
        liv_log_debug("good bye.");
}


