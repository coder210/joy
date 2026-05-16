#include "../flecs.h"
#include "../protocol/adventure.pb.h"
#include <string>
#include <map>
#include <queue>
#include <joy2d/jcore.h>
#include <joy2d/jsys.h>
#include <joy2d/jmath.h>
#include <joy2d/jutils.h>
#include <joy2d/jtext.h>
#include <joy2d/jcollision.h>
#include <joy2d/jshapes.h>
#include <joy2d/jnetwork.h>
#include "../layers/debug_layer.h"
#include "../components/connection_component.h"
#include "../components/player_component.h"
#include "../components/logic_velocity_component.h"
#include "../systems/effect_lifecycle_system.h"
#include "../systems/lerp_system.h"
#include "../systems/drawing_attack_ray_effect_system.h"
#include "../systems/drawing_entity_system.h"
#include "../context.h"
#include "game_scene.h"


// 位掩码定义
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;
const int INPUT_ATTACK = 1 << 4;

static const fp_t MOVE_SPEED = fp_from_float(5.0f);

struct game_scene {
	scene_p scene;
	flecs::world world;

	int g_id = 1;
	int g_frameid = 1;

	simple_fps_counter_p sample_fps;

	bool upPressed = false;
	bool downPressed = false;
	bool leftPressed = false;
	bool rightPressed = false;

	float accumulator = 0.0f;

	float serverTickTimer = 0.0f;
	float SERVER_TICK_INTERVAL = 1.0f / 20.0f;

	netserver_p netserver;
	Context* ctx;

	flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
	flecs::query<ConnectionComponent> connection_query;
	flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
	flecs::query<IdComponent, LogicRectComponent, TransformComponent> drawing_entity_query;
	flecs::query<AttackRayEffectComponent> drawing_attack_rayeffect_query;

	std::vector<adventure::S2CPlayerJoin> player_joins;
	std::vector<adventure::S2CPlayerLeave> player_leaves;
	std::vector<adventure::S2CPlayerInput> player_inputs;

	std::map<int, std::string> commands;
	std::queue<std::string> command_queue;
	std::map<int, std::string> worlds;

};


static int GenId(game_scene_p game_scene)
{
	return game_scene->g_id++;
}

static void HandleLoading(game_scene_p g, int conv, adventure::C2S* c2s)
{
	log_info("C2S_CMD_LOADING");
	flecs::entity entity = g->world.entity().add<ConnectionComponent>();
	auto conn = entity.get_mut<ConnectionComponent>();
	conn->conv = conv;
	conn->frameid = std::max(g->g_frameid - 1, 1);
	conn->health = 10;

	adventure::S2C s2c;
	s2c.set_cmd(adventure::S2C_CMD_LOADING);

	adventure::S2CMap* map = s2c.mutable_map();
	map->set_conv(conn->conv);
	map->set_frame_id(conn->frameid);
	adventure::S2CWorld* s2c_world = map->mutable_world();
	auto it = g->worlds.find(conn->frameid);
	if (it == g->worlds.end()) {
		log_info("没有world");
		return;
	}
	if (!s2c_world->ParseFromArray(it->second.c_str(), it->second.length())) {
		log_info("解析失败");
		return;
	}
	auto data = s2c.SerializeAsString();
	netserver_send(g->netserver, conn->conv, data.c_str(), data.length());
}

static void AddPlayerJoin(game_scene_p g, int conv, adventure::C2S* c2s)
{
	adventure::S2CPlayerJoin player_join;
	player_join.set_conv(conv);
	player_join.set_position_x(c2s->player_join().position_x());
	player_join.set_position_y(c2s->player_join().position_y());
	g->player_joins.push_back(player_join);
}

static void AddPlayerLeave(game_scene_p g, int conv, adventure::C2S* c2s)
{
	adventure::S2CPlayerLeave player_leave;
	player_leave.set_conv(conv);
	g->player_leaves.push_back(player_leave);
}

static void AddPlayerInput(game_scene_p g, int conv, adventure::C2S* c2s)
{
	adventure::S2CPlayerInput player_input;
	player_input.set_conv(conv);
	player_input.set_keycode(c2s->player_input().keycode());
	g->player_inputs.push_back(player_input);
}

static void HandleHeartbeat(game_scene_p g, int conv, adventure::C2S* c2s)
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

static void Attack(game_scene_p self, LogicPositionComponent* p,
	LogicRectComponent& currRect,
	IdComponent& currId)
{
	ray2df_t ray;
	ray.origin.x = fp_add(p->x, fp_mul(currRect.width, fp_half()));
	ray.origin.y = p->y;
	ray.direction.x = fp_from_float(.0f);
	ray.direction.y = fp_from_float(-1.0f);

	ray2d_collisionf_t nearest_hit = { 0 };
	IdComponent* nearest_id = nullptr;
	float nearest_dist = 999999.0f;

	self->body_query.each([&](IdComponent& id,
		LogicRectComponent& r, LogicPositionComponent& pos) {
		if (currId.id == id.id) return;

		rectanglef_t rect = { 0 };
		rect.x = pos.x;
		rect.y = pos.y;
		rect.width = r.width;
		rect.height = r.height;
		ray2d_collisionf_t result = collision2df_get_ray_rectangle(ray, rect);

		if (result.hit) {
			float current_dist = fp_to_float(fp_sub(ray.origin.y, result.point.y));
			if (current_dist < nearest_dist) {
				nearest_dist = current_dist;
				nearest_hit = result;
				nearest_id = &id;
			}
		}
	});

	if (nearest_id != nullptr) {
		nearest_id->hp--;
		if (nearest_id->hp <= 0) {}

		SDL_FRect line;
		float end_x = fp_to_float(ray.origin.x);
		float end_y = fp_to_float(ray.origin.y);
		float start_x = fp_to_float(nearest_hit.point.x);
		float start_y = fp_to_float(nearest_hit.point.y);
		line.w = 0.05f;
		line.h = (end_y - start_y);
		line.x = start_x - line.w * 0.5f;
		line.y = start_y;
		self->world.entity().set<AttackRayEffectComponent>({ line.x, line.y, line.w, line.h, 0.1f });
	}
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
		Attack(self, p, currRect, currId);
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
		adventure::S2CPlayerLeave playerLeave;
		playerLeave.set_conv(msg->conv);
		self->player_leaves.push_back(playerLeave);
	}
	else if (msg->type == NET_TYPE_MESSAGE) {
		adventure::C2S c2s;
		if (c2s.ParseFromArray(msg_data, msg_len)) {
			if (c2s.cmd() == adventure::CMD_LOADING) {
				HandleLoading(self, msg->conv, &c2s);
			}
			else if (c2s.cmd() == adventure::CMD_PLAYER_JOIN) {
				AddPlayerJoin(self, msg->conv, &c2s);
			}
			else if (c2s.cmd() == adventure::CMD_PLAYER_LEAVE) {
				AddPlayerLeave(self, msg->conv, &c2s);
			}
			else if (c2s.cmd() == adventure::CMD_PLAYER_INPUT) {
				AddPlayerInput(self, msg->conv, &c2s);
			}
			else if (c2s.cmd() == adventure::CMD_PLAYER_HEART) {
				HandleHeartbeat(self, msg->conv, &c2s);
			}
		}
		else {
			log_info("Failed to parse message from conv=%d, %s", msg->conv, msg_data);
		}
	}
	if (msg_data) {
		SDL_free(msg_data);
	}
}

// 帧同步：收集命令
static void CollectCommandSystem(game_scene_p self)
{
	adventure::S2C s2c;
	s2c.set_cmd(adventure::S2C_CMD_COMMAND);

	adventure::S2CCommand* command = s2c.mutable_command();
	command->set_frame_id(self->g_frameid++);

	for (auto& pj : self->player_joins) {
		*command->add_player_joins() = pj;
	}
	for (auto& pl : self->player_leaves) {
		*command->add_player_leaves() = pl;
	}
	for (auto& pi : self->player_inputs) {
		*command->add_player_inputs() = pi;
	}

	auto command_data = s2c.SerializeAsString();
	self->commands[command->frame_id()] = command_data;
	self->command_queue.push(command_data);

	self->player_joins.clear();
	self->player_leaves.clear();
	self->player_inputs.clear();

	// 收集世界实体状态
	adventure::S2CWorld s2c_world;
	s2c_world.set_entity_id(self->g_id);
	self->body_query.each([&](flecs::entity e, IdComponent& id,
		LogicRectComponent& r, LogicPositionComponent& pos) {
		adventure::S2CEntity* ent = s2c_world.add_entities();
		ent->set_id(id.id);
		if (e.has<PlayerComponent>()) {
			ent->set_type(adventure::S2C_TYPE_PLAYER);
			ent->set_player_conv(e.get_mut<PlayerComponent>()->conv);
		}
		else {
			ent->set_type(adventure::S2C_TYPE_NORMAL);
		}
		ent->set_hp(id.hp);
		ent->set_position_x(pos.x);
		ent->set_position_y(pos.y);
		ent->set_width(r.width);
		ent->set_height(r.height);
	});

	self->worlds[command->frame_id()] = s2c_world.SerializeAsString();

	// 限制 worlds 和 commands 最多保留 1000 帧
	const size_t MAX_FRAMES = 1000;
	while (self->commands.size() > MAX_FRAMES) {
		self->commands.erase(self->commands.begin());
	}
	while (self->worlds.size() > MAX_FRAMES) {
		self->worlds.erase(self->worlds.begin());
	}
}

// 帧同步：执行命令
static void HandleCommandSystem(game_scene_p self)
{
	if (self->command_queue.empty()) return;

	std::string command_data = self->command_queue.front();
	adventure::S2C s2c;

	if (!s2c.ParseFromString(command_data)) {
		log_info("Failed to parse command");
		self->command_queue.pop();
		return;
	}

	// 创建玩家
	for (auto& player_join : s2c.command().player_joins()) {
		log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
		fp_t x = player_join.position_x();
		fp_t y = player_join.position_y();

		self->world.entity()
			.set<IdComponent>({ GenId(self), 10 })
			.set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
			.set<LogicPositionComponent>({ x, y })
			.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
			.set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 1, 1 })
			.set<PlayerComponent>({ player_join.conv() });
	}

	// 玩家离开
	self->world.defer_begin();
	for (auto& player_leave : s2c.command().player_leaves()) {
		int conv = player_leave.conv();
		self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
			if (p.conv == conv) {
				e.destruct();
			}
		});
	}
	self->world.defer_end();

	// 应用输入
	for (auto& input : s2c.command().player_inputs()) {
		self->player_query.each([&](PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
			if (p.conv != input.conv()) return;
			ApplyInput(self, &pos, r, id, input.conv(), input.keycode());
		});
	}

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
	//self->netserver = netserver_create(NET_SERVER_WEBSOCKET, "192.168.2.42", 10000);
	self->netserver = netserver_create(NET_SERVER_WEBSOCKET, "192.168.1.28", 10000);

	debug_layer_p debug_layer = create_debug_layer();
	scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));

	self->world.set_ctx(self->ctx);

	self->world.entity()
		.set<IdComponent>({ GenId(self), 100 })
		.set<LogicRectComponent>({ fp_from_float(12), fp_from_float(0.5f) })
		.set<LogicPositionComponent>({ fp_from_float(0), fp_from_float(0) })
		.set<TransformComponent>({ 0,0,0,1,1 });
	self->world.entity()
		.set<IdComponent>({ GenId(self), 100 })
		.set<LogicRectComponent>({ fp_from_float(.5f), fp_from_float(12) })
		.set<LogicPositionComponent>({ fp_from_float(0), fp_from_float(0) })
		.set<TransformComponent>({ 0,0,0,1,1 });
	self->world.entity()
		.set<IdComponent>({ GenId(self), 100 })
		.set<LogicRectComponent>({ fp_from_float(.5f), fp_from_float(12) })
		.set<LogicPositionComponent>({ fp_from_float(12), fp_from_float(0) })
		.set<TransformComponent>({ 12,0,0,1,1 });
	self->world.entity()
		.set<IdComponent>({ GenId(self), 100 })
		.set<LogicRectComponent>({ fp_from_float(12), fp_from_float(0.5f) })
		.set<LogicPositionComponent>({ fp_from_float(0), fp_from_float(9) })
		.set<TransformComponent>({ 0,9, 0,1,1 });


	self->world.system<LogicPositionComponent, TransformComponent>().each(LerpSystem);
	self->world.system<AttackRayEffectComponent>().each(EffectLifecycleSystem);

	self->drawing_entity_query = self->world.query<IdComponent, LogicRectComponent, TransformComponent>();
	self->drawing_attack_rayeffect_query = self->world.query<AttackRayEffectComponent>();
	self->body_query = self->world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();
	self->connection_query = self->world.query<ConnectionComponent>();
	self->player_query = self->world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();

	adventure::S2CWorld s2c_world;
	self->body_query.each([&](flecs::entity e, IdComponent& id,
		LogicRectComponent& r, LogicPositionComponent& pos) {
			adventure::S2CEntity* ent = s2c_world.add_entities();
			ent->set_id(id.id);
			if (e.has<PlayerComponent>()) {
				ent->set_type(adventure::S2C_TYPE_PLAYER);
				ent->set_player_conv(e.get_mut<PlayerComponent>()->conv);
			}
			else {
				ent->set_type(adventure::S2C_TYPE_NORMAL);
			}
			ent->set_hp(id.hp);
			ent->set_width(r.width);
			ent->set_height(r.height);
			ent->set_position_x(pos.x);
			ent->set_position_y(pos.y);
		});

	self->worlds[self->g_frameid] = s2c_world.SerializeAsString();
}

static void on_handle_event(scene_p s, const void* ev)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	SDL_Event* event = (SDL_Event*)ev;

	if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
		bool isDown = event->type == SDL_EVENT_KEY_DOWN;
		switch (event->key.key) {
		case SDLK_W: self->upPressed = isDown; break;
		case SDLK_S: self->downPressed = isDown; break;
		case SDLK_A: self->leftPressed = isDown; break;
		case SDLK_D: self->rightPressed = isDown; break;
		case SDLK_Q: {
			SDL_Event quit_event;
			SDL_zero(quit_event);
			quit_event.type = SDL_EVENT_QUIT;
			SDL_PushEvent(&quit_event);
			break;
		}
		default: break;
		}
	}
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

	// 独立的帧同步 Tick（20Hz）
	self->serverTickTimer += dt;
	if (self->serverTickTimer >= self->SERVER_TICK_INTERVAL) {
		self->serverTickTimer -= self->SERVER_TICK_INTERVAL;
		CollectCommandSystem(self);
		HandleCommandSystem(self);
		NotifySystem(self);
	}
}

static void on_render(scene_p s)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	self->drawing_entity_query.each(DrawingEntitySystem);
	self->drawing_attack_rayeffect_query.each(DrawingAttackRayEffectSystem);
}

static void on_destroy(scene_p s)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	simple_fps_destory(self->sample_fps);
	netserver_destroy(self->netserver);
	
}

game_scene_p game_scene_create(Context* ctx)
{
	game_scene_p self = new game_scene();
	self->scene = scene_create("game_server");
	self->ctx = ctx;
	self->world.component<ConnectionComponent>();
	self->world.component<LogicRectComponent>();
	self->world.component<LogicPositionComponent>();
	self->world.component<LogicVelocityComponent>();
	self->world.component<TransformComponent>();
	self->world.component<PlayerComponent>();

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
