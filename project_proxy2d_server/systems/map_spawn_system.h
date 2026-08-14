#ifndef MAP_SPAWN_SYSTEM_H
#define MAP_SPAWN_SYSTEM_H

#include "../context.h"
#include "../../project_proxy2d_shared/systems/map_def.h"

// 在 OnStart 阶段运行，根据 MAP_LAYOUT 创建静态碰撞地形实体。
// 这些实体不带 Player/Enemy 标记，会被 collision_system 视为碰撞体，
// 也会被 snapshot_system 打包同步给客户端（保证客户端本地碰撞一致）。
static void map_spawn_system(entt::registry& registry, Context* ctx, float /*dt*/) {
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            char c = MAP_LAYOUT[y][x];
            if (!map_is_collidable(c)) continue;

            float px = (float)x + 0.5f; // 格子中心（米）
            float py = (float)y + 0.5f;

            entt::entity e = registry.create();
            registry.emplace<IdComponent>(e, get_entity_id(ctx));
            // 碰撞盒 = 整格 [x, x+1]，LogicPosition 取左上角（与玩家/敌人一致）
            registry.emplace<LogicPositionComponent>(e, fp_from_float((float)x), fp_from_float((float)y));
            registry.emplace<LogicRectComponent>(e, fp_from_float(1), fp_from_float(1));
            // 渲染原点 = 碰撞盒中心 = 格子中心
            registry.emplace<TransformComponent>(e, px, py, 0.0f, 1.0f, 1.0f);
        }
    }
}

#endif
