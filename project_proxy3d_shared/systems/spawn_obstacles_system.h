#ifndef SPAWN_OBSTACLES_SYSTEM_H
#define SPAWN_OBSTACLES_SYSTEM_H
#include "../context.h"
#include "../project_proxy3d_shared/components/static_mesh_component.h"
#include <joy/joy.h>
#include <cmath>

// 静态碰撞体生成 system：在 OnStart 阶段一次性创建（只执行一次）
// 数据来自 joy_editor/main8888.cpp（SDL_AppInit 中的 boxDefs / 球 / OBB / 斜坡）
// 旋转物体统一用 MESH + 真实世界顶点（gjk3df_init_mesh），与渲染几何完全一致
static void spawn_obstacles(entt::registry& registry, Context* ctx, float dt) {
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
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ sphere, { 60, 200, 90, 255 }, 0.0f });
    }

    // ---- 旋转 45° 的 OBB 箱子（-2,0.5,2,0.8³, 绕 Y 轴 45°）----
    // 用 OBB + 半尺寸 + 旋转欧拉角，走 jgjk.c 的 GJK3DF_CTYPE_OBB 支持函数（正确分离旋转盒）
    // 注意：必须 emplace LogicPositionComponent，否则 collision_system 的
    // static_view（view<Collider, LogicPosition>）遍历不到它，碰撞会被完全跳过导致穿模！
    {
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, id++);
        float rad = ft_radians(45.0f);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(-2), fp_from_float(0.5f), fp_from_float(2));
        registry.emplace<LogicRectComponent>(e, fp_from_float(0.8f), fp_from_float(0.8f));
        ColliderComponent col;
        col.type = ColliderComponent::OBB;
        col.half_size = { fp_from_float(0.4f), fp_from_float(0.4f), fp_from_float(0.4f) };
        col.rotation  = { fp_zero(), fp_from_float(rad), fp_zero() };   // 绕 Y 轴 45°
        registry.emplace<ColliderComponent>(e, col);
        registry.emplace<TransformComponent>(e, vec3_t{ -2, 0.5f, 2 }, ft_radians(45.0f), vec3_t{ 0.8f, 0.8f, 0.8f });
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ cube, { 255, 165, 0, 255 }, 0.0f });
    }

    // ---- 旋转 45° 的 MESH 箱子（-2,0.5,-2,0.8³, 绕 Y 轴 45°）----
    // 用 MESH + 真实世界顶点（绕 Y 轴旋转），与渲染 TransformComponent.yaw=45° 一致
    // 注意：必须 emplace LogicPositionComponent，否则 collision_system 遍历不到它导致穿模
    {
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, id++);
        float half = 0.4f;
        vec3f_t local[8] = {
            { fp_from_float(-half), fp_from_float( half), fp_from_float(-half) },
            { fp_from_float( half), fp_from_float( half), fp_from_float(-half) },
            { fp_from_float( half), fp_from_float( half), fp_from_float( half) },
            { fp_from_float(-half), fp_from_float( half), fp_from_float( half) },
            { fp_from_float(-half), fp_from_float(-half), fp_from_float(-half) },
            { fp_from_float( half), fp_from_float(-half), fp_from_float(-half) },
            { fp_from_float( half), fp_from_float(-half), fp_from_float( half) },
            { fp_from_float(-half), fp_from_float(-half), fp_from_float( half) },
        };
        float rad = ft_radians(45.0f);
        mat44f_t t = mat44f_translate(fp_from_float(-2.0f), fp_from_float(0.5f), fp_from_float(-2.0f));
        mat44f_t r = mat44f_rotate_y(fp_from_float(rad));
        mat44f_t transform = mat44f_mul(t, r);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(-2), fp_from_float(0.5f), fp_from_float(-2));
        registry.emplace<LogicRectComponent>(e, fp_from_float(0.8f), fp_from_float(0.8f));
        ColliderComponent col;
        col.type = ColliderComponent::MESH;
        col.verts_num = 8;
        for (int i = 0; i < 8; i++) {
            vec4f_t v = { local[i].x, local[i].y, local[i].z, fp_one() };
            vec4f_t res = mat44f_mul_vec4f(transform, v);
            col.verts[i] = { res.x, res.y, res.z };
        }
        registry.emplace<ColliderComponent>(e, col);
        registry.emplace<TransformComponent>(e, vec3_t{ -2, 0.5f, -2 }, ft_radians(45.0f), vec3_t{ 0.8f, 0.8f, 0.8f });
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ cube, { 255, 200, 0, 255 }, 0.0f });
    }

    // ---- 三角斜坡（-6,0,0,w2 l3 h1.5，非旋转三角棱柱）----
    // 顶点与 r3d_mesh_ramp_lit(w,l,h) 一一对应（basePos 为角点），用 MESH 直接喂 GJK
    {
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, id++);
        float bx = -6.0f, by = 0.0f, bz = 0.0f;
        float w = 2.0f, l = 3.0f, h = 1.5f;
        ColliderComponent col;
        col.type = ColliderComponent::MESH;
        col.verts_num = 6;
        col.verts[0] = { fp_from_float(bx),        fp_from_float(by),        fp_from_float(bz) };
        col.verts[1] = { fp_from_float(bx + w),    fp_from_float(by),        fp_from_float(bz) };
        col.verts[2] = { fp_from_float(bx),        fp_from_float(by + h),    fp_from_float(bz + l) };
        col.verts[3] = { fp_from_float(bx + w),    fp_from_float(by + h),    fp_from_float(bz + l) };
        col.verts[4] = { fp_from_float(bx),        fp_from_float(by),        fp_from_float(bz + l) };
        col.verts[5] = { fp_from_float(bx + w),    fp_from_float(by),        fp_from_float(bz + l) };
        registry.emplace<ColliderComponent>(e, col);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(bx), fp_zero(), fp_zero());
        registry.emplace<LogicRectComponent>(e, fp_from_float(w), fp_from_float(l));
        registry.emplace<TransformComponent>(e, vec3_t{ bx, by, bz }, 0.0f, vec3_t{ 1, 1, 1 });
        registry.emplace<StaticMeshComponent>(e, StaticMeshComponent{ ramp, { 180, 140, 90, 255 }, 0.0f });
    }

}

#endif
