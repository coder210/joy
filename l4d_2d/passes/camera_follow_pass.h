#ifndef CAMERA_FOLLOW_PASS_H
#define CAMERA_FOLLOW_PASS_H
#include "../components/global_state_component.h"
#include "../components/player_component.h"
#include "../components/transform_component.h"
#include "../protocol.h"
#include "../systems/map_def.h"   // MAP_H：横版相机垂直夹取用

struct Context;

// 将摄像机跟随指定玩家，使玩家尽量居中于屏幕。
// 逻辑屏幕 480x320，PPM=50，半视口（米） = (480/2/50, 320/2/50) = (4.8, 3.2)。
// 相机做"边界夹取"：跟随后再把视口夹进 [0, MAP_W]×[0, MAP_H]，避免视口越出地图
// 而把背景底色（宣纸白）露出来。地图比视口小时退化为居中（两侧等量底色，无法完全避免）。
static void camera_follow_pass(entt::registry& registry, Context* /*ctx*/, float /*dt*/) {
    auto& gs = registry.ctx().get<GlobalState>();
    if (gs.camera_follow_conv < 0) return;

    // 视口在世界里的实际尺寸（米）= 逻辑屏 / PPM / zoom
    const float z      = (gs.view_zoom > 0.0f) ? gs.view_zoom : 1.0f;
    const float view_w = 9.6f / z;   // 480 / 50 / zoom
    const float view_h = 6.4f / z;   // 320 / 50 / zoom

    registry.view<PlayerComponent, TransformComponent>().each(
        [&gs, view_w, view_h](auto /*e*/, PlayerComponent& pc, TransformComponent& t)
    {
        if (pc.conv == gs.camera_follow_conv) {
            // ---- X：跟随 + 前视偏置 → 夹取到地图边界 ----
            float cx = t.position.x - view_w * 0.5f + gs.camera_bias_x;
            const float max_x = (float)MAP_W - view_w;
            if (max_x <= 0.0f) {
                cx = max_x * 0.5f;                       // 地图比视口窄：居中
            } else {
                if (cx < 0.0f) cx = 0.0f;
                if (cx > max_x) cx = max_x;
            }
            gs.camera_position.x = cx;

            // ---- Y：side_view 跟随玩家；否则用 camera_lock_y 手动锁 / 随玩家上下 ----
            float cy;
            if (gs.side_view) {
                cy = t.position.y - view_h * 0.5f;
            } else {
                cy = (gs.camera_lock_y >= 0.0f) ? (gs.camera_lock_y - view_h * 0.5f)
                                                : (t.position.y - view_h * 0.5f);
            }
            const float max_y = (float)MAP_H - view_h;
            if (max_y <= 0.0f) {
                cy = max_y * 0.5f;                       // 地图比视口矮：居中
            } else {
                if (cy < 0.0f) cy = 0.0f;
                if (cy > max_y) cy = max_y;
            }
            gs.camera_position.y = cy;
        }
    });
}

#endif