#include "loading_scene.h"
#include "../context.h"
#include <SDL3/SDL.h>

// ============================================================
// BaseScene 实现
// ============================================================

BaseScene::BaseScene(const char* name) {
    m_scene = scene_create(name);
    scene_set_userdata(m_scene, this);

    // 注册 C 回调桥接
    scene_set_load_callback(m_scene,   Bridge_OnLoad);
    scene_set_update_callback(m_scene, Bridge_OnUpdate);
    scene_set_render_callback(m_scene,  Bridge_OnRender);
    scene_set_destroy_callback(m_scene, Bridge_OnDestroy);
}

BaseScene::~BaseScene() {
    if (m_scene) {
        scene_destroy(m_scene);
        m_scene = nullptr;
    }
}

void BaseScene::Bridge_OnLoad(scene_p s) {
    auto* self = static_cast<BaseScene*>(scene_get_userdata(s));
    if (self) self->OnStart();
}

void BaseScene::Bridge_OnUpdate(scene_p s, float dt) {
    auto* self = static_cast<BaseScene*>(scene_get_userdata(s));
    if (self) self->OnUpdate(dt);
}

void BaseScene::Bridge_OnRender(scene_p s) {
    auto* self = static_cast<BaseScene*>(scene_get_userdata(s));
    if (self) self->OnRender();
}

void BaseScene::Bridge_OnDestroy(scene_p s) {
    auto* self = static_cast<BaseScene*>(scene_get_userdata(s));
    if (self) self->OnDestroy();
}

// ============================================================
// LoadingScene 实现
// ============================================================

LoadingScene::LoadingScene()
    : BaseScene("Loading") {
}

LoadingScene::~LoadingScene() = default;


static void menu_box_render(node_p n, const void* arg) {
        float x, y;
        SDL_Renderer* renderer = (SDL_Renderer*)arg;
        node_get_world_position(n, &x, &y);
        int z = node_get_zorder(n);
        SDL_Color c;
        if (z >= 30)      c = { 80, 160, 255, 255 };
        else if (z >= 20) c = { 80, 255, 120, 255 };
        else              c = { 255, 80, 80, 255 };
        SDL_FRect r = { x - 30, y - 30, 60, 60 };
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer, &r);
}

static void menu_box_update(node_p n, float dt) {
        (void)dt;
        float bx = 0; float by = 0;
        // 从 userdata 读取相位偏移，做浮动动画
        float phase = 0.0f;
        float* p = (float*)node_get_userdata(n);
        if (p) phase = *p;
        static float t = 0.0f;
        t += 0.016f;
        // base 坐标存在 scale_x/scale_y 中（偷懒用法）
        node_get_scale(n, &bx, &by);
        node_set_position(n, bx + sinf(t * 1.5f + phase) * 30.0f,
                by + cosf(t * 1.2f + phase) * 20.0f);
}

static void menu_box_destroy(node_p n) {
        float* p = (float*)node_get_userdata(n);
        free(p);
}



void LoadingScene::OnStart() {
    SDL_Log("[LoadingScene] OnStart - 开始加载资源...");
    m_progress = 0.0f;

    float bases[3][2] = { {200,150}, {500,200}, {350,400} };
    int zs[3] = { 10, 20, 30 };
    float phases[3] = { 0.0f, 1.5f, 3.0f };
    for (int i = 0; i < 3; i++) {
            node_p n = node_create();
            node_set_position(n, bases[i][0], bases[i][1]);
            node_set_zorder(n, zs[i]);
            node_set_scale(n, bases[i][0], bases[i][1]); // 偷存 base
            node_set_update_callback(n, menu_box_update);
            node_set_render_callback(n, menu_box_render);
            node_set_destroy_callback(n, menu_box_destroy);
            float* ph = (float*)malloc(sizeof(float));
            *ph = phases[i];
            node_set_userdata(n, ph);
            scene_add_root_node(this->m_scene, n);
    }

}

void LoadingScene::OnUpdate(float dt) {
    // 模拟加载进度
    m_progress += dt * 0.5f; // 2秒完成
    if (m_progress >= 1.0f) {
        m_progress = 1.0f;
        //SDL_Log("[LoadingScene] 加载完成! 可切换到下一场景");
    }
}

void LoadingScene::OnRender() {
    // TODO: 通过 Context 获取 renderer 绘制进度条
    // 示例: SDL_SetRenderDrawColor(ctx->renderer, 255,255,255,255);
}
