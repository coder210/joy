#include "loading_scene.h"
#include <SDL3/SDL.h>
#include <joy/jcore.h>
#include <joy/jtextures.h>
#include <stdlib.h>
#include <math.h>
#include "../layers/menu_layer.h"

struct logo_data {
    image_p image;
};

struct deco_data {
    image_p image;
};

struct loading_scene {
    scene_p scene;
    context* ctx;
};

static void on_load(scene_p s) {
    loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
    log_info("[loading scene] start...");

    // 添加菜单层
    menu_layer_p menu = create_menu_layer(self->ctx);
    scene_add_root_node(self->scene, menu_layer_get_node(menu));

    int logical_w, logical_h;
    SDL_GetRenderLogicalPresentation(self->ctx->renderer, &logical_w, &logical_h, NULL);

    // ---- Logo ----
    logo_data* ld = new logo_data{};
    ld->image = image_create(self->ctx->renderer, "joy2d_editor_textures/logo.png");
    scene_node_p logo_node = scene_node_create();
    scene_node_set_position(logo_node, 0, 0);
    scene_node_set_zorder(logo_node, 50);
    scene_node_set_userdata(logo_node, ld);
    scene_node_set_render_callback(logo_node, [](scene_node_p n, const void* arg) {
        SDL_Renderer* renderer = (SDL_Renderer*)arg;
        logo_data* ld = (logo_data*)scene_node_get_userdata(n);
        if (!ld || !ld->image) return;
        int w, h;
        SDL_GetRenderLogicalPresentation(renderer, &w, &h, NULL);
        SDL_FRect dest = { (w - 400) / 2.0f, 40, 400, 100 };
        image_draw1(ld->image, NULL, &dest);
    });
    scene_node_set_destroy_callback(logo_node, [](scene_node_p n) {
        logo_data* ld = (logo_data*)scene_node_get_userdata(n);
        if (ld) {
            if (ld->image) image_destroy(ld->image);
            delete ld;
        }
    });
    scene_add_root_node(self->scene, logo_node);

    // ---- 左侧装饰 ----
    deco_data* dd_l = new deco_data{};
    dd_l->image = image_create(self->ctx->renderer, "joy2d_editor_textures/deco/01.png");
    scene_node_p deco_left = scene_node_create();
    scene_node_set_position(deco_left, 0, 0);
    scene_node_set_zorder(deco_left, 40);
    scene_node_set_userdata(deco_left, dd_l);
    scene_node_set_render_callback(deco_left, [](scene_node_p n, const void* arg) {
        SDL_Renderer* renderer = (SDL_Renderer*)arg;
        deco_data* dd = (deco_data*)scene_node_get_userdata(n);
        if (!dd || !dd->image) return;
        int w, h;
        SDL_GetRenderLogicalPresentation(renderer, &w, &h, NULL);
        SDL_FRect dest = { 20, (h - 200) / 2.0f, 80, 200 };
        image_draw1(dd->image, NULL, &dest);
    });
    scene_node_set_destroy_callback(deco_left, [](scene_node_p n) {
        deco_data* dd = (deco_data*)scene_node_get_userdata(n);
        if (dd) {
            if (dd->image) image_destroy(dd->image);
            delete dd;
        }
    });
    scene_add_root_node(self->scene, deco_left);

    // ---- 右侧装饰 ----
    deco_data* dd_r = new deco_data{};
    dd_r->image = image_create(self->ctx->renderer, "joy2d_editor_textures/deco/02.png");
    scene_node_p deco_right = scene_node_create();
    scene_node_set_position(deco_right, 0, 0);
    scene_node_set_zorder(deco_right, 40);
    scene_node_set_userdata(deco_right, dd_r);
    scene_node_set_render_callback(deco_right, [](scene_node_p n, const void* arg) {
        SDL_Renderer* renderer = (SDL_Renderer*)arg;
        deco_data* dd = (deco_data*)scene_node_get_userdata(n);
        if (!dd || !dd->image) return;
        int w, h;
        SDL_GetRenderLogicalPresentation(renderer, &w, &h, NULL);
        SDL_FRect dest = { w - 100, (h - 200) / 2.0f, 80, 200 };
        image_draw1(dd->image, NULL, &dest);
    });
    scene_node_set_destroy_callback(deco_right, [](scene_node_p n) {
        deco_data* dd = (deco_data*)scene_node_get_userdata(n);
        if (dd) {
            if (dd->image) image_destroy(dd->image);
            delete dd;
        }
    });
    scene_add_root_node(self->scene, deco_right);
}

static void on_update(scene_p s, float dt) {
    (void)s;
    (void)dt;
}

static void on_render(scene_p s) {
    (void)s;
}

static void on_destroy(scene_p s) {
    loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
    delete self;
}

loading_scene_p loading_scene_create(context* ctx) {
    loading_scene_p self = new loading_scene_t();
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
        delete s;
    }
}

scene_p loading_scene_get_scene(loading_scene_p s) {
    return s ? s->scene : NULL;
}
