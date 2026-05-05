//#define SDL_MAIN_USE_CALLBACKS 1
//#include <SDL3/SDL_main.h>
//#include <joy2d/jsys.h>
//#include <joy2d/jcore.h>
//#include <joy2d/jmath.h>
//#include <joy2d/jutils.h>
//#include <joy2d/jtext.h>
//#include <joy2d/jcollision.h>
//#include <joy2d/jshapes.h>
//#include <string>
//#include <algorithm>
//#include "Systems.h"
//#include "Context.h"
//
//const int PIXELS_PER_METER = 50;
//
//static flecs::world world;
//
//static const fp_t MOVE_SPEED = fp_from_float(5.0f);
//
//// 输入掩码
//const int INPUT_UP = 1 << 0;
//const int INPUT_DOWN = 1 << 1;
//const int INPUT_LEFT = 1 << 2;
//const int INPUT_RIGHT = 1 << 3;
//const int INPUT_ATTACK = 1 << 4;
//
//
//// ----------------------------------------------------------------------
//// 计算移动后的新位置
//// ----------------------------------------------------------------------
//static void calc_move_step(fp_t* out_x, fp_t* out_y, int input)
//{
//        auto ctx = world.get_mut<Context>();
//        fp_t delta = fp_from_float(ctx->FIXED_TIMESTEP);
//        fp_t step = fp_mul(MOVE_SPEED, delta);
//
//        *out_x = fp_zero();
//        *out_y = fp_zero();
//
//        if (input & INPUT_UP)    *out_y = fp_sub(*out_y, step);
//        if (input & INPUT_DOWN)  *out_y = fp_add(*out_y, step);
//        if (input & INPUT_LEFT)  *out_x = fp_sub(*out_x, step);
//        if (input & INPUT_RIGHT) *out_x = fp_add(*out_x, step);
//}
//
//// ----------------------------------------------------------------------
//// 检测并处理与所有实体的碰撞
//// ----------------------------------------------------------------------
//static void resolve_collision(LogicPositionComponent* p,
//        LogicRectComponent& currRect, IdComponent& currId)
//{
//        auto ctx = world.get_mut<Context>();
//
//        ctx->body_query.each([&](IdComponent& other_id,
//                LogicRectComponent& r,
//                LogicPositionComponent& other_pos) {
//                        // 跳过自己
//                        if (other_id.id == currId.id) return;
//
//                        // 构建两个实体的矩形
//                        rectanglef_t curr_rect = { p->x, p->y, currRect.width, currRect.height };
//                        rectanglef_t other_rect = { other_pos.x, other_pos.y, r.width, r.height };
//
//                        // 检测碰撞
//                        contact2df_t contact;
//                        if (collision2df_get_rectangles(curr_rect, fp_zero(),
//                                other_rect, fp_zero(), &contact)) {
//                                // contact->depth 可能是负值（表示重叠），取绝对值确保为正
//                                fp_t depth = contact.depth;
//                                if (depth < fp_zero()) {
//                                        depth = fp_sub(fp_zero(), depth);
//                                }
//                                // 沿法线反方向分离（normal 从当前实体指向对方）
//                                fp_t nx = fp_sub(fp_zero(), contact.normal.x);
//                                fp_t ny = fp_sub(fp_zero(), contact.normal.y);
//                                p->x = fp_add(p->x, fp_mul(nx, depth));
//                                p->y = fp_add(p->y, fp_mul(ny, depth));
//                        }
//                });
//}
//
//// 定义最近目标查找结果的结构体
//struct NearestAttackTarget {
//        IdComponent* id = nullptr;
//        ray2d_collisionf_t hit = { 0 };
//        float distance = 999999.0f;
//};
//
//// 公共函数：从攻击者位置向上发射射线，找到最近的命中目标
//static NearestAttackTarget find_nearest_target_in_attack_ray(
//        LogicPositionComponent* p,
//        LogicRectComponent& currRect,
//        IdComponent& currId)
//{
//        auto ctx = world.get_mut<Context>();
//        NearestAttackTarget result;
//
//        // 构造射线（起点为矩形上边中点，方向竖直向上）
//        ray2df_t ray;
//        ray.origin.x = fp_add(p->x, fp_mul(currRect.width, fp_half()));
//        ray.origin.y = p->y;
//        ray.direction.x = fp_from_float(0.0f);
//        ray.direction.y = fp_from_float(-1.0f);
//
//        // 遍历所有碰撞体
//        ctx->body_query.each([&](IdComponent& id,
//                LogicRectComponent& r,
//                LogicPositionComponent& pos) {
//                        // 跳过自身
//                        if (currId.id == id.id) return;
//
//                        rectanglef_t rect = {
//                            pos.x,
//                            pos.y,
//                            r.width,
//                            r.height
//                        };
//                        ray2d_collisionf_t coll = collision2df_get_ray_rectangle(ray, rect);
//
//                        if (coll.hit) {
//                                // 垂直射线，距离 = 射线起点Y - 碰撞点Y
//                                float dist = fp_to_float(fp_sub(ray.origin.y, coll.point.y));
//                                if (dist < result.distance) {
//                                        result.distance = dist;
//                                        result.hit = coll;
//                                        result.id = &id;
//                                }
//                        }
//                });
//
//        return result;
//}
//
//// 攻击（减血 + 特效）
//static void attack_with_damage_and_effect(LogicPositionComponent* p,
//        LogicRectComponent& currRect,
//        IdComponent& currId,
//        int conv, int input)
//{
//        NearestAttackTarget target = find_nearest_target_in_attack_ray(p, currRect, currId);
//
//        if (target.id != nullptr) {
//                // 减血
//                if (target.id->hp > 0)
//                        target.id->hp--;
//                else
//                        target.id->hp = 0;
//
//                // 绘制攻击射线特效
//                float start_x = fp_to_float(target.hit.point.x);
//                float start_y = fp_to_float(target.hit.point.y);
//                float end_x = fp_to_float(p->x + fp_mul(currRect.width, fp_half()));
//                float end_y = fp_to_float(p->y);
//
//                SDL_FRect line;
//                line.w = 0.05f;
//                line.h = end_y - start_y;
//                line.x = start_x - line.w * 0.5f;
//                line.y = start_y;
//                world.entity().set<AttackRayEffectComponent>({ line.x, line.y, line.w, line.h, 0.1f });
//        }
//}
//
//// 仅攻击（减血，无特效）
//static void attack_damage_only(LogicPositionComponent* p,
//        LogicRectComponent& currRect,
//        IdComponent& currId,
//        int conv, int input)
//{
//        NearestAttackTarget target = find_nearest_target_in_attack_ray(p, currRect, currId);
//
//        if (target.id != nullptr) {
//                if (target.id->hp > 0)
//                        target.id->hp--;
//                else
//                        target.id->hp = 0;
//        }
//}
//
//// 仅显示特效（不减血）
//static void attack_effect_only(LogicPositionComponent* p,
//        LogicRectComponent& currRect,
//        IdComponent& currId,
//        int conv, int input)
//{
//        NearestAttackTarget target = find_nearest_target_in_attack_ray(p, currRect, currId);
//
//        if (target.id != nullptr) {
//                float start_x = fp_to_float(target.hit.point.x);
//                float start_y = fp_to_float(target.hit.point.y);
//                float end_x = fp_to_float(p->x + fp_mul(currRect.width, fp_half()));
//                float end_y = fp_to_float(p->y);
//
//                SDL_FRect line;
//                line.w = 0.05f;
//                line.h = end_y - start_y;
//                line.x = start_x - line.w * 0.5f;
//                line.y = start_y;
//                world.entity().set<AttackRayEffectComponent>({ line.x, line.y, line.w, line.h, 0.1f });
//        }
//}
//
//static void movement(LogicPositionComponent* p,
//        LogicRectComponent& currRect, IdComponent& currId, int conv, int input)
//{
//        // 计算移动量
//        fp_t move_x, move_y;
//        calc_move_step(&move_x, &move_y, input & (INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT));
//
//        // 先应用移动
//        p->x = fp_add(p->x, move_x);
//        p->y = fp_add(p->y, move_y);
//
//        // 然后检测并处理碰撞
//        resolve_collision(p, currRect, currId);
//}
//
//// ----------------------------------------------------------------------
//// 加载地图（收到 S2C_CMD_LOADING）
//// ----------------------------------------------------------------------
//static void HandleLoading(adventure::S2C* s2c)
//{
//        auto ctx = world.get_mut<Context>();
//        ctx->ready = true;
//        ctx->local_conv = s2c->map().conv();
//        ctx->server_frameid = s2c->map().frame_id();
//        ctx->server_entity_id = s2c->map().world().entity_id();
//
//        for (auto& entity : s2c->map().world().entities()) {
//                auto e = world.entity()
//                        .set<IdComponent>({ entity.id(), entity.hp() })
//                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
//                        .set<LogicPositionComponent>({ entity.position_x(), entity.position_y() })
//                        .set<TransformComponent>({ fp_to_float(entity.position_x()),
//                                                   fp_to_float(entity.position_y()), 0, 0, 0, 0 });
//                if (entity.type() == adventure::S2C_TYPE_PLAYER) {
//                        e.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) });
//                        e.set<PlayerComponent>({ entity.player_conv() });
//                }
//        }
//
//        // 保存当前快照（预测前的状态）
//        std::map<int, StateSnapshot> snapshots;
//        ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
//                LogicRectComponent& r, LogicPositionComponent& pos) {
//                        snapshots.insert({ id.id, { pos.x, pos.y } });
//                });
//        ctx->snapshots.insert({ ctx->server_frameid, snapshots });
//
//        // 告知服务器本地玩家加入
//        adventure::C2S c2s;
//        c2s.set_cmd(adventure::CMD_PLAYER_JOIN);
//        auto* join = c2s.mutable_player_join();
//        join->set_position_x(fp_from_float(1.2f));
//        join->set_position_y(fp_from_float(2.3f));
//        std::string data = c2s.SerializeAsString();
//        netclient_send(ctx->netclient, data.c_str(), data.size());
//}
//
//// ----------------------------------------------------------------------
//// 处理服务器命令帧（包含玩家加入/离开、输入、可选的世界快照）
//// ----------------------------------------------------------------------
//static void HandleCommand(adventure::S2C& s2c)
//{
//        auto ctx = world.get_mut<Context>();
//        {
//                // 先回滚到上一个快照状态（如果有的话），再应用服务器命令
//                auto iter = ctx->snapshots.find(ctx->server_frameid);
//                if (iter != ctx->snapshots.end()) {
//                        auto snapshots = iter->second;
//                        ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
//                                LogicRectComponent& r, LogicPositionComponent& pos) {
//                                        auto it = snapshots.find(id.id);
//                                        if (it != snapshots.end()) {
//                                                pos.x = it->second.position_x;
//                                                pos.y = it->second.position_y;
//                                        }
//                                });
//                }
//                else {
//                        int i = 0;
//                }
//        }
//
//        ctx->server_frameid = s2c.command().frame_id();
//
//        // 1. 处理新玩家加入
//        for (auto& player_join : s2c.command().player_joins()) {
//                log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
//                fp_t x = player_join.position_x();
//                fp_t y = player_join.position_y();
//                world.entity()
//                        .set<IdComponent>({ ctx->server_entity_id++, 10 })
//                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
//                        .set<LogicPositionComponent>({ x, y })
//                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
//                        .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 0, 0, 0 })
//                        .set<PlayerComponent>({ player_join.conv() });
//        }
//
//        // 2. 处理玩家离开
//        world.defer_begin();
//        for (auto& player_leave : s2c.command().player_leaves()) {
//                ctx->player_query.each([&](flecs::entity entity,
//                        PlayerComponent& p,
//                        IdComponent& id,
//                        LogicRectComponent& r,
//                        LogicPositionComponent& pos) {
//                                if (p.conv == player_leave.conv()) entity.destruct();
//                        });
//        }
//        world.defer_end();
//
//        // 5. 应用所有玩家的输入
//        for (auto& input : s2c.command().player_inputs()) {
//                ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
//                        LogicRectComponent& r, LogicPositionComponent& pos) {
//                                if (p.conv == input.conv()) {
//                                        if (input.conv() == ctx->local_conv) {
//                                                // 按先进先出（FIFO）确认输入
//                                                // 如果 keycode 匹配，移除最早的那个输入
//                                                if (!ctx->pending_inputs.empty()
//                                                        && ctx->pending_inputs.front() == input.keycode()) {
//                                                        log_info("%d:%d", ctx->pending_inputs.front(), input.keycode());
//                                                        ctx->pending_inputs.erase(ctx->pending_inputs.begin());
//                                                }
//                                                else {
//                                                        // keycode 不匹配，可能是乱序或重复
//                                                        // 尝试在队列中找到匹配的输入并移除
//                                                        bool found = false;
//                                                        for (auto it = ctx->pending_inputs.begin();
//                                                                it != ctx->pending_inputs.end(); ++it) {
//                                                                if (*it == input.keycode()) {
//                                                                        log_error("%d:%d", *it, input.keycode());
//                                                                        ctx->pending_inputs.erase(it);
//                                                                        found = true;
//                                                                        break;
//                                                                }
//                                                        }
//                                                        if (!found) {
//                                                                log_error("input:%d not found in pending", input.keycode());
//                                                        }
//                                                }
//                                                if (input.keycode() & INPUT_ATTACK) {
//                                                        attack_damage_only(&pos, r, id, input.conv(), input.keycode());
//                                                }
//                                                movement(&pos, r, id, input.conv(), input.keycode());
//                                        }
//                                        else {
//                                                if (input.keycode() & INPUT_ATTACK) {
//                                                        attack_with_damage_and_effect(&pos, r, id, input.conv(), input.keycode());
//                                                }
//                                                movement(&pos, r, id, input.conv(), input.keycode());
//                                        }
//                                }
//                        });
//        }
//
//        // 保存当前服务帧权威快照
//        {
//                std::map<int, StateSnapshot> snapshots;
//                ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
//                        LogicRectComponent& r, LogicPositionComponent& pos) {
//                                snapshots.insert({ id.id, { pos.x, pos.y } });
//                        });
//                ctx->snapshots.insert({ ctx->server_frameid, snapshots });
//        }
//
//        // 执行后面的预测
//        {
//                // 重放未确认的输入
//                for (auto& input : ctx->pending_inputs) {
//                        ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
//                                LogicRectComponent& r, LogicPositionComponent& pos) {
//                                        if (p.conv == ctx->local_conv) {
//                                                movement(&pos, r, id, p.conv, input);
//                                        }
//                                });
//                }
//        }
//
//}
//
//// ----------------------------------------------------------------------
//// 网络消息回调
//// ----------------------------------------------------------------------
//static void OnMessage(net_message_p msg, void*)
//{
//        auto ctx = world.get_mut<Context>();
//        if (msg->type == NET_TYPE_CONNECTED) {
//                adventure::C2S c2s;
//                c2s.set_cmd(adventure::CMD_LOADING);
//                std::string d = c2s.SerializeAsString();
//                netclient_send(ctx->netclient, d.c_str(), d.size());
//        }
//        else if (msg->type == NET_TYPE_MESSAGE) {
//                adventure::S2C s2c;
//                if (s2c.ParseFromArray(msg->data, msg->len)) {
//                        if (s2c.cmd() == adventure::S2C_CMD_LOADING) {
//                                HandleLoading(&s2c);
//                        }
//                        else if (s2c.cmd() == adventure::S2C_CMD_COMMAND) {
//                                HandleCommand(s2c);
//                        }
//                }
//        }
//        if (msg->data) SDL_free(msg->data);
//}
//
//// ----------------------------------------------------------------------
//// 心跳发送
//// ----------------------------------------------------------------------
//static void SendHeartbeat(float dt)
//{
//        auto ctx = world.get_mut<Context>();
//        ctx->heartbeatTimer += dt;
//        if (ctx->heartbeatTimer >= 3.f) {
//                ctx->heartbeatTimer = 0.f;
//                adventure::C2S c2s;
//                c2s.set_cmd(adventure::CMD_PLAYER_HEART);
//                std::string d = c2s.SerializeAsString();
//                netclient_send(ctx->netclient, d.c_str(), d.size());
//        }
//}
//
//// ----------------------------------------------------------------------
//// 固定步长更新（接收网络消息 + 预测发送）
//// ----------------------------------------------------------------------
//static void FixedUpdate(float dt)
//{
//        auto ctx = world.get_mut<Context>();
//
//        // --- 先处理网络消息（确认输入必须在发送新输入之前）---
//        // 避免 FixedUpdate push 新输入后，网络消息才处理旧确认导致的错位
//        net_message_t msg;
//        if (netclient_poll_message(ctx->netclient, &msg)) {
//                OnMessage(&msg, NULL);
//        }
//
//        // --- 预测：保存快照 -> 记录输入 -> 立即移动（预测） -> 发送到服务器 ---
//        if (ctx->ready) {
//                int send_mask = ctx->current_input_mask;
//                if (ctx->attack_triggered) {
//                        send_mask |= INPUT_ATTACK;
//                        ctx->attack_triggered = false;
//                }
//                if (send_mask != 0) {
//                        // 记录待确认输入
//                        ctx->pending_inputs.push_back(send_mask);
//
//                        // 立即应用输入（预测移动）
//                        ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
//                                LogicRectComponent& r, LogicPositionComponent& pos) {
//                                        if (p.conv == ctx->local_conv) {
//                                                if (send_mask & INPUT_ATTACK) {
//                                                        attack_effect_only(&pos, r, id, ctx->local_conv, send_mask);
//                                                }
//                                                movement(&pos, r, id, ctx->local_conv, send_mask);
//                                                return;
//                                        }
//                                });
//
//                        // 发送输入到服务器
//                        adventure::C2S c2s;
//                        c2s.set_cmd(adventure::CMD_PLAYER_INPUT);
//                        auto* pi = c2s.mutable_player_input();
//                        pi->set_keycode(send_mask);
//                        std::string data = c2s.SerializeAsString();
//                        netclient_send(ctx->netclient, data.c_str(), data.size());
//                }
//        }
//}
//
//// ----------------------------------------------------------------------
//// SDL3 回调函数
//// ----------------------------------------------------------------------
//SDL_AppResult SDL_AppInit(void**, int, char**)
//{
//        sys_init_netenv();
//        if (!SDL_Init(SDL_INIT_VIDEO)) {
//                SDL_Log("SDL init failed: %s", SDL_GetError());
//                return SDL_APP_FAILURE;
//        }
//
//        world.component<Context>();
//        world.component<ConnectionComponent>();
//        world.component<LogicRectComponent>();
//        world.component<LogicPositionComponent>();
//        world.component<LogicVelocityComponent>();
//        world.component<TransformComponent>();
//        world.component<PlayerComponent>();
//        world.set<Context>({});
//        auto ctx = world.get_mut<Context>();
//        game_timer_init(&ctx->game_timer);
//        game_timer_set_target_fps(&ctx->game_timer, 60);
//        game_timer_set_time_scale(&ctx->game_timer, 1.0f);
//        ctx->sample_fps = simple_fps_create();
//
//        SDL_CreateWindowAndRenderer("client", 640, 480, 0, &ctx->window, &ctx->renderer);
//        SDL_SetRenderLogicalPresentation(ctx->renderer, 640, 480, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_STRETCH);
//        ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.1.25", 10000);
//        //ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.2.61", 10000);
//        //ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "8.148.188.213", 10000);
//
//        // 检查网络客户端是否创建成功
//        if (!ctx->netclient) {
//                return SDL_APP_FAILURE;
//        }
//
//        netclient_set_callback(ctx->netclient, OnMessage, nullptr);
//
//        world.system<LogicPositionComponent, TransformComponent>().each(LerpSystem);
//        world.system<AttackRayEffectComponent>().each(EffectLifecycleSystem);
//
//        ctx->sync_query = world.query<IdComponent, LogicPositionComponent, TransformComponent>();
//        ctx->renderer_query = world.query<IdComponent, LogicRectComponent, TransformComponent>();
//        ctx->renderer_attack_rayeffect_query = world.query<AttackRayEffectComponent>();
//
//        ctx->player_query = world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
//        ctx->body_query = world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();
//
//        ctx->resources = new Resources(ctx->renderer);
//        ctx->debugLayer = new DebugLayer(ctx->resources);
//        ctx->mobileInputLayer = new MobileInputLayer(ctx->resources, ctx->renderer);
//
//        return SDL_APP_CONTINUE;
//}
//
//SDL_AppResult SDL_AppEvent(void*, SDL_Event* e)
//{
//        auto ctx = world.get_mut<Context>();
//
//        if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
//
//        // 键盘事件：更新输入掩码
//        if (e->type == SDL_EVENT_KEY_DOWN || e->type == SDL_EVENT_KEY_UP) {
//                bool is_down = (e->type == SDL_EVENT_KEY_DOWN);
//                int key_mask = 0;
//                switch (e->key.key) {
//                case SDLK_W: key_mask = INPUT_UP; break;
//                case SDLK_S: key_mask = INPUT_DOWN; break;
//                case SDLK_A: key_mask = INPUT_LEFT; break;
//                case SDLK_D: key_mask = INPUT_RIGHT; break;
//                case SDLK_J: key_mask = INPUT_ATTACK; break;
//                case SDLK_Q: return SDL_APP_SUCCESS;
//                default: break;
//                }
//                if (key_mask) {
//                        if (is_down) {
//                                ctx->current_input_mask |= key_mask;
//                                if (key_mask == INPUT_ATTACK) {
//                                        ctx->attack_triggered = true;
//                                }
//                        }
//                        else {
//                                ctx->current_input_mask &= ~key_mask;
//                        }
//                }
//        }
//        // 移动输入事件（触屏）
//        else if (e->type == MOBILE_INPUT_EVENT) {
//                int new_mask = ctx->current_input_mask;
//                switch (e->user.code) {
//                case MOBILE_INPUT_UP:    new_mask |= INPUT_UP; break;
//                case MOBILE_INPUT_DOWN:  new_mask |= INPUT_DOWN; break;
//                case MOBILE_INPUT_LEFT:  new_mask |= INPUT_LEFT; break;
//                case MOBILE_INPUT_RIGHT: new_mask |= INPUT_RIGHT; break;
//                case MOBILE_INPUT_RELEASE_UP:    new_mask &= ~INPUT_UP; break;
//                case MOBILE_INPUT_RELEASE_DOWN:  new_mask &= ~INPUT_DOWN; break;
//                case MOBILE_INPUT_RELEASE_LEFT:  new_mask &= ~INPUT_LEFT; break;
//                case MOBILE_INPUT_RELEASE_RIGHT: new_mask &= ~INPUT_RIGHT; break;
//                case MOBILE_INPUT_ATTACK:
//                        ctx->attack_triggered = true;
//                        break;
//                default:
//                        new_mask &= ~(INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT);
//                        break;
//                }
//                if (new_mask != ctx->current_input_mask) {
//                        ctx->current_input_mask = new_mask;
//                }
//        }
//
//        ctx->mobileInputLayer->ListenEvent(e);
//        return ctx->running ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
//}
//
//SDL_AppResult SDL_AppIterate(void*)
//{
//        auto ctx = world.get_mut<Context>();
//        game_timer_update(&ctx->game_timer);
//        float dt = game_timer_get_delta_time(&ctx->game_timer);
//        ctx->accumulator += dt;
//
//        netclient_update(ctx->netclient);
//
//        int fps = simple_fps_update(ctx->sample_fps);
//
//        // ---------- 固定步长物理更新（60Hz） ----------
//        if (ctx->accumulator >= ctx->FIXED_TIMESTEP) {
//                FixedUpdate(ctx->FIXED_TIMESTEP);
//                ctx->accumulator -= ctx->FIXED_TIMESTEP;
//        }
//
//        world.progress(dt);
//
//        // ---------- 心跳发送（1Hz，可保留原样或也独立） ----------
//        // // 原 SendHeartbeat 内部已使用 heartbeat_timer 做1秒限制，可以继续在 iterate 中调用，
//        // 但为了避免依赖 FixedLogicUpdate，现在直接调用即可（内部计时器决定是否真正发送）
//        SendHeartbeat(dt);
//
//        SDL_SetRenderDrawColor(ctx->renderer, 100, 100, 100, 255);
//        SDL_RenderClear(ctx->renderer);
//
//        ctx->renderer_query.each(RendererSystem);
//        ctx->renderer_attack_rayeffect_query.each(RendererAttackRayEffectSystem);
//
//        ctx->debugLayer->Update(ctx->server_frameid, fps);
//        ctx->debugLayer->Draw(ctx->renderer);
//        ctx->mobileInputLayer->Update();
//        ctx->mobileInputLayer->Draw();
//
//        SDL_RenderPresent(ctx->renderer);
//        return SDL_APP_CONTINUE;
//}
//
//void SDL_AppQuit(void*, SDL_AppResult)
//{
//        auto ctx = world.get_mut<Context>();
//        delete ctx->resources;
//        delete ctx->debugLayer;
//        delete ctx->mobileInputLayer;
//        simple_fps_destory(ctx->sample_fps);
//        netclient_destroy(ctx->netclient);
//        SDL_DestroyRenderer(ctx->renderer);
//        SDL_DestroyWindow(ctx->window);
//        sys_release_netenv();
//        google::protobuf::ShutdownProtobufLibrary();
//}