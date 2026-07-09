#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include <joy/joy.h>

// 碰撞系统：在 movement_system 之后运行，分为两轮
// 1. 与地图墙体碰撞（无 IdComponent 的静态碰撞体）
// 2. 与动态体碰撞（有 IdComponent 的其他实体）
static void collision_system(flecs::entity e, PlayerComponent& p, LogicRectComponent& rect, LogicPositionComponent& pos)
{
    Context* ctx = (Context*)e.world().get_ctx();

    // 遍历所有玩家实体，做 collision resolve
    //ctx->player_query.each([&](flecs::entity e, PlayerComponent&, IdComponent& id,
    //    LogicRectComponent& rect, LogicPositionComponent& pos) {

    //    // === 第一轮：地图墙体碰撞 ===
    //    // 查询 "有 LogicRectComponent + LogicPositionComponent，无 IdComponent" 的实体
    //    // 这些是 init_tilemap_system 创建的静态碰撞体
    //    auto tile_query = ctx->ecs.query_builder<LogicRectComponent, LogicPositionComponent>()
    //        .without<IdComponent>()
    //        .build();

    //    tile_query.each([&](flecs::entity tile_entity,
    //        LogicRectComponent& tile_rect, LogicPositionComponent& tile_pos) {

    //        rectanglef_t curr_rect = { pos.x, pos.y, rect.width, rect.height };
    //        rectanglef_t other_rect = { tile_pos.x, tile_pos.y, tile_rect.width, tile_rect.height };

    //        contact2df_t contact;
    //        if (collision2df_get_rectangles(curr_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
    //            fp_t depth = contact.depth;
    //            if (depth < fp_zero()) depth = fp_sub(fp_zero(), depth);
    //            fp_t nx = fp_sub(fp_zero(), contact.normal.x);
    //            fp_t ny = fp_sub(fp_zero(), contact.normal.y);
    //            pos.x = fp_add(pos.x, fp_mul(nx, depth));
    //            pos.y = fp_add(pos.y, fp_mul(ny, depth));
    //        }
    //    });

    //    // === 第二轮：动态体碰撞（复用现有 body_query）===
    //    ctx->body_query.each([&](IdComponent& other_id,
    //        LogicRectComponent& other_rect, LogicPositionComponent& other_pos) {

    //        if (other_id.id == id.id) return; // 排除自身

    //        rectanglef_t curr_rect = { pos.x, pos.y, rect.width, rect.height };
    //        rectanglef_t other_rect_f = { other_pos.x, other_pos.y, other_rect.width, other_rect.height };

    //        contact2df_t contact;
    //        if (collision2df_get_rectangles(curr_rect, fp_zero(), other_rect_f, fp_zero(), &contact)) {
    //            fp_t depth = contact.depth;
    //            if (depth < fp_zero()) depth = fp_sub(fp_zero(), depth);
    //            fp_t nx = fp_sub(fp_zero(), contact.normal.x);
    //            fp_t ny = fp_sub(fp_zero(), contact.normal.y);
    //            pos.x = fp_add(pos.x, fp_mul(nx, depth));
    //            pos.y = fp_add(pos.y, fp_mul(ny, depth));
    //        }
    //    });
    //});

}

#endif // COLLISION_SYSTEM_H
