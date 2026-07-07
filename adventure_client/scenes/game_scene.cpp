#include <map>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <joy/joy.h>
#include "game_scene.h"
#include "../../adventure_shared/protocol.h"
#include "../systems/animator_update_system.h"
#include "../systems/animator_render_system.h"
#include "../systems/lerp_system.h"
#include "../systems/init_tilemap_system.h"
#include "../systems/effect_lifecycle_system.h"
#include "../systems/drawing_entity_system.h"
#include "../systems/drawing_attack_ray_effect_system.h"
#include "../systems/input_consume_system.h"
#include "../layers/debug_layer.h"
#include "../layers/gameplay_controls_layer.h"
#include "../asset_manager.h"


static const fp_t MOVE_SPEED = fp_from_float(5.0f);



// ========== 动画定义辅助函数 ==========
static flecs::entity create_warrior_anim_def(flecs::world& world, const char* tex_path) {
        auto def_entity = world.entity();
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
        def_entity.set<AnimatorDef>(def);
        return def_entity;
}

// 玩家状态机（使用 ECS PlayerActionComponent）

struct game_scene {
        scene_p scene;
        context* ctx;


        // 远程玩家方向追踪（从静态局部变量移到 struct）
        std::map<flecs::entity_t, int>  tracked_dir;
        std::map<flecs::entity_t, fp_t> tracked_last_px;
        std::map<flecs::entity_t, fp_t> tracked_last_py;
        float step_timer = 0;

};

static void handle_loading(game_scene_p self, int conv, int frame_id, int eid, const S2CEntity* entities, int ec)
{
        self->ctx->ready = true;
        self->ctx->local_conv = conv;
        self->ctx->server_frameid = frame_id;
        self->ctx->server_entity_id = eid;

        // 创建动画定义实体
        //flecs::entity player_anim_def = create_warrior_anim_def(self->ctx->ecs,
                //"adventure_client/textures/knights/troops/warrior/warrior_blue.png");
        //flecs::entity enemy_anim_def = create_warrior_anim_def(self->ctx->ecs,
                //"adventure_client/textures/knights/troops/warrior/warrior_red.png");

        for (int i = 0; i < ec; i++) {
                auto& entity = entities[i];
                int64_t hp = entity.hp;
                auto e = self->ctx->ecs.entity()
                        .set<IdComponent>({ entity.id })
                        .set<LogicRectComponent>({ entity.width, entity.height })
                        .set<LogicPositionComponent>({ entity.position_x, entity.position_y })
                        .set<TransformComponent>({ fp_to_float(entity.position_x),
                                                   fp_to_float(entity.position_y), 0, 1, 1 });
                if (entity.type == S2C_TYPE_PLAYER) {
                        e.set<PlayerActionComponent>({})
                                //.set<Animator>({ player_anim_def })
                                //.set<AnimParams>({})
                                .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                                .set<PlayerComponent>({ entity.player_conv });
                }
                else if (entity.type == S2C_TYPE_ENEMY) {
                        e.add<EnemyComponent>()
                                //.set<AIComponent>({ 0, 0,
                                 //       fp_from_float(4.0f), fp_from_float(15.0f),
                                  //      fp_from_float(1.5f), 0, 0, 0, 0 })
                                .set<PlayerActionComponent>({})
                                //.set<Animator>({ enemy_anim_def })
                                //.set<AnimParams>({})
                                .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) });
                }
        }

        // 保存当前快照
        std::map<int, state_snapshot> snapshots;
        self->ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
                LogicRectComponent& r, LogicPositionComponent& pos) {
                        snapshots.insert({ id.id, { pos.x, pos.y } });
                });
        self->ctx->snapshots.insert({ self->ctx->server_frameid, snapshots });

        // 告知服务器本地玩家加入
        auto data = pack_c2s(CMD_PLAYER_JOIN, fp_from_float(3.2f), fp_from_float(2.3f), 0, 0);
        netclient_send(self->ctx->netclient, data.c_str(), (int)data.size());
}

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
        self->ctx->body_query.each([&](IdComponent& other_id,
                LogicRectComponent& r,
                LogicPositionComponent& other_pos) {
                        // 跳过自己
                        if (other_id.id == currId.id) return;

                        // 构建两个实体的矩形
                        rectanglef_t curr_rect = { p->x, p->y, currRect.width, currRect.height };
                        rectanglef_t other_rect = { other_pos.x, other_pos.y, r.width, r.height };

                        // 检测碰撞
                        contact2df_t contact;
                        if (collision2df_get_rectangles(curr_rect, fp_zero(),
                                other_rect, fp_zero(), &contact)) {
                                // contact->depth 可能是负值（表示重叠），取绝对值确保为正
                                fp_t depth = contact.depth;
                                if (depth < fp_zero()) {
                                        depth = fp_sub(fp_zero(), depth);
                                }
                                // 沿法线反方向分离（normal 从当前实体指向对方）
                                fp_t nx = fp_sub(fp_zero(), contact.normal.x);
                                fp_t ny = fp_sub(fp_zero(), contact.normal.y);
                                p->x = fp_add(p->x, fp_mul(nx, depth));
                                p->y = fp_add(p->y, fp_mul(ny, depth));
                        }
                });
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
        self->ctx->body_query.each([&](IdComponent& id,
                LogicRectComponent& r,
                LogicPositionComponent& pos) {
                        // 跳过自身
                        if (currId.id == id.id) return;

                        rectanglef_t rect = {
                            pos.x,
                            pos.y,
                            r.width,
                            r.height
                        };
                        ray2d_collisionf_t coll = collision2df_get_ray_rectangle(ray, rect);

                        if (coll.hit) {
                                // 垂直射线，距离 = 射线起点Y - 碰撞点Y
                                float dist = fp_to_float(fp_sub(ray.origin.y, coll.point.y));
                                if (dist < result.distance) {
                                        result.distance = dist;
                                        result.hit = coll;
                                        result.id = &id;
                                }
                        }
                });

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
        self->ctx->body_query.each([&](flecs::entity e, IdComponent& id, LogicRectComponent& r, LogicPositionComponent& pos) {
                if (currId.id == id.id) return;
                if (hit_enemies ? !e.has<EnemyComponent>() : !e.has<PlayerComponent>()) return;
                rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
                contact2df_t contact;
                if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
                        //if (id.hp > 0) id.hp--;
                }
                });
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
        // 预测特效已注释（无实际伤害，仅本地视觉预览）
}

static void movement(game_scene_p self, LogicPositionComponent* p,
        LogicRectComponent& currRect,
        IdComponent& currId,
        int conv, int input)
{
        // 计算移动量
        fp_t move_x, move_y;
        calc_move_step(self, &move_x, &move_y, input & (INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT));

        // 先应用移动
        p->x = fp_add(p->x, move_x);
        p->y = fp_add(p->y, move_y);

        // 然后检测并处理碰撞
        resolve_collision(self, p, currRect, currId);
}

// AI 更新：巡逻 → 追击 → 攻击
static void update_ai(game_scene_p self)
{
        self->ctx->enemy_query.each([&](flecs::entity e, EnemyComponent&, AIComponent& ai,
                IdComponent& id, LogicRectComponent& rect, LogicPositionComponent& pos) {

                        fp_t speed = fp_from_float(2.0f);
                        fp_t step = fp_mul(speed, fp_from_float(self->ctx->FIXED_TIMESTEP));

                        // 查找检测范围内最近玩家
                        fp_t detect_sq = fp_mul(ai.detect_radius, ai.detect_radius);
                        fp_t nearest_dist_sq = detect_sq;
                        fp_t nearest_player_x = 0, nearest_player_y = 0;
                        bool found = false;

                        self->ctx->player_query.each([&](PlayerComponent&, IdComponent&,
                                LogicRectComponent&, LogicPositionComponent& pp) {
                                        fp_t dist_sq = collision2df_get_squared_distance_point_circle(
                                                vec2f_t{ pp.x, pp.y }, circlef_t{ {pos.x, pos.y}, ai.detect_radius });
                                        if (dist_sq < nearest_dist_sq) {
                                                nearest_dist_sq = dist_sq;
                                                nearest_player_x = pp.x;
                                                nearest_player_y = pp.y;
                                                found = true;
                                        }
                                });

                        if (found) {
                                // 检测到玩家
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
                                                ai.state = 2; // 攻击
                                                attack_by_hitbox(self, &pos, rect, id, atk_dir, false); // 敌人攻击只伤玩家
                                                if (e.has<AnimParams>()) {
                                                        auto* p = e.get_mut<AnimParams>();
                                                        p->attack = true;
                                                        p->dir = atk_dir;
                                                }
                                        }
                                }
                                else {
                                        ai.state = 1; // 追击
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
                                // 巡逻
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
                });
}


static void handle_command(game_scene_p self, int frame_id, int jn, int ln, int in,
        const S2CPlayerJoin* joins, const S2CPlayerLeave* leaves, const S2CPlayerInput* inputs)
{
        {
                auto iter = self->ctx->snapshots.find(self->ctx->server_frameid);
                if (iter != self->ctx->snapshots.end()) {
                        const auto& snapshots = iter->second;
                        self->ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
                                LogicRectComponent& r, LogicPositionComponent& pos) {
                                        auto it = snapshots.find(id.id);
                                        if (it != snapshots.end()) {
                                                pos.x = it->second.position_x;
                                                pos.y = it->second.position_y;
                                        }
                                });
                }
        }

        self->ctx->server_frameid = frame_id;

        // 1. 处理新玩家加入
        for (int i = 0; i < jn; i++) {
                auto& pj = joins[i];
                log_info("%d:CMD_PLAYER_JOIN", pj.conv);
                fp_t x = pj.position_x;
                fp_t y = pj.position_y;
                flecs::entity anim_def = create_warrior_anim_def(self->ctx->ecs,
                        "adventure_client/textures/knights/troops/warrior/warrior_blue.png");
                self->ctx->ecs.entity()
                        .set<IdComponent>({ self->ctx->server_entity_id++ })
                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
                        .set<LogicPositionComponent>({ x, y })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 1, 1 })
                        .set<PlayerComponent>({ pj.conv })
                        .set<PlayerActionComponent>({})
                        .set<Animator>({ anim_def })
                        .set<AnimParams>({});
        }

        // 2. 处理玩家离开
        self->ctx->ecs.defer_begin();
        for (int i = 0; i < ln; i++) {
                int conv = leaves[i].conv;
                self->ctx->player_query.each([&](flecs::entity entity,
                        PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv == conv) entity.destruct();
                        });
        }
        self->ctx->ecs.defer_end();

        // 5. 应用所有玩家的输入
        for (int i = 0; i < in; i++) {
                auto& input = inputs[i];
                int input_combined = input.dir | (input.attack ? INPUT_ATTACK : 0);
                self->ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
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
                        });
        }

        // 保存快照
        {
                std::map<int, state_snapshot> snapshots;
                self->ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                snapshots.insert({ id.id, { pos.x, pos.y } });
                        });
                self->ctx->snapshots.insert({ self->ctx->server_frameid, snapshots });

                // 限制快照增长，最多保留 1000 帧
                const size_t MAX_SNAPSHOT_FRAMES = 1000;
                while (self->ctx->snapshots.size() > MAX_SNAPSHOT_FRAMES)
                        self->ctx->snapshots.erase(self->ctx->snapshots.begin());
        }

        // 预测
        for (auto& input : self->ctx->pending_inputs) {
                self->ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv == self->ctx->local_conv)
                                        movement(self, &pos, r, id, p.conv, input);
                        });
        }
        update_ai(self);
}

//static void handle_command_s2c(game_scene_p self, const char* data, int len)
//{
//        int frame_id, jn = 0, ln = 0, in = 0;
//        S2CPlayerJoin joins[32]; S2CPlayerLeave leaves[32]; S2CPlayerInput inputs[64];
//        if (unpack_command(data, len, &frame_id, joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in))
//                handle_command(self, frame_id, jn, ln, in, joins, leaves, inputs);
//}


static void network_receive(net_message_p msg, void* arg)
{
        game_scene_p self = (game_scene_p)arg;
        if (msg->type == NET_TYPE_CONNECTED) {
                auto d = pack_c2s(CMD_LOADING, 0, 0, 0, 0);
                netclient_send(self->ctx->netclient, d.c_str(), (int)d.size());
        }
        else if (msg->type == NET_TYPE_DISCONNECTED) {
                log_info("disconnected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                // 先解析首元素 cmd 决定是 loading 还是 command
                jmsgpack_unpacked_t r;
                jmsgpack_unpacked_init(&r);
                size_t off = 0;
                int is_loading = 0;
                if (jmsgpack_unpack_next(&r, (const char*)msg->data, (int)msg->len, &off) >= 0
                        && r.data.type == JMSGPACK_OBJECT_ARRAY && r.data.via.array.size >= 2
                        && r.data.via.array.ptr[0].type == JMSGPACK_OBJECT_POSITIVE_INTEGER)
                        is_loading = (r.data.via.array.ptr[0].via.u64 == S2C_CMD_LOADING);
                jmsgpack_unpacked_destroy(&r);

                if (is_loading) {
                        int conv, frame_id, eid;
                        S2CEntity entities[512];
                        int ec = unpack_loading((const char*)msg->data, (int)msg->len, &conv, &frame_id, &eid, entities, 512);
                        if (ec >= 0)
                                handle_loading(self, conv, frame_id, eid, entities, ec);
                }
                else {
                        self->ctx->command_queue.push(std::string(msg->data, msg->len));
                }
        }
        if (msg->type == NET_TYPE_MESSAGE && msg->data)
                netmessage_free_data(msg);
}

static void on_load(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        log_info("[game scene] start...");

        debug_layer_p debug_layer = create_debug_layer();
        gameplay_controls_layer_p gameplay_controls_layer = create_gameplay_controls_layer(self->ctx);
        scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));
        scene_add_root_node(self->scene, gameplay_controls_layer_get_node(gameplay_controls_layer));
        self->ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.1.5", 30000);
        self->ctx->ecs.component<ConnectionComponent>();
        self->ctx->ecs.component<IdComponent>();
        self->ctx->ecs.component<LogicRectComponent>();
        self->ctx->ecs.component<LogicPositionComponent>();
        self->ctx->ecs.component<LogicVelocityComponent>();
        self->ctx->ecs.component<TransformComponent>();
        self->ctx->ecs.component<PlayerComponent>();
        self->ctx->ecs.component<AttackRayEffectComponent>();
        self->ctx->ecs.component<PlayerActionComponent>();
        self->ctx->ecs.component<AIComponent>();
        self->ctx->ecs.component<EnemyComponent>();
        self->ctx->ecs.component<AnimatorDef>();
        self->ctx->ecs.component<Animator>();
        self->ctx->ecs.component<AnimParams>();

        self->ctx->ecs.add<NetComponent>();

        self->ctx->ecs.add<NetComponent>();
        log_info("NetComponent count: %d", self->ctx->ecs.filter<NetComponent>().count());

        /*  self->ctx->ecs.system<LogicPositionComponent, TransformComponent>().each(lerp_system);
          self->ctx->ecs.system<AttackRayEffectComponent>().each(effect_lifecycle_system);
          self->ctx->ecs.system<Animator, AnimParams, PlayerActionComponent>()
                  .each(animator_update_system);*/
        self->ctx->sync_query = self->ctx->ecs.query<IdComponent, LogicPositionComponent, TransformComponent>();
        self->ctx->drawing_entity_query = self->ctx->ecs.query<IdComponent, LogicRectComponent, TransformComponent>();
        self->ctx->attack_ray_effect_query = self->ctx->ecs.query<AttackRayEffectComponent>();
        self->ctx->player_query = self->ctx->ecs.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        self->ctx->body_query = self->ctx->ecs.query<IdComponent, LogicRectComponent, LogicPositionComponent>();
        self->ctx->enemy_query = self->ctx->ecs.query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();

        // 预构建渲染 query
        self->ctx->render_player_query = self->ctx->ecs.query<TransformComponent, LogicRectComponent,
                Animator, AnimParams, PlayerActionComponent, PlayerComponent>();
        self->ctx->render_enemy_query = self->ctx->ecs.query<TransformComponent, LogicRectComponent,
                Animator, AnimParams, PlayerActionComponent, EnemyComponent>();
        self->ctx->healthbar_query = self->ctx->ecs.query<IdComponent, TransformComponent>();

        self->ctx->ecs.system("init_tilemap")
                .kind(flecs::OnStart)
                .iter(init_tilemap_system);

        // 帧同步：消费输入 + 收集命令（由外部固定步长累加器驱动，不需要 .rate()）
        self->ctx->ecs.system<NetComponent>()
                .kind(flecs::PreUpdate)
                .iter(input_consume_system);


        // ---- 音频 ----
        self->ctx->audio_dev = audio_open_device();
        self->ctx->step_sound = audio_shot_create("adventure_client/sounds/grass-step.wav", self->ctx->audio_dev);
        audio_shot_set_gain(self->ctx->step_sound, 1.0f);
        self->ctx->bgm = audio_bgm_create("adventure_client/sounds/bgm.wav", self->ctx->audio_dev);
        audio_bgm_set_gain(self->ctx->bgm, 0.5f);

}


static void fixedupdate(game_scene_p self, float dt)
{
        if (!self->ctx->netclient) return;
        net_message_t msg;
        if (netclient_poll_message(self->ctx->netclient, &msg)) {
                network_receive(&msg, self);
        }

        if (!self->ctx->ready) return;

        // 攻击触发：直接设置 ECS 状态 + 发送输入
        if (self->ctx->attack_triggered) {
                self->ctx->attack_triggered = false;
                self->ctx->pending_inputs.push_back(INPUT_ATTACK);
                self->ctx->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv == self->ctx->local_conv) {
                                        attack_effect_only(self, &pos, r, id, self->ctx->local_conv, INPUT_ATTACK);
                                        if (e.has<PlayerActionComponent>()) {
                                                auto* act = e.get_mut<PlayerActionComponent>();
                                                act->state = PlayerActionComponent::ATTACK;
                                                act->dir = self->ctx->local_dir;
                                        }
                                        if (e.has<AnimParams>()) {
                                                auto* params = e.get_mut<AnimParams>();
                                                params->attack = true;
                                                params->dir = self->ctx->local_dir;
                                        }
                                }
                        });
                {
                        auto data = pack_c2s(CMD_PLAYER_INPUT, 0, 0, 0, 1);
                        netclient_send(self->ctx->netclient, data.c_str(), (int)data.size());
                }
                return;
        }

        // 攻击中不处理移动
        bool is_attacking = false;
        self->ctx->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent&,
                LogicRectComponent&, LogicPositionComponent&) {
                        if (p.conv == self->ctx->local_conv && e.has<PlayerActionComponent>())
                                is_attacking = (e.get<PlayerActionComponent>()->state == PlayerActionComponent::ATTACK);
                });
        if (is_attacking) return;

        if (has_dir(self->ctx->current_input_mask)) {
                self->ctx->pending_inputs.push_back(self->ctx->current_input_mask);
                self->ctx->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv == self->ctx->local_conv) {
                                        movement(self, &pos, r, id, self->ctx->local_conv, self->ctx->current_input_mask);
                                        // 预测移动时立即更新方向，使翻转在当前帧生效
                                        if (e.has<AnimParams>()) {
                                                auto* params = e.get_mut<AnimParams>();
                                                params->dir = input_to_dir(self->ctx->current_input_mask);
                                        }
                                        if (e.has<PlayerActionComponent>()) {
                                                auto* act = e.get_mut<PlayerActionComponent>();
                                                act->dir = input_to_dir(self->ctx->current_input_mask);
                                                if (act->state != PlayerActionComponent::ATTACK)
                                                        act->state = PlayerActionComponent::WALK;
                                        }
                                }
                        });
                auto data = pack_c2s(CMD_PLAYER_INPUT, 0, 0, self->ctx->current_input_mask, 0);
                netclient_send(self->ctx->netclient, data.c_str(), (int)data.size());
        }

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
                if (key_mask == INPUT_ATTACK)
                        self->ctx->attack_triggered = true;
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

        // 标准键盘/鼠标事件 → 通过 dispatch_event 分发
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
        self->ctx->body_query.each([&](flecs::entity e, IdComponent& id, LogicRectComponent& r, LogicPositionComponent& pos) {
                if (currId.id == id.id) return;
                if (!e.has<PlayerComponent>()) return; // 敌人只攻击玩家
                rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
                contact2df_t contact;
                if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
                        //if (id.hp > 0) id.hp--;
                }
                });
}


static void on_update(scene_p s, float dt) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        if (self->ctx->netclient) {
                netclient_update(self->ctx->netclient);
        }
        net_message_t msg;
        if (netclient_poll_message(self->ctx->netclient, &msg)) {
                network_receive(&msg, self);
        }

        // 从队列取出命令，解析后写到 ECS 组件上
        if (!self->ctx->command_queue.empty()) {
                std::string command_data = self->ctx->command_queue.front();
                self->ctx->command_queue.pop();

                // 解析命令数据
                int frame_id, jn = 0, ln = 0, in = 0;
                S2CPlayerJoin joins[32];
                S2CPlayerLeave leaves[32];
                S2CPlayerInput inputs[64];
                bool ok = unpack_command(command_data.c_str(), (int)command_data.size(),
                        &frame_id, joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in);
                if (ok) {
                        auto* cmd = self->ctx->ecs.get_mut<NetComponent>();
                        cmd->frame_id = ok ? frame_id : -1;
                        cmd->num_joins = jn;  memcpy(cmd->joins, joins, jn * sizeof(S2CPlayerJoin));
                        cmd->num_leaves = ln; memcpy(cmd->leaves, leaves, ln * sizeof(S2CPlayerLeave));
                        cmd->num_inputs = in; memcpy(cmd->inputs, inputs, in * sizeof(S2CPlayerInput));
                }

                self->ctx->ecs.progress(self->ctx->FIXED_TIMESTEP);
        }

        //// ---------- 同步 PlayerAction → AnimParams（在 ECS systems 之前） ----------
        //self->ctx->ecs.defer_begin();
        //self->player_query.each([&](flecs::entity e, PlayerComponent&, IdComponent&,
        //        LogicRectComponent&, LogicPositionComponent&) {
        //                if (e.has<AnimParams>() && e.has<PlayerActionComponent>()) {
        //                        auto* params = e.get_mut<AnimParams>();
        //                        auto* act = e.get<PlayerActionComponent>();
        //                        params->speed = (act->state == PlayerActionComponent::WALK) ? 1.0f : 0.0f;
        //                        params->dir = act->dir;
        //                }
        //        });
        //self->ctx->ecs.defer_end();

        //self->ctx->ecs.progress(dt);

        //// ---------- 心跳 ----------
        //send_heartbeat(self, dt);

        //// ---------- 摄像机跟随 + 远程状态同步 + 脚步音效 ----------
        //self->ctx->ecs.defer_begin();
        //{
        //        self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent&,
        //                LogicRectComponent&, LogicPositionComponent& pos) {
        //                        if (p.conv == (int)self->local_conv) {
        //                                // 摄像机跟随
        //                                float target_cam_x = fp_to_float(pos.x) - 6.4f;
        //                                float target_cam_y = fp_to_float(pos.y) - 4.8f;
        //                                float lerp = dt * 5.0f;
        //                                if (lerp > 1.0f) lerp = 1.0f;
        //                                self->ctx->camera_x += (target_cam_x - self->ctx->camera_x) * lerp;
        //                                self->ctx->camera_y += (target_cam_y - self->ctx->camera_y) * lerp;

        //                                // 本地玩家状态同步 + AnimParams 同步（不覆盖 ATTACK）
        //                                if (e.has<PlayerActionComponent>()) {
        //                                        auto* act = e.get_mut<PlayerActionComponent>();
        //                                        if (act->state != PlayerActionComponent::ATTACK) {
        //                                                act->state = has_dir(self->current_input_mask) ? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
        //                                                act->dir = self->local_dir;
        //                                        }
        //                                }
        //                                // 同时同步 AnimParams，避免与 pre-sync 产生一帧偏差
        //                                if (e.has<AnimParams>()) {
        //                                        auto* params = e.get_mut<AnimParams>();
        //                                        const auto* act = e.get<PlayerActionComponent>();
        //                                        params->speed = (act->state == PlayerActionComponent::WALK) ? 1.0f : 0.0f;
        //                                        params->dir = act->dir;
        //                                }

        //                                // 脚步音效
        //                                if (e.has<PlayerActionComponent>() && e.get<PlayerActionComponent>()->state == PlayerActionComponent::WALK) {
        //                                        self->step_timer += dt;
        //                                        if (self->step_timer >= 0.30f) {
        //                                                if (self->step_sound) audio_shot_play(self->step_sound);
        //                                                self->step_timer = 0;
        //                                        }
        //                                }
        //                                else { self->step_timer = 0; }
        //                        }
        //                        else {
        //                                // 远程玩家方向追踪
        //                                flecs::entity_t eid = e;
        //                                auto& ldi = self->remote_dirs[eid];
        //                                auto& lx = self->tracked_last_px[eid];
        //                                auto& ly = self->tracked_last_py[eid];
        //                                if (lx == 0 && ly == 0) { lx = pos.x; ly = pos.y; }
        //                                fp_t dx = fp_sub(pos.x, lx), dy = fp_sub(pos.y, ly);
        //                                fp_t th = fp_from_float(0.001f);
        //                                int new_dir = ldi;
        //                                if (dx > th)        new_dir = 2;
        //                                else if (dx < -th)  new_dir = 1;
        //                                if (dy < -th)       new_dir = 3;
        //                                else if (dy > th)   new_dir = 0;
        //                                bool moved = (dx > th || dx < -th || dy > th || dy < -th);
        //                                if (moved) { lx = pos.x; ly = pos.y; ldi = new_dir; }
        //                                if (e.has<PlayerActionComponent>()) {
        //                                        auto* act = e.get_mut<PlayerActionComponent>();
        //                                        act->dir = new_dir;
        //                                        if (act->state != PlayerActionComponent::ATTACK)
        //                                                act->state = moved ? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
        //                                }
        //                                // 同步到 AnimParams
        //                                if (e.has<AnimParams>()) {
        //                                        auto* params = e.get_mut<AnimParams>();
        //                                        params->speed = moved ? 1.0f : 0.0f;
        //                                        params->dir = new_dir;
        //                                }
        //                        }
        //                });
        //}
        //self->ctx->ecs.defer_end();
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

        //// ---- ECS 实体（碰撞体） ----
        //self->drawing_entity_query.each(drawing_entity_system);
        //self->attack_ray_effect_query.each(drawing_attack_ray_effect_system);

        //// ---- ECS 精灵渲染（玩家） ----
        //self->render_player_query.each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
        //        Animator& anim, AnimParams& params, PlayerActionComponent& act, PlayerComponent&) {
        //                animator_render_system(it.entity(i), t, r, anim, act);
        //        });

        //// ---- ECS 精灵渲染（敌人） ----
        //self->render_enemy_query.each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
        //        Animator& anim, AnimParams& params, PlayerActionComponent& act, EnemyComponent&) {
        //                animator_render_system(it.entity(i), t, r, anim, act);
        //        });

        //// ---- 血条渲染（所有有血量实体头顶） ----
        //{
        //        context* ctx = self->ctx;
        //        self->healthbar_query.each([ctx](flecs::iter& it, size_t i, IdComponent& id, TransformComponent& t) {
        //                flecs::entity e = it.entity(i);
        //                if (!e.has<PlayerComponent>() && !e.has<EnemyComponent>()) return;
        //                if (id.hp <= 0) return;

        //                float sx = (t.position_x - ctx->camera_x) * PIXELS_PER_METER;
        //                float sy = (t.position_y - ctx->camera_y) * PIXELS_PER_METER;
        //                // 精灵 192px, 碰撞盒 ~30px, 精灵顶部 = sy - (192-30)/2 = sy - 81
        //                const float SPRITE_OFF = 81.0f;
        //                float bar_w = 48.0f, bar_h = 8.0f;
        //                float bar_x = sx - bar_w / 2.0f, bar_y = sy - SPRITE_OFF - bar_h - 4.0f;
        //                // 黑色边框
        //                jrect_t bg = { bar_x - 1, bar_y - 1, bar_w + 2, bar_h + 2 };
        //                r2d_set_color(ctx->r2d, 0, 0, 0, 255);
        //                shape_draw_rectangle(ctx->r2d, "fill", bg);
        //                // 深灰背景
        //                jrect_t base = { bar_x, bar_y, bar_w, bar_h };
        //                r2d_set_color(ctx->r2d, 40, 40, 40, 255);
        //                shape_draw_rectangle(ctx->r2d, "fill", base);
        //                // 彩色填充
        //                float ratio = (float)id.hp / 10.0f;
        //                if (ratio > 1.0f) ratio = 1.0f;
        //                if (ratio < 0) ratio = 0;
        //                jrect_t fill = { bar_x, bar_y, bar_w * ratio, bar_h };
        //                if (ratio > 0.6f)      r2d_set_color(ctx->r2d, 60, 210, 60, 255);
        //                else if (ratio > 0.3f) r2d_set_color(ctx->r2d, 210, 210, 60, 255);
        //                else                   r2d_set_color(ctx->r2d, 220, 60, 60, 255);
        //                shape_draw_rectangle(ctx->r2d, "fill", fill);
        //                });
        //}
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

game_scene_p game_scene_create(context* ctx) {
        game_scene_p self = new game_scene();
        self->scene = scene_create("game");
        self->ctx = ctx;
        self->ctx->ecs.set_ctx(ctx);
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
