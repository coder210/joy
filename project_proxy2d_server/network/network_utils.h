#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H
#include "../context.h"
#include "../../project_proxy2d_shared/systems/state_checksum.h"

// ========== 网络命令处理（handle_cmd_*）==========
static void handle_cmd_loading(Context* ctx, int conv)
{
        log_info("C2S_CMD_LOADING");
        auto view = ctx->ecs.view<ConnectionComponent>();
        entt::entity conn = entt::null;
        for (auto e : view) {
                if (ctx->ecs.get<ConnectionComponent>(e).conv == conv) {
                        conn = e;
                        break;
                }
        }
        if (conn == entt::null) {
                conn = ctx->ecs.create();
                ctx->ecs.emplace<ConnectionComponent>(conn, conv);
        }
        auto& comp = ctx->ecs.get<ConnectionComponent>(conn);
        comp.is_loaded = false;
        comp.frameid = ctx->ecs.ctx().get<GlobalState>().next_frame_id;
}

static void handle_cmd_player_join(Context* ctx, int conv, int64_t x, int64_t y)
{
        S2CPlayerJoin j = { conv, x, y };
        ctx->player_joins.push_back(j);
}

static void handle_cmd_player_leave(Context* ctx, int conv)
{
        S2CPlayerLeave l = { conv };
        ctx->player_leaves.push_back(l);
}

static void handle_cmd_player_input(Context* ctx, int conv, uint32_t seq, int dir, int attack, int ranged)
{
        // log_info("[SERVER] handle_input: conv=%d dir=%d attack=%d ranged=%d", conv, dir, attack, ranged);
        S2CPlayerInput i = { seq, conv, dir, (int8_t)attack, (int8_t)ranged };
        ctx->player_inputs.push_back(i);
}

static void handle_cmd_heartbeat(Context* ctx, int conv)
{
        log_info("%d: C2S_CMD_HEARTBEAT", conv);
        uint64_t ms_ticks;
        utils_get_current_time(&ms_ticks);
        auto view = ctx->ecs.view<ConnectionComponent>();
        entt::entity conn = entt::null;
        for (auto e : view) {
                if (ctx->ecs.get<ConnectionComponent>(e).conv == conv) {
                        conn = e;
                        break;
                }
        }
        if (conn == entt::null) {
                log_debug("Heartbeat from unknown conv: %d", conv);
                return;
        }
        auto& comp = ctx->ecs.get<ConnectionComponent>(conn);
        comp.health = 10;
        comp.last_heartbeat = (uint32_t)ms_ticks;
}

// ========== 网络接收 ==========
static void network_receive(Context* ctx)
{
        net_message_t msg;
        while (netserver_poll_message(ctx->net_server, &msg)) {
                if (msg.type == NET_TYPE_CONNECTED) {
                        log_info("connected=%d", msg.conv);
                }
                else if (msg.type == NET_TYPE_DISCONNECTED) {
                        log_info("disconnected=%d", msg.conv);
                        S2CPlayerLeave l = { msg.conv };
                        ctx->player_leaves.push_back(l);
                }
                else if (msg.type == NET_TYPE_MESSAGE) {
                        int cmd, input_dir = 0, input_attack = 0, input_ranged = 0; int64_t join_x = 0, join_y = 0;
                        if (unpack_c2s((const char*)msg.data, (int)msg.len, &cmd, &join_x, &join_y, &input_dir, &input_attack, &input_ranged)) {
                                switch (cmd) {
                                case CMD_LOADING:       handle_cmd_loading(ctx, msg.conv); break;
                                case CMD_PLAYER_JOIN:   handle_cmd_player_join(ctx, msg.conv, join_x, join_y); break;
                                case CMD_PLAYER_LEAVE:  handle_cmd_player_leave(ctx, msg.conv); break;
                                case CMD_PLAYER_INPUT:  handle_cmd_player_input(ctx, msg.conv, 0, input_dir, input_attack, input_ranged); break;
                                case CMD_PLAYER_HEART:  handle_cmd_heartbeat(ctx, msg.conv); break;
                                }
                        }
                        else {
                                log_info("Failed to parse message from conv=%d", msg.conv);
                        }
                }
                if (msg.type == NET_TYPE_MESSAGE && msg.data) {
                        netmessage_free_data(&msg);
                }
        }
}

// ========== 收集输入 + 打包命令帧 ==========
static void collect_input(Context* ctx)
{
        int frame_id = ctx->ecs.ctx().get<GlobalState>().next_frame_id++;
        S2CPlayerJoin joins[32]; int num_joins = 0;
        for (auto& j : ctx->player_joins)
                if (num_joins < 32) joins[num_joins++] = j;
        S2CPlayerLeave leaves[32]; int num_leaves = 0;
        for (auto& l : ctx->player_leaves)
                if (num_leaves < 32) leaves[num_leaves++] = l;
        S2CPlayerInput inputs[64]; int num_inputs = 0;
        for (auto& inp : ctx->player_inputs)
                if (num_inputs < 64) inputs[num_inputs++] = inp;

        // 帧同步状态指纹：与客户端共用同一公式（state_checksum.h）
        std::vector<ChecksumEntry> entries;
        auto pos_view = ctx->ecs.view<IdComponent, PlayerComponent, LogicPositionComponent>();
        for (auto e : pos_view) {
                auto& idc = pos_view.get<IdComponent>(e);
                auto& lpc = pos_view.get<LogicPositionComponent>(e);
                entries.push_back({ idc.id, lpc.x, lpc.y });
        }
        char checksum[64] = { 0 };
        compute_players_md5(entries, checksum, sizeof(checksum));

        auto command_data = pack_s2c(S2C_CMD_COMMAND, frame_id, joins, num_joins, leaves, num_leaves, inputs, num_inputs, checksum);
        ctx->commands[frame_id] = command_data;
        ctx->command_queue.push(command_data);

        ctx->player_joins.clear();
        ctx->player_leaves.clear();
        ctx->player_inputs.clear();

        // 只保留最近 300 帧命令（≈15s @20Hz），防止无玩家时内存线性增长。
        // std::map 按帧号有序，begin() 即最旧帧；补帧最多追 2 帧、新玩家对齐
        // 用 lower_bound 命中最近帧即可，因此 300 帧缓冲完全够用。
        if (ctx->commands.size() > 300)
                ctx->commands.erase(ctx->commands.begin());
}

// ========== 转发命令帧给客户端 ==========
static void input_forwarding(Context* ctx)
{
        auto view = ctx->ecs.view<ConnectionComponent>();
        for (auto e : view) {
                auto& comp = view.get<ConnectionComponent>(e);
                if (comp.is_loaded) {
                        int next_frame_id = ctx->ecs.ctx().get<GlobalState>().next_frame_id;
                        // 一次最多补发 2 帧平滑网络；但绝不越过已打包的最新帧，
                        // 保证客户端严格按序收到全部命令帧（缺帧会让两端状态漂移导致 checksum MISMATCH）
                        int taget_frameid = next_frame_id;
                        if (taget_frameid - comp.frameid > 2) taget_frameid = comp.frameid + 2;
                        for (int i = comp.frameid; i < taget_frameid; i++) {
                                auto it = ctx->commands.find(i);
                                if (it != ctx->commands.end()) {
                                        netserver_send(ctx->net_server, comp.conv, it->second.c_str(), it->second.size());
                                }
                                else {
                                        log_error("Frame %d not found in commands, skip sending.", i);
                                }
                        }
                        comp.frameid = taget_frameid;
                }
                else {
                        auto it = ctx->world_snapshots.lower_bound(comp.frameid);
                        if (it == ctx->world_snapshots.end() || it->first > comp.frameid) {
                                if (it == ctx->world_snapshots.begin()) {
                                        log_info("WARNING: no snapshot cache found for frame %d (client %d), sending empty loading", comp.frameid, comp.conv);
                                        auto data = pack_s2c_loading(comp.conv, comp.frameid,
                                                ctx->ecs.ctx().get<GlobalState>().next_entity_id, nullptr, 0);
                                        netserver_send(ctx->net_server, comp.conv, data.c_str(), data.size());
                                        return;
                                }
                                --it;
                        }
                        S2CEntity entities[512];
                        int ec = 0, eid = 0;
                        ec = unpack_world(it->second.c_str(), (int)it->second.size(), &eid, entities, 512);
                        auto data = pack_s2c_loading(comp.conv, comp.frameid, eid, entities, ec);
                        netserver_send(ctx->net_server, comp.conv, data.c_str(), data.size());
                        comp.is_loaded = true;
                }
        }
}

#endif
