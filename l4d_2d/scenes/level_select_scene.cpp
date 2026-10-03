#include <cstring>
#include <cstdio>
#include "level_select_scene.h"
#include "character_select_scene.h"
#include "loading_scene.h"
#include "../asset_manager.h"
#include "../systems/character_def.h"

// 关卡元信息 —— 对应 level_sources.h 的 5 张 64x64 地图（LEVEL_SOURCES_COUNT）
struct LevelMeta { const char* name; const char* desc; };
static const LevelMeta LEVEL_META[] = {
        { "荒野迷城", "开阔原野，篱墙成列，练手入门" },
        { "寒石遗迹", "石垣交错，迂回周旋" },
        { "幽林深谷", "林墙环绕，步步为营" },
        { "龙脊荒冢", "机关密布，险中求胜" },
        { "幽冥龙穴", "巷战兼程，大显身手" },
};
static const int LEVEL_META_COUNT = (int)(sizeof(LEVEL_META) / sizeof(LEVEL_META[0]));

// 关卡按钮尺寸（与 on_load 中创建一致）
static const float LEVEL_W = 300.0f, LEVEL_H = 44.0f;

struct level_select_scene {
        scene_p scene;
        Context* ctx;
        GameMode target_mode;
        scene_node_p ui_node;
        button_p btn_levels[6];
        button_p btn_confirm, btn_back;
        int selected;
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
        level_select_scene_p self = (level_select_scene_p)scene_node_get_userdata(n);
        jevent_callbacks_t cbs = {};
        cbs.on_mousepressed = [](float x, float y, int btn, void* arg) {
                (void)btn;
                level_select_scene_p self = (level_select_scene_p)arg;
                for (int i = 0; i < LEVEL_META_COUNT; i++)
                        if (self->btn_levels[i] && button_is_pressed(self->btn_levels[i], x, y))
                                self->selected = i;
                if (self->btn_confirm && button_is_pressed(self->btn_confirm, x, y)) {
                        self->ctx->selected_level = self->selected + 1;
                        request_scene(self->ctx,
                                loading_scene_get_scene(
                                        loading_scene_create(self->ctx, self->target_mode)));
                        return; // 已登记切换，立即返回避免处理后续回调
                }
                if (self->btn_back && button_is_pressed(self->btn_back, x, y))
                        request_scene(self->ctx,
                                character_select_scene_get_scene(
                                        character_select_scene_create(self->ctx, self->target_mode)));
        };
        cbs.on_mousereleased = [](float x, float y, int btn, void* arg) {
                (void)x; (void)y; (void)btn;
                level_select_scene_p self = (level_select_scene_p)arg;
                for (int i = 0; i < LEVEL_META_COUNT; i++)
                        if (self->btn_levels[i]) button_set_pressed(self->btn_levels[i], false);
                if (self->btn_confirm) button_set_pressed(self->btn_confirm, false);
                if (self->btn_back)    button_set_pressed(self->btn_back, false);
        };
        cbs.on_mousemoved = [](float x, float y, float dx, float dy, void* arg) {
                (void)dx; (void)dy;
                level_select_scene_p self = (level_select_scene_p)arg;
                for (int i = 0; i < LEVEL_META_COUNT; i++)
                        if (self->btn_levels[i]) button_is_hovered(self->btn_levels[i], x, y);
                if (self->btn_confirm) button_is_hovered(self->btn_confirm, x, y);
                if (self->btn_back)    button_is_hovered(self->btn_back, x, y);
        };
        dispatch_event(cbs, e, self);
}

static void draw_level(level_select_scene_p self, int i)
{
        Context* ctx = self->ctx;
        jr2d_p r2d = ctx->r2d;
        button_p btn = self->btn_levels[i];
        if (!btn) return;
        vec2_t p = button_get_position(btn);
        float x = p.x, y = p.y, w = LEVEL_W, h = LEVEL_H;
        bool sel = (i == self->selected);
        font_p font24 = AssetManager::GetInstance()->GetSimheiFont24();
        font_p font12 = AssetManager::GetInstance()->GetSimheiFont12();

        // 背景：选中墨色，未选中宣纸色
        if (sel) r2d_set_color(r2d, 70, 70, 78, 255);
        else     r2d_set_color(r2d, 248, 246, 240, 255);
        jrect_t bg = { x, y, w, h };
        shape_draw_rectangle(r2d, "fill", bg);

        // 边框
        r2d_set_color(r2d, 96, 96, 104, 255);
        jrect_t b = { x, y, w, h };
        shape_draw_rectangle(r2d, "line", b);

        // 文本
        char buf[64];
        jcolor_t name_c = sel ? jcolor_t{ 255, 255, 255, 255 } : jcolor_t{ 40, 40, 45, 255 };
        snprintf(buf, sizeof(buf), "第 %d 关 · %s", i + 1, LEVEL_META[i].name);
        jtext_printx(r2d, font24, buf, (int)strlen(buf), name_c, x + 14.0f, y + 5.0f);
        jcolor_t desc_c = sel ? jcolor_t{ 220, 220, 226, 255 } : jcolor_t{ 110, 110, 118, 255 };
        jtext_printx(r2d, font12, LEVEL_META[i].desc, (int)strlen(LEVEL_META[i].desc), desc_c, x + 14.0f, y + 26.0f);
}

static void on_load(scene_p s) {
        level_select_scene_p self = (level_select_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        log_info("[level select scene] start...");
        font_p font24 = AssetManager::GetInstance()->GetSimheiFont24();

        int w, h;
        r2d_get_logical_presentation(ctx->r2d, &w, &h, NULL);

        self->ui_node = scene_node_create();
        scene_node_set_userdata(self->ui_node, self);
        scene_node_set_event_callback(self->ui_node, on_event);
        scene_node_set_zorder(self->ui_node, 100);
        scene_add_root_node(self->scene, self->ui_node);

        self->selected = (ctx->selected_level >= 1 && ctx->selected_level <= LEVEL_META_COUNT)
                ? ctx->selected_level - 1 : 0;

        // 关卡按钮：左侧竖列（5 关），确认/返回移右侧竖排，避免纵向重叠
        const float bx = 40.0f;
        for (int i = 0; i < LEVEL_META_COUNT; i++) {
                self->btn_levels[i] = button_create(ctx->r2d, 0.0f, 0.0f, LEVEL_W / 480.0f, LEVEL_H / 320.0f);
                setup_button_style(self->btn_levels[i]);
                button_set_position(self->btn_levels[i], bx, 74.0f + (float)i * 48.0f);
        }

        self->btn_confirm = button_create(ctx->r2d, 0.0f, 0.0f, 120.0f / 480.0f, 45.0f / 320.0f);
        setup_button_style(self->btn_confirm);
        button_set_text_offset(self->btn_confirm, 0, 3.0f);
        button_set_textx(self->btn_confirm, font24, "开始", (int)strlen("开始"), { 0, 0, 0, 255 });
        button_set_position(self->btn_confirm, 360, 156);

        self->btn_back = button_create(ctx->r2d, 0.0f, 0.0f, 90.0f / 480.0f, 45.0f / 320.0f);
        setup_button_style(self->btn_back);
        button_set_text_offset(self->btn_back, 0, 3.0f);
        button_set_textx(self->btn_back, font24, "返回", (int)strlen("返回"), { 0, 0, 0, 255 });
        button_set_position(self->btn_back, 360, 212);
}

static void on_render(scene_p s) {
        level_select_scene_p self = (level_select_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        jr2d_p r2d = ctx->r2d;
        font_p font24 = AssetManager::GetInstance()->GetSimheiFont24();

        int w, h;
        r2d_get_logical_presentation(r2d, &w, &h, NULL);

        // 白底蓝图风
        r2d_set_color(r2d, 255, 255, 255, 255);
        jrect_t full = { 0, 0, (float)w, (float)h };
        shape_draw_rectangle(r2d, "fill", full);

        // 标题
        jcolor_t tc = { 60, 60, 70, 255 };
        jtext_printx(r2d, font24, "选择关卡", (int)strlen("选择关卡"), tc, 190, 20);

        // 当前角色提示
        char chBuf[64];
        snprintf(chBuf, sizeof(chBuf), "当前角色：%s", character_def(self->ctx->selected_char_id).name);
        jcolor_t lc = { 110, 110, 118, 255 };
        font_p font12 = AssetManager::GetInstance()->GetSimheiFont12();
        jtext_printx(r2d, font12, chBuf, (int)strlen(chBuf), lc, 180, 52);

        for (int i = 0; i < LEVEL_META_COUNT; i++) draw_level(self, i);

        if (self->btn_confirm) button_draw(self->btn_confirm);
        if (self->btn_back)    button_draw(self->btn_back);
}

static void on_destroy(scene_p s) {
        level_select_scene_p self = (level_select_scene_p)scene_get_userdata(s);
        for (int i = 0; i < LEVEL_META_COUNT; i++)
                if (self->btn_levels[i]) button_destroy(self->btn_levels[i]);
        if (self->btn_confirm) button_destroy(self->btn_confirm);
        if (self->btn_back)    button_destroy(self->btn_back);
        delete self;
}

level_select_scene_p level_select_scene_create(Context* ctx, GameMode mode) {
        level_select_scene_p self = new level_select_scene_t();
        self->scene = scene_create("LevelSelect");
        self->ctx = ctx;
        self->target_mode = mode;

        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);

        return self;
}

scene_p level_select_scene_get_scene(level_select_scene_p s) {
        return s ? s->scene : NULL;
}