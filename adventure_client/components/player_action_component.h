#pragma once

struct PlayerActionComponent {
    enum State : int { IDLE = 0, WALK = 1, ATTACK = 2 };
    State state = IDLE;
    int   dir = 0;          // 0=down 1=left 2=right 3=up
    float timer = 0;        // 攻击/状态计时器
    float step_timer = 0;   // 脚步间隔计时器
};
