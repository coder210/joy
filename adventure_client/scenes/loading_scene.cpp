#include "loading_scene.h"
#include <SDL3/SDL.h>
#include <joy2d/jcore.h>
#include <stdlib.h>
#include <math.h>

struct loading_scene {
    scene_p scene;
    context* ctx;
};

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

static void menu_box_event(scene_node_p n, const void *e) {
       
}

static void menu_box_destroy(scene_node_p n) {
    float* p = (float*)scene_node_get_userdata(n);
    free(p);
}

static void on_load(scene_p s) {
    loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
    log_info("[loading scene] onstart...");

    float bases[3][2] = { {200,150}, {500,200}, {350,400} };
    int zs[3] = { 10, 20, 30 };
    float phases[3] = { 0.0f, 1.5f, 3.0f };

    for (int i = 0; i < 3; i++) {
        scene_node_p n = scene_node_create();
        scene_node_set_position(n, bases[i][0], bases[i][1]);
        scene_node_set_zorder(n, zs[i]);
        scene_node_set_scale(n, bases[i][0], bases[i][1]);
        scene_node_set_update_callback(n, menu_box_update);
	scene_node_set_event_callback(n, menu_box_event);
        scene_node_set_render_callback(n, menu_box_render);
        scene_node_set_destroy_callback(n, menu_box_destroy);

        float* ph = (float*)malloc(sizeof(float));
        *ph = phases[i];
        scene_node_set_userdata(n, ph);

        scene_add_root_node(self->scene, n);
    }
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
    loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
    free(self);
}

loading_scene_p loading_scene_create(context* ctx) {
    loading_scene_p self = (loading_scene_p)malloc(sizeof(loading_scene_t));
    self->scene = scene_create("Loading");
    self->ctx = ctx;

    scene_set_userdata(self->scene, self);
    scene_set_load_callback(self->scene, on_load);
    scene_set_update_callback(self->scene, on_update);
    scene_set_render_callback(self->scene, on_render);
    scene_set_destroy_callback(self->scene, on_destroy);

    return self;
}

void loading_scene_destroy(loading_scene_p s) {
    if (s) {
        if (s->scene) {
            scene_destroy(s->scene);
        }
        free(s);
    }
}

scene_p loading_scene_get_scene(loading_scene_p s) {
    return s ? s->scene : NULL;
}
