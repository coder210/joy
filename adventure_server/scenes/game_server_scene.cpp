#include "game_server_scene.h"
#include "../Systems.h"
#include "../Context.h"
#include <joy2d/jsys.h>
#include <joy2d/jmath.h>
#include <joy2d/jutils.h>
#include <joy2d/jtext.h>
#include <joy2d/jcollision.h>
#include <joy2d/jshapes.h>
#include <string>

const int PIXELS_PER_METER = 50;

// 位掩码定义
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;
const int INPUT_ATTACK = 1 << 4;

static const fp_t MOVE_SPEED = fp_from_float(5.0f);

struct game_server_scene {
	scene_p scene;
	flecs::world world;
};

// 模块级 self 指针，供静态函数访问场景状态
static game_server_scene_p g_self = NULL;

static int GenId(Context* ns)
{
	return ns->g_id++;
}

static void HandleLoading(int conv, adventure::C2S* c2s)
{
	log_info("C2S_CMD_LOADING");
	auto ctx = g_self->world.get_mut<Context>();
	flecs::entity entity = g_self->world.entity().add<ConnectionComponent>();
	auto conn = entity.get_mut<ConnectionComponent>();
	conn->conv = conv;
	conn->frameid = std::max(ctx->g_frameid - 1, 1);
	conn->health = 10;

	adventure::S2C s2c;
	s2c.set_cmd(adventure::S2C_CMD_LOADING);

	adventure::S2CMap* map = s2c.mutable_map();
	map->set_conv(conn->conv);
	map->set_frame_id(conn->frameid);
	adventure::S2CWorld* s2c_world = map->mutable_world();
	auto it = ctx->worlds.find(conn->frameid);
	if (it == ctx->worlds.end()) {
		log_info("没有world");
		return;
	}
	if (!s2c_world->ParseFromArray(it->second.c_str(), it->second.length())) {
		log_info("解析失败");
		return;
	}
	auto data = s2c.SerializeAsString();
	netserver_send(ctx->server, conn->conv, data.c_str(), data.length());
}

static void AddPlayerJoin(int conv, adventure::C2S* c2s)
{
	adventure::S2CPlayerJoin player_join;
	player_join.set_conv(conv);
	player_join.set_position_x(c2s->player_join().position_x());
	player_join.set_position_y(c2s->player_join().position_y());
	g_self->world.get_mut<Context>()->player_joins.push_back(player_join);
}

static void AddPlayerLeave(int conv, adventure::C2S* c2s)
{
	adventure::S2CPlayerLeave player_leave;
	player_leave.set_conv(conv);
	g_self->world.get_mut<Context>()->player_leaves.push_back(player_leave);
}

static void AddPlayerInput(int conv, adventure::C2S* c2s)
{
	adventure::S2CPlayerInput player_input;
	player_input.set_conv(conv);
	player_input.set_keycode(c2s->player_input().keycode());
	g_self->world.get_mut<Context>()->player_inputs.push_back(player_input);
}

static void HandleHeartbeat(int conv, adventure::C2S* c2s)
{
	log_info("%d: C2S_CMD_HEARTBEAT", conv);
	auto ctx = g_self->world.get_mut<Context>();
	ctx->connection_query.each([&](ConnectionComponent& conn) {
		if (conn.conv == conv) {
			conn.health = 10;
			return false;
		}
		return true;
	});
}

static void Attack(LogicPositionComponent* p,
	LogicRectComponent& currRect,
	IdComponent& currId)
{
	auto ctx = g_self->world.get_mut<Context>();
	ray2df_t ray;
	ray.origin.x = fp_add(p->x, fp_mul(currRect.width, fp_half()));
	ray.origin.y = p->y;
	ray.direction.x = fp_from_float(.0f);
	ray.direction.y = fp_from_float(-1.0f);

	ray2d_collisionf_t nearest_hit = { 0 };
	IdComponent* nearest_id = nullptr;
	float nearest_dist = 999999.0f;

	ctx->body_query.each([&](IdComponent& id,
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
		g_self->world.entity().set<AttackRayEffectComponent>({ line.x, line.y, line.w, line.h, 0.1f });
	}
}

static void calc_move_step(fp_t* out_x, fp_t* out_y, int input)
{
	auto ctx = g_self->world.get_mut<Context>();
	fp_t delta = fp_from_float(ctx->FIXED_TIMESTEP);
	fp_t step = fp_mul(MOVE_SPEED, delta);

	*out_x = fp_zero();
	*out_y = fp_zero();

	if (input & INPUT_UP)    *out_y = fp_sub(*out_y, step);
	if (input & INPUT_DOWN)  *out_y = fp_add(*out_y, step);
	if (input & INPUT_LEFT)  *out_x = fp_sub(*out_x, step);
	if (input & INPUT_RIGHT) *out_x = fp_add(*out_x, step);
}

static void resolve_collision(LogicPositionComponent* p,
	LogicRectComponent& currRect, IdComponent& currId)
{
	auto ctx = g_self->world.get_mut<Context>();

	ctx->body_query.each([&](IdComponent& other_id,
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

static void ApplyInput(LogicPositionComponent* p, LogicRectComponent& currRect,
	IdComponent& currId, int conv, int input)
{
	if (input & INPUT_ATTACK) {
		Attack(p, currRect, currId);
	}

	fp_t move_x, move_y;
	calc_move_step(&move_x, &move_y, input & (INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT));

	p->x = fp_add(p->x, move_x);
	p->y = fp_add(p->y, move_y);

	resolve_collision(p, currRect, currId);
}

static void OnMessage(net_message_p msg, void* userdata)
{
	void* msg_data = msg->data;
	size_t msg_len = msg->len;

	if (msg->type == NET_TYPE_CONNECTED) {
		log_info("connected=%d", msg->conv);
	}
	else if (msg->type == NET_TYPE_DISCONNECTED) {
		log_info("disconnected=%d", msg->conv);
		auto ctx = g_self->world.get_mut<Context>();
		adventure::S2CPlayerLeave playerLeave;
		playerLeave.set_conv(msg->conv);
		ctx->player_leaves.push_back(playerLeave);
	}
	else if (msg->type == NET_TYPE_MESSAGE) {
		adventure::C2S c2s;
		if (c2s.ParseFromArray(msg_data, msg_len)) {
			if (c2s.cmd() == adventure::CMD_LOADING) {
				HandleLoading(msg->conv, &c2s);
			}
			else if (c2s.cmd() == adventure::CMD_PLAYER_JOIN) {
				AddPlayerJoin(msg->conv, &c2s);
			}
			else if (c2s.cmd() == adventure::CMD_PLAYER_LEAVE) {
				AddPlayerLeave(msg->conv, &c2s);
			}
			else if (c2s.cmd() == adventure::CMD_PLAYER_INPUT) {
				AddPlayerInput(msg->conv, &c2s);
			}
			else if (c2s.cmd() == adventure::CMD_PLAYER_HEART) {
				HandleHeartbeat(msg->conv, &c2s);
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
static void CollectCommandSystem()
{
	auto ctx = g_self->world.get_mut<Context>();
	adventure::S2C s2c;
	s2c.set_cmd(adventure::S2C_CMD_COMMAND);

	adventure::S2CCommand* command = s2c.mutable_command();
	command->set_frame_id(ctx->g_frameid++);

	for (auto& pj : ctx->player_joins) {
		*command->add_player_joins() = pj;
	}
	for (auto& pl : ctx->player_leaves) {
		*command->add_player_leaves() = pl;
	}
	for (auto& pi : ctx->player_inputs) {
		*command->add_player_inputs() = pi;
	}

	auto command_data = s2c.SerializeAsString();
	ctx->commands[command->frame_id()] = command_data;
	ctx->command_queue.push(command_data);

	ctx->player_joins.clear();
	ctx->player_leaves.clear();
	ctx->player_inputs.clear();

	// 收集世界实体状态
	adventure::S2CWorld s2c_world;
	s2c_world.set_entity_id(ctx->g_id);
	ctx->body_query.each([&](flecs::entity e, IdComponent& id,
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
	});

	ctx->worlds[command->frame_id()] = s2c_world.SerializeAsString();

	// 限制 worlds 和 commands 最多保留 1000 帧
	const size_t MAX_FRAMES = 1000;
	while (ctx->commands.size() > MAX_FRAMES) {
		ctx->commands.erase(ctx->commands.begin());
	}
	while (ctx->worlds.size() > MAX_FRAMES) {
		ctx->worlds.erase(ctx->worlds.begin());
	}
}

// 帧同步：执行命令
static void HandleCommandSystem()
{
	auto ctx = g_self->world.get_mut<Context>();
	if (ctx->command_queue.empty()) return;

	std::string command_data = ctx->command_queue.front();
	adventure::S2C s2c;

	if (!s2c.ParseFromString(command_data)) {
		log_info("Failed to parse command");
		ctx->command_queue.pop();
		return;
	}

	// 创建玩家
	for (auto& player_join : s2c.command().player_joins()) {
		log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
		fp_t x = player_join.position_x();
		fp_t y = player_join.position_y();

		g_self->world.entity()
			.set<IdComponent>({ GenId(ctx), 10 })
			.set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
			.set<LogicPositionComponent>({ x, y })
			.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
			.set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 0, 0, 0 })
			.set<PlayerComponent>({ player_join.conv() });
	}

	// 玩家离开
	g_self->world.defer_begin();
	for (auto& player_leave : s2c.command().player_leaves()) {
		int conv = player_leave.conv();
		ctx->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
			if (p.conv == conv) {
				e.destruct();
			}
		});
	}
	g_self->world.defer_end();

	// 应用输入
	for (auto& input : s2c.command().player_inputs()) {
		ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
			if (p.conv != input.conv()) return;
			ApplyInput(&pos, r, id, input.conv(), input.keycode());
		});
	}

	ctx->command_queue.pop();
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
static void NotifySystem()
{
	auto ctx = g_self->world.get_mut<Context>();
	ctx->connection_query.each([&](flecs::entity e, ConnectionComponent& conn) {
		int taget_frameid = GetTargetFrameId(conn.frameid, ctx->g_frameid);
		for (int i = conn.frameid; i < taget_frameid; i++) {
			auto it = ctx->commands.find(i);
			if (it != ctx->commands.end()) {
				netserver_send(ctx->server, conn.conv, it->second.c_str(), it->second.size());
			}
			else {
				log_error("Frame %d not found in commands, skip sending.", i);
			}
		}
		conn.frameid = taget_frameid;
	});
}

// 固定逻辑帧更新
static void FixedUpdate(float dt)
{
	auto ctx = g_self->world.get_mut<Context>();
	net_message_t msg;
	if (netserver_poll_message(ctx->server, &msg)) {
		OnMessage(&msg, NULL);
	}
}

// ==================== 场景回调 ====================

static void on_load(scene_p s)
{
	game_server_scene_p self = (game_server_scene_p)scene_get_userdata(s);
	g_self = self;

	auto ctx = self->world.get_mut<Context>();
	game_timer_init(&ctx->game_timer);
	game_timer_set_target_fps(&ctx->game_timer, 60);
	game_timer_set_time_scale(&ctx->game_timer, 1.0f);
	ctx->sample_fps = simple_fps_create();

	if (!SDL_CreateWindowAndRenderer("adventure/server", 640, 480, 0, &ctx->window, &ctx->renderer)) {
		SDL_Log("Window failed: %s", SDL_GetError());
		return;
	}
	SDL_SetRenderLogicalPresentation(ctx->renderer, 640, 480, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_STRETCH);
	ctx->server = netserver_create(NET_SERVER_WEBSOCKET, "192.168.2.32", 10000);

	self->world.entity()
		.set<IdComponent>({ GenId(ctx), 100 })
		.set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(0.6f) })
		.set<LogicPositionComponent>({ fp_from_float(1), fp_from_float(1) })
		.set<TransformComponent>({ 1,1 });
	self->world.entity()
		.set<IdComponent>({ GenId(ctx), 100 })
		.set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(0.6f) })
		.set<LogicPositionComponent>({ fp_from_float(4), fp_from_float(4) })
		.set<TransformComponent>({ 2,2 });

	self->world.system<LogicPositionComponent, TransformComponent>().each(LerpSystem);
	self->world.system<AttackRayEffectComponent>().each(EffectLifecycleSystem);

	ctx->renderer_query = self->world.query<IdComponent, LogicRectComponent, TransformComponent>();
	ctx->renderer_attack_rayeffect_query = self->world.query<AttackRayEffectComponent>();

	StartupSystem(self->world);
}

static void on_handle_event(scene_p s, const void* ev)
{
	game_server_scene_p self = (game_server_scene_p)scene_get_userdata(s);
	SDL_Event* event = (SDL_Event*)ev;
	auto ctx = self->world.get_mut<Context>();

	if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
		bool isDown = event->type == SDL_EVENT_KEY_DOWN;
		switch (event->key.key) {
		case SDLK_W: ctx->upPressed = isDown; break;
		case SDLK_S: ctx->downPressed = isDown; break;
		case SDLK_A: ctx->leftPressed = isDown; break;
		case SDLK_D: ctx->rightPressed = isDown; break;
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
	game_server_scene_p self = (game_server_scene_p)scene_get_userdata(s);
	auto ctx = self->world.get_mut<Context>();

	// 更新定时器
	game_timer_update(&ctx->game_timer);
	float fixed_dt = game_timer_get_delta_time(&ctx->game_timer);
	ctx->accumulator += fixed_dt;
	netserver_update(ctx->server);
	simple_fps_update(ctx->sample_fps);

	// 固定步长物理更新（50Hz）
	if (ctx->accumulator >= ctx->FIXED_TIMESTEP) {
		FixedUpdate(ctx->FIXED_TIMESTEP);
		ctx->accumulator -= ctx->FIXED_TIMESTEP;
	}

	self->world.progress(fixed_dt);

	// 独立的帧同步 Tick（20Hz）
	ctx->serverTickTimer += fixed_dt;
	if (ctx->serverTickTimer >= ctx->SERVER_TICK_INTERVAL) {
		ctx->serverTickTimer -= ctx->SERVER_TICK_INTERVAL;
		CollectCommandSystem();
		HandleCommandSystem();
		NotifySystem();
	}
}

static void on_render(scene_p s)
{
	game_server_scene_p self = (game_server_scene_p)scene_get_userdata(s);
	auto ctx = self->world.get_mut<Context>();

	SDL_SetRenderDrawColor(ctx->renderer, 100, 100, 100, 255);
	SDL_RenderClear(ctx->renderer);

	ctx->renderer_query.each(RendererSystem);
	ctx->renderer_attack_rayeffect_query.each(RendererAttackRayEffectSystem);

	ctx->debugLayer->Update(ctx->g_frameid, simple_fps_value(ctx->sample_fps));
	ctx->debugLayer->Draw(ctx->renderer);

	SDL_RenderPresent(ctx->renderer);
}

static void on_destroy(scene_p s)
{
	game_server_scene_p self = (game_server_scene_p)scene_get_userdata(s);
	auto ctx = self->world.get_mut<Context>();

	delete ctx->resources;
	delete ctx->debugLayer;
	simple_fps_destory(ctx->sample_fps);
	netserver_destroy(ctx->server);
	SDL_DestroyWindow(ctx->window);
	SDL_DestroyRenderer(ctx->renderer);

	google::protobuf::ShutdownProtobufLibrary();
	g_self = NULL;
}

game_server_scene_p game_server_scene_create()
{
	game_server_scene_p self = new game_server_scene();
	self->scene = scene_create("game_server");

	// 初始化 flecs world 和 Context（类似 client game_scene）
	self->world.component<Context>();
	self->world.component<ConnectionComponent>();
	self->world.component<LogicRectComponent>();
	self->world.component<LogicPositionComponent>();
	self->world.component<LogicVelocityComponent>();
	self->world.component<TransformComponent>();
	self->world.component<PlayerComponent>();
	self->world.set<Context>({});

	scene_set_userdata(self->scene, self);
	scene_set_load_callback(self->scene, on_load);
	scene_set_event_callback(self->scene, on_handle_event);
	scene_set_update_callback(self->scene, on_update);
	scene_set_render_callback(self->scene, on_render);
	scene_set_destroy_callback(self->scene, on_destroy);

	return self;
}

scene_p game_server_scene_get_scene(game_server_scene_p g)
{
	return g ? g->scene : NULL;
}
