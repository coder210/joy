#ifndef BOMB_COMPONENT_H
#define BOMB_COMPONENT_H
#include <joy/jmath.h>

// 炸弹组件：玩家投掷武器（攻击键触发）。帧同步（lockstep）下由 bomb_shoot_system
// 确定性生成，服务端/客户端两端使用 get_entity_id 分配相同 ID，无需网络同步。
// 瞬时实体：不进入快照（见 snapshot_system 的排除分支），否则客户端加载时重复创建导致帧同步错位。
struct BombComponent {
    int  state = 0;     // 0=OFF(飞行中) 1=ON(引信燃烧) 2=EXPLODING(爆炸)
    int  owner_conv = 0;// 投掷者 conv（与子弹一致：>0 为玩家）
    fp_t vx = 0, vy = 0;// 速度（m/s）
    int  flight_ticks = 0;  // OFF 态剩余 tick（飞行阶段；落地或超时即转入引信态）
    int  fuse_ticks   = 0;  // ON 态剩余 tick（引信倒计时）
    int  explode_ticks = 0; // EXPLODING 态剩余 tick（仅视觉，伤害在转入瞬间结算一次）
    float anim_time = 0;    // 渲染帧时钟（仅表现，dt 推进，不进快照、不影响同步）

    BombComponent() {}
    BombComponent(int st, int owner, fp_t ivx, fp_t ivy, int flight, int fuse, int exp)
        : state(st), owner_conv(owner), vx(ivx), vy(ivy),
          flight_ticks(flight), fuse_ticks(fuse), explode_ticks(exp) {}
};

// 炸弹几何/逻辑常量（逻辑单位 m / tick）
static constexpr float BOMB_SIZE        = 0.6f;   // 碰撞盒宽高
static constexpr float BOMB_HALF        = 0.3f;   // 半宽高
static constexpr float BOMB_THROW_SPEED = 7.0f;   // 投掷水平初速（m/s）
static constexpr float BOMB_THROW_UP    = 4.0f;   // 投掷上抛初速（m/s）
static constexpr float BOMB_GRAVITY     = 34.0f;  // 重力加速度（与角色一致，帧同步确定性）
static constexpr int   BOMB_FLIGHT_TICKS = 60;    // 飞行阶段上限 3.0s @20Hz（落地或超时转引信态）
static constexpr int   BOMB_FUSE_TICKS  = 50;     // 引信时长 2.5s @20Hz
static constexpr int   BOMB_EXPLODE_TICKS = 15;   // 爆炸表现 0.75s @20Hz（9 帧素材 @12fps≈0.75s）
static constexpr float BOMB_RADIUS      = 2.5f;   // 爆炸伤害半径（m）
static constexpr float BOMB_DAMAGE      = 60.0f;  // 爆炸伤害（秒杀 50hp 敌人）

// 状态枚举（与 BombComponent.state 对齐）
static constexpr int BOMB_ST_OFF = 0;
static constexpr int BOMB_ST_ON  = 1;
static constexpr int BOMB_ST_EXPLODING = 2;

#endif
