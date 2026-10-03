#include <cstring>
#include "character_select_scene.h"
#include "level_select_scene.h"
#include "loading_scene.h"
#include "main_scene.h"
#include "online_scene.h"
#include "../asset_manager.h"
#include "../systems/character_def.h"

struct character_select_scene {
        scene_p scene;
        Context* ctx;
        GameMode target_mode;
        scene_node_p ui_node;
        button_p btn_cards[CHAR_COUNT];
        button_p btn_confirm, btn_back;
        inputbox_p nickname_input;
        int selected;
};

// 角色卡片尺寸（与 on_load 中创建一致）
static const float CARD_W = 140.0f, CARD_H = 130.0f;

static void setup_button_style(button_p btn) {
        jcolor_t normal  = { 255, 255, 255, 255 };
        jcolor_t hover   = { 248, 246, 240, 255 };
        jcolor_t pressed = { 230, 228, 222, 255 };
        button_set_normal_color(btn, normal);
        button_set_hover_color(btn, hover);
        button_set_pressed_color(btn, pressed);
}

static void go_to_level(character_select_scene_p self)
{
        // 延迟切换：仅登记目标场景，由 main.cpp 在本次事件分发结束后执行替换，避免回调内释放 self
        self->ctx->pending_scene =
                level_select_scene_get_scene(
                        level_select_scene_create(self->ctx, self->target_mode));
        push_user_event(TRANSITION_EVENT, 0);
}

static void go_to_loading(character_select_scene_p self)
{
        self->ctx->pending_scene =
                loading_scene_get_scene(
                        loading_scene_create(self->ctx, self->target_mode));
        push_user_event(TRANSITION_EVENT, 0);
}

static void go_back(character_select_scene_p self)
{
        scene_p back = (self->target_mode == MODE_SINGLE)
                ? main_scene_get_scene(main_scene_create(self->ctx))
                : online_scene_get_scene(online_scene_create(self->ctx));
        self->ctx->pending_scene = back;
        push_user_event(TRANSITION_EVENT, 0);
}

static void on_event(scene_node_p n, const void* e) {
        character_select_scene_p self = (character_select_scene_p)scene_node_get_userdata(n);
        jevent_callbacks_t cbs = {};
        cbs.on_process_event = [](const void* ev, void* arg) {
                character_select_scene_p self = (character_select_scene_p)arg;
                if (self->nickname_input) inputbox_process_event(self->nickname_input, ev);
        };
        cbs.on_mousepressed = [](float x, float y, int btn, void* arg) {
                (void)btn;
                character_select_scene_p self = (character_select_scene_p)arg;
                for (int i = 0; i < CHAR_COUNT; i++)
                        if (self->btn_cards[i] && button_is_pressed(self->btn_cards[i], x, y))
                                self->selected = i;
                if (self->btn_confirm && button_is_pressed(self->btn_confirm, x, y)) {
                        // 记录昵称（空则默认"玩家"）
                        const char* nick = self->nickname_input
                                ? inputbox_get_text(self->nickname_input) : "";
                        const char* s = nick;
                        while (*s == ' ' || *s == '\t') s++;
                        if (!*s) s = "玩家";
                        snprintf(self->ctx->selected_nickname,
                                sizeof(self->ctx->selected_nickname), "%s", s);
                        self->ctx->selected_char_id = self->selected;
                        // 客户端：关卡由房主统一下发，跳过选关页直接进 loading
                        if (self->target_mode == MODE_CLIENT) go_to_loading(self);
                        else go_to_level(self);
                        return; // scene 已替换、self 已释放，必须立即返回
                }
                if (self->btn_back && button_is_pressed(self->btn_back, x, y))
                        go_back(self);
        };
        cbs.on_mousereleased = [](float x, float y, int btn, void* arg) {
                (void)x; (void)y; (void)btn;
                character_select_scene_p self = (character_select_scene_p)arg;
                for (int i = 0; i < CHAR_COUNT; i++)
                        if (self->btn_cards[i]) button_set_pressed(self->btn_cards[i], false);
                if (self->btn_confirm) button_set_pressed(self->btn_confirm, false);
                if (self->btn_back)    button_set_pressed(self->btn_back, false);
        };
        cbs.on_mousemoved = [](float x, float y, float dx, float dy, void* arg) {
                (void)dx; (void)dy;
                character_select_scene_p self = (character_select_scene_p)arg;
                for (int i = 0; i < CHAR_COUNT; i++)
                        if (self->btn_cards[i]) button_is_hovered(self->btn_cards[i], x, y);
                if (self->btn_confirm) button_is_hovered(self->btn_confirm, x, y);
                if (self->btn_back)    button_is_hovered(self->btn_back, x, y);
        };
        dispatch_event(cbs, e, self);
}

static void draw_card(character_select_scene_p self, int i)
{
        Context* ctx = self->ctx;
        jr2d_p r2d = ctx->r2d;
        const CharacterDef& cd = character_def(i);
        button_p btn = self->btn_cards[i];
        if (!btn) return;
        vec2_t p = button_get_position(btn);
        float x = p.x, y = p.y, w = CARD_W, h = CARD_H;
        bool sel = (i == self->selected);
        font_p font24 = AssetManager::GetInstance()->GetSimheiFont24();
        font_p font12 = AssetManager::GetInstance()->GetSimheiFont12();

        // 卡片背景：白底，选中用角色色淡染
        if (sel) r2d_set_color(r2d, cd.fill_r, cd.fill_g, cd.fill_b, 255);
        else     r2d_set_color(r2d, 248, 246, 240, 255);
        jrect_t bg = { x, y, w, h };
        shape_draw_rectangle(r2d, "fill", bg);

        // 边框：选中墨色粗边，未选中淡墨细边
        float bw = sel ? 4.0f : 2.0f;
        r2d_set_color(r2d, sel ? 30 : 120, sel ? 30 : 120, sel ? 35 : 128, 255);
        jrect_t b = { x, y, w, h };
        shape_draw_rectangle(r2d, "line", b);

        // 角色名（墨色，选中白字）
        jcolor_t name_c = sel ? jcolor_t{ 255, 255, 255, 255 } : jcolor_t{ 40, 40, 45, 255 };
        int nl = (int)strlen(cd.name);
        int nw = nl * 12;
        int logical_w, logical_h;
        (void)logical_w; (void)logical_h;
        jtext_printx(r2d, font24, cd.name, nl, name_c, x + (w - nw) / 2.0f, y + 10.0f);

        // 属性（小号）
        jcolor_t attr_c = sel ? jcolor_t{ 245, 245, 245, 255 } : jcolor_t{ 90, 90, 98, 255 };
        char buf[64];
        snprintf(buf, sizeof(buf), "血量 %d", cd.max_hp);
        jtext_printx(r2d, font12, buf, (int)strlen(buf), attr_c, x + 12.0f, y + 44.0f);
        snprintf(buf, sizeof(buf), "近战 %d", cd.melee_damage);
        jtext_printx(r2d, font12, buf, (int)strlen(buf), attr_c, x + 12.0f, y + 62.0f);
        snprintf(buf, sizeof(buf), "远程 %d", cd.bullet_damage);
        jtext_printx(r2d, font12, buf, (int)strlen(buf), attr_c, x + 12.0f, y + 80.0f);
        snprintf(buf, sizeof(buf), "移速 %.1f", cd.move_speed);
        jtext_printx(r2d, font12, buf, (int)strlen(buf), attr_c, x + 12.0f, y + 98.0f);
}

static void on_load(scene_p s) {
        character_select_scene_p self = (character_select_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        log_info("[character select scene] start...");
        font_p font24 = AssetManager::GetInstance()->GetSimheiFont24();

        int w, h;
        r2d_get_logical_presentation(ctx->r2d, &w, &h, NULL);

        self->ui_node = scene_node_create();
        scene_node_set_userdata(self->ui_node, self);
        scene_node_set_event_callback(self->ui_node, on_event);
        scene_node_set_zorder(self->ui_node, 100);
        scene_add_root_node(self->scene, self->ui_node);

        // 3 张角色卡片（横向排列）
        const float cw = CARD_W, ch = CARD_H, gap = 14.0f;
        const float cx0 = (w - (cw * CHAR_COUNT + gap * (CHAR_COUNT - 1))) / 2.0f;
        const float cy = 74.0f;
        for (int i = 0; i < CHAR_COUNT; i++) {
                self->btn_cards[i] = button_create(ctx->r2d, 0.0f, 0.0f, cw / 480.0f, ch / 320.0f);
                setup_button_style(self->btn_cards[i]);
                button_set_position(self->btn_cards[i], cx0 + i * (cw + gap), cy);
        }
        self->selected = (ctx->selected_char_id >= 0 && ctx->selected_char_id < CHAR_COUNT)
                ? ctx->selected_char_id : 0;

        // 昵称输入框（标题下方、卡片上方）
                self->nickname_input = inputbox_create(ctx->r2d, 0.11458f, 0.11875f, 0.41667f, 0.075f, font24);
        inputbox_set_text(self->nickname_input, ctx->selected_nickname);

        // 确认 / 返回
        self->btn_confirm = button_create(ctx->r2d, 0.0f, 0.0f, 120.0f / 480.0f, 40.0f / 320.0f);
        setup_button_style(self->btn_confirm);
        button_set_text_offset(self->btn_confirm, 0, 3.0f);
        button_set_textx(self->btn_confirm, font24, "开始游戏", (int)strlen("开始游戏"), { 0, 0, 0, 255 });
        button_set_position(self->btn_confirm, 180, 244);

        self->btn_back = button_create(ctx->r2d, 0.0f, 0.0f, 80.0f / 480.0f, 40.0f / 320.0f);
        setup_button_style(self->btn_back);
        button_set_text_offset(self->btn_back, 0, 3.0f);
        button_set_textx(self->btn_back, font24, "返回", (int)strlen("返回"), { 0, 0, 0, 255 });
        button_set_position(self->btn_back, 15, 244);
}

static void on_render(scene_p s) {
        character_select_scene_p self = (character_select_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        jr2d_p r2d = ctx->r2d;
        font_p font24 = AssetManager::GetInstance()->GetSimheiFont24();

        int w, h;
        r2d_get_logical_presentation(r2d, &w, &h, NULL);

        // 白底蓝图风
        r2d_set_color(r2d, 255, 255, 255, 255);
        jrect_t full = { 0, 0, (float)w, (float)h };
        shape_draw_rectangle(r2d, "fill", full);

        // 标题（墨色）
        jcolor_t tc = { 60, 60, 70, 255 };
        jtext_printx(r2d, font24, "选择角色", (int)strlen("选择角色"), tc, 190, 10);

        // 昵称标签
        jcolor_t lc = { 50, 50, 60, 255 };
        jtext_printx(r2d, font24, "昵称", (int)strlen("昵称"), lc, 15, 44);
        if (self->nickname_input) inputbox_draw(self->nickname_input);

        for (int i = 0; i < CHAR_COUNT; i++) draw_card(self, i);

        if (self->btn_confirm) button_draw(self->btn_confirm);
        if (self->btn_back)    button_draw(self->btn_back);
}

static void on_destroy(scene_p s) {
        character_select_scene_p self = (character_select_scene_p)scene_get_userdata(s);
        for (int i = 0; i < CHAR_COUNT; i++)
                if (self->btn_cards[i]) button_destroy(self->btn_cards[i]);
        if (self->nickname_input) inputbox_destroy(self->nickname_input);
        if (self->btn_confirm) button_destroy(self->btn_confirm);
        if (self->btn_back)    button_destroy(self->btn_back);
        delete self;
}

character_select_scene_p character_select_scene_create(Context* ctx, GameMode mode) {
        character_select_scene_p self = new character_select_scene_t();
        self->scene = scene_create("CharSelect");
        self->ctx = ctx;
        self->target_mode = mode;

        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);

        return self;
}

scene_p character_select_scene_get_scene(character_select_scene_p s) {
        return s ? s->scene : NULL;
}