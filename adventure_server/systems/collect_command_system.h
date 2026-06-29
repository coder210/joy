#ifndef COLLECT_COMMAND_SYSTEM_H
#define COLLECT_COMMAND_SYSTEM_H
#include <algorithm>
#include "../flecs.h"
#include "../context.h"
#include "../components/connection_component.h"
#include "../components/net_component.h"


static void collect_command_system(flecs::iter& it)
{
    Context* ctx = (Context*)it.world().get_ctx();
    int frame_id = ctx->global_frame_id++;

    S2CPlayerJoin joins[32]; int jn = 0;
    for (auto& j : ctx->player_joins) 
        if (jn < 32) joins[jn++] = j;
    S2CPlayerLeave leaves[32]; int ln = 0;
    for (auto& l : ctx->player_leaves) 
        if (ln < 32) leaves[ln++] = l;
    S2CPlayerInput inputs[64]; int in = 0;
    for (auto& i : ctx->player_inputs) 
        if (in < 64) inputs[in++] = i;

    // 只收集玩家实体，按 id 排序确保顺序一致
    std::vector<std::pair<int, std::pair<fp_t, fp_t>>> positions;
    ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
        LogicRectComponent& r, LogicPositionComponent& pos) {
            positions.push_back({ id.id, { (int64_t)pos.x, (int64_t)pos.y } });
        });
    std::sort(positions.begin(), positions.end());

    std::string pos_data;
    for (auto& entry : positions) {
        char buf[64];
        int n = snprintf(buf, sizeof(buf), "%d:%lld:%lld,",
            entry.first, (long long)entry.second.first,
            (long long)entry.second.second);
        pos_data.append(buf, n);
    }
    char md5_out[64] = { 0 };
    utils_md5x(pos_data.c_str(), pos_data.size(), md5_out);

    log_info("[checksum] frame=%d md5=%s", frame_id, md5_out);
    auto command_data = pack_s2c(S2C_CMD_COMMAND, frame_id, joins, jn, leaves, ln, inputs, in, md5_out);
    ctx->commands[frame_id] = command_data;
    ctx->command_queue.push(command_data);

    ctx->player_joins.clear();
    ctx->player_leaves.clear();
    ctx->player_inputs.clear();

    // 收集世界实体状态
    S2CEntity entities[512]; int ec = 0;
    ctx->body_query.each([&](flecs::entity e, IdComponent& id,
        LogicRectComponent& r, LogicPositionComponent& pos) {
            if (e.has<LocalOnlyComponent>() || ec >= 512) return;
            auto& ent = entities[ec++]; memset(&ent, 0, sizeof(ent));
            ent.id = id.id;
            if (e.has<PlayerComponent>()) { ent.type = S2C_TYPE_PLAYER; ent.player_conv = e.get_mut<PlayerComponent>()->conv; }
            else if (e.has<EnemyComponent>()) { ent.type = S2C_TYPE_ENEMY; }
            else { ent.type = S2C_TYPE_NORMAL; }
            ent.hp = id.hp; ent.position_x = pos.x; ent.position_y = pos.y;
            ent.width = r.width; ent.height = r.height;
        });
    ctx->world_snapshots[frame_id] = pack_world(ctx->global_frame_id, entities, ec);

    // 限制 worlds 和 commands 最多保留 1000 帧
    const size_t MAX_FRAMES = 1000;
    while (ctx->commands.size() > MAX_FRAMES) {
        ctx->commands.erase(ctx->commands.begin());
    }
    while (ctx->world_snapshots.size() > MAX_FRAMES) {
        ctx->world_snapshots.erase(ctx->world_snapshots.begin());
    }
}


#endif
