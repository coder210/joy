#include "loading_scene.h"
#include <joy/joy.h>
#include <stdio.h>
#include "game_scene.h"
#include "../asset_manager.h"

struct loading_scene {
        scene_p scene;
        Context* ctx;
        GameMode target_mode;
        scene_p target_scene;
        float elapsed;
        bool loaded;
        int dot_count;
        float dot_timer;
};

static void on_load(scene_p s) {
        loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
        (void)self;
        log_info("[loading scene] loading...");
}

static void on_update(scene_p s, float dt) {
        loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
        self->elapsed += dt;

        // 加点动画：每 0.4s 切换一个点
        self->dot_timer += dt;
        if (self->dot_timer >= 0.4f) {
                self->dot_count = (self->dot_count + 1) % 4;
                self->dot_timer = 0;
        }

        // 0.5s 后创建游戏场景（模拟"加载"过程）
        if (!self->loaded && self->elapsed >= 0.5f) {
                reset_context_for_mode(self->ctx, self->target_mode);
                self->target_scene = game_scene_get_scene(game_scene_create(self->ctx));
                self->loaded = true;
        }

        // 1.5s 后切换到游戏场景
        if (self->loaded && self->elapsed >= 1.5f) {
                self->ctx->pending_scene = self->target_scene;
                push_user_event(TRANSITION_EVENT, TRANSITION_LOADING_DONE);
        }
}

static void on_render(scene_p s) {
        loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
        jr2d_p r2d = self->ctx->r2d;
        font_p font = AssetManager::GetInstance()->GetSimheiFont24();

        // 背景：白底（水墨/图纸风）
        jcolor_t bg = { 255, 255, 255, 255 };
        r2d_set_color(r2d, bg.r, bg.g, bg.b, bg.a);
        int w, h;
        r2d_get_logical_presentation(r2d, &w, &h, NULL);
        jrect_t full = { 0, 0, (float)w, (float)h };
        shape_draw_rectangle(r2d, "fill", full);

        // 加载文字（居中，墨色）
        jcolor_t c = { 40, 40, 45, 255 };
        char text[32];
        int dot_count = self->dot_count;
        if (dot_count == 0) snprintf(text, sizeof(text), "加载中");
        else if (dot_count == 1) snprintf(text, sizeof(text), "加载中.");
        else if (dot_count == 2) snprintf(text, sizeof(text), "加载中..");
        else snprintf(text, sizeof(text), "加载中...");
        jtext_printx(r2d, font, text, (int)strlen(text), c, (w - 120) / 2.0f, h / 2.0f - 12);
}

static void on_destroy(scene_p s) {
        loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
        delete self;
}

loading_scene_p loading_scene_create(Context* ctx, GameMode mode) {
        loading_scene_p self = new loading_scene_t();
        self->scene = scene_create("Loading");
        self->ctx = ctx;
        self->target_mode = mode;

        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_update_callback(self->scene, on_update);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);

        return self;
}

scene_p loading_scene_get_scene(loading_scene_p s) {
        return s ? s->scene : NULL;
}