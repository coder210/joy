#ifndef GLOBAL_STATE_COMPONENT_H
#define GLOBAL_STATE_COMPONENT_H
#include <joy/jmath.h>
#include <joy/joy.h>

// 全局单例状态（通过 registry.ctx().get<GlobalState>() 访问）
struct GlobalState {
    // 帧同步
    int next_entity_id = 1;
    int next_frame_id  = 1;       // 逻辑帧序号：每个固定步长 tick +1（两端确定性一致）

    // 实际逻辑帧率（每秒统计一次，仅用于调试显示）
    int logic_fps = 0;

    // 摄像机
    vec2_t camera_position = { 0.0f, 0.0f };
    int camera_follow_conv = -1;  // 要跟随的玩家 conv，-1 表示不跟随

    // 确定性随机数（帧同步用）：seed 由服务端生成并通过 loading 协议下发，
    // 两端以同一 seed 创建同一序列，保证 AI/巡逻等行为完全一致。
    int random_seed = 0;
    random_num_p random = NULL;   // utils_random_create(seed) 创建，帧同步期间共享推进

    // A* 寻路工作区（存放 pathfind_astar 搜索过程的临时缓冲，含地图可通行性缓存）。
    // 仅作临时计算缓冲，不是帧同步状态、不参与 checksum，不随网络序列化。
    // 放 GlobalState 全局单例（随 ECS ctx 一起 emplace/get），避免函数内 static 数组的
    // 跨编译单元/可重入隐患。MAX_CELLS = 30x20 地图格数。
    struct AstarWork {
        static const int MAX_CELLS = 600;   // MAP_W * MAP_H

        int   gscore[MAX_CELLS];
        int   fscore[MAX_CELLS];
        int   came[MAX_CELLS];
        bool  closed[MAX_CELLS];
        bool  in_open[MAX_CELLS];
        int   heap[MAX_CELLS];              // 二叉堆：按 fscore 最小堆
        bool  walkable[MAX_CELLS];          // 地图可通行性缓存（避免每格重复查表）
        bool  walkable_init = false;
    } astar_work;
};

#endif
