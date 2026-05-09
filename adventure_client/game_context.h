#ifndef CONTEXT_H
#define CONTEXT_H

#include <SDL3/SDL.h>

struct context {
        bool running = true;
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
};



#endif