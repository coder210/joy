#ifndef BULLET_COMPONENT_H
#define BULLET_COMPONENT_H
#include <joy/jmath.h>

// 子弹组件：帧同步（lockstep）下由 ai_system 确定性生成，
// 服务端/客户端两端使用 get_entity_id 分配相同 ID，无需网络同步。
struct BulletComponent {
    fp_t vx, vy;        // 速度（m/s）
    fp_t damage;        // 命中伤害
    int  owner_conv;    // 发射者 conv（0 = 敌人发射）
    float life;         // 剩余存活秒数
};

// 子弹几何常量（逻辑单位 m）
static constexpr float BULLET_SIZE = 0.2f;   // 碰撞盒宽高
static constexpr float BULLET_HALF = 0.1f;   // 半宽高
static constexpr float BULLET_LIFE = 2.5f;   // 最大存活时间（秒）

#endif
