#include "game_scene.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <math.h>

struct game_scene {
    scene_p scene;
    Context2* ctx;

    netclient_p netclient = NULL;

    std::map<int, int> server_inputs;
    bool ready = false;
    int server_frameid = 0;
    int server_entity_id = 0;

    Resources* resources = NULL;
    DebugLayer* debugLayer = NULL;
    MobileInputLayer* mobileInputLayer = NULL;



    // 按键状态
    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
    bool attackPressed = false;

    // 固定逻辑步
    Uint64 lastTime = SDL_GetTicks();
    float accumulator = 0.0f;
    float FIXED_TIMESTEP = 1.0f / 50.0f;

    int current_input_mask = 0;
    float heartbeatTimer = 0.0f;

    bool attack_triggered = false;   // 攻击键按下待发送

    // ========== 预测回滚相关 ==========
    uint32_t local_conv = 0;                     // 本地玩家的 conv（连接标识）
    std::map<int, std::map<int, StateSnapshot>> snapshots;        // 按 frame_id 递增存储的历史快照
    std::vector<int> pending_inputs;     // 已发送但未确认的输入

    // Flecs 查询
    flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
    flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
    flecs::query<IdComponent, LogicPositionComponent, TransformComponent> sync_query;
    flecs::query<IdComponent, LogicRectComponent, TransformComponent> renderer_query;
    flecs::query<AttackRayEffectComponent> renderer_attack_rayeffect_query;

};

// ============================================================
// 节点回调（static，内部使用）
// ============================================================
static void menu_box_render(scene_node_p n, const void* arg) {
    float x, y;
    SDL_Renderer* renderer = (SDL_Renderer*)arg;
    scene_node_get_world_position(n, &x, &y);
    int z = scene_node_get_zorder(n);
    SDL_Color c;
    if (z >= 30)      c = { 80, 160, 255, 255 };
    else if (z >= 20) c = { 80, 255, 120, 255 };
    else              c = { 255, 80, 80, 255 };
    SDL_FRect r = { x - 30, y - 30, 60, 60 };
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &r);
}

static void menu_box_update(scene_node_p n, float dt) {
    (void)dt;
    float bx, by;
    float phase = 0.0f;
    float* p = (float*)scene_node_get_userdata(n);
    if (p) phase = *p;
    static float t = 0.0f;
    t += 0.016f;
    scene_node_get_scale(n, &bx, &by);
    scene_node_set_position(n, bx + sinf(t * 1.5f + phase) * 30.0f,
            by + cosf(t * 1.2f + phase) * 20.0f);
}

static void menu_box_destroy(scene_node_p n) {
    float* p = (float*)scene_node_get_userdata(n);
    free(p);
}

// ============================================================
// 场景回调（static）
// ============================================================

static void on_load(scene_p s) {
    game_scene_p self = (game_scene_p)scene_get_userdata(s);
    log_info("[GameScene] OnStart...");

   
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
