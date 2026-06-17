#define STB_IMAGE_IMPLEMENTATION
#include <joy/external/stb_image.h>
#include <unordered_map>
#include <map>
#include <string>
#include <algorithm>
#include <joy/jmsgpack.h>
#include "../flecs.h"
#include "game_scene.h"
#include <joy/jmath.h>
#include <joy/jcore.h>
#include <joy/jr2d.h>
#include <joy/jcollision.h>
#include <joy/jnetwork.h>
#include <joy/jtilemap.h>
#include <joy/jtilemap_render.h>
#include <joy/jaudio.h>
#include "../components/sprite_sheet_component.h"
#include "../components/player_action_component.h"
#include "../components/ai_component.h"
#include "../components/enemy_component.h"
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

// ===== 协议枚举/类型定义（基于 jmsgpack） =====
enum C2SCmd_ {
    CMD_UNKNOWN = 0, CMD_PLAYER_JOIN = 1, CMD_PLAYER_LEAVE = 2,
    CMD_PLAYER_INPUT = 3, CMD_PLAYER_HEART = 4, CMD_LOADING = 5,
};
enum S2CCmd_ { S2C_CMD_NONE = 0, S2C_CMD_LOADING = 1, S2C_CMD_COMMAND = 2, };
enum S2CType_ { S2C_TYPE_NORMAL = 0, S2C_TYPE_PLAYER = 1, S2C_TYPE_ENEMY = 2, };

// ===== 协议类型定义 =====
struct S2CPlayerJoin { int32_t conv; int64_t position_x, position_y; };
struct S2CPlayerLeave { int32_t conv; };
struct S2CPlayerInput { int32_t conv, keycode; };
struct S2CEntity { int32_t id, type; int64_t position_x, position_y, width, height, hp; int32_t player_conv; };

static int mp_write_cb(void* data, const char* buf, size_t len) {
    ((std::string*)data)->append(buf, len); return 0;
}
static inline int64_t jmp_int(const jmsgpack_object_t* o) {
    if (o->type == JMSGPACK_OBJECT_POSITIVE_INTEGER) return (int64_t)o->via.u64;
    if (o->type == JMSGPACK_OBJECT_NEGATIVE_INTEGER) return o->via.i64;
    return 0;
}
static inline uint32_t jmp_arr_size(const jmsgpack_object_t* o, int idx) {
    return (o->type == JMSGPACK_OBJECT_ARRAY && (int)o->via.array.size > idx) ? o->via.array.size : 0;
}
static inline const jmsgpack_object_t* jmp_arr_at(const jmsgpack_object_t* o, int idx) {
    return (o->type == JMSGPACK_OBJECT_ARRAY && (int)o->via.array.size > idx) ? &o->via.array.ptr[idx] : nullptr;
}

static std::string pack_c2s(int cmd, int64_t join_x, int64_t join_y, int input_kc) {
    std::string out; jmsgpack_packer_t pk; jmsgpack_packer_init(&pk, &out, mp_write_cb);
    jmsgpack_pack_array(&pk, 2); jmsgpack_pack_int(&pk, cmd);
    if (cmd == 1) { jmsgpack_pack_array(&pk, 2); jmsgpack_pack_int64(&pk, join_x); jmsgpack_pack_int64(&pk, join_y); }
    else if (cmd == 3) { jmsgpack_pack_array(&pk, 1); jmsgpack_pack_int(&pk, input_kc); }
    else { jmsgpack_pack_nil(&pk); }
    return out;
}

static void unpack_entity(const jmsgpack_object_t* o, S2CEntity* e) {
    if (!o || o->type != JMSGPACK_OBJECT_ARRAY || o->via.array.size < 8) return;
    e->id = (int)jmp_int(&o->via.array.ptr[0]); e->type = (int)jmp_int(&o->via.array.ptr[1]);
    e->position_x = jmp_int(&o->via.array.ptr[2]); e->position_y = jmp_int(&o->via.array.ptr[3]);
    e->width = jmp_int(&o->via.array.ptr[4]); e->height = jmp_int(&o->via.array.ptr[5]);
    e->hp = jmp_int(&o->via.array.ptr[6]); e->player_conv = (int)jmp_int(&o->via.array.ptr[7]);
}

// 解析 S2C loading 消息: 返回实体数，通过出参返回 conv/frame_id/eid
static int unpack_loading(const char* data, int len, int* conv, int* frame_id, int* eid,
    S2CEntity* entities, int max_ec) {
    jmsgpack_unpacked_t r; jmsgpack_unpacked_init(&r);
    size_t off = 0;
    if (jmsgpack_unpack_next(&r, data, len, &off) < 0) { jmsgpack_unpacked_destroy(&r); return -1; }
    auto* pl = jmp_arr_at(&r.data, 1);
    if (!pl || pl->type != JMSGPACK_OBJECT_ARRAY || pl->via.array.size < 3) { jmsgpack_unpacked_destroy(&r); return -1; }
    if (conv) *conv = (int)jmp_int(&pl->via.array.ptr[0]);
    if (frame_id) *frame_id = (int)jmp_int(&pl->via.array.ptr[1]);
    auto* w = jmp_arr_at(pl, 2);
    if (!w || w->type != JMSGPACK_OBJECT_ARRAY || w->via.array.size < 2) { jmsgpack_unpacked_destroy(&r); return -1; }
    if (eid) *eid = (int)jmp_int(&w->via.array.ptr[0]);
    auto* arr = jmp_arr_at(w, 1);
    if (!arr) { jmsgpack_unpacked_destroy(&r); return 0; }
    int n = (int)arr->via.array.size; if (n > max_ec) n = max_ec;
    for (int i = 0; i < n; i++) unpack_entity(&arr->via.array.ptr[i], &entities[i]);
    jmsgpack_unpacked_destroy(&r);
    return n;
}

// 解析 S2C command 消息: 返回 1 成功
static int unpack_command(const char* data, int len, int* frame_id,
    S2CPlayerJoin* joins, int max_jn, int* jn,
    S2CPlayerLeave* leaves, int max_ln, int* ln,
    S2CPlayerInput* inputs, int max_in, int* in) {
    memset(joins, 0, max_jn * sizeof(S2CPlayerJoin)); *jn = 0;
    memset(leaves, 0, max_ln * sizeof(S2CPlayerLeave)); *ln = 0;
    memset(inputs, 0, max_in * sizeof(S2CPlayerInput)); *in = 0;
    jmsgpack_unpacked_t r; jmsgpack_unpacked_init(&r);
    size_t off = 0;
    if (jmsgpack_unpack_next(&r, data, len, &off) < 0) { jmsgpack_unpacked_destroy(&r); return 0; }
    auto* pl = jmp_arr_at(&r.data, 1);
    if (!pl || pl->type != JMSGPACK_OBJECT_ARRAY || pl->via.array.size < 5) { jmsgpack_unpacked_destroy(&r); return (int)jmp_int(&r.data.via.array.ptr[0]) == 0; }
    if (frame_id) *frame_id = (int)jmp_int(&pl->via.array.ptr[0]);
    auto xarr = [&](int idx, auto* out, int max, int& cnt) {
        auto* a = jmp_arr_at(pl, idx); if (!a) return;
        int n = (int)a->via.array.size; if (n > max) n = max; cnt = n;
        for (int i = 0; i < n; i++) {
            auto* o = &a->via.array.ptr[i]; auto& d = out[i];
            using T = std::decay_t<decltype(d)>;
            if constexpr (std::is_same_v<T, S2CPlayerJoin>) {
                if (o->type == JMSGPACK_OBJECT_ARRAY && o->via.array.size >= 3) { d.conv = (int)jmp_int(&o->via.array.ptr[0]); d.position_x = jmp_int(&o->via.array.ptr[1]); d.position_y = jmp_int(&o->via.array.ptr[2]); }
            } else if constexpr (std::is_same_v<T, S2CPlayerLeave>) {
                if (o->type == JMSGPACK_OBJECT_ARRAY && o->via.array.size >= 1) { d.conv = (int)jmp_int(&o->via.array.ptr[0]); }
            } else if constexpr (std::is_same_v<T, S2CPlayerInput>) {
                if (o->type == JMSGPACK_OBJECT_ARRAY && o->via.array.size >= 2) { d.conv = (int)jmp_int(&o->via.array.ptr[0]); d.keycode = (int)jmp_int(&o->via.array.ptr[1]); }
            }
        }
    };
    xarr(1, joins, max_jn, *jn); xarr(2, leaves, max_ln, *ln); xarr(3, inputs, max_in, *in);
    jmsgpack_unpacked_destroy(&r);
    return 1;
}

// 输入掩码
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;
const int INPUT_ATTACK = 1 << 4;
const int INPUT_DIR_MASK = INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT;

// 从 keycode 提取方向索引 (0=下, 1=左, 2=右, 3=上)
static int input_to_dir(int keycode) {
	if (keycode & INPUT_DOWN)  return 0;
	if (keycode & INPUT_LEFT)  return 1;
	if (keycode & INPUT_RIGHT) return 2;
	if (keycode & INPUT_UP)    return 3;
	return 0;
}
static bool has_dir(int keycode) { return (keycode & INPUT_DIR_MASK) != 0; }

// 状态快照（用于预测回滚）
struct state_snapshot {
	fp_t position_x, position_y;
};

// 玩家状态机（使用 ECS PlayerActionComponent）

// 纹理缓存
static std::unordered_map<std::string, image_p> g_tex_cache;

image_p tex_cache_get(jr2d_p r2d, const char* path) {
	std::string k(path);
	auto it = g_tex_cache.find(k);
	if (it != g_tex_cache.end()) return it->second;
	image_p t = image_create(r2d, path);
	g_tex_cache[k] = t;
	return t;
}

void tex_cache_clear() {
	for (auto& kv : g_tex_cache)
		if (kv.second) image_destroy(kv.second);
	g_tex_cache.clear();
}

struct game_scene {
	scene_p scene;
	context* ctx;
	netclient_p netclient;
	flecs::world ecs_world;

	// ---- tilemap + 音频 ----
	jtilemap_p          tilemap = NULL;
	jtilemap_render_p   tilemap_render = NULL;
	uint32_t            audio_dev = 0;
	audio_shot_p        step_sound = NULL;
	audio_bgm_p         bgm = NULL;
	int local_dir = 0; // 本地玩家方向
	// 远程玩家方向追踪
	std::map<flecs::entity_t, int>  remote_dirs;

	// AI 查询
	flecs::query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent> enemy_query;

	std::map<int, int> server_inputs;
	bool ready = false;
	int server_frameid = 0;
	int server_entity_id = 0;

	// 固定逻辑步
	Uint64 lastTime = get_ticks();
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

static void handle_loading(game_scene_p self, int conv, int frame_id, int eid, const S2CEntity* entities, int ec)
{
	self->ready = true;
	self->local_conv = conv;
	self->server_frameid = frame_id;
	self->server_entity_id = eid;

	self->ecs_world.defer_begin();
	for (int i = 0; i < ec; i++) {
		auto& entity = entities[i];
		int64_t hp = entity.hp;
		auto e = self->ecs_world.entity()
			.set<IdComponent>({ entity.id, hp })
			.set<LogicRectComponent>({ entity.width, entity.height })
			.set<LogicPositionComponent>({ entity.position_x, entity.position_y })
			.set<TransformComponent>({ fp_to_float(entity.position_x),
						   fp_to_float(entity.position_y), 0, 1, 1 });
		if (entity.type == S2C_TYPE_PLAYER) {
			SpriteSheetComponent ss{};
			snprintf(ss.path, sizeof(ss.path), "adventure_client/textures/knights/troops/warrior/warrior_blue.png");
			ss.frame_w = 192; ss.frame_h = 192;
			ss.idle_row = 0; ss.walk_row = 1;
			ss.atk_rows[0] = 4; ss.atk_rows[1] = 2;
			ss.atk_rows[2] = 2; ss.atk_rows[3] = 6;
			ss.frame_count = 6; ss.frame_duration = 0.15f;
			e.set<PlayerActionComponent>({});
			e.set<SpriteSheetComponent>(ss);
			e.set<AnimationFrameComponent>({});
			e.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) });
			e.set<PlayerComponent>({ entity.player_conv });
		}
		else if (entity.type == S2C_TYPE_ENEMY) {
			SpriteSheetComponent ss{};
			snprintf(ss.path, sizeof(ss.path), "adventure_client/textures/knights/troops/warrior/warrior_red.png");
			ss.frame_w = 192; ss.frame_h = 192;
			ss.idle_row = 0; ss.walk_row = 1;
			ss.atk_rows[0] = 4; ss.atk_rows[1] = 2;
			ss.atk_rows[2] = 2; ss.atk_rows[3] = 6;
			ss.frame_count = 6; ss.frame_duration = 0.15f;
			e.add<EnemyComponent>()
				.set<AIComponent>({ 0, 0,
					fp_from_float(4.0f), fp_from_float(15.0f),
					fp_from_float(1.5f), 0, 0, 0, 0 })
				.set<PlayerActionComponent>({})
				.set<SpriteSheetComponent>(ss)
				.set<AnimationFrameComponent>({})
				.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) });
		}
	}
	self->ecs_world.defer_end();

	// 保存当前快照
	std::map<int, state_snapshot> snapshots;
	self->player_query.each([&](PlayerComponent& p, IdComponent& id,
		LogicRectComponent& r, LogicPositionComponent& pos) {
			snapshots.insert({ id.id, { pos.x, pos.y } });
		});
	self->snapshots.insert({ self->server_frameid, snapshots });

	// 告知服务器本地玩家加入
	auto data = pack_c2s(CMD_PLAYER_JOIN, fp_from_float(3.2f), fp_from_float(2.3f), 0);
	netclient_send(self->netclient, data.c_str(), (int)data.size());
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

// 通用碰撞盒攻击辅助（dir: 0=下 1=左 2=右 3=上）
static void attack_by_hitbox(game_scene_p self,
	LogicPositionComponent* p, LogicRectComponent& currRect, IdComponent& currId, int dir)
{
	fp_t atk_range = fp_from_float(0.7f);
	fp_t ox = 0, oy = 0;
	if (dir == 0)      oy = atk_range;
	else if (dir == 1) ox = -atk_range;
	else if (dir == 2) ox = atk_range;
	else if (dir == 3) oy = -atk_range;
	rectanglef_t atk_rect = { fp_add(p->x, ox), fp_add(p->y, oy), currRect.width, currRect.height };
	self->body_query.each([&](IdComponent& id, LogicRectComponent& r, LogicPositionComponent& pos) {
		if (currId.id == id.id) return;
		rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
		contact2df_t contact;
		if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
			if (id.hp > 0) id.hp--;
		}
		});
}

// 攻击（减血 + 特效）
static void attack_with_damage_and_effect(game_scene_p self,
	LogicPositionComponent* p, LogicRectComponent& currRect,
	IdComponent& currId, int conv, int input)
{
	attack_by_hitbox(self, p, currRect, currId, input_to_dir(input));
}

// 仅攻击（减血，无特效）
static void attack_damage_only(game_scene_p self,
	LogicPositionComponent* p, LogicRectComponent& currRect,
	IdComponent& currId, int conv, int input)
{
	attack_by_hitbox(self, p, currRect, currId, input_to_dir(input));
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
						attack_by_hitbox(self, &pos, rect, id, atk_dir);
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
		auto iter = self->snapshots.find(self->server_frameid);
		if (iter != self->snapshots.end()) {
			const auto& snapshots = iter->second;
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

	self->server_frameid = frame_id;

	// 1. 处理新玩家加入
	for (int i = 0; i < jn; i++) {
		auto& pj = joins[i];
		log_info("%d:CMD_PLAYER_JOIN", pj.conv);
		fp_t x = pj.position_x;
		fp_t y = pj.position_y;
		SpriteSheetComponent ss{};
		snprintf(ss.path, sizeof(ss.path), "adventure_client/textures/knights/troops/warrior/warrior_blue.png");
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
			.set<PlayerComponent>({ pj.conv })
			.set<PlayerActionComponent>({})
			.set<SpriteSheetComponent>(ss)
			.set<AnimationFrameComponent>({});
	}

	// 2. 处理玩家离开
	self->ecs_world.defer_begin();
	for (int i = 0; i < ln; i++) {
		int conv = leaves[i].conv;
		self->player_query.each([&](flecs::entity entity,
			PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
				if (p.conv == conv) entity.destruct();
			});
	}
	self->ecs_world.defer_end();

	// 5. 应用所有玩家的输入
	for (int i = 0; i < in; i++) {
		auto& input = inputs[i];
		self->player_query.each([&](PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
				if (p.conv == input.conv) {
					if (input.conv == self->local_conv) {
						if (!self->pending_inputs.empty()
							&& self->pending_inputs.front() == input.keycode) {
							log_info("%d:%d", self->pending_inputs.front(), input.keycode);
							self->pending_inputs.erase(self->pending_inputs.begin());
						} else {
							bool found = false;
							for (auto it = self->pending_inputs.begin();
								it != self->pending_inputs.end(); ++it) {
								if (*it == input.keycode) {
									log_error("%d:%d", *it, input.keycode);
									self->pending_inputs.erase(it);
									found = true; break;
								}
							}
							if (!found)
								log_error("input:%d not found in pending", input.keycode);
						}
						if (input.keycode & INPUT_ATTACK)
							attack_damage_only(self, &pos, r, id, input.conv, input.keycode);
						movement(self, &pos, r, id, input.conv, input.keycode);
					} else {
						if (input.keycode & INPUT_ATTACK)
							attack_with_damage_and_effect(self, &pos, r, id, input.conv, input.keycode);
						movement(self, &pos, r, id, input.conv, input.keycode);
					}
				}
			});
	}

	// 保存快照
	{
		std::map<int, state_snapshot> snapshots;
		self->player_query.each([&](PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
				snapshots.insert({ id.id, { pos.x, pos.y } });
			});
		self->snapshots.insert({ self->server_frameid, snapshots });
	}

	// 预测
	for (auto& input : self->pending_inputs) {
		self->player_query.each([&](PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
				if (p.conv == self->local_conv)
					movement(self, &pos, r, id, p.conv, input);
			});
	}
	update_ai(self);
}

static void handle_command_s2c(game_scene_p self, const char* data, int len)
{
	int frame_id, jn = 0, ln = 0, in = 0;
	S2CPlayerJoin joins[32]; S2CPlayerLeave leaves[32]; S2CPlayerInput inputs[64];
	if (unpack_command(data, len, &frame_id, joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in))
		handle_command(self, frame_id, jn, ln, in, joins, leaves, inputs);
}


static void on_message(net_message_p msg, void* arg)
{
	game_scene_p self = (game_scene_p)arg;
	if (msg->type == NET_TYPE_CONNECTED) {
		auto d = pack_c2s(CMD_LOADING, 0, 0, 0);
		netclient_send(self->netclient, d.c_str(), (int)d.size());
	}
	else if (msg->type == NET_TYPE_MESSAGE) {
		// 先解析首元素 cmd 决定是 loading 还是 command
		jmsgpack_unpacked_t r; jmsgpack_unpacked_init(&r);
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
			if (ec >= 0) handle_loading(self, conv, frame_id, eid, entities, ec);
		} else {
			handle_command_s2c(self, (const char*)msg->data, (int)msg->len);
		}
	}
	if (msg->data) free(msg->data);
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
	self->ecs_world.component<AIComponent>();
	self->ecs_world.component<EnemyComponent>();

	self->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.1.21", 10000);
	//self->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.2.42", 10000);
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
	self->enemy_query = self->ecs_world.query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();

	// ---- tilemap 渲染 ----
	self->tilemap = jtilemap_load_file("adventure_client/tilemap/map.lua");
	if (self->tilemap) {
		self->tilemap_render = jtilemap_render_create(self->ctx->r2d, "adventure_client/", NULL);
		log_info("[tilemap] loaded %dx%d", self->tilemap->width, self->tilemap->height);
	}
	else log_error("[tilemap] failed to load");

	// ---- 音频 ----
	self->audio_dev = audio_open_device();
	self->step_sound = audio_shot_create("adventure_client/sounds/ak47.wav", self->audio_dev);
	audio_shot_set_gain(self->step_sound, 1.0f);
	self->bgm = audio_bgm_create("adventure_client/sounds/bgm.wav", self->audio_dev);
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
					if (e.has<PlayerActionComponent>()) {
						auto* act = e.get_mut<PlayerActionComponent>();
						act->state = PlayerActionComponent::ATTACK;
						act->dir = self->local_dir;
					}
				}
			});
		{
		auto data = pack_c2s(CMD_PLAYER_INPUT, 0, 0, INPUT_ATTACK);
		netclient_send(self->netclient, data.c_str(), (int)data.size());
		}
		return;
	}

	// 攻击中不处理移动
	bool is_attacking = false;
	self->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent&,
		LogicRectComponent&, LogicPositionComponent&) {
			if (p.conv == self->local_conv && e.has<PlayerActionComponent>())
				is_attacking = (e.get<PlayerActionComponent>()->state == PlayerActionComponent::ATTACK);
		});
	if (is_attacking) return;

	if (has_dir(self->current_input_mask)) {
		self->pending_inputs.push_back(self->current_input_mask);
		self->player_query.each([&](PlayerComponent& p, IdComponent& id,
			LogicRectComponent& r, LogicPositionComponent& pos) {
				if (p.conv == self->local_conv)
					movement(self, &pos, r, id, self->local_conv, self->current_input_mask);
			});
		auto data = pack_c2s(CMD_PLAYER_INPUT, 0, 0, self->current_input_mask);
		netclient_send(self->netclient, data.c_str(), (int)data.size());
	}
}

static void send_heartbeat(game_scene_p self, float dt)
{
	if (!self->netclient) return;
	self->heartbeatTimer += dt;
	if (self->heartbeatTimer >= 3.f) {
		self->heartbeatTimer = 0.f;
		auto d = pack_c2s(CMD_PLAYER_HEART, 0, 0, 0);
		netclient_send(self->netclient, d.c_str(), d.size());
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
		self->current_input_mask |= key_mask;
		if (key_mask == INPUT_ATTACK) self->attack_triggered = true;
		if (key_mask & INPUT_DIR_MASK)
			self->local_dir = input_to_dir(key_mask);
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
	if (key_mask) self->current_input_mask &= ~key_mask;
}

static void on_handle_event(scene_p s, const void* ev)
{
	game_scene_p self = (game_scene_p)scene_get_userdata(s);

	// 自定义触屏/UI 事件
	if (is_user_event(ev, GAMEPLAY_CONTROL_EVENT)) {
		int new_mask = self->current_input_mask;
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
			self->attack_triggered = true;
			break;
		default:
			new_mask &= ~(INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT);
			break;
		}
		if (new_mask != self->current_input_mask) {
			self->current_input_mask = new_mask;
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

// 敌人碰撞盒攻击
static void ai_attack_target(game_scene_p self, LogicPositionComponent* p,
	LogicRectComponent& currRect, IdComponent& currId)
{
	rectanglef_t atk_rect = { p->x, p->y, currRect.width, currRect.height };
	self->body_query.each([&](IdComponent& id, LogicRectComponent& r, LogicPositionComponent& pos) {
		if (currId.id == id.id) return;
		rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
		contact2df_t contact;
		if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
			if (id.hp > 0) id.hp--;
		}
		});
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

					// 本地玩家状态同步(不覆盖 ATTACK)
					if (e.has<PlayerActionComponent>()) {
						auto* act = e.get_mut<PlayerActionComponent>();
						if (act->state != PlayerActionComponent::ATTACK) {
							act->state = has_dir(self->current_input_mask) ? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
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
					}
					else { step_timer = 0; }
				}
				else {
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
					bool moved = (dx > th || dx < -th || dy > th || dy < -th);
					if (moved) { lx = pos.x; ly = pos.y; ldi = new_dir; }
					if (e.has<PlayerActionComponent>()) {
						auto* act = e.get_mut<PlayerActionComponent>();
						act->dir = new_dir;
						if (act->state != PlayerActionComponent::ATTACK)
							act->state = moved ? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
					}
				}
			});
	}
	self->ecs_world.defer_end();









}

static void on_render(scene_p s) {
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	r2d_set_color(self->ctx->r2d, 100, 100, 100, 255);

	// ---- tilemap 背景 ----
	if (self->tilemap && self->tilemap_render) {
		float cam_x_px = self->ctx->camera_x * (float)PIXELS_PER_METER;
		float cam_y_px = self->ctx->camera_y * (float)PIXELS_PER_METER;
		jtilemap_render_all(self->tilemap_render, self->tilemap, cam_x_px, cam_y_px, get_ticks());
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

	// ---- ECS 精灵渲染（敌人） ----
	self->ecs_world.filter_builder<TransformComponent, LogicRectComponent,
		SpriteSheetComponent, AnimationFrameComponent, PlayerActionComponent, EnemyComponent>()
		.build().each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
			SpriteSheetComponent& ss, AnimationFrameComponent& af,
			PlayerActionComponent& act, EnemyComponent&) {
			sprite_render_system(it.entity(i), t, r, ss, af, act);
		});

	// ---- 血条渲染（所有有血量实体头顶） ----
	{
		context* ctx = self->ctx;
		self->ecs_world.filter_builder<IdComponent, TransformComponent>()
			.build().each([ctx](flecs::iter& it, size_t i, IdComponent& id, TransformComponent& t) {
			flecs::entity e = it.entity(i);
			if (!e.has<PlayerComponent>() && !e.has<EnemyComponent>()) return;
			if (id.hp <= 0) return;

			float sx = (t.position_x - ctx->camera_x) * PIXELS_PER_METER;
			float sy = (t.position_y - ctx->camera_y) * PIXELS_PER_METER;
			// 精灵 192px, 碰撞盒 ~30px, 精灵顶部 = sy - (192-30)/2 = sy - 81
			const float SPRITE_OFF = 81.0f;
			float bar_w = 48.0f, bar_h = 8.0f;
			float bar_x = sx - bar_w / 2.0f, bar_y = sy - SPRITE_OFF - bar_h - 4.0f;
			// 黑色边框
			jrect_t bg = { bar_x - 1, bar_y - 1, bar_w + 2, bar_h + 2 };
			r2d_set_color(ctx->r2d, 0, 0, 0, 255);
			shape_draw_rectangle(ctx->r2d, "fill", bg);
			// 深灰背景
			jrect_t base = { bar_x, bar_y, bar_w, bar_h };
			r2d_set_color(ctx->r2d, 40, 40, 40, 255);
			shape_draw_rectangle(ctx->r2d, "fill", base);
			// 彩色填充
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

static void on_destroy(scene_p s) {
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	if (self->netclient) { netclient_destroy(self->netclient); self->netclient = NULL; }
	tex_cache_clear();
	audio_shot_destroy(self->step_sound);
	audio_bgm_destroy(self->bgm);
	if (self->audio_dev) audio_close_device(self->audio_dev);
	if (self->tilemap_render) jtilemap_render_destroy(self->tilemap_render);
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
