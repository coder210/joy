#pragma once

struct ActionStateComponent {
    enum State : int { IDLE = 0, WALK = 1, ATTACK = 2 };
    State state = IDLE;
    int   dir = 0;          // 0=down 1=left 2=right 3=up

    // 攻击持续计时（tick 数）：>0 视为攻击中。替代原动画状态机的"攻击动画播完复位"，
    // 由 ai_system / input_consume_input_system 每 tick 递减。
    // 时长取自原 warrior/goblin 攻击动画（6 帧 × 0.15s = 0.9s @20Hz = 18 ticks）
    int attack_timer = 0;
    static constexpr int ATTACK_DURATION = 18;

    // 远程射击冷却（tick 数）：玩家按住远程攻击键时以固定间隔发射，由 player_shoot_system 每 tick 递减
    int ranged_cooldown = 0;
    static constexpr int RANGED_COOLDOWN = 6;   // 0.3s @ 20Hz

    // 近战攻击冷却（tick 数）：攻击事件结算后置满，由 melee_attack_system 每 tick 递减，
    // 防止按住攻击键 / 敌人每 tick 产生攻击事件导致高频结算。
    int melee_cooldown = 0;
    static constexpr int MELEE_COOLDOWN = 18;   // 0.9s @ 20Hz（与 ATTACK_DURATION 一致）
};