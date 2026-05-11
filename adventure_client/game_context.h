#ifndef CONTEXT_H
#define CONTEXT_H

#include <SDL3/SDL.h>

#define PIXELS_PER_METER 50

struct context {
        bool running = true;
        float FIXED_TIMESTEP = 1.0f / 50.0f;
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
};



#endif