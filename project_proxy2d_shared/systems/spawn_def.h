#ifndef SPAWN_DEF_H
#define SPAWN_DEF_H

// 敌人生成位置和参数（服务端和客户端共享，保证初始条件一致）
// 位置对应 map_def.h 中的可通行格，避免敌人卡在障碍物里
struct SpawnDef {
    float x, y;           // 位置
    float patrol_radius;  // 巡逻半径
    float detect_radius;  // 检测范围
    float attack_radius;  // 攻击范围
    bool  ranged;         // 是否远程敌人
    float bullet_speed;   // 子弹速度（m/s）
    float bullet_interval;// 射击间隔（秒）
    float bullet_damage;  // 子弹伤害
};

inline const SpawnDef ENEMY_SPAWNS[] = {
    // 近战（出生区附近）
    {  6.5f,  2.5f, 3.0f, 6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 1
    // 远程（地图右侧开阔区）
    { 22.5f,  3.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 2
    // 远程（地图左下角）
    {  8.5f, 17.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 3
};
inline constexpr int ENEMY_SPAWN_COUNT = 3;

#endif
