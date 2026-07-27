#ifndef CLIENT_NETWORK_UTILS_H
#define CLIENT_NETWORK_UTILS_H

#include <string>
#include <vector>
#include "../context.h"
#include "../../adventure_shared/protocol.h"
#include "../../adventure_shared/components/global_state_component.h"

// ========== 加载数据处理 ==========
static void handle_loading(Context *ctx, int conv, int frame_id, int eid, const S2CEntity* entities, int ec)
{
        ctx->ready = true;
        ctx->local_conv = conv;
        ctx->ecs.ctx().get<GlobalState>().camera_follow_conv = conv;
        ctx->server_frameid = frame_id;
        ctx->server_entity_id = eid;

        for (int i = 0; i < ec; i++) {
                auto& entity = entities[i];
                auto e = ctx->ecs.create();
                ctx->ecs.emplace<IdComponent>(e, entity.id);
                ctx->ecs.emplace<LogicRectComponent>(e, entity.width, entity.height);
                ctx->ecs.emplace<LogicPositionComponent>(e, entity.position_x, entity.position_y);
                ctx->ecs.emplace<TransformComponent>(e, fp_to_float(entity.position_x),
                                                   fp_to_float(entity.position_y), 0.0f, 1.0f, 1.0f);
                if (entity.type == S2C_TYPE_PLAYER) {
                        ctx->ecs.emplace<LogicVelocityComponent>(e, fp_from_float(0), fp_from_float(0));
                        ctx->ecs.emplace<PlayerComponent>(e, entity.player_conv);
                        // 挂载动画组件(如果 AnimatorDefComponent 已注册)
                        if (ctx->ecs.ctx().contains<AnimatorDefComponent>()) {
                            ctx->ecs.emplace<AnimatorComponent>(e);
                            ctx->ecs.emplace<AnimParamsComponent>(e);
                            ctx->ecs.emplace<PlayerActionComponent>(e);
                        }
                }
                else if (entity.type == S2C_TYPE_ENEMY) {
                        ctx->ecs.emplace<EnemyComponent>(e);
                        ctx->ecs.emplace<PlayerActionComponent>(e);
                        ctx->ecs.emplace<LogicVelocityComponent>(e, fp_from_float(0), fp_from_float(0));
                        if (ctx->ecs.ctx().contains<EnemyAnimDef>()) {
                            ctx->ecs.emplace<AnimatorComponent>(e);
                            ctx->ecs.emplace<AnimParamsComponent>(e);
                        }
                }
        }

        // 保存当前快照
        std::map<int, state_snapshot> snapshots;
        auto pv = ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : pv) {
                auto& id = pv.get<IdComponent>(e);
                auto& pos = pv.get<LogicPositionComponent>(e);
                snapshots.insert({ id.id, { pos.x, pos.y } });
        }
        ctx->snapshots.insert({ ctx->server_frameid, snapshots });

        // 告知服务器本地玩家加入
        std::string data = pack_c2s(CMD_PLAYER_JOIN, fp_from_float(3.2f), fp_from_float(2.3f), 0, 0);
        netclient_send(ctx->netclient, data.c_str(), (int)data.size());
}

// ========== 发送输入 ==========
static void send_input(Context* ctx)
{
    if (!ctx->netclient) return;
    int dir = ctx->current_input_mask & INPUT_DIR_MASK;
    int attack = (ctx->current_input_mask & INPUT_ATTACK) ? 1 : 0;
    if (ctx->attack_triggered) { attack = 1; ctx->attack_triggered = false; }
    if (dir || attack) {
        auto data = pack_c2s(CMD_PLAYER_INPUT, 0, 0, dir, attack);
        netclient_send(ctx->netclient, data.c_str(), (int)data.size());
    }
}

// ========== 网络消息接收 ==========
static void network_receive(Context* ctx)
{
        net_message_t msg;
        if (netclient_poll_message(ctx->netclient, &msg)) {
            if (msg.type == NET_TYPE_CONNECTED) {
                auto d = pack_c2s(CMD_LOADING, 0, 0, 0, 0);
                netclient_send(ctx->netclient, d.c_str(), (int)d.size());
            }
            else if (msg.type == NET_TYPE_DISCONNECTED) {
                log_info("disconnected=%d", msg.conv);
            }
            else if (msg.type == NET_TYPE_MESSAGE) {
                jmsgpack_unpacked_t r;
                jmsgpack_unpacked_init(&r);
                size_t off = 0;
                int is_loading = 0;
                if (jmsgpack_unpack_next(&r, (const char*)msg.data, (int)msg.len, &off) >= 0
                    && r.data.type == JMSGPACK_OBJECT_ARRAY && r.data.via.array.size >= 2
                    && r.data.via.array.ptr[0].type == JMSGPACK_OBJECT_POSITIVE_INTEGER)
                    is_loading = (r.data.via.array.ptr[0].via.u64 == S2C_CMD_LOADING);
                jmsgpack_unpacked_destroy(&r);

                if (is_loading) {
                    int conv, frame_id, eid;
                    S2CEntity entities[512];
                    int ec = unpack_loading((const char*)msg.data, (int)msg.len, &conv, &frame_id, &eid, entities, 512);
                    if (ec >= 0)
                        handle_loading(ctx, conv, frame_id, eid, entities, ec);
                }
                else {
                    ctx->command_queue.push(std::string(msg.data, msg.len));
                }
            }
            if (msg.type == NET_TYPE_MESSAGE && msg.data)
                netmessage_free_data(&msg);
        }
}

#endif
