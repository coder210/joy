#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H
#include <SDL3/SDL.h>
#include <joy2d/jcore.h>


struct Context {
        SDL_Window* window;
        SDL_Renderer* renderer;
        game_timer_t game_timer;
        float FIXED_TIMESTEP;
};


#endif