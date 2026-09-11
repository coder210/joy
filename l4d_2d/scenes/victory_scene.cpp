#include <cstring>
#include <cstdio>
#include "victory_scene.h"
#include "level_select_scene.h"
#include "main_scene.h"
#include "../asset_manager.h"

struct victory_scene {
        scene_p scene;
        Context* ctx;
        scene_node_p ui_node;
        button_p btn_replay, btn_menu;
        float elapsed;
};

static void setup_button_style(button_p btn) {
        jcolor_t normal  = { 255, 255, 255, 255 };
        jcolor_t hover   = { 248, 246, 240, 255 };
        jcolor_t pressed = { 230, 228, 222, 255 };
        button_set_normal_color(btn, normal);
        button_set_hover_color(btn, hover);
        button_set_pressed_color(btn, pressed);
}

// 延迟切换：登记目标场景，由 main.cpp 在本次事件分发结束后执行替换，避免回调内释放 self
static void request_scene(Context* ctx, scene_p s) {
        if (!s) return;
        ctx->pending_scene = s;
        push_user_event(TRANSITION_EVENT, 0);
}

static void on_event(scene_node_p n, const void* e) {
        victory_scene_p self = (victory_scene_p)scene_node_get_userdata(n);
        jevent_callbacks_t cbs = {};
        cbs.on_mousepressed = [](float x, float y, int btn, void* arg) {
                (void)btn;
                victory_scene_p self = (victory_scene_p)arg;
                // 再战一局：回选关页（沿用当前模式）
                if (self->btn_replay && button_is_pressed(self->btn_replay, x, y)) {
                        request_scene(self->ctx,
                                level_select_scene_get_scene(
                                        level_select_scene_create(self->ctx, self->ctx->mode)));
                        return; // 已登记切换，立即返回避免处理后续回调
                }
                // 回主菜单
                if (self->btn_menu && button_is_pressed(self->btn_menu, x, y))
                        request_scene(self->ctx,
                                main_scene_get_scene(main_scene_create(self->ctx)));
        };
        cbs.on_mousereleased = [](float x, float y, int btn, void* arg) {
                (void)x; (void)y; (void)btn;
                victory_scene_p self = (victory_scene_p)arg;
                if (self->btn_replay) button_set_pressed(self->btn_replay, false);
                if (self->btn_menu)   button_set_pressed(self->btn_menu, false);
        };
        cbs.on_mousemoved = [](float x, float y, float dx, float dy, void* arg) {
                (void)dx; (void)dy;
                victory_scene_p self = (victory_scene_p)arg;
                if (self->btn_replay) button_is_hovered(self->btn_replay, x, y);
                if (self->btn_menu)   button_is_hovered(self->btn_menu, x, y);
        };
        dispatch_event(cbs, e, self);
}

static void on_load(scene_p s) {
        victory_scene_p self = (victory_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        log_info("[victory scene] start...");
        font_p font24 = AssetManager::GetInstance()->GetSimheiFont24();

        self->ui_node = scene_node_create();
        scene_node_set_userdata(self->ui_node, self);
        scene_node_set_event_callback(self->ui_node, on_event);
        scene_node_set_zorder(self->ui_node, 100);
        scene_add_root_node(self->scene, self->ui_node);

        self->btn_replay = button_create(ctx->r2d, 160, 50);
        setup_button_style(self->btn_replay);
        button_set_text_offset(self->btn_replay, 0, 3.0f);
        button_set_textx(self->btn_replay, font24, "再战一局", (int)strlen("再战一局"), { 0, 0, 0, 255 });
        button_set_position(self->btn_replay, 70, 250);

        self->btn_menu = button_create(ctx->r2d, 160, 50);
        setup_button_style(self->btn_menu);
        button_set_text_offset(self->btn_menu, 0, 3.0f);
        button_set_textx(self->btn_menu, font24, "回主菜单", (int)strlen("回主菜单"), { 0, 0, 0, 255 });
        button_set_position(self->btn_menu, 250, 250);
}

static void on_update(scene_p s, float dt) {
        victory_scene_p self = (victory_scene_p)scene_get_userdata(s);
        self->elapsed += dt;
}

static void on_render(scene_p s) {
        victory_scene_p self = (victory_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        jr2d_p r2d = ctx->r2d;
        font_p font28 = AssetManager::GetInstance()->GetSimheiFont28();
        font_p font24 = AssetManager::GetInstance()->GetSimheiFont24();

        int w, h;
        r2d_get_logical_presentation(r2d, &w, &h, NULL);

        // 白底蓝图风
        r2d_set_color(r2d, 255, 255, 255, 255);
        jrect_t full = { 0, 0, (float)w, (float)h };
        shape_draw_rectangle(r2d, "fill", full);

        // 标题（墨绿，水墨风）
        jcolor_t title_c = { 20, 120, 60, 255 };
        jtext_printx(r2d, font28, "通  关", (int)strlen("通  关"), title_c, 192, 70);

        char buf[128];
        jcolor_t sub_c = { 60, 60, 70, 255 };
        snprintf(buf, sizeof(buf), "第 %d 关的所有敌人已被肃清", ctx->selected_level);
        jtext_printx(r2d, font24, buf, (int)strlen(buf), sub_c, 130, 132);
        jcolor_t tip_c = { 110, 110, 118, 255 };
        jtext_printx(r2d, font24, "恭喜通关！", (int)strlen("恭喜通关！"), tip_c, 192, 168);

        if (self->btn_replay) button_draw(self->btn_replay);
        if (self->btn_menu)   button_draw(self->btn_menu);
}

static void on_destroy(scene_p s) {
        victory_scene_p self = (victory_scene_p)scene_get_userdata(s);
        if (self->btn_replay) button_destroy(self->btn_replay);
        if (self->btn_menu)   button_destroy(self->btn_menu);
        delete self;
}

victory_scene_p victory_scene_create(Context* ctx) {
        victory_scene_p self = new victory_scene_t();
        self->scene = scene_create("Victory");
        self->ctx = ctx;

        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_update_callback(self->scene, on_update);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);

        return self;
}

scene_p victory_scene_get_scene(victory_scene_p s) {
        return s ? s->scene : NULL;
}