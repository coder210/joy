#define SDL_MAIN_USE_CALLBACKS 1
#include "flecs.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d/sys.h>
#include <joy2d/network.h>
#include <joy2d/core.h>
#include <iostream>
#include <map>
#include <list>
#include <string>
#include "proto.h"

const int PIXELS_PER_METER = 50;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static kcpserver_p kcpserver = NULL;
static flecs::world world;
static int g_frameid = 1;
static std::map<int, std::string> world_map;


// 位掩码定义（每个方向占用一个独立位）
const int INPUT_UP = 1 << 0;   // 1
const int INPUT_DOWN = 1 << 1;   // 2
const int INPUT_LEFT = 1 << 2;   // 4
const int INPUT_RIGHT = 1 << 3;   // 8


// 方向键状态
static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;
static const fp_t MOVE_SPEED = fp_from_float(0.5f);

static Uint64 lastTime = 0;
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 60.0f;   // 60Hz固定步长

struct Connection {
	int health;
	int frameid;
	int conv;
	std::vector<s2c_player_join_t> player_joins;
	std::vector<s2c_player_leave_t> player_leaves;
	std::vector<s2c_player_input_t> player_inputs;
};

// 组件定义
struct LogicPosition { fp_t x, y; };
struct LogicVelocity { fp_t x, y; };

// 组件定义
struct NetworkSingleton {
	std::vector<s2c_player_join_t> player_joins;
	std::vector<s2c_player_leave_t> player_leaves;
	std::vector<s2c_player_input_t> player_inputs;
	std::map<int, std::string> commands;
};
struct Position { float x, y; };
struct Player {};  // 标记组件，用于标识玩家实体
struct Test {};

static void handle_cmd_ready(int conv, c2s_p c2s)
{
	log_info("C2S_CMD_READY");
	char data[JOY_MAX_BUFFER];
	int len;

	flecs::entity entity = world.entity().add<Connection>();
	auto conn = entity.get_mut<Connection>();
	conn->conv = conv;
	conn->frameid = g_frameid;
	conn->health = 10;

	s2c_t s2c;
	s2c.cmd = S2C_CMD_LOADING;
	s2c.loading.frame_id = conn->frameid;
	s2c.loading.conv = conn->conv;
	if (world_map.find(conn->frameid) != world_map.end()) {
		std::string world_data = world_map[conn->frameid];
		s2c.loading.data_len = world_data.size();
		if (s2c.loading.data_len > 0) {
			memcpy(s2c.loading.data, world_data.c_str(), s2c.loading.data_len);
		}
	}
	else {
		s2c.loading.data_len = 0;
	}
	s2c_serialize(&s2c, data, &len);
	kcpserver_send(kcpserver, conn->conv, data, len);
}

static void handle_cmd_loading(int conv, c2s_p c2s)
{
	log_info("C2S_CMD_LOADING");
}

static void handle_cmd_player_join(s2c_player_join_p player_join)
{
}

static void handle_cmd_player_leave(s2c_player_leave_p player_leave)
{
	log_info("C2S_CMD_PLAYER_LEAVE");
}

static void handle_cmd_player_input(s2c_player_input_p player_input)
{
	log_info("C2S_CMD_PLAYER_INPUT");
}

static void handle_cmd_heartbeat(int conv, c2s_p c2s)
{
	log_info("C2S_CMD_HEARTBEAT");
	world.each([&](flecs::entity e, Connection& conn) {
		if (conn.conv == conv) {
			e.get_mut<Connection>()->health = 10;
			return false;
		}
		return true;
		});
}

static void msg_callback(net_message_p msg, void* userdata)
{
	if (msg->type == NET_TYPE_CONNECTED) {
		log_info("connected=%d", msg->conv);
	}
	else if (msg->type == NET_TYPE_DISCONNECTED) {
		log_info("disconnected=%d", msg->conv);
	}
	else if (msg->type == NET_TYPE_MESSAGE) {
		log_info("msg=%s", msg->data);
		//std::string data(msg->data, msg->len);
		//std::cout << "Received message: " << data << std::endl;
		c2s_t c2s;
		if (c2s_deserialize(&c2s, msg->data, msg->len)) {
			if (c2s.cmd == C2S_CMD_READY) {
				handle_cmd_ready(msg->conv, &c2s);
			}
			else if (c2s.cmd == C2S_CMD_LOADING) {
				handle_cmd_loading(msg->conv, &c2s);
			}
			else if (c2s.cmd == C2S_CMD_PLAYER_JOIN) {
				world.query<Connection>().each([&](flecs::entity e, Connection& conn) {
					if (conn.conv == msg->conv) {
						conn.player_joins.push_back({ msg->conv, c2s.player_join.position_x , c2s.player_join.position_y });
						return false;
					}
					return true;
					});
			}
			else if (c2s.cmd == C2S_CMD_PLAYER_LEAVE) {
				world.query<Connection>().each([&](flecs::entity e, Connection& conn) {
					if (conn.conv == msg->conv) {
						conn.player_leaves.push_back({ msg->conv });
						return false;
					}
					return true;
					});
			}
			else if (c2s.cmd == C2S_CMD_PLAYER_INPUT) {
				world.query<Connection>().each([&](flecs::entity e, Connection& conn) {
					if (conn.conv == msg->conv) {
						conn.player_inputs.push_back({ msg->conv, c2s.player_input.sequence, c2s.player_input.keycode });
						return false;
					}
					return true;
					});
			}
			else if (c2s.cmd == C2S_CMD_HEARTBEAT) {
				handle_cmd_heartbeat(msg->conv, &c2s);
			}
		}
	}
	SDL_free(msg->data);  // 记得释放消息数据的内存
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	sys_init_netenv();
	SDL_SetAppMetadata("Adventure server", "1.0", "com.example.adventure-server");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("adventure/server", 640, 480, 0, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	log_info("server start");
	kcpserver = kcpserver_create("192.168.1.14", 10000);
	kcpserver_set_callback(kcpserver, msg_callback, kcpserver);

	// 注册组件
	world.component<NetworkSingleton>();
	world.component<Connection>();
	world.component<LogicPosition>();
	world.component<LogicVelocity>();
	world.component<Position>();
	world.component<Player>();

	/* 收集command,并执行 20 frames per 1 meter */
	world.set<NetworkSingleton>({});

	/* A */
	world.system<Connection>()
		.interval(0.05f)
		.each([](flecs::entity e, Connection& conn) {
		log_info("A");
		auto ns = e.world().get_mut<NetworkSingleton>();
		if (conn.player_joins.size() > 0) {
			for (int i = 0; i < conn.player_joins.size(); i++) {
				s2c_player_join_t player_join = conn.player_joins[i];
				log_info("CMD_PLAYER_JOIN");
				e.set<LogicPosition>({ player_join.position_x, player_join.position_y })
					.set<LogicVelocity>({ fp_from_float(0.0f), fp_from_float(0.0f) })
					.set<Position>({ fp_to_float(player_join.position_x), fp_to_float(player_join.position_y) })
					.add<Player>();
				ns->player_joins.push_back(player_join);
			}
			conn.player_joins.clear();
		}
		if (conn.player_leaves.size() > 0) {
			for (int i = 0; i < conn.player_leaves.size(); i++) {
				s2c_player_leave_t player_leave = conn.player_leaves[i];
				ns->player_leaves.push_back(player_leave);
			}
			conn.player_leaves.clear();
		}
		if (conn.player_inputs.size() > 0) {
			for (int i = 0; i < conn.player_inputs.size(); i++) {
				s2c_player_input_t player_input = conn.player_inputs[i];
				if (e.has<Player>() && e.has<LogicVelocity>()) {
					auto v = e.get_mut<LogicVelocity>();
					if (INPUT_UP == player_input.keycode) {
						v->y = fp_sub(v->y, MOVE_SPEED);
					}
					else if (INPUT_DOWN == player_input.keycode) {
						v->y = fp_add(v->y, MOVE_SPEED);
					}
					else if (INPUT_LEFT == player_input.keycode) {
						v->x = fp_sub(v->x, MOVE_SPEED);
					}
					else if (INPUT_RIGHT == player_input.keycode) {
						v->x = fp_add(v->x, MOVE_SPEED);
					}
				}
				ns->player_inputs.push_back(player_input);
			}
			conn.player_inputs.clear();
		}
			});

	/* B */
	world.system<Connection>()
		.interval(0.05f)
		.iter([](flecs::iter iter) {
		log_info("B");
		auto ns = iter.world().get_mut<NetworkSingleton>();
		char data[JOY_MAX_BUFFER] = { 0 };
		int len;
		s2c_t s2c;
		s2c.cmd = S2C_CMD_COMMAND;
		s2c.command.checksum_len = 0;
		s2c.command.frame_id = g_frameid++;
		if (ns->player_joins.size() > 0) {
			for (int i = 0; i < ns->player_joins.size(); i++) {
				s2c_player_join_t player_join = ns->player_joins[i];
				s2c.command.player_joins.push_back(player_join);
			}
		}
		s2c_serialize(&s2c, data, &len);
		std::string str_data(data, len);
		ns->player_joins.clear();
		ns->player_leaves.clear();
		ns->player_inputs.clear();
		ns->commands.insert({ s2c.command.frame_id, str_data });
			});

	/* C */
	world.system<Connection>()
		.interval(0.05f)
		.each([](flecs::entity e, Connection& conn) {
		log_info("C");
		auto ns = e.world().get_mut<NetworkSingleton>();
		auto iter = ns->commands.find(conn.frameid);
		if (iter != ns->commands.end()) {
			conn.frameid++;
			kcpserver_send(kcpserver, conn.conv, iter->second.c_str(), iter->second.size());
		}
			});

	// 移动系统：每帧将速度加到位置
	world.system<LogicPosition, LogicVelocity>()
		.each([=](LogicPosition& p, LogicVelocity& v) {
		p.x = fp_add(p.x, v.x);
		p.y = fp_add(p.y, v.y);
		v.x = 0;
		v.y = 0;
			});

	world.system<LogicPosition, Position>()
		.interval(0.02f)
		.each([=](LogicPosition& logic_position, Position& p) {
		p.x = fp_to_float(logic_position.x) * PIXELS_PER_METER;
		p.y = fp_to_float(logic_position.y) * PIXELS_PER_METER;
			});

	// 添加第0帧的世界数据
	//world_map.insert({ 0, "" });
	//flecs::system s = world.system<Position, const LogicVelocity>()
	//	.each([](flecs::entity e, Position& p, const LogicVelocity& v) {
	//	p.x += v.x;
	//	p.y += v.y;
	//	std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
	//		});
	//s.run();
	//world.system<LogicPosition, LogicVelocity>("Startup")
	//	.kind(flecs::OnStart)
	//	.iter([](flecs::iter& it, LogicPosition& logic_position, Position& p) {
	//	//std::cout << it.system().name() << "\n";
	//		});

	world.system<LogicPosition, LogicVelocity>("Startup")
		.kind(flecs::OnStart)
		.each([](flecs::entity e, LogicPosition& logic_position, LogicVelocity& logic_velocity) {
		NetworkSingleton *ns = e.world().get_mut<NetworkSingleton>();





			});

	world.entity().set<LogicPosition>({ fp_from_float(100.0f), fp_from_float(0.0f) })
		.set<LogicVelocity>({ fp_from_float(0.0f), fp_from_float(0.0f) })
		.set<Position>({ 100.0f, 0.0f });

	std::string str = world.to_json().c_str();



	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}

	if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
		bool isDown = (event->type == SDL_EVENT_KEY_DOWN);
		SDL_Keycode key = event->key.key;

		// 更新方向键状态
		switch (key) {
		case SDLK_W: upPressed = isDown; break;    // W -> 上
		case SDLK_S: downPressed = isDown; break;  // S -> 下
		case SDLK_A: leftPressed = isDown; break;  // A -> 左
		case SDLK_D: rightPressed = isDown; break; // D -> 右
		case SDLK_Q: if (isDown) return SDL_APP_SUCCESS; break; // Q退出
		default: break;
		}

		// 合成速度
		float vx = 0.0f, vy = 0.0f;
		if (rightPressed) vx += MOVE_SPEED;
		if (leftPressed)  vx -= MOVE_SPEED;
		if (downPressed)  vy += MOVE_SPEED;
		if (upPressed)    vy -= MOVE_SPEED;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	// 计算时间差
	Uint64 currentTime = SDL_GetPerformanceCounter();
	if (lastTime == 0) {
		lastTime = currentTime;
	}
	float deltaTime = (float)((currentTime - lastTime) / (double)SDL_GetPerformanceFrequency());
	lastTime = currentTime;

	// 固定时间步长更新
	accumulator += deltaTime;
	while (accumulator >= FIXED_TIMESTEP) {
		world.progress(FIXED_TIMESTEP);   // 以固定步长驱动ECS系统
		accumulator -= FIXED_TIMESTEP;
	}

	// 更新网络服务器
	kcpserver_update(kcpserver);

	SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	// 修复点：将 Player& 改为 Player（按值接收空标记组件）
	world.query<Player, Position>().each([=](Player /*player*/, Position& p) {
		SDL_FRect rect = { p.x - 15.0f, p.y - 15.0f, 30.0f, 30.0f };
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &rect);
		});

	SDL_RenderPresent(renderer);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	// flecs 世界会自动释放资源
	kcpserver_destroy(kcpserver);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	sys_release_netenv();
}