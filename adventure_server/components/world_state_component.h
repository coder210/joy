#ifndef WORLD_STATE_COMPONENT_H
#define WORLD_STATE_COMPONENT_H

// 全局世界状态单例（挂载在 ctx->singleton_world 实体上）
struct WorldState {
    int next_entity_id = 1;  // 下一个可用的全局实体 ID
    int next_frame_id  = 1;  // 下一个帧 ID
};

#endif
