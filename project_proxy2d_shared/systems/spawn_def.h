#ifndef SPAWN_DEF_H
#define SPAWN_DEF_H

// 敌人生成位置和参数（服务端和客户端共享，保证初始条件一致）
// 位置对应 map_def.h 中的可通行格，避免敌人卡在障碍物里
struct SpawnDef {
    float x, y;           // 位置
    float patrol_radius;  // 巡逻半径
    float detect_radius;  // 检测范围
    float attack_radius;  // 攻击范围
};

inline const SpawnDef ENEMY_SPAWNS[] = {
    // 暂时只保留 1 个敌人（出生区附近），排查不同步问题
    {  6.5f,  2.5f, 3.0f, 6.0f, 1.5f }, // 1
};
inline constexpr int ENEMY_SPAWN_COUNT = 1;

#endif
