#define STB_IMAGE_IMPLEMENTATION
#include <joy/external/stb_image.h>
#include <unordered_map>
#include "../flecs.h"
#include <joy/jmsgpack.h>
#include <string>
#include <map>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <joy/jcore.h>
#include <joy/jsys.h>
#include <joy/jmath.h>
#include <joy/jutils.h>
#include <joy/jr2d.h>
#include <joy/jcollision.h>
#include <joy/jnetwork.h>
#include <joy/jtilemap.h>
#include <joy/jtilemap_render.h>
#include "../../adventure_shared/protocol.h"
#include "../layers/debug_layer.h"
#include "../components/connection_component.h"
#include "../components/player_component.h"
#include "../components/local_only_component.h"
#include "../components/player_action_component.h"
#include "../components/sprite_sheet_component.h"
#include "../components/ai_component.h"
#include "../components/enemy_component.h"
#include "../components/logic_velocity_component.h"
#include "../systems/effect_lifecycle_system.h"
#include "../systems/lerp_system.h"
#include "../systems/animation_update_system.h"
#include "../systems/sprite_render_system.h"
#include "../systems/drawing_attack_ray_effect_system.h"
#include "../systems/drawing_entity_system.h"
#include "../context.h"
#include "game_scene.h"

static const fp_t MOVE_SPEED = fp_from_float(5.0f);



struct game_scene {
	scene_p scene;
	flecs::world world;
	jtilemap_p tilemap = NULL;
	jtilemap_render_p tilemap_render = NULL;

	int g_id = 1;
	int g_frameid = 1;

	simple_fps_counter_p sample_fps;

	float accumulator = 0.0f;

	float serverTickTimer = 0.0f;
	float SERVER_TICK_INTERVAL = 1.0f / 20.0f;

	netserver_p netserver;
	Context* ctx;

	// ---- 预构建的 ECS Query ----
	flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
	flecs::query<ConnectionComponent> connection_query;
	flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
	flecs::query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent> enemy_query;
	flecs::query<IdComponent, LogicRectComponent, TransformComponent> drawing_entity_query;
	flecs::query<AttackRayEffectComponent> drawing_attack_rayeffect_query;

	// 精灵渲染 query（避免 on_render 每帧重建 filter）
	flecs::query<TransformComponent, LogicRectComponent,
		SpriteSheetComponent, AnimationFrameComponent, PlayerActionComponent, PlayerComponent> render_player_query;
	flecs::query<TransformComponent, LogicRectComponent,
		SpriteSheetComponent, AnimationFrameComponent, PlayerActionComponent, EnemyComponent> render_enemy_query;

	// 位置同步 query（合并玩家和敌人）
	flecs::query<LogicPositionComponent, TransformComponent> position_sync_query;

	// ---- 纹理缓存 ----

	std::vector<S2CPlayerJoin> player_joins;
	std::vector<S2CPlayerLeave> player_leaves;
	std::vector<S2CPlayerInput> player_inputs;

	std::map<int, std::string> commands;
	std::queue<std::string> command_queue;
	std::map<int, std::string> worlds;

};

// 纹理缓存
static std::unordered_map<std::string, image_p> g_tex_cache;

image_p tex_cache_get(jr2d_p r, const char* path) {
	std::string k(path);
	auto it = g_tex_cache.find(k);
	if (it != g_tex_cache.end())
		return it->second;
	return NULL;
}
void tex_cache_clear() {
	for (auto& kv : g_tex_cache)
		if (kv.second)
			image_destroy(kv.second);
	g_tex_cache.clear();
}

static int GenId(game_scene_p game_scene)
{
	return game_scene->g_id++;
}

static void HandleLoading(game_scene_p g, int conv)
{
	log_info("C2S_CMD_LOADING");
	flecs::entity entity = g->world.entity().add<ConnectionComponent>();
	auto conn = entity.get_mut<ConnectionComponent>();
	conn->conv = conv;
	conn->frameid = std::max(g->g_frameid - 1, 1);
	conn->health = 10;

	auto it = g->worlds.find(conn->frameid);
	if (it == g->worlds.end()) { log_info("没有world"); return; }

	S2CEntity entities[512];
	int ec = 0, eid = 0;
	ec = unpack_world(it->second.c_str(), (int)it->second.size(), &eid, entities, 512);
	auto data = pack_s2c_loading(conn->conv, conn->frameid, eid, entities, ec);
	netserver_send(g->netserver, conn->conv, data.c_str(), (int)data.size());
}

static void AddPlayerJoin(game_scene_p g, int conv, int64_t x, int64_t y) {
	S2CPlayerJoin j = { conv, x, y }; g->player_joins.push_back(j);
}
static void AddPlayerLeave(game_scene_p g, int conv) {
	S2CPlayerLeave l = { conv }; g->player_leaves.push_back(l);
}
static void AddPlayerInput(game_scene_p g, int conv, int keycode) {
	S2CPlayerInput i = { conv, keycode }; g->player_inputs.push_back(i);
}

static void HandleHeartbeat(game_scene_p g, int conv)
{
	log_info("%d: C2S_CMD_HEARTBEAT", conv);
	g->connection_query.each([&](ConnectionComponent& conn) {
		if (conn.conv == conv) {
			conn.health = 10;
			return false;
		}
		return true;
		});
}

static void AttackTarget(game_scene_p self, LogicPositionComponent* p,
	LogicRectComponent& currRect, IdComponent& currId, int dir)
{
	// 攻击判定：向攻击方向偏移的碰撞盒（0.6m → 偏移 0.7m）
	fp_t atk_range = fp_from_float(0.7f);
	fp_t ox = 0, oy = 0;
	if (dir == 0)      oy = atk_range;
	else if (dir == 1) ox = -atk_range;
	else if (dir == 2) ox = atk_range;
	else if (dir == 3) oy = -atk_range;
	rectanglef_t atk_rect = { fp_add(p->x, ox), fp_add(p->y, oy), currRect.width, currRect.height };

	self->body_query.each([&](flecs::entity e, IdComponent& id,
		LogicRectComponent& r, LogicPositionComponent& pos) {
			if (currId.id == id.id) return;
			if (!e.has<PlayerComponent>()) return; // 只攻击玩家
			rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
			contact2df_t contact;
			if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
				if (id.hp > 1) id.hp--;
			}
		});
	// 射线攻击已注释
	/*
	ray2df_t ray;
	...
	collision2df_get_ray_rectangle(ray, rect);
	...
	*/
}

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

static void resolve_collision(game_scene_p self, LogicPositionComponent* p,
	LogicRectComponent& currRect, IdComponent& currId)
{
	self->body_query.each([&](IdComponent& other_id,
		LogicRectComponent& r,
		LogicPositionComponent& other_pos) {
			if (other_id.id == currId.id) return;

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
		});
}

static void ApplyInput(game_scene_p self,
	LogicPositionComponent* p, LogicRectComponent& currRect,
	IdComponent& currId, int conv, int input)
{
	if (input & INPUT_ATTACK) {
		// 攻击判定：向攻击方向偏移的碰撞盒
		int dir = input_to_dir(input);
		fp_t atk_range = fp_from_float(0.7f);
		fp_t ox = 0, oy = 0;
		if (dir == 0)      oy = atk_range;
		else if (dir == 1) ox = -atk_range;
		else if (dir == 2) ox = atk_range;
		else if (dir == 3) oy = -atk_range;
		rectanglef_t atk_rect = { fp_add(p->x, ox), fp_add(p->y, oy), currRect.width, currRect.height };
		self->body_query.each([&](flecs::entity e, IdComponent& id, LogicRectComponent& r, LogicPositionComponent& pos) {
			if (currId.id == id.id) return;
			if (!e.has<EnemyComponent>()) return; // 玩家攻击只伤敌人
			rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
			contact2df_t contact;
			if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
				id.hp--;
			}
			});
		// 射线攻击已注释
		/*
		ray2df_t ray;
		...
		*/
	}

	fp_t move_x, move_y;
	calc_move_step(self, &move_x, &move_y, input & (INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT));

	p->x = fp_add(p->x, move_x);
	p->y = fp_add(p->y, move_y);

	resolve_collision(self, p, currRect, currId);
}

static void OnMessage(game_scene_p self, net_message_p msg, void* userdata)
{
	void* msg_data = msg->data;
	size_t msg_len = msg->len;

	if (msg->type == NET_TYPE_CONNECTED) {
		log_info("connected=%d", msg->conv);
	}
	else if (msg->type == NET_TYPE_DISCONNECTED) {
		log_info("disconnected=%d", msg->conv);
		S2CPlayerLeave l = { msg->conv };
		self->player_leaves.push_back(l);
	}
	else if (msg->type == NET_TYPE_MESSAGE) {
		int cmd, input_kc = 0; int64_t join_x = 0, join_y = 0;
		if (unpack_c2s((const char*)msg_data, (int)msg_len, &cmd, &join_x, &join_y, &input_kc)) {
			switch (cmd) {
			case CMD_LOADING:      HandleLoading(self, msg->conv); break;
			case CMD_PLAYER_JOIN:  AddPlayerJoin(self, msg->conv, join_x, join_y); break;
			case CMD_PLAYER_LEAVE: AddPlayerLeave(self, msg->conv); break;
			case CMD_PLAYER_INPUT: AddPlayerInput(self, msg->conv, input_kc); break;
			case CMD_PLAYER_HEART: HandleHeartbeat(self, msg->conv); break;
			}
		} else {
			log_info("Failed to parse message from conv=%d", msg->conv);
		}
	}
	if (msg->type == NET_TYPE_MESSAGE && msg_data) {
		netmessage_free_data(msg);
	}
}

// 帧同步：收集命令
static void CollectCommandSystem(game_scene_p self)
{
	int frame_id = self->g_frameid++;

	S2CPlayerJoin joins[32]; int jn = 0;
	for (auto& j : self->player_joins) if (jn < 32) joins[jn++] = j;
	S2CPlayerLeave leaves[32]; int ln = 0;
	for (auto& l : self->player_leaves) if (ln < 32) leaves[ln++] = l;
	S2CPlayerInput inputs[64]; int in = 0;
	for (auto& i : self->player_inputs) if (in < 64) inputs[in++] = i;

	// 只收集玩家实体，按 id 排序确保顺序一致
	std::vector<std::pair<int, std::pair<fp_t, fp_t>>> positions;
	self->player_query.each([&](PlayerComponent& p, IdComponent& id,
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
	self->commands[frame_id] = command_data;
	self->command_queue.push(command_data);

	self->player_joins.clear();
	self->player_leaves.clear();
	self->player_inputs.clear();

	// 收集世界实体状态
	S2CEntity entities[512]; int ec = 0;
	self->body_query.each([&](flecs::entity e, IdComponent& id,
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
	self->worlds[frame_id] = pack_world(self->g_id, entities, ec);

	// 限制 worlds 和 commands 最多保留 1000 帧
	const size_t MAX_FRAMES = 1000;
	while (self->commands.size() > MAX_FRAMES) {
		self->commands.erase(self->commands.begin());
	}
	while (self->worlds.size() > MAX_FRAMES) {
		self->worlds.erase(self->worlds.begin());
	}
}


static void UpdateAI(game_scene_p self)
{
	self->enemy_query.each([&](flecs::entity e, EnemyComponent&, AIComponent& ai,
		IdComponent& id, LogicRectComponent& rect, LogicPositionComponent& pos) {

			fp_t speed = fp_from_float(2.0f);
			fp_t step = fp_mul(speed, fp_from_float(self->ctx->FIXED_TIMESTEP));

			// 查找检测范围内最近玩家
			fp_t detect_sq = fp_mul(ai.detect_radius, ai.detect_radius);
			fp_t nearest_dist_sq = detect_sq;
			fp_t nearest_player_x = 0, nearest_player_y = 0;
			bool found = false;

			self->player_query.each([&](PlayerComponent&, IdComponent&,
				LogicRectComponent&, LogicPositionComponent& pp) {
					fp_t dist_sq = collision2df_get_squared_distance_point_circle(
						vec2f_t{pp.x, pp.y}, circlef_t{ {pos.x, pos.y}, ai.detect_radius });
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
						AttackTarget(self, &pos, rect, id, atk_dir);
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


// 帧同步：执行命令
static void HandleCommandSystem(game_scene_p self)
{
	if (self->command_queue.empty()) return;

	std::string command_data = self->command_queue.front();
	int frame_id, jn = 0, ln = 0, in = 0;
	S2CPlayerJoin joins[32]; S2CPlayerLeave leaves[32]; S2CPlayerInput inputs[64];
	if (!unpack_command(command_data.c_str(), (int)command_data.size(), &frame_id,
		joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in)) {
		log_info("Failed to parse command");
		self->command_queue.pop(); return;
	}

	for (int i = 0; i < jn; i++) {
		auto& pj = joins[i];
		log_info("%d:CMD_PLAYER_JOIN", pj.conv);
		fp_t x = pj.position_x;
		fp_t y = pj.position_y;

		SpriteSheetComponent ss{};
		snprintf(ss.path, sizeof(ss.path), "adventure_server/textures/knights/troops/warrior/warrior_blue.png");
		ss.frame_w = 192; ss.frame_h = 192;
		ss.idle_row = 0; ss.walk_row = 1;
		ss.atk_rows[0] = 4; ss.atk_rows[1] = 2;
		ss.atk_rows[2] = 2; ss.atk_rows[3] = 6;
		ss.frame_count = 6; ss.frame_duration = 0.15f;
		self->world.entity()
			.set<IdComponent>({ GenId(self), 10 })
			.set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
			.set<LogicPositionComponent>({ x, y })
			.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
			.set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 1, 1 })
			.set<PlayerComponent>({ pj.conv })
			.set<PlayerActionComponent>({})
			.set<SpriteSheetComponent>(ss)
			.set<AnimationFrameComponent>({});
	}

	// 玩家离开（defer 延迟销毁，避免迭代中删除实体）
	self->world.defer_begin();
	for (int i = 0; i < ln; i++) {
		int conv = leaves[i].conv;
		self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent&, LogicRectComponent&, LogicPositionComponent&) {
			if (p.conv == conv) e.destruct();
			});
	}
	self->world.defer_end();

	// 应用输入
	for (int i = 0; i < in; i++) {
		auto& input = inputs[i];
		int dir = input_to_dir(input.keycode);
		bool moving = has_dir(input.keycode);

		self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
				if (p.conv != input.conv) return;
				if (e.has<PlayerActionComponent>()) {
					auto* act = e.get_mut<PlayerActionComponent>();
					if (input.keycode & INPUT_ATTACK) {
						act->state = PlayerActionComponent::ATTACK;
						act->dir = dir;
					}
					else {
						act->state = moving ? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
						act->dir = dir;
					}
				}
				ApplyInput(self, &pos, r, id, input.conv, input.keycode);
			});
	}

	UpdateAI(self);
	self->command_queue.pop();
}

static int GetTargetFrameId(int curr_frameid, int context_frameid)
{
	int diff = context_frameid - curr_frameid;

	if (diff >= 50)      return curr_frameid + 9;
	if (diff >= 30)      return curr_frameid;
	if (diff >= 10)      return curr_frameid + 4;
	if (diff > 2)        return curr_frameid + 2;

	return curr_frameid;
}

// 帧同步：通知客户端
static void NotifySystem(game_scene_p self)
{
	self->connection_query.each([&](flecs::entity e, ConnectionComponent& conn) {
		int taget_frameid = GetTargetFrameId(conn.frameid, self->g_frameid);
		for (int i = conn.frameid; i < taget_frameid; i++) {
			auto it = self->commands.find(i);
			if (it != self->commands.end()) {
				netserver_send(self->netserver, conn.conv, it->second.c_str(), it->second.size());
			}
			else {
				log_error("Frame %d not found in commands, skip sending.", i);
			}
		}
		conn.frameid = taget_frameid;
		});
}

// 固定逻辑帧更新
static void FixedUpdate(game_scene_p self, float dt)
{
	net_message_t msg;
	if (netserver_poll_message(self->netserver, &msg)) {
		OnMessage(self, &msg, NULL);
	}
}

static void on_load(scene_p s)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	self->sample_fps = simple_fps_create();
	self->netserver = netserver_create(NET_SERVER_WEBSOCKET, "192.168.1.4", 30000);

	debug_layer_p debug_layer = create_debug_layer();
	scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));

	self->world.set_ctx(self->ctx);

	// 本地方块：有碰撞检测，但不会同步到客户端（因为有 LocalOnlyComponent）
	/*self->world.entity()
		.add<LocalOnlyComponent>()
		.set<IdComponent>({ 10000, 100 })
		.set<LogicRectComponent>({ fp_from_float(0.5f), fp_from_float(0.5f) })
		.set<LogicPositionComponent>({ fp_from_float(5), fp_from_float(5) })
		.set<TransformComponent>({ 5,5,0,1,1 });*/

	// 加载 tilemap 碰撞墙体（替代硬编码围墙）
	self->tilemap = jtilemap_load_file("adventure_server/tilemap/map.lua");
	if (self->tilemap) {
		jtilemap_object_layer_p ol = jtilemap_find_object_layer(self->tilemap, "collision");
		if (ol) {
			for (size_t i = 0; i < kv_size(ol->objects); i++) {
				jtilemap_object_t* obj = &kv_A(ol->objects, i);
				if (obj->shape != JTILEMAP_SHAPE_RECT) continue;
				fp_t px = fp_from_float(obj->x / (float)SERVER_PPM);
				fp_t py = fp_from_float(obj->y / (float)SERVER_PPM);
				fp_t pw = fp_from_float(obj->width / (float)SERVER_PPM);
				fp_t ph = fp_from_float(obj->height / (float)SERVER_PPM);
				self->world.entity()
					.set<IdComponent>({ GenId(self), 100 })
					.set<LogicRectComponent>({ pw, ph })
					.set<LogicPositionComponent>({ px, py })
					.set<TransformComponent>({ fp_to_float(px), fp_to_float(py), 0,1,1 });
			}
			log_info("[tilemap] loaded %zu collision walls", kv_size(ol->objects));
		}
	}

	// ---- 创建敌人 ----
	const int ENEMY_COUNT = 1;
	for (int i = 0; i < ENEMY_COUNT; i++) {
		fp_t ex = fp_from_float(3);
		fp_t ey = fp_from_float(3);
		fp_t cx = ex;
		fp_t cy = ey;
		SpriteSheetComponent ess{};
		snprintf(ess.path, sizeof(ess.path), "adventure_server/textures/knights/troops/warrior/warrior_red.png");
		ess.frame_w = 192; ess.frame_h = 192;
		ess.idle_row = 0; ess.walk_row = 1;
		ess.atk_rows[0] = 4; ess.atk_rows[1] = 2;
		ess.atk_rows[2] = 2; ess.atk_rows[3] = 6;
		ess.frame_count = 6; ess.frame_duration = 0.15f;
		self->world.entity()
			.add<EnemyComponent>()
			.set<AIComponent>({
				cx, cy,
				fp_from_float(4.0f), fp_from_float(15.0f), fp_from_float(1.5f),
				0, 0,
				0, 0
				})
			.set<IdComponent>({ GenId(self), 5 })
			.set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
			.set<LogicPositionComponent>({ ex, ey })
			.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
			.set<TransformComponent>({ fp_to_float(ex), fp_to_float(ey), 0, 1, 1 })
			.set<SpriteSheetComponent>(ess)
			.set<PlayerActionComponent>({})
			.set<AnimationFrameComponent>({});
	}
	log_info("[AI] created %d enemies", ENEMY_COUNT);

	self->world.system<AttackRayEffectComponent>().each(EffectLifecycleSystem);
	self->world.system<PlayerActionComponent, SpriteSheetComponent, AnimationFrameComponent>()
		.each(animation_update_system);

	self->drawing_entity_query = self->world.query<IdComponent, LogicRectComponent, TransformComponent>();
	self->drawing_attack_rayeffect_query = self->world.query<AttackRayEffectComponent>();
	self->body_query = self->world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();
	self->connection_query = self->world.query<ConnectionComponent>();
	self->player_query = self->world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
	self->enemy_query = self->world.query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();

	// 预构建渲染 query，避免每帧重复 filter_builder().build()
	self->render_player_query = self->world.query<TransformComponent, LogicRectComponent,
		SpriteSheetComponent, AnimationFrameComponent, PlayerActionComponent, PlayerComponent>();
	self->render_enemy_query = self->world.query<TransformComponent, LogicRectComponent,
		SpriteSheetComponent, AnimationFrameComponent, PlayerActionComponent, EnemyComponent>();

	// 位置同步 query（所有同时拥有逻辑位置和渲染位置的实体）
	self->position_sync_query = self->world.query<LogicPositionComponent, TransformComponent>();

	// ---- tilemap + 精灵渲染初始化 ----
	if (self->tilemap)
		self->tilemap_render = jtilemap_render_create(self->ctx->r2d, "adventure_server/", NULL);

	S2CEntity entities[512]; int ec = 0;
	self->body_query.each([&](flecs::entity e, IdComponent& id,
		LogicRectComponent& r, LogicPositionComponent& pos) {
			if (ec >= 512) return;
			auto& ent = entities[ec++]; memset(&ent, 0, sizeof(ent));
			ent.id = id.id;
			if (e.has<PlayerComponent>()) { ent.type = S2C_TYPE_PLAYER; ent.player_conv = e.get_mut<PlayerComponent>()->conv; }
			else if (e.has<EnemyComponent>()) { ent.type = S2C_TYPE_ENEMY; }
			else { ent.type = S2C_TYPE_NORMAL; }
			ent.hp = id.hp; ent.width = r.width; ent.height = r.height;
			ent.position_x = pos.x; ent.position_y = pos.y;
		});
	self->worlds[self->g_frameid] = pack_world(self->g_id, entities, ec);
}

static void server_on_keypressed(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg)
{
	(void)arg;
	if (strcmp(key, "q") == 0 || strcmp(key, "Q") == 0) {
		push_quit_event();
	}
}

static void on_handle_event(scene_p s, const void* ev)
{
	jevent_callbacks_t cbs = {};
	cbs.on_keypressed = server_on_keypressed;
	dispatch_event(cbs, ev, NULL);
}

static void on_update(scene_p s, float dt)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);

	self->accumulator += dt;
	netserver_update(self->netserver);
	simple_fps_update(self->sample_fps);

	// 固定步长物理更新（50Hz）
	if (self->accumulator >= self->ctx->FIXED_TIMESTEP) {
		FixedUpdate(self, self->ctx->FIXED_TIMESTEP);
		self->accumulator -= self->ctx->FIXED_TIMESTEP;
	}

	self->world.progress(dt);

	// AI 更新 + 独立的帧同步 Tick（20Hz）
	self->serverTickTimer += dt;
	if (self->serverTickTimer >= self->SERVER_TICK_INTERVAL) {
		self->serverTickTimer -= self->SERVER_TICK_INTERVAL;
		CollectCommandSystem(self);
		HandleCommandSystem(self);
		NotifySystem(self);


		// 位置同步：合并玩家和敌人为一次查询
		self->position_sync_query.each([&](LogicPositionComponent& pos, TransformComponent& t) {
			t.position_x = fp_to_float(pos.x);
			t.position_y = fp_to_float(pos.y);
			});
	}

	// ---------- 摄像机跟随第一个玩家 ----------
	self->player_query.each([&](PlayerComponent&, IdComponent&,
		LogicRectComponent&, LogicPositionComponent& pos) {
			float target_cam_x = fp_to_float(pos.x) - 6.4f;
			float target_cam_y = fp_to_float(pos.y) - 4.8f;
			float lerp = dt * 5.0f;
			if (lerp > 1.0f) lerp = 1.0f;
			self->ctx->camera_x += (target_cam_x - self->ctx->camera_x) * lerp;
			self->ctx->camera_y += (target_cam_y - self->ctx->camera_y) * lerp;
			return; // 只跟第一个玩家
		});
}

static void on_render(scene_p s)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	auto* ctx = self->ctx;

	// ---- tilemap 背景 ----
	if (self->tilemap) {
		float cx = ctx->camera_x * (float)SERVER_PPM;
		float cy = ctx->camera_y * (float)SERVER_PPM;
		jtilemap_render_all(self->tilemap_render, self->tilemap, cx, cy, get_ticks());
	}

	// ---- ECS 实体（碰撞体） ----
	self->drawing_entity_query.each(DrawingEntitySystem);
	self->drawing_attack_rayeffect_query.each(DrawingAttackRayEffectSystem);

	// ---- ECS 精灵渲染（玩家） ----
	self->render_player_query.each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
		SpriteSheetComponent& ss, AnimationFrameComponent& af,
		PlayerActionComponent& act, PlayerComponent&) {
			sprite_render_system(it.entity(i), t, r, ss, af, act);
		});

	// ---- ECS 精灵渲染（敌人） ----
	self->render_enemy_query.each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
		SpriteSheetComponent& ss, AnimationFrameComponent& af,
		PlayerActionComponent& act, EnemyComponent&) {
			sprite_render_system(it.entity(i), t, r, ss, af, act);
		});

	// ---- 血条渲染（所有有血量实体头顶） ----
	{
		auto* ctx = self->ctx;
		self->world.filter_builder<IdComponent, TransformComponent>()
			.build().each([ctx](flecs::iter& it, size_t i, IdComponent& id, TransformComponent& t) {
			if (id.hp <= 0) return;
			flecs::entity e = it.entity(i);
			if (!e.has<PlayerComponent>() && !e.has<EnemyComponent>()) return;
			float sx = (t.position_x - ctx->camera_x) * SERVER_PPM;
			float sy = (t.position_y - ctx->camera_y) * SERVER_PPM;
			const float SPRITE_OFF = 81.0f;
			float bar_w = 48.0f, bar_h = 8.0f;
			float bar_x = sx - bar_w / 2.0f, bar_y = sy - SPRITE_OFF - bar_h - 4.0f;
			jrect_t bg = { bar_x - 1, bar_y - 1, bar_w + 2, bar_h + 2 };
			
			r2d_set_color(ctx->r2d, 0, 0, 0, 255);
			shape_draw_rectangle(ctx->r2d, "fill", bg);
			jrect_t base = { bar_x, bar_y, bar_w, bar_h };
			r2d_set_color(ctx->r2d, 40, 40, 40, 255);
			shape_draw_rectangle(ctx->r2d, "fill", base);
			float ratio = (float)id.hp / 10.0f;
			if (ratio > 1.0f) ratio = 1.0f;
			if (ratio < 0) ratio = 0;
			jrect_t fill = { bar_x, bar_y, bar_w * ratio, bar_h };
			if (ratio > 0.6f)      r2d_set_color(ctx->r2d, 60, 210, 60, 255);
			else if (ratio > 0.3f) r2d_set_color(ctx->r2d, 210, 210, 60, 255);
			else                   r2d_set_color(ctx->r2d, 220, 60, 60, 255);
			shape_draw_rectangle(ctx->r2d, "fill", fill);
		});
	}
}

static void on_destroy(scene_p s)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	simple_fps_destory(self->sample_fps);
	netserver_destroy(self->netserver);
	tex_cache_clear();
	if (self->tilemap_render) jtilemap_render_destroy(self->tilemap_render);
	if (self->tilemap) { jtilemap_destroy(self->tilemap); self->tilemap = NULL; }
}

game_scene_p game_scene_create(Context* ctx)
{
	game_scene_p self = new game_scene();
	self->scene = scene_create("game_server");
	self->ctx = ctx;
	// 初始化摄像机
	ctx->camera_x = 0.0f;
	ctx->camera_y = 0.0f;
	self->world.component<ConnectionComponent>();
	self->world.component<IdComponent>();
	self->world.component<LogicRectComponent>();
	self->world.component<LogicPositionComponent>();
	self->world.component<LogicVelocityComponent>();
	self->world.component<TransformComponent>();
	self->world.component<PlayerComponent>();
	self->world.component<AttackRayEffectComponent>();
	self->world.component<SpriteSheetComponent>();
	self->world.component<AnimationFrameComponent>();
	self->world.component<PlayerActionComponent>();

	self->world.component<AIComponent>();
	self->world.component<EnemyComponent>();

	scene_set_userdata(self->scene, self);
	scene_set_load_callback(self->scene, on_load);
	scene_set_event_callback(self->scene, on_handle_event);
	scene_set_update_callback(self->scene, on_update);
	scene_set_render_callback(self->scene, on_render);
	scene_set_destroy_callback(self->scene, on_destroy);

	return self;
}

scene_p game_scene_get_scene(game_scene_p g)
{
	return g ? g->scene : NULL;
}
