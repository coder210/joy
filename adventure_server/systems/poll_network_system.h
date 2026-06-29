#ifndef POLL_NETWORK_SYSTEM_H
#define POLL_NETWORK_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../components/connection_component.h"
#include "../components/net_component.h"



static void handle_cmd_loading(Context* ctx, int conv)
{
    log_info("C2S_CMD_LOADING");
    NetComponent* net = ctx->ecs.entity().get_mut<NetComponent>();
    flecs::entity entity = ctx->ecs.entity().add<ConnectionComponent>();
    auto conn = entity.get_mut<ConnectionComponent>();
    conn->conv = conv;
    conn->frameid = std::max(ctx->global_frame_id - 1, 1);
    conn->health = 10;

    auto it = ctx->world_snapshots.find(conn->frameid);
    if (it == ctx->world_snapshots.end()) { log_info("没有world"); return; }

    S2CEntity entities[512];
    int ec = 0, eid = 0;
    ec = unpack_world(it->second.c_str(), (int)it->second.size(), &eid, entities, 512);
    auto data = pack_s2c_loading(conn->conv, conn->frameid, eid, entities, ec);
    netserver_send(ctx->net_server, conn->conv, data.c_str(), (int)data.size());
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

static void handle_cmd_player_input(Context* ctx, int conv, int dir, int attack)
{
    S2CPlayerInput i = { conv, dir, (int8_t)attack };
    ctx->player_inputs.push_back(i);
}

static void handle_cmd_heartbeat(Context* ctx, int conv)
{
    log_info("%d: C2S_CMD_HEARTBEAT", conv);
    ctx->connection_query.each([&](ConnectionComponent& conn) {
        if (conn.conv == conv) {
            conn.health = 10;
            return false;
        }
        return true;
        });
}

static void on_message(Context* ctx, net_message_p msg, void* userdata)
{
    void* msg_data = msg->data;
    size_t msg_len = msg->len;

    if (msg->type == NET_TYPE_CONNECTED) {
        log_info("connected=%d", msg->conv);
    }
    else if (msg->type == NET_TYPE_DISCONNECTED) {
        log_info("disconnected=%d", msg->conv);
        S2CPlayerLeave l = { msg->conv };
        ctx->player_leaves.push_back(l);
    }
    else if (msg->type == NET_TYPE_MESSAGE) {
        int cmd, input_dir = 0, input_attack = 0; int64_t join_x = 0, join_y = 0;
        if (unpack_c2s((const char*)msg_data, (int)msg_len, &cmd, &join_x, &join_y, &input_dir, &input_attack)) {
            switch (cmd) {
            case CMD_LOADING:      
                handle_cmd_loading(ctx, msg->conv); 
                break;
            case CMD_PLAYER_JOIN:  
                handle_cmd_player_join(ctx, msg->conv, join_x, join_y);
                break;
            case CMD_PLAYER_LEAVE: 
                handle_cmd_player_leave(ctx, msg->conv);
                break;
            case CMD_PLAYER_INPUT: 
                handle_cmd_player_input(ctx, msg->conv, input_dir, input_attack);
                break;
            case CMD_PLAYER_HEART: 
                handle_cmd_heartbeat(ctx, msg->conv); 
                break;
            }
        }
        else {
            log_info("Failed to parse message from conv=%d", msg->conv);
        }
    }
    if (msg->type == NET_TYPE_MESSAGE && msg_data) {
        netmessage_free_data(msg);
    }
}


// 在文件顶部或 on_load 前定义静态函数
static void poll_network_system(flecs::iter& it)
{
    Context* ctx = (Context*)it.world().get_ctx();
    net_message_t msg;
    //log_info("poll_network_system");
    while (netserver_poll_message(ctx->net_server, &msg)) {
        on_message(ctx, &msg, NULL);
    }
}

#endif
