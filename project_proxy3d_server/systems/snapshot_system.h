#ifndef SNAPSHOT_SYSTEM_H
#define SNAPSHOT_SYSTEM_H
#include "../context.h"

static void snapshot_system(entt::registry& registry, Context* ctx, float dt) {
        (void)dt;
        ctx->num_entities = 0;

        auto body_view = registry.view<IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : body_view) {
                if (registry.all_of<LocalOnlyComponent>(e))
                        continue;
                // 静态障碍物（箱子/球/OBB/斜坡）由客户端本地 spawn_obstacles 创建并渲染，
                // 不通过 snapshot 同步，避免重复创建与渲染不一致。
                if (registry.all_of<StaticMeshComponent>(e))
                        continue;
                if (ctx->num_entities >= 512)
                        return;

                auto& id = body_view.get<IdComponent>(e);
                auto& rect = body_view.get<LogicRectComponent>(e);
                auto& pos = body_view.get<LogicPositionComponent>(e);

                S2CEntity& ent = ctx->entities[ctx->num_entities++];
                memset(&ent, 0, sizeof(ent));
                ent.id = id.id;
                if (registry.all_of<PlayerComponent>(e)) {
                        ent.type = S2C_TYPE_PLAYER;
                        ent.player_conv = registry.get<PlayerComponent>(e).conv;
                } else if (registry.all_of<EnemyComponent>(e)) {
                        ent.type = S2C_TYPE_ENEMY;
                        if (registry.all_of<SpawnIdxComponent>(e))
                            ent.ai_type = registry.get<SpawnIdxComponent>(e).idx;
                } else if (registry.all_of<StaticMeshComponent>(e)) {
                        ent.type = S2C_TYPE_PROP;
                } else {
                        ent.type = S2C_TYPE_NORMAL;
                }
                ent.hp = 0;
                ent.position_x = pos.x;
                ent.position_y = pos.y;
                ent.position_z = pos.z;
                ent.width = rect.width;
                ent.height = rect.height;
                if (registry.all_of<ActionStateComponent>(e)) {
                        float yaw = registry.get<ActionStateComponent>(e).yaw;
                        ent.yaw_quantized = (int16_t)(yaw / 3.14159265f * 32767.0f);
                }
        }

        auto& gs = ctx->ecs.ctx().get<GlobalState>();
        int frame_id = gs.next_frame_id - 1;
        ctx->world_snapshots[frame_id] = pack_world(gs.next_entity_id, ctx->entities, ctx->num_entities);
}

#endif
