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
#include <joy/jtext.h>
#include <joy/jcollision.h>
#include <joy/jshapes.h>
#include <joy/jnetwork.h>
#include <joy/jtilemap.h>
#include <joy/jtilemap_render.h>
#include <joy/jevent.h>

enum { SERVER_PPM = 50 };
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

// ===== 协议结构体定义（基于 jmsgpack 二次封装） =====
enum C2SCmd_ {
    CMD_UNKNOWN       = 0, CMD_PLAYER_JOIN  = 1, CMD_PLAYER_LEAVE = 2,
    CMD_PLAYER_INPUT  = 3, CMD_PLAYER_HEART = 4, CMD_LOADING      = 5,
};
enum S2CCmd_  { S2C_CMD_NONE = 0, S2C_CMD_LOADING = 1, S2C_CMD_COMMAND = 2, };
enum S2CType_ { S2C_TYPE_NORMAL = 0, S2C_TYPE_PLAYER = 1, S2C_TYPE_ENEMY = 2, };

struct S2CPlayerJoin { int32_t conv; int64_t position_x, position_y; };
struct S2CPlayerLeave { int32_t conv; };
struct S2CPlayerInput { int32_t conv, keycode; };
struct S2CEntity { int32_t id, type; int64_t position_x, position_y, width, height, hp; int32_t player_conv; };

// ===== jmsgpack 回写入器（写入 std::string） =====
static int mp_write_cb(void* data, const char* buf, size_t len) {
    ((std::string*)data)->append(buf, len); return 0;
}

// ===== 打包辅助（C++ 内联包装 jmsgpack C API） =====
static void pack_entity(jmsgpack_packer_t* pk, const S2CEntity& e) {
    jmsgpack_pack_array(pk, 8);
    jmsgpack_pack_int(pk, e.id); jmsgpack_pack_int(pk, e.type);
    jmsgpack_pack_int64(pk, e.position_x); jmsgpack_pack_int64(pk, e.position_y);
    jmsgpack_pack_int64(pk, e.width); jmsgpack_pack_int64(pk, e.height);
    jmsgpack_pack_int64(pk, e.hp); jmsgpack_pack_int(pk, e.player_conv);
}
static void pack_join(jmsgpack_packer_t* pk, const S2CPlayerJoin& j) {
    jmsgpack_pack_array(pk, 3);
    jmsgpack_pack_int(pk, j.conv); jmsgpack_pack_int64(pk, j.position_x); jmsgpack_pack_int64(pk, j.position_y);
}
static void pack_leave(jmsgpack_packer_t* pk, const S2CPlayerLeave& l) {
    jmsgpack_pack_array(pk, 1); jmsgpack_pack_int(pk, l.conv);
}
static void pack_input(jmsgpack_packer_t* pk, const S2CPlayerInput& i) {
    jmsgpack_pack_array(pk, 2); jmsgpack_pack_int(pk, i.conv); jmsgpack_pack_int(pk, i.keycode);
}

// C2S 打包: [cmd, payload]
static std::string pack_c2s(int cmd, int64_t join_x, int64_t join_y, int input_kc) {
    char buf[256]; jmsgpack_packer_t pk; jmsgpack_packer_init(&pk, (std::string*)buf, mp_write_cb);
    // FIXME: 简化实现，直接用 std::string
    std::string out;
    jmsgpack_packer_t pk2; jmsgpack_packer_init(&pk2, &out, mp_write_cb);
    if (cmd == CMD_PLAYER_JOIN) {
        jmsgpack_pack_array(&pk2, 2); jmsgpack_pack_int(&pk2, cmd);
        jmsgpack_pack_array(&pk2, 2); jmsgpack_pack_int64(&pk2, join_x); jmsgpack_pack_int64(&pk2, join_y);
    } else if (cmd == CMD_PLAYER_INPUT) {
        jmsgpack_pack_array(&pk2, 2); jmsgpack_pack_int(&pk2, cmd);
        jmsgpack_pack_array(&pk2, 1); jmsgpack_pack_int(&pk2, input_kc);
    } else {
        jmsgpack_pack_array(&pk2, 2); jmsgpack_pack_int(&pk2, cmd); jmsgpack_pack_nil(&pk2);
    }
    return out;
}

// S2C 世界打包
static std::string pack_world(int entity_id, const S2CEntity* entities, int count) {
    std::string out; jmsgpack_packer_t pk; jmsgpack_packer_init(&pk, &out, mp_write_cb);
    jmsgpack_pack_array(&pk, 2); jmsgpack_pack_int(&pk, entity_id);
    jmsgpack_pack_array(&pk, count);
    for (int i = 0; i < count; i++) pack_entity(&pk, entities[i]);
    return out;
}

// S2C 打包: [cmd, payload]
static std::string pack_s2c(int16_t cmd, int32_t frame_id,
    const S2CPlayerJoin* joins, int jn, const S2CPlayerLeave* leaves, int ln,
    const S2CPlayerInput* inputs, int in, const char* checksum)
{
    std::string out; jmsgpack_packer_t pk; jmsgpack_packer_init(&pk, &out, mp_write_cb);
    jmsgpack_pack_array(&pk, 2); jmsgpack_pack_int(&pk, cmd);
    if (cmd == S2C_CMD_COMMAND) {
        jmsgpack_pack_array(&pk, 5);
        jmsgpack_pack_int(&pk, frame_id);
        jmsgpack_pack_array(&pk, jn); for (int i = 0; i < jn; i++) pack_join(&pk, joins[i]);
        jmsgpack_pack_array(&pk, ln); for (int i = 0; i < ln; i++) pack_leave(&pk, leaves[i]);
        jmsgpack_pack_array(&pk, in); for (int i = 0; i < in; i++) pack_input(&pk, inputs[i]);
        jmsgpack_pack_str(&pk, checksum ? strlen(checksum) : 0);
        if (checksum) jmsgpack_pack_str_body(&pk, checksum, strlen(checksum));
    } else {
        jmsgpack_pack_nil(&pk);
    }
    return out;
}

// S2C 加载打包
static std::string pack_s2c_loading(int32_t conv, int32_t frame_id, int32_t world_eid,
    const S2CEntity* entities, int ec)
{
    std::string out; jmsgpack_packer_t pk; jmsgpack_packer_init(&pk, &out, mp_write_cb);
    jmsgpack_pack_array(&pk, 2); jmsgpack_pack_int(&pk, S2C_CMD_LOADING);
    jmsgpack_pack_array(&pk, 3);
    jmsgpack_pack_int(&pk, conv); jmsgpack_pack_int(&pk, frame_id);
    jmsgpack_pack_array(&pk, 2); jmsgpack_pack_int(&pk, world_eid);
    jmsgpack_pack_array(&pk, ec);
    for (int i = 0; i < ec; i++) pack_entity(&pk, entities[i]);
    return out;
}

// ===== 解包辅助 =====
static int64_t jmp_int(const jmsgpack_object_t* o) {
    if (o->type == JMSGPACK_OBJECT_POSITIVE_INTEGER) return (int64_t)o->via.u64;
    if (o->type == JMSGPACK_OBJECT_NEGATIVE_INTEGER) return o->via.i64;
    return 0;
}

// 解包 C2S
static int unpack_c2s(const char* data, int len, int* cmd, int64_t* join_x, int64_t* join_y, int* input_kc) {
    jmsgpack_unpacked_t r; jmsgpack_unpacked_init(&r);
    size_t off = 0;
    if (jmsgpack_unpack_next(&r, data, len, &off) < 0) { jmsgpack_unpacked_destroy(&r); return 0; }
    if (r.data.type != JMSGPACK_OBJECT_ARRAY || r.data.via.array.size < 2) { jmsgpack_unpacked_destroy(&r); return 0; }
    *cmd = (int)jmp_int(&r.data.via.array.ptr[0]);
    auto* payload = &r.data.via.array.ptr[1];
    if (payload->type == JMSGPACK_OBJECT_ARRAY) {
        switch (*cmd) {
        case CMD_PLAYER_JOIN:
            if (payload->via.array.size >= 2) { *join_x = jmp_int(&payload->via.array.ptr[0]); *join_y = jmp_int(&payload->via.array.ptr[1]); }
            break;
        case CMD_PLAYER_INPUT:
            if (payload->via.array.size >= 1) *input_kc = (int)jmp_int(&payload->via.array.ptr[0]);
            break;
        }
    }
    jmsgpack_unpacked_destroy(&r);
    return 1;
}

// 解包 S2C 世界
static int unpack_world(const char* data, int len, int* eid, S2CEntity* entities, int max_ec) {
    jmsgpack_unpacked_t r; jmsgpack_unpacked_init(&r);
    size_t off = 0;
    if (jmsgpack_unpack_next(&r, data, len, &off) < 0) { jmsgpack_unpacked_destroy(&r); return 0; }
    if (r.data.type != JMSGPACK_OBJECT_ARRAY || r.data.via.array.size < 2) { jmsgpack_unpacked_destroy(&r); return 0; }
    *eid = (int)jmp_int(&r.data.via.array.ptr[0]);
    auto* arr = &r.data.via.array.ptr[1];
    int n = (int)arr->via.array.size;
    if (n > max_ec) n = max_ec;
    for (int i = 0; i < n; i++) {
        auto* o = &arr->via.array.ptr[i];
        if (o->type != JMSGPACK_OBJECT_ARRAY || o->via.array.size < 8) continue;
        auto& e = entities[i]; memset(&e, 0, sizeof(e));
        e.id = (int)jmp_int(&o->via.array.ptr[0]); e.type = (int)jmp_int(&o->via.array.ptr[1]);
        e.position_x = jmp_int(&o->via.array.ptr[2]); e.position_y = jmp_int(&o->via.array.ptr[3]);
        e.width = jmp_int(&o->via.array.ptr[4]); e.height = jmp_int(&o->via.array.ptr[5]);
        e.hp = jmp_int(&o->via.array.ptr[6]); e.player_conv = (int)jmp_int(&o->via.array.ptr[7]);
    }
    jmsgpack_unpacked_destroy(&r);
    return n;
}

// 解包 S2C（获取 cmd + 命令数据）
static int unpack_s2c_cmd(const char* data, int len, int* cmd, int* frame_id,
    S2CPlayerJoin* joins, int max_jn, int* jn,
    S2CPlayerLeave* leaves, int max_ln, int* ln,
    S2CPlayerInput* inputs, int max_in, int* in)
{
    jmsgpack_unpacked_t r; jmsgpack_unpacked_init(&r);
    size_t off = 0;
    if (jmsgpack_unpack_next(&r, data, len, &off) < 0) { jmsgpack_unpacked_destroy(&r); return 0; }
    if (r.data.type != JMSGPACK_OBJECT_ARRAY || r.data.via.array.size < 2) { jmsgpack_unpacked_destroy(&r); return 0; }
    *cmd = (int)jmp_int(&r.data.via.array.ptr[0]);
    auto* pl = &r.data.via.array.ptr[1];
    if (pl->type != JMSGPACK_OBJECT_ARRAY || pl->via.array.size < 5) { jmsgpack_unpacked_destroy(&r); return 1; }
    if (pl->via.array.size >= 1) *frame_id = (int)jmp_int(&pl->via.array.ptr[0]);
    auto xarr = [&](int idx, auto* out, int max, int& cnt) {
        cnt = 0;
        if (idx >= (int)pl->via.array.size) return;
        auto* a = &pl->via.array.ptr[idx];
        if (a->type != JMSGPACK_OBJECT_ARRAY) return;
        int n = (int)a->via.array.size; if (n > max) n = max;
        for (int i = 0; i < n; i++) {
            auto* o = &a->via.array.ptr[i]; auto& d = out[cnt]; memset(&d, 0, sizeof(d));
            using T = std::decay_t<decltype(d)>;
            if constexpr (std::is_same_v<T, S2CPlayerJoin>) {
                if (o->type == JMSGPACK_OBJECT_ARRAY && o->via.array.size >= 3) {
                    d.conv = (int)jmp_int(&o->via.array.ptr[0]); d.position_x = jmp_int(&o->via.array.ptr[1]); d.position_y = jmp_int(&o->via.array.ptr[2]);
                    cnt++;
                }
            } else if constexpr (std::is_same_v<T, S2CPlayerLeave>) {
                if (o->type == JMSGPACK_OBJECT_ARRAY && o->via.array.size >= 1) { d.conv = (int)jmp_int(&o->via.array.ptr[0]); cnt++; }
            } else if constexpr (std::is_same_v<T, S2CPlayerInput>) {
                if (o->type == JMSGPACK_OBJECT_ARRAY && o->via.array.size >= 2) { d.conv = (int)jmp_int(&o->via.array.ptr[0]); d.keycode = (int)jmp_int(&o->via.array.ptr[1]); cnt++; }
            }
        }
    };
    xarr(1, joins, max_jn, *jn);
    xarr(2, leaves, max_ln, *ln);
    xarr(3, inputs, max_in, *in);
    jmsgpack_unpacked_destroy(&r);
    return 1;
}


// 位掩码定义
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;
const int INPUT_ATTACK = 1 << 4;
const int INPUT_DIR_MASK = INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT;

static int input_to_dir(int keycode) {
	if (keycode & INPUT_DOWN)  return 0;
	if (keycode & INPUT_LEFT)  return 1;
	if (keycode & INPUT_RIGHT) return 2;
	if (keycode & INPUT_UP)    return 3;
	return 0;
}
static bool has_dir(int keycode) { return (keycode & INPUT_DIR_MASK) != 0; }

static const fp_t MOVE_SPEED = fp_from_float(5.0f);

struct game_scene {
	scene_p scene;
	flecs::world world;
	jtilemap_p tilemap = NULL;

	int g_id = 1;
	int g_frameid = 1;

	simple_fps_counter_p sample_fps;

	float accumulator = 0.0f;

	float serverTickTimer = 0.0f;
	float SERVER_TICK_INTERVAL = 1.0f / 20.0f;

	netserver_p netserver;
	Context* ctx;

	flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
	flecs::query<ConnectionComponent> connection_query;
	flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
	flecs::query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent> enemy_query;
	flecs::query<IdComponent, LogicRectComponent, TransformComponent> drawing_entity_query;
	flecs::query<AttackRayEffectComponent> drawing_attack_rayeffect_query;

	std::vector<S2CPlayerJoin> player_joins;
	std::vector<S2CPlayerLeave> player_leaves;
	std::vector<S2CPlayerInput> player_inputs;

	std::map<int, std::string> commands;
	std::queue<std::string> command_queue;
	std::map<int, std::string> worlds;

};

// 纹理缓存
static std::unordered_map<std::string, jsdl_texture_t*> g_tex_cache;

jsdl_texture_t* tex_cache_get(jsdl_renderer_t* r, const char* path) {
	std::string k(path);
	auto it = g_tex_cache.find(k);
	if (it != g_tex_cache.end()) return it->second;
	int w, h, ch;
	auto* d = stbi_load(path, &w, &h, &ch, 4);
	if (!d) return nullptr;
	auto* s = jsdl_create_surface_from(w, h, JSDL_PIXELFORMAT_RGBA32, d, w * 4);
	if (!s) { stbi_image_free(d); return nullptr; }
	auto* t = jsdl_create_texture_from_surface(r, s);
	jsdl_destroy_surface(s);
	stbi_image_free(d);
	g_tex_cache[k] = t;
	return t;
}

void tex_cache_clear() {
	for (auto& kv : g_tex_cache)
		if (kv.second) jsdl_destroy_texture(kv.second);
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
				if (id.hp <= 1) {
					id.hp = 1;
				}
				else{
					id.hp--;
				}
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
	if (msg_data) {
		SDL_free(msg_data);
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
	int cmd, frame_id, jn = 0, ln = 0, in = 0;
	S2CPlayerJoin joins[32]; S2CPlayerLeave leaves[32]; S2CPlayerInput inputs[64];
	if (!unpack_s2c_cmd(command_data.c_str(), (int)command_data.size(), &cmd, &frame_id,
		joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in) || cmd != S2C_CMD_COMMAND) {
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

	// 玩家离开
	self->world.defer_begin();
	for (int i = 0; i < ln; i++) {
		int conv = leaves[i].conv;
		self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
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
	self->netserver = netserver_create(NET_SERVER_WEBSOCKET, "192.168.1.21", 10000);
	//self->netserver = netserver_create(NET_SERVER_WEBSOCKET, "192.168.1.28", 10000);

	debug_layer_p debug_layer = create_debug_layer();
	scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));

	self->world.set_ctx(self->ctx);

	// 本地方块：有碰撞检测，但不会同步到客户端（因为有 LocalOnlyComponent）
	self->world.entity()
		.add<LocalOnlyComponent>()
		.set<IdComponent>({ 10000, 100 })
		.set<LogicRectComponent>({ fp_from_float(0.5f), fp_from_float(0.5f) })
		.set<LogicPositionComponent>({ fp_from_float(5), fp_from_float(5) })
		.set<TransformComponent>({ 5,5,0,1,1 });

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

	// ---- tilemap + 精灵渲染初始化 ----
	if (self->tilemap)
		jtilemap_render_init(self->ctx->renderer, "adventure_server/", NULL);

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
	if (SDL_strcmp(key, "q") == 0 || SDL_strcmp(key, "Q") == 0) {
		SDL_Event quit_event;
		SDL_zero(quit_event);
		quit_event.type = SDL_EVENT_QUIT;
		jsdl_push_quit_event();
	}
}

static void on_handle_event(scene_p s, const void* ev)
{
	event_callbacks_t cbs = {};
	cbs.on_keypressed = server_on_keypressed;
	dispatch_event(cbs, (const SDL_Event*)ev, NULL);
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


		// 位置同步：HandleCommandSystem/UpdateAI 更新了 LogicPositionComponent，
		// 手动同步 TransformComponent 确保渲染正确
		self->player_query.each([&](flecs::entity e, PlayerComponent&, IdComponent&,
			LogicRectComponent&, LogicPositionComponent& pos) {
				if (e.has<TransformComponent>()) {
					auto* t = e.get_mut<TransformComponent>();
					t->position_x = fp_to_float(pos.x);
					t->position_y = fp_to_float(pos.y);
				}
			});
		self->enemy_query.each([&](flecs::entity e, EnemyComponent&, AIComponent&,
			IdComponent&, LogicRectComponent&, LogicPositionComponent& pos) {
				if (e.has<TransformComponent>()) {
					auto* t = e.get_mut<TransformComponent>();
					t->position_x = fp_to_float(pos.x);
					t->position_y = fp_to_float(pos.y);
				}
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
		jtilemap_render_all(self->tilemap, cx, cy, jsdl_get_ticks());
	}

	// ---- ECS 实体（碰撞体） ----
	self->drawing_entity_query.each(DrawingEntitySystem);
	self->drawing_attack_rayeffect_query.each(DrawingAttackRayEffectSystem);

	// ---- ECS 精灵渲染（玩家） ----
	self->world.filter_builder<TransformComponent, LogicRectComponent,
		SpriteSheetComponent, AnimationFrameComponent, PlayerActionComponent, PlayerComponent>()
		.build().each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
			SpriteSheetComponent& ss, AnimationFrameComponent& af,
			PlayerActionComponent& act, PlayerComponent&) {
				sprite_render_system(it.entity(i), t, r, ss, af, act);
			});

	// ---- ECS 精灵渲染（敌人） ----
	self->world.filter_builder<TransformComponent, LogicRectComponent,
		SpriteSheetComponent, AnimationFrameComponent, PlayerActionComponent, EnemyComponent>()
		.build().each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
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
			jsdl_rect_t bg = { bar_x - 1, bar_y - 1, bar_w + 2, bar_h + 2 };
			jsdl_set_render_draw_color(ctx->renderer, 0, 0, 0, 255);
			jsdl_render_fill_rect(ctx->renderer, &bg);
			jsdl_rect_t base = { bar_x, bar_y, bar_w, bar_h };
			jsdl_set_render_draw_color(ctx->renderer, 40, 40, 40, 255);
			jsdl_render_fill_rect(ctx->renderer, &base);
			float ratio = (float)id.hp / 10.0f;
			if (ratio > 1.0f) ratio = 1.0f;
			if (ratio < 0) ratio = 0;
			jsdl_rect_t fill = { bar_x, bar_y, bar_w * ratio, bar_h };
			if (ratio > 0.6f)      jsdl_set_render_draw_color(ctx->renderer, 60, 210, 60, 255);
			else if (ratio > 0.3f) jsdl_set_render_draw_color(ctx->renderer, 210, 210, 60, 255);
			else                   jsdl_set_render_draw_color(ctx->renderer, 220, 60, 60, 255);
			jsdl_render_fill_rect(ctx->renderer, &fill);
		});
	}
}

static void on_destroy(scene_p s)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	simple_fps_destory(self->sample_fps);
	netserver_destroy(self->netserver);
	tex_cache_clear();
	jtilemap_render_destroy();
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
