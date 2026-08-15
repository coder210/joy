#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include "../components/enemy_component.h"
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/player_component.h"
#include "../components/bullet_component.h"

struct Context;

// 玩家与碰撞体碰撞处理 system（通用碰撞，碰撞体为带 LogicRect 的实体）
// 在 OnUpdate 阶段运行，在 movement_system 之后
// 检测所有玩家与碰撞层的碰撞，修正玩家位置
static void collision_system(entt::registry& registry, Context* ctx, float dt) {
    (void)ctx;
    (void)dt;

    // 移动体视图：所有玩家/敌人（同时需有 LogicRectComponent + LogicPositionComponent）
    auto mover_view = registry.view<LogicRectComponent, LogicPositionComponent>();

    // 地图碰撞体视图：所有带 LogicRectComponent + LogicPositionComponent 的实体
    auto all_view = registry.view<LogicRectComponent, LogicPositionComponent>();

    for (auto mover_entity : mover_view) {
        // 只对玩家/敌人做碰撞修正，静态碰撞体本身不参与
        if (!registry.all_of<PlayerComponent>(mover_entity) &&
            !registry.all_of<EnemyComponent>(mover_entity)) continue;

        auto& mover_rect = mover_view.get<LogicRectComponent>(mover_entity);
        auto& mover_pos = mover_view.get<LogicPositionComponent>(mover_entity);

        // 记录 movement 修改后的坐标（用于监测哪些移动体被修正）
        fp_t orig_x = mover_pos.x;
        fp_t orig_y = mover_pos.y;
        bool was_corrected = false;

        // 构建移动体碰撞盒子（x/y 跟随 LogicPositionComponent）
        rectanglef_t mover_box = { mover_pos.x, mover_pos.y, mover_rect.width, mover_rect.height };

        for (auto tile_entity : all_view) {
            // 跳过移动体自身、其他玩家/敌人实体（保持现状：玩家与敌人互不碰撞）
            if (tile_entity == mover_entity) continue;
            if (registry.all_of<PlayerComponent>(tile_entity)) continue;
            if (registry.all_of<EnemyComponent>(tile_entity)) continue;
            // 子弹为瞬时实体，碰撞由 bullet_system 自行处理，不应推挤移动体
            if (registry.all_of<BulletComponent>(tile_entity)) continue;

            auto& tile_rect = all_view.get<LogicRectComponent>(tile_entity);
            auto& tile_pos = all_view.get<LogicPositionComponent>(tile_entity);

            if (tile_rect.width == fp_zero() || tile_rect.height == fp_zero()) continue;

            rectanglef_t tile_box = { tile_pos.x, tile_pos.y, tile_rect.width, tile_rect.height };

            contact2df_t contact;
            if (collision2df_get_rectangles(mover_box, fp_zero(), tile_box, fp_zero(), &contact)) {
                // 碰撞发生，沿法线方向将移动体推出
                fp_t depth = contact.depth;
                if (depth < fp_zero()) {
                    depth = fp_sub(fp_zero(), depth);
                }
                fp_t nx = fp_sub(fp_zero(), contact.normal.x);
                fp_t ny = fp_sub(fp_zero(), contact.normal.y);
                mover_pos.x = fp_add(mover_pos.x, fp_mul(nx, depth));
                mover_pos.y = fp_add(mover_pos.y, fp_mul(ny, depth));

                // 更新 mover_box 以继续与其他 tile 碰撞检测
                mover_box.x = mover_pos.x;
                mover_box.y = mover_pos.y;

                was_corrected = true;
            }
        }

        // 监测：该移动体的 movement 坐标被碰撞修正
        if (was_corrected) {
            /*
            log_info("COLLISION: mover entity %d collision corrected (moved from (%.2f, %.2f) to (%.2f, %.2f))",
                (int)mover_entity, fp_to_float(orig_x), fp_to_float(orig_y),
                fp_to_float(mover_pos.x), fp_to_float(mover_pos.y));
                */
        }
    }
}

#endif
