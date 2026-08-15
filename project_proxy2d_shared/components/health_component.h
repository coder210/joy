#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

// 血量组件：子弹命中扣血，HP 通过快照同步给客户端 HUD 显示
struct HealthComponent {
    float hp;      // 当前血量
    float max_hp;  // 最大血量
};

// 玩家初始血量（远程敌人子弹伤害结算用）
static constexpr float PLAYER_MAX_HP = 100.0f;

#endif
