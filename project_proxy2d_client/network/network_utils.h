#ifndef CLIENT_NETWORK_UTILS_H
#define CLIENT_NETWORK_UTILS_H

#include <string>
#include <vector>
#include "../context.h"
#include "../../project_proxy2d_shared/protocol.h"
#include "../../project_proxy2d_shared/components/global_state_component.h"
#include "../../project_proxy2d_shared/components/ai_component.h"
#include "../../project_proxy2d_shared/components/health_component.h"
#include "../../project_proxy2d_shared/components/spawn_point_component.h"
#include "../../project_proxy2d_shared/systems/spawn_def.h"

// ========== 加载数据处理 ==========
static void handle_loading(Context *ctx, int conv, int frame_id, int eid, const S2CEntity* entities, int ec)
{
        log_info("handle_loading");
        ctx->ready = true;
        ctx->local_conv = conv;
        ctx->ecs.ctx().get<GlobalState>().camera_follow_conv = conv;
        ctx->server_frameid = frame_id;
        ctx->server_entity_id = eid;

        for (int i = 0; i < ec; i++) {
                auto& entity = entities[i];
                // 地图(S2C_TYPE_NORMAL)由客户端本地 map_spawn_system 生成，不通过网络同步
                if (entity.type == S2C_TYPE_NORMAL) continue;
                auto e = ctx->ecs.create();
                ctx->ecs.emplace<IdComponent>(e, entity.id);
                ctx->ecs.emplace<LogicRectComponent>(e, entity.width, entity.height);
                ctx->ecs.emplace<LogicPositionComponent>(e, entity.position_x, entity.position_y);
                // 快照 position 是碰撞盒左上角，Transform 取碰撞盒中心（与 server 端一致）
                ctx->ecs.emplace<TransformComponent>(e,
                                                   fp_to_float(entity.position_x) + fp_to_float(entity.width) * 0.5f,
                                                   fp_to_float(entity.position_y) + fp_to_float(entity.height) * 0.5f,
                                                   0.0f, 1.0f, 1.0f);
                if (entity.type == S2C_TYPE_PLAYER) {
                        ctx->ecs.emplace<LogicVelocityComponent>(e, fp_from_float(0), fp_from_float(0));
                        ctx->ecs.emplace<PlayerComponent>(e, entity.player_conv);
                        // 动作状态组件：几何渲染的朝向 / 攻击表现依赖它
                        ctx->ecs.emplace<ActionStateComponent>(e);
                        // 血量组件：从快照恢复（服务端子弹伤害结算结果）
                        ctx->ecs.emplace<HealthComponent>(e,
                            (int)entity.hp, PLAYER_MAX_HP);
                        // 出生点：与 server 端 join 位置一致（默认地图中央）
                        ctx->ecs.emplace<SpawnPointComponent>(e,
                            DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y);
                }
                else if (entity.type == S2C_TYPE_ENEMY) {
                        ctx->ecs.emplace<EnemyComponent>(e);
                        ctx->ecs.emplace<ActionStateComponent>(e);
                        //ctx->ecs.emplace<LogicVelocityComponent>(e, fp_from_float(0), fp_from_float(0));
                        // 使用服务端打包的 ai_type 作为 SpawnDef 索引（避免位置匹配方式在 AI 移动后失效）
                        int si = (int)entity.ai_type;
                        if (si >= 0 && si < ENEMY_SPAWN_COUNT) {
                            auto& sd = ENEMY_SPAWNS[si];
                            // 血量组件：从快照恢复（玩家远程子弹伤害结算结果）
                            ctx->ecs.emplace<HealthComponent>(e,
                                (int)entity.hp, ENEMY_MAX_HP);
                            // 出生点：从 SpawnDef 恢复（死亡重生传送目标）
                            ctx->ecs.emplace<SpawnPointComponent>(e,
                                fp_from_float(sd.x), fp_from_float(sd.y));
                            ctx->ecs.emplace<AIComponent>(e,
                                fp_from_float(sd.x), fp_from_float(sd.y),
                                fp_from_float(sd.patrol_radius),
                                fp_from_float(sd.detect_radius),
                                fp_from_float(sd.attack_radius),
                                sd.ranged,                                   // 是否远程敌人
                                fp_from_float(sd.bullet_speed),              // 子弹速度
                                fp_from_float(sd.bullet_damage),             // 子弹伤害
                                (int)(sd.bullet_interval * 20.0f + 0.5f),    // 射击间隔（tick）
                                0);                                          // 射击冷却
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

        // 告知服务器本地玩家加入（出生在地图中央开阔地，避免视野内大片边界绿墙）
        std::string data = pack_c2s(CMD_PLAYER_JOIN, fp_from_float(15.5f), fp_from_float(10.5f), 0, 0);
        netclient_send(ctx->netclient, data.c_str(), (int)data.size());
}

// ========== 发送输入 ==========
static void send_input(Context* ctx)
{
    if (!ctx->netclient) return;
    int dir = ctx->current_input_mask & INPUT_DIR_MASK;
    int attack = (ctx->current_input_mask & INPUT_ATTACK) ? 1 : 0;
    if (ctx->attack_triggered) { attack = 1; ctx->attack_triggered = false; }
    int ranged = (ctx->current_input_mask & INPUT_RANGED) ? 1 : 0;
    if (dir || attack || ranged) {
        auto data = pack_c2s(CMD_PLAYER_INPUT, 0, 0, dir, attack, ranged);
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
