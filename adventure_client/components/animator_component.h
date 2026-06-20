#pragma once
#include "../flecs.h"
#include "animator_def.h"

// ★ 运行时状态（每个实体一份）
struct Animator {
    flecs::entity_t def;           // 关联的定义实体
    int current_state = 0;
    int current_frame = 0;
    float frame_elapsed = 0;
    float state_elapsed = 0;
    bool just_entered = true;
    bool flip_h = false;
};

// ★ 驱动参数（每个实体一份）
struct AnimParams {
    float speed = 0;    // 移动速度（0=idle, >0=walk）
    int dir = 0;        // 方向（0=下 1=左 2=右 3=上）
    bool attack = false; // 攻击 trigger（消耗型）
};
