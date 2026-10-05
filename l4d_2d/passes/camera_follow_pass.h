#ifndef CAMERA_FOLLOW_PASS_H
#define CAMERA_FOLLOW_PASS_H
#include "../components/global_state_component.h"
#include "../components/player_component.h"
#include "../components/transform_component.h"
#include "../protocol.h"
#include "../systems/map_def.h"   // MAP_H：横版相机垂直夹取用

struct Context;

// 将摄像机跟随指定玩家，使玩家居中于屏幕
// 逻辑屏幕 480x320，PPM=50，偏移量 = (480/2/50, 320/2/50) = (4.8, 3.2)
static void camera_follow_pass(entt::registry& registry, Context* /*ctx*/, float /*dt*/) {
    auto& gs = registry.ctx().get<GlobalState>();
    if (gs.camera_follow_conv < 0) return;

    registry.view<PlayerComponent, TransformComponent>().each(
        [&gs](auto /*e*/, PlayerComponent& pc, TransformComponent& t)
    {
        if (pc.conv == gs.camera_follow_conv) {
            // X：始终跟随 + 前视偏置（横版看前方）
            gs.camera_position.x = t.position.x - 4.8f + gs.camera_bias_x;
            // Y：side_view 自动锁走廊中线（首次跟随取玩家 Y）；否则 camera_lock_y 手动锁；否则随玩家上下
            if (gs.side_view) {
                // 横版：垂直跟随玩家，但夹取到地图范围内（不露出地图外）
                const float view_h = 6.4f;                        // 320 / 50 = 6.4m
                float cy = t.position.y - view_h * 0.5f;
                const float max_y = ((float)MAP_H > view_h) ? ((float)MAP_H - view_h) : 0.0f;
                if (cy < 0.0f) cy = 0.0f;
                if (cy > max_y) cy = max_y;
                gs.camera_position.y = cy;
            } else {
                gs.camera_position.y = (gs.camera_lock_y >= 0.0f)
                        ? (gs.camera_lock_y - 3.2f)
                        : (t.position.y - 3.2f);
            }
        }
    });
}

#endif