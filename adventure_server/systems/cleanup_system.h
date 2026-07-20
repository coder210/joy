#ifndef CLEANUP_SYSTEM_H
#define CLEANUP_SYSTEM_H
#include "../context.h"

// 帧尾清理系统：移除帧内临时组件
static void cleanup_system(entt::registry& registry, Context* ctx, float dt) {
        (void)ctx;
        (void)dt;

        // 清理 PlayerStateInput（每帧处理后必须清除）
        auto view = registry.view<PlayerStateInput>();
        for (auto e : view) {
                registry.remove<PlayerStateInput>(e);
        }
}

#endif
