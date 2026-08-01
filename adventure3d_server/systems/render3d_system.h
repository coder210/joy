#ifndef RENDER3D_SYSTEM_H
#define RENDER3D_SYSTEM_H

#include <joy/joy.h>
#include "../context.h"
#include "../../adventure3d_shared/components/transform_component.h"
#include "../../adventure3d_shared/components/player_component.h"
#include "../../adventure3d_shared/components/enemy_component.h"
#include "../../adventure3d_shared/components/collider_component.h"
#include "../../adventure3d_shared/components/global_state_component.h"
#include "../../adventure3d_shared/components/action_state_component.h"

static void render3d_system(Context* ctx, float /*dt*/) {
    if (!ctx->r3d) return;

    auto& gs = ctx->ecs.ctx().get<GlobalState>();

    // top-down camera: observe from left-rear-above of the target
    vec3_t target = gs.camera_target;
    vec3_t eye = {
        target.x - 10.0f,
        target.y + 15.0f,
        target.z - 10.0f
    };
    vec3_t up = { 0.0f, 1.0f, 0.0f };
    r3d_camera_lookat(ctx->r3d, eye, target, up, 45.0f);

    r3d_begin(ctx->r3d);

    // floor
    if (ctx->mesh_floor) {
        jcolor_t gray = { 128, 128, 128, 255 };
        r3d_draw_mesh(ctx->r3d, ctx->mesh_floor, vec3_t{ 0.0f, 0.0f, 0.0f }, vec3_t{ 1.0f, 1.0f, 1.0f }, gray);
    }

    // entities
    auto view = ctx->ecs.view<TransformComponent>();
    for (auto e : view) {
        auto& t = view.get<TransformComponent>(e);

        render_mesh_p mesh = nullptr;
        jcolor_t color = { 255, 255, 255, 255 };
        if (ctx->ecs.all_of<PlayerComponent>(e)) {
            mesh = ctx->mesh_player;
            color = { 100, 150, 255, 255 };
        } else if (ctx->ecs.all_of<EnemyComponent>(e)) {
            mesh = ctx->mesh_enemy;
            color = { 255, 80, 80, 255 };
        } else if (ctx->ecs.all_of<ColliderComponent>(e)) {
            mesh = ctx->mesh_enemy;
            color = { 160, 160, 160, 255 };
        }

        if (mesh) {
            r3d_draw_mesh(ctx->r3d, mesh, t.position, t.scale, color);
        }
    }

    r3d_end(ctx->r3d);
}

#endif
