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
                } else {
                        ent.type = S2C_TYPE_NORMAL;
                }
                ent.hp = 0;
                ent.position_x = pos.x;
                ent.position_y = pos.y;
                ent.width = rect.width;
                ent.height = rect.height;
        }

        auto& ws = ctx->ecs.get<WorldState>(ctx->singleton_world);
        int frame_id = ws.next_frame_id - 1;
        ctx->world_snapshots[frame_id] = pack_world(ws.next_entity_id, ctx->entities, ctx->num_entities);
}

#endif
