#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H
#include <joy/jmath.h>

// 玩家初始血量（远程敌人子弹伤害结算用）
static constexpr float PLAYER_MAX_HP = 100.0f;
// 敌人初始血量（玩家远程子弹伤害结算用）
static constexpr float ENEMY_MAX_HP = 50.0f;
// 重生耗时（秒）
static constexpr float PLAYER_RESPAWN_TIME = 3.0f;
// 默认出生点（与 client handle_loading 发送的 join 位置一致：地图中央开阔地）
static const fp_t PLAYER_SPAWN_X = fp_from_float(15.5f);
static const fp_t PLAYER_SPAWN_Y = fp_from_float(10.5f);

// 血量组件：子弹命中扣血，HP 通过快照同步给客户端 HUD 显示
struct HealthComponent {
    float hp;            // 当前血量
    float max_hp;        // 最大血量
    bool  dead;          // 死亡标记（帧同步确定性：两端同一 tick 置位/清除，不通过快照同步）
    float respawn_timer; // 重生倒计时（秒），死亡期间由 player_death_system 递减
    fp_t  spawn_x;       // 出生点（重生传送目标）
    fp_t  spawn_y;

    HealthComponent() = default;
    HealthComponent(float h, float m, bool d = false, float rt = 0.0f,
                    fp_t sx = PLAYER_SPAWN_X, fp_t sy = PLAYER_SPAWN_Y)
        : hp(h), max_hp(m), dead(d), respawn_timer(rt), spawn_x(sx), spawn_y(sy) {}
};

#endif
