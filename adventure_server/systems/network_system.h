#ifndef NETWORK_SYSTEM_H
#define NETWORK_SYSTEM_H
#include "../context.h"

// ========== 网络命令处理（handle_cmd_*）==========
static void handle_cmd_loading(Context* ctx, int conv)
{
        log_info("C2S_CMD_LOADING");
        auto it = ctx->sessions.find(conv);
        if (it == ctx->sessions.end()) {
                Session sess;
                sess.conv = conv;
                ctx->sessions[conv] = sess;
                it = ctx->sessions.find(conv);
        }
        it->second.is_loaded = false;
        it->second.frameid = ctx->ecs.get<WorldState>(ctx->singleton_world).next_frame_id;
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

static void handle_cmd_player_input(Context* ctx, int conv, uint32_t seq, int dir, int attack)
{
        S2CPlayerInput i = { seq, conv, dir, (int8_t)attack };
        ctx->player_inputs.push_back(i);
}

static void handle_cmd_heartbeat(Context* ctx, int conv)
{
        log_info("%d: C2S_CMD_HEARTBEAT", conv);
        uint64_t ms_ticks;
        utils_get_current_time(&ms_ticks);
        auto it = ctx->sessions.find(conv);
        if (it == ctx->sessions.end()) {
                log_debug("Heartbeat from unknown conv: %d", conv);
                return;
        }
        entt::entity player = it->second.player_entity;
        if (player != entt::null && ctx->ecs.valid(player)) {
                auto& conn = ctx->ecs.get_or_emplace<ConnectionComponent>(player);
                conn.health = 10;
                conn.last_heartbeat = (uint32_t)ms_ticks;
        }
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
                        int cmd, input_dir = 0, input_attack = 0; int64_t join_x = 0, join_y = 0;
                        if (unpack_c2s((const char*)msg.data, (int)msg.len, &cmd, &join_x, &join_y, &input_dir, &input_attack)) {
                                switch (cmd) {
                                case CMD_LOADING:       handle_cmd_loading(ctx, msg.conv); break;
                                case CMD_PLAYER_JOIN:   handle_cmd_player_join(ctx, msg.conv, join_x, join_y); break;
                                case CMD_PLAYER_LEAVE:  handle_cmd_player_leave(ctx, msg.conv); break;
                                case CMD_PLAYER_INPUT:  handle_cmd_player_input(ctx, msg.conv, 0, input_dir, input_attack); break;
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

// ========== 帧同步辅助 ==========
static int get_target_frame_id(int curr_frameid, int context_frameid)
{
        int diff = context_frameid - curr_frameid;
        if (diff >= 50)      return curr_frameid + 9;
        if (diff >= 30)      return curr_frameid;
        if (diff >= 10)      return curr_frameid + 4;
        if (diff > 2)        return curr_frameid + 2;
        return curr_frameid;
}

// ========== 收集输入 + 打包命令帧 ==========
static void collect_input(Context* ctx)
{
        int frame_id = ctx->ecs.get<WorldState>(ctx->singleton_world).next_frame_id++;
        S2CPlayerJoin joins[32]; int num_joins = 0;
        for (auto& j : ctx->player_joins)
                if (num_joins < 32) joins[num_joins++] = j;
        S2CPlayerLeave leaves[32]; int num_leaves = 0;
        for (auto& l : ctx->player_leaves)
                if (num_leaves < 32) leaves[num_leaves++] = l;
        S2CPlayerInput inputs[64]; int num_inputs = 0;
        for (auto& inp : ctx->player_inputs)
                if (num_inputs < 64) inputs[num_inputs++] = inp;

        std::vector<std::pair<int, std::pair<fp_t, fp_t>>> player_positions;
        for (auto& [conv, sess] : ctx->sessions) {
                if (sess.player_entity == entt::null) continue;
                if (!ctx->ecs.valid(sess.player_entity)) continue;
                const auto* id_comp = ctx->ecs.try_get<IdComponent>(sess.player_entity);
                const auto* pos_comp = ctx->ecs.try_get<LogicPositionComponent>(sess.player_entity);
                if (!id_comp || !pos_comp) continue;
                player_positions.push_back({ id_comp->id, { pos_comp->x, pos_comp->y } });
        }
        std::sort(player_positions.begin(), player_positions.end());

        std::string checksum_input;
        for (auto& player : player_positions) {
                char buf[64];
                int n = snprintf(buf, sizeof(buf), "%d:%lld:%lld,",
                        player.first, (long long)player.second.first,
                        (long long)player.second.second);
                checksum_input.append(buf, n);
        }
        char checksum[64] = { 0 };
        utils_md5x(checksum_input.c_str(), checksum_input.size(), checksum);

        auto command_data = pack_s2c(S2C_CMD_COMMAND, frame_id, joins, num_joins, leaves, num_leaves, inputs, num_inputs, checksum);
        ctx->commands[frame_id] = command_data;
        ctx->command_queue.push(command_data);

        ctx->player_joins.clear();
        ctx->player_leaves.clear();
        ctx->player_inputs.clear();
}

// ========== 转发命令帧给客户端 ==========
static void input_forwarding(Context* ctx)
{
        for (auto& [conv, sess] : ctx->sessions) {
                if (sess.is_loaded) {
                        int taget_frameid = get_target_frame_id(sess.frameid,
                                ctx->ecs.get<WorldState>(ctx->singleton_world).next_frame_id);
                        for (int i = sess.frameid; i < taget_frameid; i++) {
                                auto it = ctx->commands.find(i);
                                if (it != ctx->commands.end()) {
                                        netserver_send(ctx->net_server, sess.conv, it->second.c_str(), it->second.size());
                                }
                                else {
                                        log_error("Frame %d not found in commands, skip sending.", i);
                                }
                        }
                        sess.frameid = taget_frameid;
                }
                else {
                        auto it = ctx->world_snapshots.find(sess.frameid);
                        if (it == ctx->world_snapshots.end()) {
                                snapshot_system(ctx->ecs, ctx, 0);
                                it = ctx->world_snapshots.find(sess.frameid);
                                if (it == ctx->world_snapshots.end()) {
                                        auto data = pack_s2c_loading(conv, sess.frameid,
                                                ctx->ecs.get<WorldState>(ctx->singleton_world).next_entity_id, nullptr, 0);
                                        netserver_send(ctx->net_server, sess.conv, data.c_str(), data.size());
                                        return;
                                }
                        }
                        S2CEntity entities[512];
                        int ec = 0, eid = 0;
                        ec = unpack_world(it->second.c_str(), (int)it->second.size(), &eid, entities, 512);
                        auto data = pack_s2c_loading(conv, sess.frameid, eid, entities, ec);
                        netserver_send(ctx->net_server, sess.conv, data.c_str(), data.size());
                }
        }
}

#endif
