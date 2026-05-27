#pragma once

struct PlayerActionComponent {
    enum State : int { IDLE = 0, WALK = 1, ATTACK = 2 };
    State state = IDLE;
    int   dir = 0;
    float timer = 0;
    float step_timer = 0;
};
