#ifndef CONTEXT_H
#define CONTEXT_H
#include <SDL3/SDL.h>
#include <joy/jcore.h>

#define PIXELS_PER_METER 50

struct context {
        bool running;
        float FIXED_TIMESTEP;
        game_timer_t game_timer;
        SDL_Window* window;
        SDL_Renderer* renderer;
        float camera_x, camera_y; // 摄像机世界坐标(米)
};



#endif