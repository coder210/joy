/* ============================================================================
 *  joy 即时模式 UI 层（jui_im.c）
 *
 *  jui 原有控件是 retained 模式，适合固定布局的游戏 HUD；但层级树、控制台、
 *  资源列表这类行数动态变化的界面，retained 需要手工管理每一行的生命周期。
 *
 *  本层提供即时模式写法：调用方每帧只描述“这里有个按钮”，控件实例由 ctx
 *  按 id 池化复用——既保留了 button 的颜色过渡动画、slider 的拖拽状态，
 *  又不必由调用方管理 create/destroy。
 *
 *  关键设计：
 *   - 复用 jui 的 button / checkbox / slider（retained 实例），按 id 池化。
 *   - 裁剪基于 joy_r2d_push_clip_rect / joy_r2d_pop_clip_rect，滚动面板的基础。
 *   - 文字统一走 joy_r2d_text_printx 缓存，避免每帧建销纹理。
 * ============================================================================ */

#include <SDL3/SDL.h>
#include "joy.h"

#include <stdlib.h>
#include <string.h>

/* ------------------------------ 结构定义 ------------------------------ */

typedef struct joy_ui_slot {
        int       id;         /* -1 = 空闲 */
        joy_button_p  btn;
        joy_checkbox_p cb;
        joy_slider_p  sl;
} joy_ui_slot_t;

struct joy_ui_ctx {
        joy_r2d_p r2d;
        joy_font_p font;
        joy_ui_theme_t theme;

        /* 输入（窗口坐标，内部转逻辑坐标） */
        float  mx, my;             /* 逻辑坐标 */
        float  winx, winy;
        bool   ldown, lpressed, lreleased;
        float  wheel;
        bool   wheel_used;
        bool   shift, ctrl;

        /* 即时模式状态 */
        int    hot, active;
        int    widget_seq;
        int    id_base;

        /* 池化控件：复用 retained 实例 */
        joy_ui_slot_t* slots;
        int         slot_count;
        int         slot_cap;

        /* 裁剪栈镜像（用于命中测试） */
        int     clip_count;
        joy_rect_t clip_stack[32];
        bool    has_clip;

        /* 滚动区 */
        struct { int id; float off; } scrolls[32];
        int scroll_count;

        /* 每帧一次性输入 */
        bool  consumed;            /* begin 已消费 */
        float logx, logy;
};

/* ------------------------------ 主题 ------------------------------ */

static joy_ui_theme_t jui_default_theme(void)
{
        joy_ui_theme_t t;
        t.text         = (joy_color_t){ 212, 212, 212, 255 };
        t.btn_normal   = (joy_color_t){ 51,  51,  55,  255 };
        t.btn_hover    = (joy_color_t){ 70,  70,  75,  255 };
        t.btn_pressed  = (joy_color_t){ 23,  124, 189, 255 };
        t.btn_text     = (joy_color_t){ 212, 212, 212, 255 };
        t.cb_box       = (joy_color_t){ 51,  51,  55,  255 };
        t.cb_check     = (joy_color_t){ 23,  124, 189, 255 };
        t.cb_hover     = (joy_color_t){ 70,  70,  75,  255 };
        t.cb_text      = (joy_color_t){ 212, 212, 212, 255 };
        t.sl_track     = (joy_color_t){ 51,  51,  55,  255 };
        t.sl_fill      = (joy_color_t){ 23,  124, 189, 255 };
        t.sl_thumb     = (joy_color_t){ 150, 150, 156, 255 };
        t.sl_thumb_hover = (joy_color_t){ 200, 200, 210, 255 };
        t.panel        = (joy_color_t){ 37,  37,  38,  255 };
        t.panel_title  = (joy_color_t){ 45,  45,  48,  255 };
        t.border       = (joy_color_t){ 62,  62,  66,  255 };
        return t;
}

/* ------------------------------ 工具 ------------------------------ */

static joy_rect_t JUI_RECT(float x, float y, float w, float h)
{
        joy_rect_t r; r.x = x; r.y = y; r.w = w; r.h = h; return r;
}

static bool jui_pt_in(float x, float y, joy_rect_t r)
{
        return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

/* ------------------------------ 生命周期 ------------------------------ */

joy_ui_ctx_p joy_ui_ctx_create(joy_r2d_p r2d, joy_font_p font)
{
        joy_ui_ctx_p ctx = (joy_ui_ctx_p)calloc(1, sizeof(joy_ui_ctx_t));
        if (!ctx) return NULL;
        ctx->r2d  = r2d;
        ctx->font = font;
        ctx->theme = jui_default_theme();
        return ctx;
}

void joy_ui_ctx_destroy(joy_ui_ctx_p ctx)
{
        if (!ctx) return;
        for (int i = 0; i < ctx->slot_count; i++) {
                joy_ui_slot_t* s = &ctx->slots[i];
                if (s->btn) joy_ui_button_destroy(s->btn);
                if (s->cb)  joy_ui_checkbox_destroy(s->cb);
                if (s->sl)  joy_ui_slider_destroy(s->sl);
        }
        free(ctx->slots);
        free(ctx);
}

void joy_ui_set_theme(joy_ui_ctx_p ctx, const joy_ui_theme_t* theme)
{
        if (ctx && theme) ctx->theme = *theme;
}

/* ------------------------------ 槽位池 ------------------------------ */

static joy_ui_slot_t* jui_get_slot(joy_ui_ctx_p ctx, int id)
{
        if (ctx->slot_count < 32) {
                if (ctx->slot_count >= ctx->slot_cap) {
                        int nc = ctx->slot_cap ? ctx->slot_cap * 2 : 64;
                        joy_ui_slot_t* ns = (joy_ui_slot_t*)realloc(ctx->slots, (size_t)nc * sizeof(joy_ui_slot_t));
                        if (!ns) return NULL;
                        ctx->slots = ns;
                        ctx->slot_cap = nc;
                }
                joy_ui_slot_t* s = &ctx->slots[ctx->slot_count++];
                memset(s, 0, sizeof(*s));
                s->id = id;
                return s;
        }
        /* 已有足够槽位：查找或回收空闲 */
        joy_ui_slot_t* free_slot = NULL;
        for (int i = 0; i < ctx->slot_count; i++) {
                joy_ui_slot_t* s = &ctx->slots[i];
                if (s->id == id) return s;
                if (s->id < 0 && !free_slot) free_slot = s;
        }
        if (free_slot) { free_slot->id = id; return free_slot; }
        return NULL;
}

/* 每帧结束后清空槽位 id，下帧按需重新分配 */
static void jui_reset_slots(joy_ui_ctx_p ctx)
{
        for (int i = 0; i < ctx->slot_count; i++) ctx->slots[i].id = -1;
}

/* ------------------------------ 输入 ------------------------------ */

void joy_ui_begin(joy_ui_ctx_p ctx)
{
        if (!ctx) return;
        ctx->consumed = true;
        /* 由调用方在每帧开始喂入的窗口坐标已存于 ctx->winx/winy，
           这里转为逻辑坐标，供 joy_ui_hit / 控件使用 */
        joy_vec2_t p = joy_r2d_coordinates_from_window(ctx->r2d, ctx->winx, ctx->winy);
        ctx->mx = p.x;
        ctx->my = p.y;
        ctx->logx = p.x;
        ctx->logy = p.y;
        ctx->widget_seq = 0;
        ctx->hot = 0;
}

void joy_ui_end(joy_ui_ctx_p ctx)
{
        if (!ctx) return;
        ctx->consumed = false;
        ctx->lpressed = false;
        ctx->lreleased = false;
        ctx->wheel_used = false;
        ctx->wheel = 0.0f;
        jui_reset_slots(ctx);
        ctx->widget_seq = 0;
}

void joy_ui_input_motion(joy_ui_ctx_p ctx, float win_x, float win_y)
{
        if (!ctx) return;
        ctx->winx = win_x;
        ctx->winy = win_y;
}

void joy_ui_input_down(joy_ui_ctx_p ctx, int button)
{
        if (!ctx) return;
        if (button == 0) { ctx->ldown = true; ctx->lpressed = true; }
}

void joy_ui_input_up(joy_ui_ctx_p ctx, int button)
{
        if (!ctx) return;
        if (button == 0) { ctx->ldown = false; ctx->lreleased = true; }
}

void joy_ui_input_wheel(joy_ui_ctx_p ctx, float dy)
{
        if (!ctx) return;
        ctx->wheel += dy;
}

void joy_ui_input_key(joy_ui_ctx_p ctx, bool shift, bool ctrl)
{
        if (!ctx) return;
        ctx->shift = shift;
        ctx->ctrl  = ctrl;
}

void joy_ui_get_mouse(joy_ui_ctx_p ctx, float* out_x, float* out_y)
{
        if (out_x) *out_x = ctx ? ctx->mx : 0.0f;
        if (out_y) *out_y = ctx ? ctx->my : 0.0f;
}

bool joy_ui_is_down(joy_ui_ctx_p ctx)    { return ctx && ctx->ldown; }
bool joy_ui_is_pressed(joy_ui_ctx_p ctx) { return ctx && ctx->lpressed; }
bool joy_ui_is_released(joy_ui_ctx_p ctx){ return ctx && ctx->lreleased; }
float joy_ui_take_wheel(joy_ui_ctx_p ctx)
{
        if (!ctx || ctx->wheel_used) return 0.0f;
        ctx->wheel_used = true;
        return ctx->wheel;
}
bool joy_ui_is_shift(joy_ui_ctx_p ctx)   { return ctx && ctx->shift; }
bool joy_ui_is_ctrl(joy_ui_ctx_p ctx)    { return ctx && ctx->ctrl; }

/* ------------------------------ 裁剪 ------------------------------ */

void joy_ui_push_clip(joy_ui_ctx_p ctx, joy_rect_t rect)
{
        if (!ctx) return;
        if (ctx->clip_count >= 32) return;
        joy_r2d_push_clip_rect(ctx->r2d, rect);
        ctx->clip_stack[ctx->clip_count++] = rect;
        ctx->has_clip = true;
}

void joy_ui_pop_clip(joy_ui_ctx_p ctx)
{
        if (!ctx || ctx->clip_count <= 0) return;
        ctx->clip_count--;
        joy_r2d_pop_clip_rect(ctx->r2d);
        ctx->has_clip = ctx->clip_count > 0;
}

void joy_ui_reset_clip(joy_ui_ctx_p ctx)
{
        if (!ctx) return;
        ctx->clip_count = 0;
        ctx->has_clip = false;
        joy_r2d_reset_clip_stack(ctx->r2d);
}

bool joy_ui_hit(joy_ui_ctx_p ctx, joy_rect_t rect)
{
        if (!ctx) return false;
        if (!jui_pt_in(ctx->mx, ctx->my, rect)) return false;
        if (ctx->has_clip) {
                /* 只需检查最内层裁剪 */
                joy_rect_t c = ctx->clip_stack[ctx->clip_count - 1];
                if (!jui_pt_in(ctx->mx, ctx->my, c)) return false;
        }
        return true;
}

/* ------------------------------ 绘制辅助 ------------------------------ */

void joy_ui_fill_rect(joy_ui_ctx_p ctx, joy_rect_t r, joy_color_t c)
{
        if (!ctx) return;
        joy_r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        joy_r2d_shape_draw_rectangle(ctx->r2d, "fill", r);
}

void joy_ui_stroke_rect(joy_ui_ctx_p ctx, joy_rect_t r, joy_color_t c)
{
        if (!ctx) return;
        joy_r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        joy_r2d_shape_draw_rectangle(ctx->r2d, "line", r);
}

void joy_ui_line(joy_ui_ctx_p ctx, float x1, float y1, float x2, float y2, joy_color_t c)
{
        if (!ctx) return;
        joy_r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        joy_r2d_shape_draw_line(ctx->r2d, x1, y1, x2, y2);
}

int joy_ui_text(joy_ui_ctx_p ctx, float x, float y, const char* s, joy_color_t c)
{
        if (!ctx || !ctx->font || !s) return 0;
        int w, h;
        if (!joy_r2d_text_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, &w, &h)) return 0;
        joy_r2d_text_printx(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, x, y);
        return w;
}

bool joy_ui_text_measure(joy_ui_ctx_p ctx, const char* s, joy_color_t c, int* out_w, int* out_h)
{
        if (!ctx || !ctx->font || !s) return false;
        return joy_r2d_text_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, out_w, out_h);
}

int joy_ui_text_centered(joy_ui_ctx_p ctx, joy_rect_t r, const char* s, joy_color_t c)
{
        if (!ctx || !ctx->font || !s) return 0;
        int w = 0, h = 0;
        if (!joy_r2d_text_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, &w, &h)) return 0;
        joy_r2d_text_printx(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c,
                     r.x + (r.w - w) * 0.5f, r.y + (r.h - h) * 0.5f);
        return w;
}

/* ------------------------------ 面板 ------------------------------ */

void joy_ui_panel_begin(joy_ui_ctx_p ctx, joy_rect_t r, const char* title)
{
        if (!ctx) return;
        joy_ui_fill_rect(ctx, r, ctx->theme.panel);
        joy_rect_t th = JUI_RECT(r.x, r.y, r.w, 22.0f);
        joy_ui_fill_rect(ctx, th, ctx->theme.panel_title);
        if (title) joy_ui_text(ctx, r.x + 8.0f, r.y + 5.0f, title, (joy_color_t){ 133, 133, 133, 255 });
        joy_ui_line(ctx, r.x, r.y + 22.0f, r.x + r.w, r.y + 22.0f, ctx->theme.border);
        joy_ui_push_clip(ctx, JUI_RECT(r.x + 1.0f, r.y + 23.0f, r.w - 2.0f, r.h - 24.0f));
}

void joy_ui_panel_end(joy_ui_ctx_p ctx)
{
        if (ctx) joy_ui_pop_clip(ctx);
}

void joy_ui_panel_frame(joy_ui_ctx_p ctx, joy_rect_t r)
{
        if (ctx) joy_ui_stroke_rect(ctx, r, ctx->theme.border);
}

/* ------------------------------ 滚动 ------------------------------ */

float joy_ui_scroll_begin(joy_ui_ctx_p ctx, int id, joy_rect_t view, float content_h)
{
        if (!ctx) return 0.0f;
        float* off = NULL;
        for (int i = 0; i < ctx->scroll_count; i++) {
                if (ctx->scrolls[i].id == id) { off = &ctx->scrolls[i].off; break; }
        }
        if (!off && ctx->scroll_count < 32) {
                ctx->scrolls[ctx->scroll_count].id = id;
                ctx->scrolls[ctx->scroll_count].off = 0.0f;
                off = &ctx->scrolls[ctx->scroll_count].off;
                ctx->scroll_count++;
        }
        if (!off) return 0.0f;

        float max_off = content_h > view.h ? content_h - view.h : 0.0f;
        float dy = joy_ui_take_wheel(ctx);
        if (dy != 0.0f && joy_ui_hit(ctx, view)) {
                *off -= dy * 46.0f;
        }
        if (*off < 0) *off = 0;
        if (*off > max_off) *off = max_off;
        return *off;
}

void joy_ui_scroll_end(joy_ui_ctx_p ctx)
{
        /* 预留：滚动区结束后若需要恢复裁剪，可在这里处理 */
}

void joy_ui_scroll_set_offset(joy_ui_ctx_p ctx, int id, float off, float content_h, float view_h)
{
        float* target = NULL;
        int i;
        if (!ctx) return;
        for (i = 0; i < ctx->scroll_count; i++) {
                if (ctx->scrolls[i].id == id) { target = &ctx->scrolls[i].off; break; }
        }
        if (!target) {
                if (ctx->scroll_count >= 32) return;
                ctx->scrolls[ctx->scroll_count].id = id;
                ctx->scrolls[ctx->scroll_count].off = 0.0f;
                target = &ctx->scrolls[ctx->scroll_count].off;
                ctx->scroll_count++;
        }
        if (content_h > 0.0f && view_h > 0.0f) {
                float max_off = content_h > view_h ? content_h - view_h : 0.0f;
                if (off < 0) off = 0;
                if (off > max_off) off = max_off;
        }
        *target = off;
}

void joy_ui_scrollbar(joy_ui_ctx_p ctx, joy_rect_t view, float content_h, float off)
{
        if (!ctx || content_h <= view.h) return;
        float thumb_h = view.h * (view.h / content_h);
        if (thumb_h < 24.0f) thumb_h = 24.0f;
        float t = content_h > view.h ? off / (content_h - view.h) : 0.0f;
        float ty = view.y + (view.h - thumb_h) * t;
        joy_ui_fill_rect(ctx, JUI_RECT(view.x + view.w - 6.0f, view.y, 5.0f, view.h),
                      ctx->theme.panel_title);
        joy_ui_fill_rect(ctx, JUI_RECT(view.x + view.w - 6.0f, ty, 5.0f, thumb_h),
                      (joy_color_t){ 110, 110, 116, 255 });
}

/* ------------------------------ 控件 ------------------------------ */

bool joy_ui_button(joy_ui_ctx_p ctx, int id, joy_rect_t r, const char* text)
{
        if (!ctx || !ctx->font) return false;
        joy_ui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        if (!s->btn) {
                s->btn = joy_ui_button_create(ctx->r2d, 0.0f, 0.0f, r.w / 480.0f, r.h / 320.0f);
                if (!s->btn) return false;
                joy_ui_button_set_textx(s->btn, ctx->font, text, (int)SDL_strlen(text), ctx->theme.btn_text);
        }
        joy_button_p b = s->btn;
        joy_ui_button_set_position(b, r.x, r.y);
        joy_ui_button_set_size(b, r.w, r.h);
        joy_ui_button_set_normal_color(b, ctx->theme.btn_normal);
        joy_ui_button_set_hover_color(b, ctx->theme.btn_hover);
        joy_ui_button_set_pressed_color(b, ctx->theme.btn_pressed);

        bool hovered = joy_ui_hit(ctx, r);
        joy_ui_button_set_hovered(b, hovered);
        bool clicked = false;
        if (hovered && ctx->lpressed) { ctx->active = id; ctx->hot = id; }
        if (ctx->active == id && ctx->lreleased) {
                if (hovered) clicked = true;
                ctx->active = 0;
        }
        joy_ui_button_set_pressed(b, ctx->active == id);

        joy_ui_button_draw(b);
        return clicked;
}

bool joy_ui_checkbox(joy_ui_ctx_p ctx, int id, joy_rect_t r, bool* value, const char* label)
{
        if (!ctx || !ctx->font) return false;
        joy_ui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        /* 复用 checkbox：label 只在创建时定，之后位置由 set_position 移动 */
        if (!s->cb) {
                s->cb = joy_ui_checkbox_create(r.x / 480.0f, r.y / 320.0f, 14.0f / 480.0f, label);
                if (!s->cb) return false;
        }
        joy_checkbox_p cb = s->cb;
        joy_ui_checkbox_set_r2d(cb, ctx->r2d);
        joy_ui_checkbox_set_position(cb, (int)r.x, (int)r.y);
        joy_ui_checkbox_set_colors(cb, ctx->theme.cb_box, ctx->theme.cb_check, ctx->theme.cb_hover);
        joy_ui_checkbox_set_text_color(cb, ctx->theme.cb_text);

        joy_checkbox_state_t cur = *value ? JOY_CHECKBOX_CHECKED : JOY_CHECKBOX_UNCHECKED;
        if (joy_ui_checkbox_get_state(cb) != cur) joy_ui_checkbox_set_state(cb, cur);

        bool hovered = joy_ui_hit(ctx, r);
        joy_ui_checkbox_set_hovered(cb, hovered);

        bool changed = false;
        if (hovered && ctx->lpressed) ctx->active = id;
        if (ctx->active == id && ctx->lreleased) {
                if (hovered) { *value = !*value; changed = true; }
                ctx->active = 0;
        }

        joy_ui_checkbox_draw(cb, ctx->r2d, ctx->font);
        return changed;
}

bool joy_ui_slider(joy_ui_ctx_p ctx, int id, joy_rect_t track, float* value, float minv, float maxv)
{
        if (!ctx) return false;
        joy_ui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        if (!s->sl) {
                s->sl = joy_ui_slider_create(ctx->r2d, track.x / 480.0f, track.y / 320.0f, track.w / 480.0f, track.h / 320.0f,
                                      minv, maxv, *value);
                if (!s->sl) return false;
        }
        joy_slider_p sl = s->sl;
        joy_ui_slider_set_rect(sl, track.x, track.y, track.w, track.h);
        joy_ui_slider_set_range(sl, minv, maxv);
        joy_ui_slider_set_colors(sl, ctx->theme.sl_track, ctx->theme.sl_fill,
                          ctx->theme.sl_thumb, ctx->theme.sl_thumb_hover);

        /* 反向同步：外部可能改了 value */
        if (joy_ui_slider_get_value(sl) != *value) joy_ui_slider_set_value(sl, *value);

        bool hovered = joy_ui_hit(ctx, track);
        joy_ui_slider_set_hovered(sl, hovered);

        bool changed = false;
        if (hovered && ctx->lpressed) { ctx->active = id; }
        if (ctx->active == id) {
                /* 拖拽：从滑块中心到鼠标横坐标 */
                float ratio = (ctx->mx - track.x) / track.w;
                if (ratio < 0) ratio = 0;
                if (ratio > 1) ratio = 1;
                float nv = minv + ratio * (maxv - minv);
                if (nv != *value) { *value = nv; changed = true; }
                if (ctx->lreleased) ctx->active = 0;
        }

        joy_ui_slider_draw(sl);
        return changed;
}
