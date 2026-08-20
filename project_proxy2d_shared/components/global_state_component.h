#ifndef GLOBAL_STATE_COMPONENT_H
#define GLOBAL_STATE_COMPONENT_H
#include <joy/jmath.h>

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
};

#endif
