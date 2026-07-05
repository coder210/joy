#ifndef SNAPSHOT_SYSTEM_H
#define SNAPSHOT_SYSTEM_H
#include "../flecs.h"
#include "../context.h"

static void snapshot_system(flecs::iter& it)
{
    Context* ctx = (Context*)it.world().get_ctx();

    // 1. 每次迭代前重置缓冲区
    ctx->num_entities = 0;

    // 2. 收集所有非 LocalOnly 的实体
    ctx->body_query.each([&](flecs::entity e, IdComponent& id,
        LogicRectComponent& rect, LogicPositionComponent& pos) {
            if (e.has<LocalOnlyComponent>())
                return;
            if (ctx->num_entities >= 512)
                return;

            S2CEntity& ent = ctx->entities[ctx->num_entities++];
            memset(&ent, 0, sizeof(ent));
            ent.id = id.id;
            if (e.has<PlayerComponent>()) {
                ent.type = S2C_TYPE_PLAYER;
                ent.player_conv = e.get_mut<PlayerComponent>()->conv;
            } else if (e.has<EnemyComponent>()) {
                ent.type = S2C_TYPE_ENEMY;
            } else {
                ent.type = S2C_TYPE_NORMAL;
            }
            ent.hp = 0;
            ent.position_x = pos.x;
            ent.position_y = pos.y;
            ent.width = rect.width;
            ent.height = rect.height;
        });

    // 3. 打包快照
    int frame_id = ctx->global_frame_id - 1; // 当前已收集的帧
    ctx->world_snapshots[frame_id] = pack_world(ctx->global_entity_id, ctx->entities, ctx->num_entities);

    // 4. 限制保留帧数
   /* const size_t MAX_FRAMES = 1000;
    while (ctx->commands.size() > MAX_FRAMES)
              ctx->commands.erase(ctx->commands.begin());
    while (ctx->world_snapshots.size() > MAX_FRAMES)
        ctx->world_snapshots.erase(ctx->world_snapshots.begin());*/
}

#endif
