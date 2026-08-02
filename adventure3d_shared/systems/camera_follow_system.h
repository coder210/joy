#ifndef CAMERA_FOLLOW_SYSTEM_H
#define CAMERA_FOLLOW_SYSTEM_H
#include "../components/global_state_component.h"
#include "../components/player_component.h"
#include "../components/transform_component.h"
#include "../components/action_state_component.h"
#include "../protocol.h"

struct Context;

// 3D 俯视角：摄像机目标跟随玩家，偏移在 render3d_system 中设置；
// 相机水平朝向跟随玩家朝向（鼠标 yaw），移动方向与相机朝向一致
static void camera_follow_system(entt::registry& registry, Context* /*ctx*/, float /*dt*/) {
    auto& gs = registry.ctx().get<GlobalState>();
    if (gs.camera_follow_conv < 0) return;

    auto view = registry.view<PlayerComponent, TransformComponent>();
    for (auto e : view) {
        if (registry.get<PlayerComponent>(e).conv == gs.camera_follow_conv) {
            auto& t = registry.get<TransformComponent>(e);
            gs.camera_target.x = t.position.x;
            gs.camera_target.y = t.position.y;
            gs.camera_target.z = t.position.z;
            if (registry.all_of<ActionStateComponent>(e))
                gs.camera_yaw = registry.get<ActionStateComponent>(e).yaw;
            return;
        }
    }
}

#endif