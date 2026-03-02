//#define SDL_MAIN_USE_CALLBACKS 1
//#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>
//#include "../core/calculator.h"
//#include "joy.h"
//
//SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
//{
//        app_p app;
//        const char* config_file;
//        if (!SDL_Init(SDL_INIT_EVENTS)) {
//                log_error("Couldn't initialize SDL: %s", SDL_GetError());
//                return SDL_APP_FAILURE;
//        }
//        double t = add(1.2f, 2.3f);
//        log_info("t=%f", t);
//        //config_file = argc > 1 ? argv[1] : "scripts/config4.lua";
//        //config_file = argc > 1 ? argv[1] : "scripts/local_config.lua";
//        config_file = argc > 1 ? argv[1] : "scripts/server_config.lua";
//        app = joy_create(config_file);
//        if (app) {
//                *appstate = app;
//                return SDL_APP_CONTINUE;
//        }
//        else {
//                return SDL_APP_FAILURE;
//        }
//}
//
//SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
//{
//        app_p app;
//        app = (app_p)appstate;
//        joy_event(app, event);
//        return SDL_APP_CONTINUE;
//}
//
//SDL_AppResult SDL_AppIterate(void* appstate)
//{
//        app_p app;
//        app = (app_p)appstate;
//        joy_update(app);
//        if (joy_running(app)) {
//                return SDL_APP_CONTINUE;
//        }
//        else {
//                return SDL_APP_SUCCESS;
//        }
//}
//
//void SDL_AppQuit(void* appstate, SDL_AppResult result)
//{
//        app_p app;
//        app = (app_p)appstate;
//        if (result == SDL_APP_SUCCESS) {
//                joy_destroy(app);
//        }
//        log_debug("good bye.");
//}


/*
 * This example code creates an SDL window and renderer, and then clears the
 * window to a different color every frame, so you'll effectively get a window
 * that's smoothly fading between colors.
 *
 * This code is public domain. Feel free to use it for any purpose!
 */

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
extern "C" {
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d_core/calculator.h>
}

#include <map>
#include <string>

 /* We will use this renderer to draw into this window every frame. */
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static std::map<std::string, double> cache;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", 640, 480, 0, &window, &renderer)) {
                SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        cache.insert(std::make_pair("1.2f+2.3f", add(1.2f, 2.3f)));
        cache.insert(std::make_pair("1f+2f", add(1.0f, 2.0f)));
        for (auto it = cache.begin(); it != cache.end(); ++it) {
                //std::cout << it->first << ": " << it->second << std::endl;
                SDL_Log("t=%s, %f", it->first.c_str(), it->second);
        }

        double t = add(1.2f, 2.3f);
        SDL_Log("t=%f", t);

        return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
        }
        return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
        const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
        /* choose the color for the frame we will draw. The sine wave trick makes it fade between colors smoothly. */
        const float red = (float)(0.5 + 0.5 * SDL_sin(now));
        const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
        const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
        SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

        /* clear the window to the draw color. */
        SDL_RenderClear(renderer);

        /* put the newly-cleared rendering on the screen. */
        SDL_RenderPresent(renderer);

        return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        /* SDL will clean up the window/renderer for us. */
}

