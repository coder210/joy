#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H
#include <SDL3/SDL.h>
#include <joy2d/jcore.h>
#include <joy2d/jnetwork.h>


struct Context {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
        game_timer_t game_timer;
};


#endif