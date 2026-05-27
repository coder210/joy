#define STB_IMAGE_IMPLEMENTATION
#include <joy/external/stb_image.h>
#include <unordered_map>
#include <string>

#include "../protocol/adventure.pb.h"
#include "../flecs.h"
#include "game_scene.h"
#include <SDL3/SDL.h>
#include <joy/jmath.h>
#include <joy/jcore.h>
#include <joy/jtext.h>
#include <joy/jcollision.h>
#include <joy/jnetwork.h>
#include <joy/jtilemap.h>
#include <joy/jtilemap_render.h>
#include <joy/jaudio.h>
#include "../components/sprite_sheet_component.h"
#include "../components/player_action_component.h"
#include "../systems/animation_update_system.h"
#include "../systems/sprite_render_system.h"
#include "../layers/debug_layer.h"
#include "../layers/gameplay_controls_layer.h"
#include "../components/connection_component.h"
#include "../components/logic_velocity_component.h"
#include "../systems/lerp_system.h"
#include "../systems/effect_lifecycle_system.h"
#include "../systems/drawing_entity_system.h"
#include "../systems/drawing_attack_ray_effect_system.h"


static const fp_t MOVE_SPEED = fp_from_float(5.0f);

// 输入掩码
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;
const int INPUT_ATTACK = 1 << 4;

// 状态快照（用于预测回滚）
struct state_snapshot {
        fp_t position_x, position_y;
};

// 玩家状态机（使用 ECS PlayerActionComponent）

// 纹理缓存
static std::unordered_map<std::string, SDL_Texture*> g_tex_cache;

SDL_Texture* tex_cache_get(SDL_Renderer* r, const char* path) {
    std::string k(path);
    auto it = g_tex_cache.find(k);
    if (it != g_tex_cache.end()) return it->second;
    int w, h, ch;
    auto* d = stbi_load(path, &w, &h, &ch, 4);
    if (!d) return nullptr;
    auto* s = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, d, w * 4);
    if (!s) { stbi_image_free(d); return nullptr; }
    auto* t = SDL_CreateTextureFromSurface(r, s);
    SDL_DestroySurface(s);
    stbi_image_free(d);
    g_tex_cache[k] = t;
    return t;
}

void tex_cache_clear() {
    for (auto& kv : g_tex_cache)
        if (kv.second) SDL_DestroyTexture(kv.second);
    g_tex_cache.clear();
}

struct game_scene {
        scene_p scene;
        context* ctx;
        netclient_p netclient;
        flecs::world ecs_world;

        // ---- tilemap + 音频 ----
        jtilemap_p          tilemap = NULL;
        uint32_t            audio_dev = 0;
        audio_shot_p        step_sound = NULL;
        audio_bgm_p         bgm = NULL;
        int local_dir = 0; // 本地玩家方向
        // 远程玩家方向追踪
        std::map<flecs::entity_t, int>  remote_dirs;

        std::map<int, int> server_inputs;
        bool ready = false;
        int server_frameid = 0;
        int server_entity_id = 0;

        // 固定逻辑步
        Uint64 lastTime = SDL_GetTicks();
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 50.0f;

        int current_input_mask = 0;
        float heartbeatTimer = 0.0f;

        bool attack_triggered = false;   // 攻击键按下待发送

        // ========== 预测回滚相关 ==========
        uint32_t local_conv = 0;                     // 本地玩家的 conv（连接标识）
        std::map<int, std::map<int, state_snapshot>> snapshots;        // 按 frame_id 递增存储的历史快照
        std::vector<int> pending_inputs;     // 已发送但未确认的输入

        flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
        flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
        flecs::query<IdComponent, LogicPositionComponent, TransformComponent> sync_query;
        flecs::query<IdComponent, LogicRectComponent, TransformComponent> drawing_entity_query;
        flecs::query<AttackRayEffectComponent> attack_ray_effect_query;

};

static void handle_loading(game_scene_p self, adventure::S2C* s2c)
{
        self->ready = true;
        self->local_conv = s2c->map().conv();
        self->server_frameid = s2c->map().frame_id();
        self->server_entity_id = s2c->map().world().entity_id();

        for (auto& entity : s2c->map().world().entities()) {
                auto e = self->ecs_world.entity()
                        .set<IdComponent>({ entity.id(), entity.hp() })
                        .set<LogicRectComponent>({ entity.width(), entity.height() })
                        .set<LogicPositionComponent>({ entity.position_x(), entity.position_y() })
                        .set<TransformComponent>({ fp_to_float(entity.position_x()),
                                                   fp_to_float(entity.position_y()), 0, 1, 1 });
                if (entity.type() == adventure::S2C_TYPE_PLAYER) {
                        SpriteSheetComponent ss{};
                        snprintf(ss.path, sizeof(ss.path), "joy2d_editor_textures/knights/troops/warrior/warrior_blue.png");
                        ss.frame_w = 192; ss.frame_h = 192;
                        ss.idle_row = 0; ss.walk_row = 1;
                        ss.atk_rows[0] = 4; ss.atk_rows[1] = 2;
                        ss.atk_rows[2] = 2; ss.atk_rows[3] = 6;
                        ss.frame_count = 6; ss.frame_duration = 0.15f;
                        e.set<PlayerActionComponent>({});
                        e.set<SpriteSheetComponent>(ss);
                        e.set<AnimationFrameComponent>({});
                        e.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) });
                        e.set<PlayerComponent>({ entity.player_conv() });
                }
        }

        // 保存当前快照（预测前的状态）
        std::map<int, state_snapshot> snapshots;
        self->player_query.each([&](PlayerComponent& p, IdComponent& id,
                LogicRectComponent& r, LogicPositionComponent& pos) {
                        snapshots.insert({ id.id, { pos.x, pos.y } });
                });
        self->snapshots.insert({ self->server_frameid, snapshots });

        // 告知服务器本地玩家加入
        adventure::C2S c2s;
        c2s.set_cmd(adventure::CMD_PLAYER_JOIN);
        auto* join = c2s.mutable_player_join();
        join->set_position_x(fp_from_float(1.2f));
        join->set_position_y(fp_from_float(2.3f));
        std::string data = c2s.SerializeAsString();
        netclient_send(self->netclient, data.c_str(), data.size());
}

// ----------------------------------------------------------------------
// 计算移动后的新位置
// ----------------------------------------------------------------------
static void calc_move_step(game_scene_p self, fp_t* out_x, fp_t* out_y, int input)
{
        fp_t delta = fp_from_float(self->FIXED_TIMESTEP);
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
        self->body_query.each([&](IdComponent& other_id,
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
        self->body_query.each([&](IdComponent& id,
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

// 攻击（减血 + 特效）
static void attack_with_damage_and_effect(game_scene_p self,
        LogicPositionComponent* p, LogicRectComponent& currRect,
        IdComponent& currId,
        int conv, int input)
{
        nearest_attack_target target = find_nearest_target_in_attack_ray(self, p, currRect, currId);

        if (target.id != nullptr) {
                // 减血
                if (target.id->hp > 0)
                        target.id->hp--;
                else
                        target.id->hp = 0;

                // 绘制攻击射线特效
                float start_x = fp_to_float(target.hit.point.x);
                float start_y = fp_to_float(target.hit.point.y);
                float end_x = fp_to_float(p->x + fp_mul(currRect.width, fp_half()));
                float end_y = fp_to_float(p->y);

                SDL_FRect line;
                line.w = 0.05f;
                line.h = end_y - start_y;
                line.x = start_x - line.w * 0.5f;
                line.y = start_y;
                self->ecs_world.entity().set<AttackRayEffectComponent>({
                        line.x, line.y, line.w, line.h, 0.1f
                        });
        }
}

// 仅攻击（减血，无特效）
static void attack_damage_only(game_scene_p self,
        LogicPositionComponent* p,
        LogicRectComponent& currRect,
        IdComponent& currId,
        int conv, int input)
{
        nearest_attack_target target;
        target = find_nearest_target_in_attack_ray(self, p, currRect, currId);

        if (target.id != nullptr) {
                if (target.id->hp > 0)
                        target.id->hp--;
                else
                        target.id->hp = 0;
        }
}

// 仅显示特效（不减血）
static void attack_effect_only(game_scene_p self, LogicPositionComponent* p,
        LogicRectComponent& currRect,
        IdComponent& currId,
        int conv, int input)
{
        nearest_attack_target target;
        target = find_nearest_target_in_attack_ray(self, p, currRect, currId);
        if (target.id != nullptr) {
                float start_x = fp_to_float(target.hit.point.x);
                float start_y = fp_to_float(target.hit.point.y);
                float end_x = fp_to_float(p->x + fp_mul(currRect.width, fp_half()));
                float end_y = fp_to_float(p->y);

                SDL_FRect line;
                line.w = 0.05f;
                line.h = end_y - start_y;
                line.x = start_x - line.w * 0.5f;
                line.y = start_y;
                self->ecs_world.entity().set<AttackRayEffectComponent>({
                        line.x, line.y, line.w, line.h, 0.1f });
        }
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



static void handle_command(game_scene_p self, adventure::S2C& s2c)
{
        {
                // 先回滚到上一个快照状态（如果有的话），再应用服务器命令
                auto iter = self->snapshots.find(self->server_frameid);
                if (iter != self->snapshots.end()) {
                        auto snapshots = iter->second;
                        self->player_query.each([&](PlayerComponent& p, IdComponent& id,
                                LogicRectComponent& r, LogicPositionComponent& pos) {
                                        auto it = snapshots.find(id.id);
                                        if (it != snapshots.end()) {
                                                pos.x = it->second.position_x;
                                                pos.y = it->second.position_y;
                                        }
                                });
                }
        }

        self->server_frameid = s2c.command().frame_id();

        // 1. 处理新玩家加入
        for (auto& player_join : s2c.command().player_joins()) {
                log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
                fp_t x = player_join.position_x();
                fp_t y = player_join.position_y();
                SpriteSheetComponent ss{};
                snprintf(ss.path, sizeof(ss.path), "joy2d_editor_textures/knights/troops/warrior/warrior_blue.png");
                ss.frame_w = 192; ss.frame_h = 192;
                ss.idle_row = 0; ss.walk_row = 1;
                ss.atk_rows[0] = 4; ss.atk_rows[1] = 2;
                ss.atk_rows[2] = 2; ss.atk_rows[3] = 6;
                ss.frame_count = 6; ss.frame_duration = 0.15f;
                self->ecs_world.entity()
                        .set<IdComponent>({ self->server_entity_id++, 10 })
                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
                        .set<LogicPositionComponent>({ x, y })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 1, 1 })
                        .set<PlayerComponent>({ player_join.conv() })
                        .set<PlayerActionComponent>({})
                        .set<SpriteSheetComponent>(ss)
                        .set<AnimationFrameComponent>({});
        }

        // 2. 处理玩家离开
        self->ecs_world.defer_begin();
        for (auto& player_leave : s2c.command().player_leaves()) {
                self->player_query.each([&](flecs::entity entity,
                        PlayerComponent& p,
                        IdComponent& id,
                        LogicRectComponent& r,
                        LogicPositionComponent& pos) {
                                if (p.conv == player_leave.conv()) {
                                        entity.destruct();
                                }
                        });
        }
        self->ecs_world.defer_end();

        // 5. 应用所有玩家的输入
        for (auto& input : s2c.command().player_inputs()) {
                self->player_query.each([&](PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv == input.conv()) {
                                        if (input.conv() == self->local_conv) {
                                                // 按先进先出（FIFO）确认输入
                                                // 如果 keycode 匹配，移除最早的那个输入
                                                if (!self->pending_inputs.empty()
                                                        && self->pending_inputs.front() == input.keycode()) {
                                                        log_info("%d:%d", self->pending_inputs.front(), input.keycode());
                                                        self->pending_inputs.erase(self->pending_inputs.begin());
                                                }
                                                else {
                                                        // keycode 不匹配，可能是乱序或重复
                                                        // 尝试在队列中找到匹配的输入并移除
                                                        bool found = false;
                                                        for (auto it = self->pending_inputs.begin();
                                                                it != self->pending_inputs.end(); ++it) {
                                                                if (*it == input.keycode()) {
                                                                        log_error("%d:%d", *it, input.keycode());
                                                                        self->pending_inputs.erase(it);
                                                                        found = true;
                                                                        break;
                                                                }
                                                        }
                                                        if (!found) {
                                                                log_error("input:%d not found in pending", input.keycode());
                                                        }
                                                }
                                                if (input.keycode() & INPUT_ATTACK) {
                                                        attack_damage_only(self, &pos, r, id, input.conv(), input.keycode());
                                                }
                                                movement(self, &pos, r, id, input.conv(), input.keycode());
                                        }
                                        else {
                                                if (input.keycode() & INPUT_ATTACK) {
                                                        attack_with_damage_and_effect(self, &pos, r, id, input.conv(), input.keycode());
                                                }
                                                movement(self, &pos, r, id, input.conv(), input.keycode());
                                        }
                                }
                        });
        }

        // 保存当前服务帧权威快照
        {
                std::map<int, state_snapshot> snapshots;
                self->player_query.each([&](PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                snapshots.insert({ id.id, { pos.x, pos.y } });
                        });
                self->snapshots.insert({ self->server_frameid, snapshots });
        }

        // 执行后面的预测
        {
                // 重放未确认的输入
                for (auto& input : self->pending_inputs) {
                        self->player_query.each([&](PlayerComponent& p, IdComponent& id,
                                LogicRectComponent& r, LogicPositionComponent& pos) {
                                        if (p.conv == self->local_conv) {
                                                movement(self, &pos, r, id, p.conv, input);
                                        }
                                });
                }
        }

}


static void on_message(net_message_p msg, void* arg)
{
        game_scene_p self = (game_scene_p)arg;
        if (msg->type == NET_TYPE_CONNECTED) {
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_LOADING);
                std::string d = c2s.SerializeAsString();
                netclient_send(self->netclient, d.c_str(), d.size());
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                adventure::S2C s2c;
                if (s2c.ParseFromArray(msg->data, msg->len)) {
                        if (s2c.cmd() == adventure::S2C_CMD_LOADING) {
                                handle_loading(self, &s2c);
                        }
                        else if (s2c.cmd() == adventure::S2C_CMD_COMMAND) {
                                handle_command(self, s2c);
                        }
                }
        }
        if (msg->data) SDL_free(msg->data);
}

static void on_load(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        log_info("[game scene] start...");

        debug_layer_p debug_layer = create_debug_layer();
        gameplay_controls_layer_p gameplay_controls_layer = create_gameplay_controls_layer(self->ctx);
        scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));
        scene_add_root_node(self->scene, gameplay_controls_layer_get_node(gameplay_controls_layer));

        self->ecs_world.component<ConnectionComponent>();
        self->ecs_world.component<IdComponent>();
        self->ecs_world.component<LogicRectComponent>();
        self->ecs_world.component<LogicPositionComponent>();
        self->ecs_world.component<LogicVelocityComponent>();
        self->ecs_world.component<TransformComponent>();
        self->ecs_world.component<PlayerComponent>();
        self->ecs_world.component<AttackRayEffectComponent>();
        self->ecs_world.component<SpriteSheetComponent>();
        self->ecs_world.component<AnimationFrameComponent>();
        self->ecs_world.component<PlayerActionComponent>();

        //self->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.1.28", 10000);
        self->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.2.42", 10000);
        //self->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "8.148.188.213", 10000);

        self->ecs_world.system<LogicPositionComponent, TransformComponent>().each(lerp_system);
        self->ecs_world.system<AttackRayEffectComponent>().each(effect_lifecycle_system);
        self->ecs_world.system<PlayerActionComponent, SpriteSheetComponent, AnimationFrameComponent>()
                .each(animation_update_system);
        self->sync_query = self->ecs_world.query<IdComponent, LogicPositionComponent, TransformComponent>();
        self->drawing_entity_query = self->ecs_world.query<IdComponent, LogicRectComponent, TransformComponent>();
        self->attack_ray_effect_query = self->ecs_world.query<AttackRayEffectComponent>();
        self->player_query = self->ecs_world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        self->body_query = self->ecs_world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();

        // ---- tilemap 渲染 ----
        self->tilemap = jtilemap_load_file("joy2d_editor_tilemap/map.lua");
        if (self->tilemap) {
                jtilemap_render_init(self->ctx->renderer, NULL, NULL);
                log_info("[tilemap] loaded %dx%d", self->tilemap->width, self->tilemap->height);
        } else log_error("[tilemap] failed to load");

        // ---- 音频 ----
        self->audio_dev = audio_open_device();
        self->step_sound = audio_shot_create("joy2d_editor_sounds/ak47.wav", self->audio_dev);
        audio_shot_set_gain(self->step_sound, 1.0f);
        self->bgm = audio_bgm_create("joy2d_editor_sounds/bgm.wav", self->audio_dev);
        audio_bgm_set_gain(self->bgm, 0.5f);

}


static void fixedupdate(game_scene_p self, float dt)
{
	if (!self->netclient) return;
	net_message_t msg;
	if (netclient_poll_message(self->netclient, &msg)) {
		on_message(&msg, self);
	}

	if (!self->ready) return;

	// 攻击触发：直接设置 ECS 状态 + 发送输入
	if (self->attack_triggered) {
		self->attack_triggered = false;
		self->pending_inputs.push_back(INPUT_ATTACK);
		self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
			if (p.conv == self->local_conv) {
				attack_effect_only(self, &pos, r, id, self->local_conv, INPUT_ATTACK);
				e.set<PlayerActionComponent>({PlayerActionComponent::ATTACK, self->local_dir, 0, 0});
			}
		});
		adventure::C2S c2s;
		c2s.set_cmd(adventure::CMD_PLAYER_INPUT);
		c2s.mutable_player_input()->set_keycode(INPUT_ATTACK);
		std::string data = c2s.SerializeAsString();
		netclient_send(self->netclient, data.c_str(), data.size());
		return;
	}

	// 攻击中不处理移动（从 ECS 组件状态判断）
	bool attacking = false;
	self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent&,
		LogicRectComponent&, LogicPositionComponent&) {
		if (p.conv == self->local_conv && e.has<PlayerActionComponent>())
			attacking = (e.get<PlayerActionComponent>()->state == PlayerActionComponent::ATTACK);
	});
	if (attacking) return;

	bool has_dir = (self->current_input_mask & (INPUT_UP|INPUT_DOWN|INPUT_LEFT|INPUT_RIGHT)) != 0;

	if (has_dir) {
		self->pending_inputs.push_back(self->current_input_mask);
		self->player_query.each([&](PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
			if (p.conv == self->local_conv)
				movement(self, &pos, r, id, self->local_conv, self->current_input_mask);
		});
		adventure::C2S c2s;
		c2s.set_cmd(adventure::CMD_PLAYER_INPUT);
		c2s.mutable_player_input()->set_keycode(self->current_input_mask);
		std::string data = c2s.SerializeAsString();
		netclient_send(self->netclient, data.c_str(), data.size());
	}
}

static void send_heartbeat(game_scene_p self, float dt)
{
        if (!self->netclient) return;
        self->heartbeatTimer += dt;
        if (self->heartbeatTimer >= 3.f) {
                self->heartbeatTimer = 0.f;
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_PLAYER_HEART);
                std::string d = c2s.SerializeAsString();
                netclient_send(self->netclient, d.c_str(), d.size());
        }
}

static void on_handle_event(scene_p s, const void* ev)
{
        SDL_Event* e = (SDL_Event*)ev;
        game_scene_p self = (game_scene_p)scene_get_userdata(s);

        if (e->type == SDL_EVENT_KEY_DOWN || e->type == SDL_EVENT_KEY_UP) {
                bool is_down = (e->type == SDL_EVENT_KEY_DOWN);
                int key_mask = 0;
                switch (e->key.key) {
                case SDLK_W: key_mask = INPUT_UP; break;
                case SDLK_S: key_mask = INPUT_DOWN; break;
                case SDLK_A: key_mask = INPUT_LEFT; break;
                case SDLK_D: key_mask = INPUT_RIGHT; break;
                case SDLK_SPACE: key_mask = INPUT_ATTACK; break;
                default: break;
                }
                if (key_mask) {
                        if (is_down) {
                                self->current_input_mask |= key_mask;
                                if (key_mask == INPUT_ATTACK) self->attack_triggered = true;
                                // 更新本地玩家朝向
                                if (key_mask & INPUT_RIGHT) self->local_dir = 2;
                                else if (key_mask & INPUT_LEFT) self->local_dir = 1;
                                else if (key_mask & INPUT_UP) self->local_dir = 3;
                                else if (key_mask & INPUT_DOWN) self->local_dir = 0;
                        }
                        else {
                                self->current_input_mask &= ~key_mask;
                        }
                }
        }
        // 移动输入事件（触屏）
        else if (e->type == GAMEPLAY_CONTROL_EVENT) {
                int new_mask = self->current_input_mask;
                switch (e->user.code) {
                case GAMEPLAY_CONTROL_UP:    new_mask |= INPUT_UP; break;
                case GAMEPLAY_CONTROL_DOWN:  new_mask |= INPUT_DOWN; break;
                case GAMEPLAY_CONTROL_LEFT:  new_mask |= INPUT_LEFT; break;
                case GAMEPLAY_CONTROL_RIGHT: new_mask |= INPUT_RIGHT; break;
                case GAMEPLAY_CONTROL_RELEASE_UP:    new_mask &= ~INPUT_UP; break;
                case GAMEPLAY_CONTROL_RELEASE_DOWN:  new_mask &= ~INPUT_DOWN; break;
                case GAMEPLAY_CONTROL_RELEASE_LEFT:  new_mask &= ~INPUT_LEFT; break;
                case GAMEPLAY_CONTROL_RELEASE_RIGHT: new_mask &= ~INPUT_RIGHT; break;
                case GAMEPLAY_CONTROL_ATTACK:
                        self->attack_triggered = true;
                        break;
                default:
                        new_mask &= ~(INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT);
                        break;
                }
                if (new_mask != self->current_input_mask) {
                        self->current_input_mask = new_mask;
                }
        }
}

static void on_update(scene_p s, float dt) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        if (!self->netclient) return;
        netclient_update(self->netclient);
        self->accumulator += dt;
        if (self->accumulator >= self->FIXED_TIMESTEP) {
                fixedupdate(self, self->FIXED_TIMESTEP);
                self->accumulator -= self->FIXED_TIMESTEP;
        }

        self->ecs_world.progress(dt);

        // ---------- 心跳 ----------
        send_heartbeat(self, dt);

	// ---------- 摄像机跟随 + 远程状态同步 + 脚步音效 ----------
	self->ecs_world.defer_begin();
	{
		static float step_timer = 0;
		self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent&,
			LogicRectComponent&, LogicPositionComponent& pos) {
			if (p.conv == (int)self->local_conv) {
				// 摄像机跟随
				float target_cam_x = fp_to_float(pos.x) - 6.4f;
				float target_cam_y = fp_to_float(pos.y) - 4.8f;
				float lerp = dt * 5.0f;
				if (lerp > 1.0f) lerp = 1.0f;
				self->ctx->camera_x += (target_cam_x - self->ctx->camera_x) * lerp;
				self->ctx->camera_y += (target_cam_y - self->ctx->camera_y) * lerp;

				// 本地玩家状态同步(不覆盖 ATTACK, ATTACK 由 fixedupdate + animation_update_system 管理)
				if (e.has<PlayerActionComponent>()) {
					auto* act = e.get_mut<PlayerActionComponent>();
					if (act->state != PlayerActionComponent::ATTACK) {
						bool has_dir = (self->current_input_mask & (INPUT_UP|INPUT_DOWN|INPUT_LEFT|INPUT_RIGHT)) != 0;
						act->state = has_dir ? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
						act->dir = self->local_dir;
					}
				}

				// 脚步音效
				if (e.has<PlayerActionComponent>() && e.get<PlayerActionComponent>()->state == PlayerActionComponent::WALK) {
					step_timer += dt;
					if (step_timer >= 0.30f) {
						if (self->step_sound) audio_shot_play(self->step_sound);
						step_timer = 0;
					}
				} else { step_timer = 0; }
			} else {
				// 远程玩家方向追踪
				flecs::entity_t eid = e;
				auto& ldi = self->remote_dirs[eid];
				static std::map<flecs::entity_t, fp_t> last_px, last_py;
				auto& lx = last_px[eid]; auto& ly = last_py[eid];
				if (lx == 0 && ly == 0) { lx = pos.x; ly = pos.y; }
				fp_t dx = fp_sub(pos.x, lx), dy = fp_sub(pos.y, ly);
				fp_t th = fp_from_float(0.001f);
				int new_dir = ldi;
				if (dx > th)        new_dir = 2;
				else if (dx < -th)  new_dir = 1;
				if (dy < -th)       new_dir = 3;
				else if (dy > th)   new_dir = 0;
				if (dx > th || dx < -th || dy > th || dy < -th) {
					lx = pos.x; ly = pos.y; ldi = new_dir;
				}
				if (e.has<PlayerActionComponent>()) {
					auto* act = e.get_mut<PlayerActionComponent>();
					act->dir = new_dir;
					if (act->state != PlayerActionComponent::ATTACK) {
						act->state = (dx > th || dx < -th || dy > th || dy < -th)
							? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
					}
				}
			}
		});
	}
	self->ecs_world.defer_end();









}

static void on_render(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        SDL_SetRenderDrawColor(self->ctx->renderer, 100, 100, 100, 255);

        // ---- tilemap 背景 ----
        if (self->tilemap) {
                float cam_x_px = self->ctx->camera_x * (float)PIXELS_PER_METER;
                float cam_y_px = self->ctx->camera_y * (float)PIXELS_PER_METER;
                jtilemap_render_all(self->tilemap, cam_x_px, cam_y_px, SDL_GetTicks());
        }

        // ---- ECS 实体（碰撞体） ----
        self->drawing_entity_query.each(drawing_entity_system);
        self->attack_ray_effect_query.each(drawing_attack_ray_effect_system);

        // ---- ECS 精灵渲染（玩家） ----
        self->ecs_world.filter_builder<TransformComponent, LogicRectComponent,
            SpriteSheetComponent, AnimationFrameComponent, PlayerActionComponent, PlayerComponent>()
            .build().each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
                    SpriteSheetComponent& ss, AnimationFrameComponent& af,
                    PlayerActionComponent& act, PlayerComponent&) {
                sprite_render_system(it.entity(i), t, r, ss, af, act);
            });
}

static void on_destroy(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        if (self->netclient) { netclient_destroy(self->netclient); self->netclient = NULL; }
        tex_cache_clear();
        audio_shot_destroy(self->step_sound);
        audio_bgm_destroy(self->bgm);
        if (self->audio_dev) audio_close_device(self->audio_dev);
        jtilemap_render_destroy();
        if (self->tilemap) jtilemap_destroy(self->tilemap);
        delete self;
}

game_scene_p game_scene_create(context* ctx) {
        game_scene_p self = new game_scene();
        self->scene = scene_create("game");
        self->ctx = ctx;
        self->accumulator = 0;
        self->ecs_world.set_ctx(ctx);
        // 初始化摄像机
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
