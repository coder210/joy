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

// ============================================================
// 压力测试：30 个敌人，近战/远程混合，均匀覆盖整张地图
// （30x20 地图从 415 个可通行格中均匀采样，覆盖各区域）
// ============================================================
inline const SpawnDef ENEMY_SPAWNS[] = {
    {  1.5f,  1.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 1  ranged
    { 14.5f,  1.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 2  melee
    {  4.5f,  2.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 3  ranged
    { 26.5f,  2.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 4  ranged
    { 15.5f,  3.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 5  melee
    {  5.5f,  4.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 6  melee
    { 24.5f,  4.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 7  ranged
    {  9.5f,  5.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 8  ranged
    { 25.5f,  5.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 9  melee
    { 11.5f,  6.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 10 melee
    {  1.5f,  7.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 11 melee
    { 16.5f,  7.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 12 melee
    {  7.5f,  8.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 13 melee
    { 23.5f,  8.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 14 melee
    { 11.5f,  9.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 15 ranged
    { 28.5f,  9.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 16 melee
    { 16.5f, 10.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 17 melee
    {  8.5f, 11.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 18 melee
    { 28.5f, 11.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 19 ranged
    { 15.5f, 12.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 20 melee
    {  4.5f, 13.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 21 melee
    { 18.5f, 13.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 22 ranged
    {  8.5f, 14.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 23 melee
    { 26.5f, 14.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 24 ranged
    { 14.5f, 15.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 25 melee
    {  3.5f, 16.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 26 ranged
    { 18.5f, 16.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 27 melee
    {  6.5f, 17.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 28 ranged
    { 25.5f, 17.5f, 3.0f,  6.0f, 1.5f, false, 0.0f, 0.0f, 0.0f }, // 29 melee
    { 15.5f, 18.5f, 2.0f, 10.0f, 6.0f, true, 6.0f, 1.5f, 10.0f }, // 30 ranged
};
inline constexpr int ENEMY_SPAWN_COUNT = 30;

#endif
