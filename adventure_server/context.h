#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H
#include <joy/jsdl.h>
#include <joy/jcore.h>

struct Context {
        jsdl_window_t* window;
        jsdl_renderer_t* renderer;
        game_timer_t game_timer;
        float FIXED_TIMESTEP;
        float camera_x, camera_y; // 摄像机世界坐标(米)
};


#endif