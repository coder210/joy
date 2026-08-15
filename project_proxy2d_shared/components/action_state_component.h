#pragma once

struct ActionStateComponent {
    enum State : int { IDLE = 0, WALK = 1, ATTACK = 2 };
    State state = IDLE;
    int   dir = 0;          // 0=down 1=left 2=right 3=up

    // 攻击持续计时（秒）：>0 视为攻击中。替代原动画状态机的"攻击动画播完复位"，
    // 由 ai_system / input_consume_input_system 每 tick 递减。
    // 时长取自原 warrior/goblin 攻击动画（6 帧 × 0.15s）
    float attack_timer = 0.0f;
    static constexpr float ATTACK_DURATION = 0.9f;

    // 远程射击冷却（秒）：玩家按住远程攻击键时以固定间隔发射，由 player_shoot_system 递减
    float ranged_cooldown = 0.0f;
    static constexpr float RANGED_COOLDOWN = 0.3f;
};