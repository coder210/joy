#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include "../components/enemy_component.h"
#include "../components/logic_position_component.h"
#include "../components/collider_component.h"
#include "../components/player_component.h"
#include <joy/joy.h>

struct Context;

// 3D 碰撞处理 system
// 在 OnUpdate 阶段运行，在 movement_system 之后
// 检测所有玩家与地图碰撞层的碰撞，修正玩家位置（XZ 平面推开，Y 保持贴地）
static void collision_system(entt::registry& registry, Context* ctx, float dt) {
    (void)ctx;
    (void)dt;

    auto player_view = registry.view<PlayerComponent, ColliderComponent, LogicPositionComponent>();
    auto static_view = registry.view<ColliderComponent, LogicPositionComponent>();

    for (auto player_entity : player_view) {
        auto& player_col = player_view.get<ColliderComponent>(player_entity);
        auto& player_pos = player_view.get<LogicPositionComponent>(player_entity);

        if (player_col.type != ColliderComponent::BOX) continue;

        gjk3df_collider_t player_shape;
        vec3f_t ppos = { player_pos.x, player_pos.y, player_pos.z };
        vec3f_t phs  = { player_col.half_size.x, player_col.half_size.y, player_col.half_size.z };
        gjk3df_init_box(&player_shape, ppos, phs);

        for (auto other_entity : static_view) {
            if (other_entity == player_entity) continue;
            if (registry.all_of<PlayerComponent>(other_entity)) continue;
            if (registry.all_of<EnemyComponent>(other_entity)) continue;

            auto& other_col = static_view.get<ColliderComponent>(other_entity);
            auto& other_pos = static_view.get<LogicPositionComponent>(other_entity);

            if (other_col.type != ColliderComponent::BOX &&
                other_col.type != ColliderComponent::SPHERE) continue;

            gjk3df_collider_t other_shape;
            vec3f_t opos = { other_pos.x, other_pos.y, other_pos.z };
            if (other_col.type == ColliderComponent::SPHERE) {
                gjk3df_init_sphere(&other_shape, opos, other_col.radius);
            } else {
                vec3f_t ohs  = { other_col.half_size.x, other_col.half_size.y, other_col.half_size.z };
                gjk3df_init_box(&other_shape, opos, ohs);
            }

            gjk3df_contact_t contact;
            vec3f_t init_dir = { fp_one(), fp_zero(), fp_zero() };
            if (gjk3df_collide(&player_shape, &other_shape, init_dir, &contact)) {
                // 沿法线方向推开玩家（法线从 other 指向 player）
                fp_t depth = contact.depth;
                if (depth < fp_zero()) depth = fp_sub(fp_zero(), depth);

                player_pos.x = fp_add(player_pos.x, fp_mul(contact.normal.x, depth));
                player_pos.y = fp_add(player_pos.y, fp_mul(contact.normal.y, depth));
                player_pos.z = fp_add(player_pos.z, fp_mul(contact.normal.z, depth));

                // 更新玩家形状以继续检测
                ppos.x = player_pos.x;
                ppos.y = player_pos.y;
                ppos.z = player_pos.z;
                gjk3df_init_box(&player_shape, ppos, phs);
            }
        }
    }
}

#endif
