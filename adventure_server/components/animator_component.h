#pragma once
#include "animator_def.h"
#include "../entt.hpp"

struct Animator {
    entt::entity def = entt::null;
    int current_state = 0;
    int current_frame = 0;
    float frame_elapsed = 0;
    float state_elapsed = 0;
    bool just_entered = true;
    bool flip_h = false;
};

struct AnimParams {
    float speed = 0;
    int dir = 0;
    bool attack = false;
};
