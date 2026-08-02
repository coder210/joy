#ifndef RENDER3D_SYSTEM_H
#define RENDER3D_SYSTEM_H

#include <joy/joy.h>
#include <cmath>
#include <cstring>
#include "../context.h"
#include "../../adventure3d_shared/components/transform_component.h"
#include "../../adventure3d_shared/components/mesh_component.h"
#include "../../adventure3d_shared/components/global_state_component.h"

static void render3d_system(Context* ctx, float /*dt*/) {
    if (!ctx->r3d) return;

    auto& gs = ctx->ecs.ctx().get<GlobalState>();

    // 固定相机：55° 俯仰角 + 正对场景（yaw=0，格子不斜），拉远看全场景
    const float CAM_DIST = 22.0f;
    const float CAM_PITCH = 55.0f * 3.14159265f / 180.0f;
    float yaw = 0.0f;  // 正对 -Z 方向，场景轴与屏幕对齐
    float cp = cosf(CAM_PITCH), sp = sinf(CAM_PITCH);
    vec3_t target = { 0.0f, 0.0f, 0.0f };
    vec3_t aim = { target.x, target.y + 1.0f, target.z };
    vec3_t eye = {
        target.x - sinf(yaw) * cp * CAM_DIST,
        target.y + sp * CAM_DIST,
        target.z - cosf(yaw) * cp * CAM_DIST
    };
    vec3_t up = { 0.0f, 1.0f, 0.0f };

    if (!r3d_begin(ctx->r3d)) return;
    r3d_camera_lookat(ctx->r3d, eye, aim, up, 60.0f);

    // 光照：环境光 + 方向光（方向随相机 yaw 旋转），与 main8888.cpp 一致
    jr3d_light_block_t lb;
    memset(&lb, 0, sizeof(lb));
    lb.ambient[0] = 0.15f; lb.ambient[1] = 0.15f; lb.ambient[2] = 0.15f; lb.ambient[3] = 1.0f;
    lb.lights[0].enabled = 1;
    lb.lights[0].type = JR3D_LIGHT_DIRECTIONAL;
    lb.lights[0].position[0] = 30.0f; lb.lights[0].position[1] = 25.0f; lb.lights[0].position[2] = 30.0f;
    lb.lights[0].target[0] = -sinf(yaw);
    lb.lights[0].target[1] = -1.0f;
    lb.lights[0].target[2] = -cosf(yaw);
    lb.lights[0].color[0] = 1.0f; lb.lights[0].color[1] = 0.95f; lb.lights[0].color[2] = 0.8f; lb.lights[0].color[3] = 1.0f;
    lb.viewPos[0] = eye.x; lb.viewPos[1] = eye.y; lb.viewPos[2] = eye.z;
    r3d_set_light_block(ctx->r3d, &lb);

    // 地面（棋盘格纹理，带光照）
    if (ctx->mesh_floor) {
        mat44_t m = mat44_identity();
        r3d_draw_mesh_lit(ctx->r3d, ctx->mesh_floor, &m, ctx->floor_tex);
    }

    // 实体（带光照 + 颜色）：每个实体通过 MeshComponent 独立持有 mesh 与颜色
    auto view = ctx->ecs.view<TransformComponent, MeshComponent>();
    for (auto e : view) {
        auto& t = view.get<TransformComponent>(e);
        auto& mc = view.get<MeshComponent>(e);
        if (!mc.mesh) continue;

        mat44_t m = mat44_identity();
        m.m0 = t.scale.x; m.m5 = t.scale.y; m.m10 = t.scale.z;
        m.m12 = t.position.x; m.m13 = t.position.y; m.m14 = t.position.z;
        r3d_draw_mesh_lit_color(ctx->r3d, mc.mesh, &m, mc.color);
    }

    r3d_end(ctx->r3d);
}

#endif
