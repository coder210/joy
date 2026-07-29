#ifndef GLOBAL_STATE_COMPONENT_H
#define GLOBAL_STATE_COMPONENT_H
#include <joy/jmath.h>

// 全局单例状态（通过 registry.ctx().get<GlobalState>() 访问）
struct GlobalState {
    // 帧同步
    int next_entity_id = 1;
    int next_frame_id  = 1;

    // 摄像机
    vec2_t camera_position = { 0.0f, 0.0f };
    int camera_follow_conv = -1;  // 要跟随的玩家 conv，-1 表示不跟随

    // AI 种子（来自服务端，保证各客户端确定性）
    int ai_seed = 0;
};

#endif
