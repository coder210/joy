#pragma once
#include <SDL3/SDL.h>

struct SpriteSheetComponent {
    char path[256];
    int frame_w;
    int frame_h;
    int idle_row;
    int walk_row;
    int atk_rows[4];
    int frame_count;
    float frame_duration;
};

struct AnimationFrameComponent {
    int cur_row;
    int cur_frame;
    float timer;
};
