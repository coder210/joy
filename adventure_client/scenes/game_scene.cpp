#include <map>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <joy/joy.h>
#include "game_scene.h"
#include "../../adventure_shared/protocol.h"
#include "../systems/animator_update_system.h"
#include "../systems/animator_render_system.h"
#include "../systems/interpolate_position_system.h"
#include "../systems/init_tilemap_system.h"
#include "../systems/effect_lifecycle_system.h"
#include "../systems/drawing_entity_system.h"
#include "../systems/drawing_attack_ray_effect_system.h"
#include "../systems/input_consume_system.h"
#include "../systems/movement_system.h"
#include "../layers/debug_layer.h"
#include "../layers/gameplay_controls_layer.h"
#include "../asset_manager.h"
#include "../network/network_utils.h"


// ========== 动画定义辅助函数 ==========
static entt::entity create_warrior_anim_def(entt::registry& registry, const char* tex_path) {
        auto def_entity = registry.create();
        AnimatorDef def;

        const int FPS = 6;
        const float DUR = 0.15f;
        const int FW = 192, FH = 192;

        // State 0: idle (row 0)
        {
                AnimStateDef s;
                s.name = "idle"; s.loop = true;
                for (int i = 0; i < FPS; i++)
                        s.clips.push_back({ tex_path, { (float)(i * FW), 0, (float)FW, (float)FH }, DUR });
                def.states.push_back(s);
        }
        // State 1: walk (row 1)
        {
                AnimStateDef s;
                s.name = "walk"; s.loop = true;
                for (int i = 0; i < FPS; i++)
                        s.clips.push_back({ tex_path, { (float)(i * FW), (float)(1 * FH), (float)FW, (float)FH }, DUR });
                def.states.push_back(s);
        }
        // State 2-5: attack (rows 4,2,2,6 for directions 下,左,右,上)
        const char* atk_names[] = { "attack_d", "attack_l", "attack_r", "attack_u" };
        int atk_rows[] = { 4, 2, 2, 6 };
        for (int d = 0; d < 4; d++) {
                AnimStateDef s;
                s.name = atk_names[d]; s.loop = false;
                for (int i = 0; i < FPS; i++)
                        s.clips.push_back({ tex_path, { (float)(i * FW), (float)(atk_rows[d] * FH), (float)FW, (float)FH }, DUR });
                def.states.push_back(s);
        }

        // Transitions
        // idle(0) ↔ walk(1) 带防抖 exit_time
        {
                AnimTransitionDef t;
                t.from_state = 0; t.to_state = 1;
                t.conditions.push_back({ "speed", AnimCondMode::GREATER, 0.5f });
                t.exit_time = 0.05f;
                def.transitions.push_back(t);
        }
        {
                AnimTransitionDef t;
                t.from_state = 1; t.to_state = 0;
                t.conditions.push_back({ "speed", AnimCondMode::LESS, 0.5f });
                t.exit_time = 0.05f;
                def.transitions.push_back(t);
        }
        // any → attack_dir (2~5), triggered by attack + dir == d
        for (int d = 0; d < 4; d++) {
                AnimTransitionDef t;
                t.from_state = -1; t.to_state = 2 + d;
                t.conditions.push_back({ "attack", AnimCondMode::TRIGGER, 0 });
                t.conditions.push_back({ "dir", AnimCondMode::EQUALS, (float)d });
                def.transitions.push_back(t);
        }
        // attack_dir → idle after 0.6s
        for (int d = 0; d < 4; d++) {
                AnimTransitionDef t;
                t.from_state = 2 + d; t.to_state = 0;
                t.exit_time = 0.6f;
                def.transitions.push_back(t);
        }

        def.default_state = 0;
        registry.emplace<AnimatorDef>(def_entity, def);
        return def_entity;
}

// 玩家状态机（使用 ECS PlayerActionComponent）

struct game_scene {
        scene_p scene;
        Context* ctx;


        // 远程玩家方向追踪（从静态局部变量移到 struct）
        std::map<entt::entity, int>  tracked_dir;
        std::map<entt::entity, fp_t> tracked_last_px;
        std::map<entt::entity, fp_t> tracked_last_py;
        float step_timer = 0;

};

// ----------------------------------------------------------------------
// 计算移动后的新位置
// ----------------------------------------------------------------------
static void calc_move_step(game_scene_p self, fp_t* out_x, fp_t* out_y, int input)
{
        fp_t delta = fp_from_float(self->ctx->FIXED_TIMESTEP);
        fp_t step = fp_mul(MOVE_SPEED, delta);

        *out_x = fp_zero();
        *out_y = fp_zero();

        if (input & INPUT_UP)    *out_y = fp_sub(*out_y, step);
        if (input & INPUT_DOWN)  *out_y = fp_add(*out_y, step);
        if (input & INPUT_LEFT)  *out_x = fp_sub(*out_x, step);
        if (input & INPUT_RIGHT) *out_x = fp_add(*out_x, step);
}

// ----------------------------------------------------------------------
// 检测并处理与所有实体的碰撞
// ----------------------------------------------------------------------
static void resolve_collision(game_scene_p self,
        LogicPositionComponent* p,
        LogicRectComponent& currRect,
        IdComponent& currId)
{
        auto body_view = self->ctx->ecs.view<IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : body_view) {
                auto& other_id = body_view.get<IdComponent>(e);
                auto& r = body_view.get<LogicRectComponent>(e);
                auto& other_pos = body_view.get<LogicPositionComponent>(e);
                if (other_id.id == currId.id) continue;

                rectanglef_t curr_rect = { p->x, p->y, currRect.width, currRect.height };
                rectanglef_t other_rect = { other_pos.x, other_pos.y, r.width, r.height };

                contact2df_t contact;
                if (collision2df_get_rectangles(curr_rect, fp_zero(),
                        other_rect, fp_zero(), &contact)) {
                        fp_t depth = contact.depth;
                        if (depth < fp_zero()) {
                                depth = fp_sub(fp_zero(), depth);
                        }
                        fp_t nx = fp_sub(fp_zero(), contact.normal.x);
                        fp_t ny = fp_sub(fp_zero(), contact.normal.y);
                        p->x = fp_add(p->x, fp_mul(nx, depth));
                        p->y = fp_add(p->y, fp_mul(ny, depth));
                }
        }
}

// 定义最近目标查找结果的结构体
struct nearest_attack_target {
        IdComponent* id = nullptr;
        ray2d_collisionf_t hit = { 0 };
        float distance = 999999.0f;
};

// 公共函数：从攻击者位置向上发射射线，找到最近的命中目标
static nearest_attack_target find_nearest_target_in_attack_ray(
        game_scene_p self,
        LogicPositionComponent* p,
        LogicRectComponent& currRect,
        IdComponent& currId)
{
        nearest_attack_target result;

        // 构造射线（起点为矩形上边中点，方向竖直向上）
        ray2df_t ray;
        ray.origin.x = fp_add(p->x, fp_mul(currRect.width, fp_half()));
        ray.origin.y = p->y;
        ray.direction.x = fp_from_float(0.0f);
        ray.direction.y = fp_from_float(-1.0f);

        // 遍历所有碰撞体
        auto body_view = self->ctx->ecs.view<IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : body_view) {
                auto& id = body_view.get<IdComponent>(e);
                auto& r = body_view.get<LogicRectComponent>(e);
                auto& pos = body_view.get<LogicPositionComponent>(e);
                if (currId.id == id.id) continue;

                rectanglef_t rect = { pos.x, pos.y, r.width, r.height };
                ray2d_collisionf_t coll = collision2df_get_ray_rectangle(ray, rect);

                if (coll.hit) {
                        float dist = fp_to_float(fp_sub(ray.origin.y, coll.point.y));
                        if (dist < result.distance) {
                                result.distance = dist;
                                result.hit = coll;
                                result.id = &id;
                        }
                }
        }

        return result;
}

// 通用碰撞盒攻击辅助（dir: 0=下 1=左 2=右 3=上）
// hit_enemies: true=只伤敌人（玩家攻击）, false=只伤玩家（敌人攻击）
static void attack_by_hitbox(game_scene_p self,
        LogicPositionComponent* p, LogicRectComponent& currRect, IdComponent& currId, int dir, bool hit_enemies)
{
        fp_t atk_range = fp_from_float(0.7f);
        fp_t ox = 0, oy = 0;
        if (dir == 0)      oy = atk_range;
        else if (dir == 1) ox = -atk_range;
        else if (dir == 2) ox = atk_range;
        else if (dir == 3) oy = -atk_range;
        rectanglef_t atk_rect = { fp_add(p->x, ox), fp_add(p->y, oy), currRect.width, currRect.height };
        auto body_view = self->ctx->ecs.view<IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : body_view) {
                auto& id = body_view.get<IdComponent>(e);
                auto& r = body_view.get<LogicRectComponent>(e);
                auto& pos = body_view.get<LogicPositionComponent>(e);
                if (currId.id == id.id) continue;
                if (hit_enemies ? !self->ctx->ecs.all_of<EnemyComponent>(e) : !self->ctx->ecs.all_of<PlayerComponent>(e)) continue;
                rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
                contact2df_t contact;
                if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
                }
        }
}

// 攻击（减血 + 特效）
static void attack_with_damage_and_effect(game_scene_p self,
        LogicPositionComponent* p, LogicRectComponent& currRect,
        IdComponent& currId, int conv, int input)
{
        attack_by_hitbox(self, p, currRect, currId, input_to_dir(input), true);
}

// 仅攻击（减血，无特效）
static void attack_damage_only(game_scene_p self,
        LogicPositionComponent* p, LogicRectComponent& currRect,
        IdComponent& currId, int conv, int input)
{
        attack_by_hitbox(self, p, currRect, currId, input_to_dir(input), true);
}

// 仅显示特效（不减血）
static void attack_effect_only(game_scene_p self, LogicPositionComponent* p,
        LogicRectComponent& currRect, IdComponent& currId, int conv, int input)
{
}

static void movement(game_scene_p self, LogicPositionComponent* p,
        LogicRectComponent& currRect,
        IdComponent& currId,
        int conv, int input)
{
        fp_t move_x, move_y;
        calc_move_step(self, &move_x, &move_y, input & (INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT));

        p->x = fp_add(p->x, move_x);
        p->y = fp_add(p->y, move_y);

        resolve_collision(self, p, currRect, currId);
}

// AI 更新：巡逻 → 追击 → 攻击
static void update_ai(game_scene_p self)
{
        auto enemy_view = self->ctx->ecs.view<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        auto player_view = self->ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : enemy_view) {
                auto& ai = enemy_view.get<AIComponent>(e);
                auto& id = enemy_view.get<IdComponent>(e);
                auto& rect = enemy_view.get<LogicRectComponent>(e);
                auto& pos = enemy_view.get<LogicPositionComponent>(e);

                fp_t speed = fp_from_float(2.0f);
                fp_t step = fp_mul(speed, fp_from_float(self->ctx->FIXED_TIMESTEP));

                fp_t detect_sq = fp_mul(ai.detect_radius, ai.detect_radius);
                fp_t nearest_dist_sq = detect_sq;
                fp_t nearest_player_x = 0, nearest_player_y = 0;
                bool found = false;

                for (auto pe : player_view) {
                        auto& pp = player_view.get<LogicPositionComponent>(pe);
                        fp_t dist_sq = collision2df_get_squared_distance_point_circle(
                                vec2f_t{ pp.x, pp.y }, circlef_t{ {pos.x, pos.y}, ai.detect_radius });
                        if (dist_sq < nearest_dist_sq) {
                                nearest_dist_sq = dist_sq;
                                nearest_player_x = pp.x;
                                nearest_player_y = pp.y;
                                found = true;
                        }
                }

                if (found) {
                        int atk_dir = 0;
                        fp_t dx = fp_sub(nearest_player_x, pos.x);
                        fp_t dy = fp_sub(nearest_player_y, pos.y);
                        bool h = (dx > 0 ? dx : -dx) > (dy > 0 ? dy : -dy);
                        atk_dir = h ? (dx > 0 ? 2 : 1) : (dy > 0 ? 0 : 3);
                        fp_t attack_sq = fp_mul(ai.attack_radius, ai.attack_radius);
                        if (nearest_dist_sq < attack_sq) {
                                ai.timer++;
                                if (ai.timer >= 60) {
                                        ai.timer = 0;
                                        ai.state = 2;
                                        attack_by_hitbox(self, &pos, rect, id, atk_dir, false);
                                        if (self->ctx->ecs.all_of<AnimParams>(e)) {
                                                auto& p = self->ctx->ecs.get<AnimParams>(e);
                                                p.attack = true;
                                                p.dir = atk_dir;
                                        }
                                }
                        }
                        else {
                                ai.state = 1;
                                fp_t dx = fp_sub(nearest_player_x, pos.x);
                                fp_t dy = fp_sub(nearest_player_y, pos.y);
                                fp_t dist = fp_sqrt(fp_add(fp_mul(dx, dx), fp_mul(dy, dy)));
                                if (dist > fp_zero()) {
                                        pos.x = fp_add(pos.x, fp_mul(fp_div(dx, dist), step));
                                        pos.y = fp_add(pos.y, fp_mul(fp_div(dy, dist), step));
                                        resolve_collision(self, &pos, rect, id);
                                }
                        }
                }
                else {
                        ai.state = 0;
                        fp_t dx = fp_sub(ai.target_x, pos.x);
                        fp_t dy = fp_sub(ai.target_y, pos.y);
                        fp_t dist_sq = fp_add(fp_mul(dx, dx), fp_mul(dy, dy));
                        fp_t arrived = fp_from_float(0.5f);
                        if (dist_sq < fp_mul(arrived, arrived)) {
                                fp_t range = ai.patrol_radius;
                                ai.target_x = fp_add(ai.patrol_center_x,
                                        fp_mul(fp_from_float((float)(rand() % 100 - 50)),
                                                fp_div(range, fp_from_float(50.0f))));
                                ai.target_y = fp_add(ai.patrol_center_y,
                                        fp_mul(fp_from_float((float)(rand() % 100 - 50)),
                                                fp_div(range, fp_from_float(50.0f))));
                        }
                        else {
                                fp_t dist = fp_sqrt(dist_sq);
                                pos.x = fp_add(pos.x, fp_mul(fp_div(dx, dist), step));
                                pos.y = fp_add(pos.y, fp_mul(fp_div(dy, dist), step));
                                resolve_collision(self, &pos, rect, id);
                        }
                }
        }
}


static void handle_command(game_scene_p self, int frame_id, int jn, int ln, int in,
        const S2CPlayerJoin* joins, const S2CPlayerLeave* leaves, const S2CPlayerInput* inputs)
{
        {
                auto iter = self->ctx->snapshots.find(self->ctx->server_frameid);
                if (iter != self->ctx->snapshots.end()) {
                        const auto& snapshots = iter->second;
                        auto pv = self->ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
                        for (auto e : pv) {
                                auto& id = pv.get<IdComponent>(e);
                                auto& pos = pv.get<LogicPositionComponent>(e);
                                auto it = snapshots.find(id.id);
                                if (it != snapshots.end()) {
                                        pos.x = it->second.position_x;
                                        pos.y = it->second.position_y;
                                }
                        }
                }
        }

        self->ctx->server_frameid = frame_id;

        // 1. 处理新玩家加入
        for (int i = 0; i < jn; i++) {
                auto& pj = joins[i];
                log_info("%d:CMD_PLAYER_JOIN", pj.conv);
                fp_t x = pj.position_x;
                fp_t y = pj.position_y;
                entt::entity anim_def = create_warrior_anim_def(self->ctx->ecs,
                        "adventure_client/textures/knights/troops/warrior/warrior_blue.png");
                entt::entity entity = self->ctx->ecs.create();
                self->ctx->ecs.emplace<IdComponent>(entity, self->ctx->server_entity_id++);
                self->ctx->ecs.emplace<LogicRectComponent>(entity, fp_from_float(.6f), fp_from_float(.6f));
                self->ctx->ecs.emplace<LogicPositionComponent>(entity, x, y);
                self->ctx->ecs.emplace<LogicVelocityComponent>(entity, fp_from_float(0), fp_from_float(0));
                self->ctx->ecs.emplace<TransformComponent>(entity, fp_to_float(x), fp_to_float(y), 0.0f, 1.0f, 1.0f);
                self->ctx->ecs.emplace<PlayerComponent>(entity, pj.conv);
                self->ctx->ecs.emplace<PlayerActionComponent>(entity);
                self->ctx->ecs.emplace<Animator>(entity, anim_def);
                self->ctx->ecs.emplace<AnimParams>(entity);
        }

        // 2. 处理玩家离开
        std::vector<entt::entity> to_destroy;
        for (int i = 0; i < ln; i++) {
                int conv = leaves[i].conv;
                auto pv = self->ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
                for (auto e : pv) {
                        auto& p = pv.get<PlayerComponent>(e);
                        if (p.conv == conv) to_destroy.push_back(e);
                }
        }
        for (auto e : to_destroy) self->ctx->ecs.destroy(e);

        // 5. 应用所有玩家的输入
        for (int i = 0; i < in; i++) {
                auto& input = inputs[i];
                int input_combined = input.dir | (input.attack ? INPUT_ATTACK : 0);
                auto pv = self->ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
                for (auto e : pv) {
                        auto& p = pv.get<PlayerComponent>(e);
                        auto& id = pv.get<IdComponent>(e);
                        auto& r = pv.get<LogicRectComponent>(e);
                        auto& pos = pv.get<LogicPositionComponent>(e);
                        if (p.conv == input.conv) {
                                if (input.conv == self->ctx->local_conv) {
                                        if (!self->ctx->pending_inputs.empty()
                                                && self->ctx->pending_inputs.front() == input_combined) {
                                                log_info("%d:%d", self->ctx->pending_inputs.front(), input_combined);
                                                self->ctx->pending_inputs.erase(self->ctx->pending_inputs.begin());
                                        }
                                        else {
                                                bool found = false;
                                                for (auto it = self->ctx->pending_inputs.begin();
                                                        it != self->ctx->pending_inputs.end(); ++it) {
                                                        if (*it == input_combined) {
                                                                log_error("%d:%d", *it, input_combined);
                                                                self->ctx->pending_inputs.erase(it);
                                                                found = true; break;
                                                        }
                                                }
                                                if (!found)
                                                        log_error("input:%d not found in pending", input_combined);
                                        }
                                        if (input.attack)
                                                attack_damage_only(self, &pos, r, id, input.conv, input_combined);
                                        movement(self, &pos, r, id, input.conv, input_combined);
                                }
                                else {
                                        if (input.attack)
                                                attack_with_damage_and_effect(self, &pos, r, id, input.conv, input_combined);
                                        movement(self, &pos, r, id, input.conv, input_combined);
                                }
                        }
                }
        }

        // 保存快照
        {
                std::map<int, state_snapshot> snapshots;
                auto pv = self->ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
                for (auto e : pv) {
                        auto& id = pv.get<IdComponent>(e);
                        auto& pos = pv.get<LogicPositionComponent>(e);
                        snapshots.insert({ id.id, { pos.x, pos.y } });
                }
                self->ctx->snapshots.insert({ self->ctx->server_frameid, snapshots });

                const size_t MAX_SNAPSHOT_FRAMES = 1000;
                while (self->ctx->snapshots.size() > MAX_SNAPSHOT_FRAMES)
                        self->ctx->snapshots.erase(self->ctx->snapshots.begin());
        }

        // 预测
        for (auto& input : self->ctx->pending_inputs) {
                auto pv = self->ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
                for (auto e : pv) {
                        auto& p = pv.get<PlayerComponent>(e);
                        auto& id = pv.get<IdComponent>(e);
                        auto& r = pv.get<LogicRectComponent>(e);
                        auto& pos = pv.get<LogicPositionComponent>(e);
                        if (p.conv == self->ctx->local_conv)
                                movement(self, &pos, r, id, p.conv, input);
                }
        }
        update_ai(self);
}


static void on_load(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        log_info("[game scene] start...");

        debug_layer_p debug_layer = create_debug_layer();
        gameplay_controls_layer_p gameplay_controls_layer = create_gameplay_controls_layer(self->ctx);
        scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));
        scene_add_root_node(self->scene, gameplay_controls_layer_get_node(gameplay_controls_layer));
        self->ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.2.39", 30000);

        // entt 不需要显式注册组件，也不需要 set_ctx
        // 创建 NetComponent 作为 singleton
        self->ctx->singleton_net = self->ctx->ecs.create();
        self->ctx->ecs.emplace<NetComponent>(self->ctx->singleton_net);

        // 注册 system
        add_system(self->ctx, OnStart,   "init_tilemap",       init_tilemap_system);
        add_system(self->ctx, PreUpdate, "input_consume",      input_consume_system);
        add_system(self->ctx, OnUpdate,  "movement",           movement_system);
        add_system(self->ctx, OnUpdate,  "animator_update",    animator_update_system);
        add_system(self->ctx, OnUpdate,  "interpolate",        interpolate_position_system);
        // 执行 OnStart phase
        run_phase(self->ctx, OnStart, 0);

        // ---- 音频 ----
        self->ctx->audio_dev = audio_open_device();
        self->ctx->step_sound = audio_shot_create("adventure_client/sounds/grass-step.wav", self->ctx->audio_dev);
        audio_shot_set_gain(self->ctx->step_sound, 1.0f);
        self->ctx->bgm = audio_bgm_create("adventure_client/sounds/bgm.wav", self->ctx->audio_dev);
        audio_bgm_set_gain(self->ctx->bgm, 0.5f);

}


static void fixed_update(game_scene_p self, float dt)
{
        if (!self->ctx->netclient) return;
        network_receive(self->ctx);

        if (!self->ctx->ready) return;

        //// 攻击触发：直接设置 ECS 状态 + 发送输入
        //if (self->ctx->attack_triggered) {
        //        self->ctx->attack_triggered = false;
        //        self->ctx->pending_inputs.push_back(INPUT_ATTACK);
        //        auto pv = self->ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        //        for (auto e : pv) {
        //                auto& p = pv.get<PlayerComponent>(e);
        //                auto& id = pv.get<IdComponent>(e);
        //                auto& r = pv.get<LogicRectComponent>(e);
        //                auto& pos = pv.get<LogicPositionComponent>(e);
        //                if (p.conv == self->ctx->local_conv) {
        //                        attack_effect_only(self, &pos, r, id, self->ctx->local_conv, INPUT_ATTACK);
        //                        if (self->ctx->ecs.all_of<PlayerActionComponent>(e)) {
        //                                auto& act = self->ctx->ecs.get<PlayerActionComponent>(e);
        //                                act.state = PlayerActionComponent::ATTACK;
        //                                act.dir = self->ctx->local_dir;
        //                        }
        //                        if (self->ctx->ecs.all_of<AnimParams>(e)) {
        //                                auto& params = self->ctx->ecs.get<AnimParams>(e);
        //                                params.attack = true;
        //                                params.dir = self->ctx->local_dir;
        //                        }
        //                }
        //        }
        //        {
        //                auto data = pack_c2s(CMD_PLAYER_INPUT, 0, 0, 0, 1);
        //                netclient_send(self->ctx->netclient, data.c_str(), (int)data.size());
        //        }
        //        return;
        //}

        //if (has_dir(self->ctx->current_input_mask)) {
        //        self->ctx->pending_inputs.push_back(self->ctx->current_input_mask);
        //        auto pv = self->ctx->ecs.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        //        for (auto e : pv) {
        //                auto& p = pv.get<PlayerComponent>(e);
        //                auto& id = pv.get<IdComponent>(e);
        //                auto& r = pv.get<LogicRectComponent>(e);
        //                auto& pos = pv.get<LogicPositionComponent>(e);
        //                if (p.conv == self->ctx->local_conv) {
        //                        movement(self, &pos, r, id, self->ctx->local_conv, self->ctx->current_input_mask);
        //                        if (self->ctx->ecs.all_of<AnimParams>(e)) {
        //                                auto& params = self->ctx->ecs.get<AnimParams>(e);
        //                                params.dir = input_to_dir(self->ctx->current_input_mask);
        //                        }
        //                        if (self->ctx->ecs.all_of<PlayerActionComponent>(e)) {
        //                                auto& act = self->ctx->ecs.get<PlayerActionComponent>(e);
        //                                act.dir = input_to_dir(self->ctx->current_input_mask);
        //                                if (act.state != PlayerActionComponent::ATTACK)
        //                                        act.state = PlayerActionComponent::WALK;
        //                        }
        //                }
        //        }
        //        auto data = pack_c2s(CMD_PLAYER_INPUT, 0, 0, self->ctx->current_input_mask, 0);
        //        netclient_send(self->ctx->netclient, data.c_str(), (int)data.size());
        //}

}

static void send_heartbeat(game_scene_p self, float dt)
{
        if (!self->ctx->netclient) return;
        self->ctx->heartbeatTimer += dt;
        if (self->ctx->heartbeatTimer >= 3.f) {
                self->ctx->heartbeatTimer = 0.f;
                auto d = pack_c2s(CMD_PLAYER_HEART, 0, 0, 0, 0);
                netclient_send(self->ctx->netclient, d.c_str(), d.size());
        }
}

static void client_on_keypressed(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg)
{
        game_scene_p self = (game_scene_p)arg;
        int key_mask = 0;
        if (strcmp(key, "w") == 0) key_mask = INPUT_UP;
        else if (strcmp(key, "W") == 0) key_mask = INPUT_UP;
        else if (strcmp(key, "s") == 0) key_mask = INPUT_DOWN;
        else if (strcmp(key, "S") == 0) key_mask = INPUT_DOWN;
        else if (strcmp(key, "a") == 0) key_mask = INPUT_LEFT;
        else if (strcmp(key, "A") == 0) key_mask = INPUT_LEFT;
        else if (strcmp(key, "d") == 0) key_mask = INPUT_RIGHT;
        else if (strcmp(key, "D") == 0) key_mask = INPUT_RIGHT;
        else if (strcmp(key, "space") == 0) key_mask = INPUT_ATTACK;
        if (key_mask) {
                self->ctx->current_input_mask |= key_mask;
                if (key_mask == INPUT_ATTACK) {

                }
                        //self->ctx->attack_triggered = true;
                if (key_mask & INPUT_DIR_MASK)
                        self->ctx->local_dir = input_to_dir(key_mask);
        }
}

static void client_on_keyreleased(const char* key, int scancode, void* arg)
{
        game_scene_p self = (game_scene_p)arg;
        int key_mask = 0;
        if (strcmp(key, "w") == 0) key_mask = INPUT_UP;
        else if (strcmp(key, "W") == 0) key_mask = INPUT_UP;
        else if (strcmp(key, "s") == 0) key_mask = INPUT_DOWN;
        else if (strcmp(key, "S") == 0) key_mask = INPUT_DOWN;
        else if (strcmp(key, "a") == 0) key_mask = INPUT_LEFT;
        else if (strcmp(key, "A") == 0) key_mask = INPUT_LEFT;
        else if (strcmp(key, "d") == 0) key_mask = INPUT_RIGHT;
        else if (strcmp(key, "D") == 0) key_mask = INPUT_RIGHT;
        else if (strcmp(key, "space") == 0) key_mask = INPUT_ATTACK;
        if (key_mask) self->ctx->current_input_mask &= ~key_mask;
}

static void on_handle_event(scene_p s, const void* ev)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);

        // 自定义触屏/UI 事件
        if (is_user_event(ev, GAMEPLAY_CONTROL_EVENT)) {
                int new_mask = self->ctx->current_input_mask;
                switch (get_event_code(ev)) {
                case GAMEPLAY_CONTROL_UP:    new_mask |= INPUT_UP; break;
                case GAMEPLAY_CONTROL_DOWN:  new_mask |= INPUT_DOWN; break;
                case GAMEPLAY_CONTROL_LEFT:  new_mask |= INPUT_LEFT; break;
                case GAMEPLAY_CONTROL_RIGHT: new_mask |= INPUT_RIGHT; break;
                case GAMEPLAY_CONTROL_RELEASE_UP:    new_mask &= ~INPUT_UP; break;
                case GAMEPLAY_CONTROL_RELEASE_DOWN:  new_mask &= ~INPUT_DOWN; break;
                case GAMEPLAY_CONTROL_RELEASE_LEFT:  new_mask &= ~INPUT_LEFT; break;
                case GAMEPLAY_CONTROL_RELEASE_RIGHT: new_mask &= ~INPUT_RIGHT; break;
                case GAMEPLAY_CONTROL_ATTACK:
                        self->ctx->attack_triggered = true;
                        break;
                default:
                        new_mask &= ~(INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT);
                        break;
                }
                if (new_mask != self->ctx->current_input_mask) {
                        self->ctx->current_input_mask = new_mask;
                }
                return;
        }

        jevent_callbacks_t cbs = {};
        cbs.on_keypressed = client_on_keypressed;
        cbs.on_keyreleased = client_on_keyreleased;
        dispatch_event(cbs, ev, self);
}

// ========== AI 系统 ==========

// 敌人碰撞盒攻击（只伤玩家）
static void ai_attack_target(game_scene_p self, LogicPositionComponent* p,
        LogicRectComponent& currRect, IdComponent& currId)
{
        rectanglef_t atk_rect = { p->x, p->y, currRect.width, currRect.height };
        auto body_view = self->ctx->ecs.view<IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : body_view) {
                auto& id = body_view.get<IdComponent>(e);
                auto& r = body_view.get<LogicRectComponent>(e);
                auto& pos = body_view.get<LogicPositionComponent>(e);
                if (currId.id == id.id) continue;
                if (!self->ctx->ecs.all_of<PlayerComponent>(e)) continue;
                rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
                contact2df_t contact;
                if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
                }
        }
}


static void on_update(scene_p s, float dt) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        if (self->ctx->netclient) {
                netclient_update(self->ctx->netclient);
                network_receive(self->ctx);
        }

        // === 固定步长逻辑 Tick（20Hz）===
        self->ctx->accumulator += dt;
        while (self->ctx->accumulator >= self->ctx->FIXED_TIMESTEP) {
            self->ctx->accumulator -= self->ctx->FIXED_TIMESTEP;
            send_input(self->ctx);
        }

        // 从队列取出命令，解析后写到 ECS 组件上
        if (!self->ctx->command_queue.empty()) {
                std::string command_data = self->ctx->command_queue.front();
                self->ctx->command_queue.pop();

                int frame_id, jn = 0, ln = 0, in = 0;
                S2CPlayerJoin joins[32];
                S2CPlayerLeave leaves[32];
                S2CPlayerInput inputs[64];
                bool ok = unpack_command(command_data.c_str(), (int)command_data.size(),
                        &frame_id, joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in);
                if (ok) {
                        auto& cmd = self->ctx->ecs.get<NetComponent>(self->ctx->singleton_net);
                        cmd.frame_id = ok ? frame_id : -1;
                        cmd.num_joins = jn;  memcpy(cmd.joins, joins, jn * sizeof(S2CPlayerJoin));
                        cmd.num_leaves = ln; memcpy(cmd.leaves, leaves, ln * sizeof(S2CPlayerLeave));
                        cmd.num_inputs = in; memcpy(cmd.inputs, inputs, in * sizeof(S2CPlayerInput));
                }

                // 统一 system 调度（PreUpdate → OnUpdate → PostUpdate）
                run_phase(self->ctx, PreUpdate, self->ctx->FIXED_TIMESTEP);
                run_phase(self->ctx, OnUpdate, self->ctx->FIXED_TIMESTEP);
                run_phase(self->ctx, PostUpdate, self->ctx->FIXED_TIMESTEP);
        }
}

static void on_render(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        r2d_set_color(self->ctx->r2d, 50, 120, 50, 255);

        // ---- tilemap 背景 ----
        if (self->ctx->tilemap && self->ctx->tilemap_render) {
                float cam_x_px = self->ctx->camera_x * (float)PIXELS_PER_METER;
                float cam_y_px = self->ctx->camera_y * (float)PIXELS_PER_METER;
                jtilemap_render_all(self->ctx->tilemap_render, self->ctx->tilemap, cam_x_px, cam_y_px, get_ticks());
        }

        // ---- ECS 实体渲染（有动画的用动画，没有的用方块）----
        auto draw_view = self->ctx->ecs.view<IdComponent, LogicRectComponent, TransformComponent>();
        for (auto e : draw_view) {
                auto& rect = draw_view.get<LogicRectComponent>(e);
                auto& t = draw_view.get<TransformComponent>(e);
                if (self->ctx->ecs.all_of<Animator, AnimParams, PlayerActionComponent>(e)) {
                        auto& anim = self->ctx->ecs.get<Animator>(e);
                        auto& act = self->ctx->ecs.get<PlayerActionComponent>(e);
                        animator_render_system(self->ctx, self->ctx->ecs, t, rect, anim, act);
                } else {
                        auto& id = draw_view.get<IdComponent>(e);
                        drawing_entity_system(self->ctx, e, id, rect, t);
                }
        }
}

static void on_destroy(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        if (self->ctx->netclient) {
                netclient_destroy(self->ctx->netclient);
                self->ctx->netclient = NULL;
        }
        AssetManager::GetInstance()->tex_cache_clear();
        audio_shot_destroy(self->ctx->step_sound);
        audio_bgm_destroy(self->ctx->bgm);
        if (self->ctx->audio_dev) audio_close_device(self->ctx->audio_dev);
        if (self->ctx->tilemap_render)
                jtilemap_render_destroy(self->ctx->tilemap_render);
        if (self->ctx->tilemap) jtilemap_destroy(self->ctx->tilemap);
        delete self;
}

game_scene_p game_scene_create(Context* ctx) {
        game_scene_p self = new game_scene();
        self->scene = scene_create("game");
        self->ctx = ctx;
        ctx->camera_x = 0.0f;
        ctx->camera_y = 0.0f;
        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_event_callback(self->scene, on_handle_event);
        scene_set_update_callback(self->scene, on_update);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);
        return self;
}

scene_p game_scene_get_scene(game_scene_p s) {
        return s ? s->scene : NULL;
}
