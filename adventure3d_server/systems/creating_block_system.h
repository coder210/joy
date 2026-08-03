#ifndef CREATING_BLOCK_SYSTEM_H
#define CREATING_BLOCK_SYSTEM_H
#include "../context.h"
#include "../components/static_mesh_component.h"
#include <joy/joy.h>
#include <cmath>

// 静态碰撞体生成 system：在 OnStart 阶段一次性创建（只执行一次）
// 数据来自 joy_editor/main8888.cpp（SDL_AppInit 中的 boxDefs / 球 / OBB / 斜坡）
// 组装方式与创建玩家一致：registry.create() + 逐个 emplace 组件
static void creating_block_system(entt::registry& registry, Context* ctx, float dt) {
    (void)dt;
    if (!ctx->r3d) return;

    // 共享基础 mesh（与 main8888.cpp createMeshes() 一致）
    render_mesh_p cube = r3d_mesh_cube_lit(ctx->r3d, 1.0f, 1.0f, 1.0f);
    render_mesh_p sphere = r3d_mesh_sphere_lit(ctx->r3d, 0.5f, 16, 12);
    render_mesh_p ramp = r3d_mesh_ramp_lit(ctx->r3d, 2.0f, 3.0f, 1.5f);

    int id = 10000;

    // 8 个障碍物箱子 {x, z, sx, sy, sz}，pos.y = sy * 0.5
    struct { float x, z, sx, sy, sz; } boxDefs[] = {
        { 3, 3, 1, 1, 1 },
        { -3, -2, 1, 0.5f, 1 },
        { 5, -4, 0.8f, 1.2f, 0.8f },
        { -4, 4, 1, 1, 1 },
        { -5, -4, 1.5f, 0.8f, 1.5f },
        { 4, -5, 0.5f, 1.5f, 0.5f },
        { 0, 5, 1, 1, 1.5f },
        { -1, -5, 1.2f, 0.6f, 1.2f },
    };
    for (auto& d : boxDefs) {
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, id++);
        registry.emplace<LogicPositionComponent>(e,
            fp_from_float(d.x), fp_from_float(d.sy * 0.5f), fp_from_float(d.z));
        registry.emplace<LogicRectComponent>(e, fp_from_float(d.sx), fp_from_float(d.sz));
        ColliderComponent col;
        col.type = ColliderComponent::BOX;
        col.half_size = { fp_from_float(d.sx * 0.5f), fp_from_float(d.sy * 0.5f), fp_from_float(d.sz * 0.5f) };
        registry.emplace<ColliderComponent>(e, col);
        registry.emplace<TransformComponent>(e,
            vec3_t{ d.x, d.sy * 0.5f, d.z }, 0.0f,
            vec3_t{ d.sx, d.sy, d.sz });
        //registry.emplace<LocalOnlyComponent>(e);
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ cube, { 150, 110, 70, 255 }, 0.0f });
    }

    // 球体 {2, 0.5, 2} 半径 0.5
    {
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, id++);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(2), fp_from_float(0.5f), fp_from_float(2));
        registry.emplace<LogicRectComponent>(e, fp_from_float(1), fp_from_float(1));
        ColliderComponent col;
        col.type = ColliderComponent::SPHERE;
        col.radius = fp_from_float(0.5f);
        registry.emplace<ColliderComponent>(e, col);
        registry.emplace<TransformComponent>(e, vec3_t{ 2, 0.5f, 2 }, 0.0f, vec3_t{ 1, 1, 1 });
        registry.emplace<LocalOnlyComponent>(e);
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ sphere, { 60, 200, 90, 255 }, 0.0f });
    }

    // OBB（旋转 45° 的箱子）pos{-2,0.5,2} size 0.8³
    {
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, id++);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(-2), fp_from_float(0.5f), fp_from_float(2));
        registry.emplace<LogicRectComponent>(e, fp_from_float(0.8f), fp_from_float(0.8f));
        ColliderComponent col;
        col.type = ColliderComponent::BOX;
        col.half_size = { fp_from_float(0.4f), fp_from_float(0.4f), fp_from_float(0.4f) };
        registry.emplace<ColliderComponent>(e, col);
        registry.emplace<TransformComponent>(e, vec3_t{ -2, 0.5f, 2 }, ft_radians(45.0f), vec3_t{ 0.8f, 0.8f, 0.8f });
        registry.emplace<LocalOnlyComponent>(e);
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ cube, { 255, 165, 0, 255 }, 0.0f });
    }

    // 三角斜坡 basePos{-6,0,0} w2 l3 h1.5
    {
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, id++);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(-6), fp_zero(), fp_zero());
        registry.emplace<LogicRectComponent>(e, fp_from_float(2), fp_from_float(3));
        ColliderComponent col;
        col.type = ColliderComponent::BOX;
        col.half_size = { fp_from_float(1.0f), fp_from_float(0.75f), fp_from_float(1.5f) };
        registry.emplace<ColliderComponent>(e, col);
        registry.emplace<TransformComponent>(e, vec3_t{ -6, 0, 0 }, 0.0f, vec3_t{ 1, 1, 1 });
        registry.emplace<LocalOnlyComponent>(e);
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ ramp, { 180, 140, 90, 255 }, 0.0f });
    }

    // 斜放长方体斜坡 basePos{6,0,0} w2 l3 h1，roll = -atan2f(h, l)
    {
        entt::entity e = registry.create();
        float roll = -atan2f(1.0f, 3.0f);
        registry.emplace<IdComponent>(e, id++);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(6), fp_from_float(0.5f), fp_zero());
        registry.emplace<LogicRectComponent>(e, fp_from_float(2), fp_from_float(3));
        ColliderComponent col;
        col.type = ColliderComponent::BOX;
        col.half_size = { fp_from_float(1.0f), fp_from_float(0.5f), fp_from_float(1.5f) };
        registry.emplace<ColliderComponent>(e, col);
        registry.emplace<TransformComponent>(e, vec3_t{ 6, 0.5f, 0 }, 0.0f, vec3_t{ 2, 1, 3 });
        registry.emplace<LocalOnlyComponent>(e);
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ cube, { 180, 140, 90, 255 }, roll });
    }
}

#endif
