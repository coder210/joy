#ifndef CONTEXT_H
#define CONTEXT_H
#include <joy/jcore.h>
#include <joy/jr2d.h>

#define PIXELS_PER_METER 50

struct context {
        bool running;
        float FIXED_TIMESTEP;
        game_timer_t game_timer;
        jwindow_p window;
        jr2d_p r2d;
        float camera_x, camera_y; // 摄像机世界坐标(米)
};

#endif