#include "game_scene.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <math.h>
#include <joy2d/jtext.h>
#include "../asset_manager.h"
#include "../layers/debug_layer.h"
#include "../layers/gameplay_controls_layer.h"

struct game_scene {
	scene_p scene;
	Context2* ctx;
	netclient_p netclient;
	flecs::world esc_world;

	//std::map<int, int> server_inputs;
	//bool ready = false;
	//int server_frameid = 0;
	//int server_entity_id = 0;

	//DebugLayer* debugLayer = NULL;
	//MobileInputLayer* mobileInputLayer = NULL;



	//// 按键状态
	//bool upPressed = false;
	//bool downPressed = false;
	//bool leftPressed = false;
	//bool rightPressed = false;
	//bool attackPressed = false;

	//// 固定逻辑步
	//Uint64 lastTime = SDL_GetTicks();
	//float accumulator = 0.0f;
	//float FIXED_TIMESTEP = 1.0f / 50.0f;

	//int current_input_mask = 0;
	//float heartbeatTimer = 0.0f;

	//bool attack_triggered = false;   // 攻击键按下待发送

	//// ========== 预测回滚相关 ==========
	//uint32_t local_conv = 0;                     // 本地玩家的 conv（连接标识）
	//std::map<int, std::map<int, StateSnapshot>> snapshots;        // 按 frame_id 递增存储的历史快照
	//std::vector<int> pending_inputs;     // 已发送但未确认的输入

	//// Flecs 查询
	//flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
	//flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
	//flecs::query<IdComponent, LogicPositionComponent, TransformComponent> sync_query;
	//flecs::query<IdComponent, LogicRectComponent, TransformComponent> renderer_query;
	//flecs::query<AttackRayEffectComponent> renderer_attack_rayeffect_query;

};

static void on_load(scene_p s) {
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	log_info("[GameScene] OnStart...");

	debug_layer_p debug_layer = create_debug_layer();
	gameplay_controls_layer_p gameplay_controls_layer = create_gameplay_controls_layer(self->ctx);
	scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));
	scene_add_root_node(self->scene, gameplay_controls_layer_get_node(gameplay_controls_layer));





}

static void on_update(scene_p s, float dt) {
	(void)s;
	(void)dt;
	// 模拟加载进度（可后续扩展）
}

static void on_render(scene_p s) {
	(void)s;
	// TODO: 绘制进度条
}

static void on_destroy(scene_p s) {
	game_scene_p self = (game_scene_p)scene_get_userdata(s);
	free(self);
}

game_scene_p game_scene_create(Context2* ctx) {
	game_scene_p self = (game_scene_p)SDL_malloc(sizeof(game_scene_t));
	SDL_assert(self);
	SDL_memset(self, 0, sizeof(game_scene_t));
	self->scene = scene_create("game");
	self->ctx = ctx;
	scene_set_userdata(self->scene, self);
	scene_set_load_callback(self->scene, on_load);
	scene_set_update_callback(self->scene, on_update);
	scene_set_render_callback(self->scene, on_render);
	scene_set_destroy_callback(self->scene, on_destroy);
	return self;
}

void game_scene_destroy(game_scene_p s) {
	if (s) {
		if (s->scene) {
			scene_destroy(s->scene);
		}
		free(s);
	}
}

scene_p game_scene_get_scene(game_scene_p s) {
	return s ? s->scene : NULL;
}
