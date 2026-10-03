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
 *   - 裁剪基于 r2d_push_clip_rect / r2d_pop_clip_rect，滚动面板的基础。
 *   - 文字统一走 jtext_printx 缓存，避免每帧建销纹理。
 * ============================================================================ */

#include <SDL3/SDL.h>
#include "joy.h"

#include <stdlib.h>
#include <string.h>

/* ------------------------------ 结构定义 ------------------------------ */

typedef struct jui_slot {
        int       id;         /* -1 = 空闲 */
        button_p  btn;
        checkbox_p cb;
        slider_p  sl;
} jui_slot_t;

struct jui_ctx {
        jr2d_p r2d;
        font_p font;
        jui_theme_t theme;

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
        jui_slot_t* slots;
        int         slot_count;
        int         slot_cap;

        /* 裁剪栈镜像（用于命中测试） */
        int     clip_count;
        jrect_t clip_stack[32];
        bool    has_clip;

        /* 滚动区 */
        struct { int id; float off; } scrolls[32];
        int scroll_count;

        /* 每帧一次性输入 */
        bool  consumed;            /* begin 已消费 */
        float logx, logy;
};

/* ------------------------------ 主题 ------------------------------ */

static jui_theme_t jui_default_theme(void)
{
        jui_theme_t t;
        t.text         = (jcolor_t){ 212, 212, 212, 255 };
        t.btn_normal   = (jcolor_t){ 51,  51,  55,  255 };
        t.btn_hover    = (jcolor_t){ 70,  70,  75,  255 };
        t.btn_pressed  = (jcolor_t){ 23,  124, 189, 255 };
        t.btn_text     = (jcolor_t){ 212, 212, 212, 255 };
        t.cb_box       = (jcolor_t){ 51,  51,  55,  255 };
        t.cb_check     = (jcolor_t){ 23,  124, 189, 255 };
        t.cb_hover     = (jcolor_t){ 70,  70,  75,  255 };
        t.cb_text      = (jcolor_t){ 212, 212, 212, 255 };
        t.sl_track     = (jcolor_t){ 51,  51,  55,  255 };
        t.sl_fill      = (jcolor_t){ 23,  124, 189, 255 };
        t.sl_thumb     = (jcolor_t){ 150, 150, 156, 255 };
        t.sl_thumb_hover = (jcolor_t){ 200, 200, 210, 255 };
        t.panel        = (jcolor_t){ 37,  37,  38,  255 };
        t.panel_title  = (jcolor_t){ 45,  45,  48,  255 };
        t.border       = (jcolor_t){ 62,  62,  66,  255 };
        return t;
}

/* ------------------------------ 工具 ------------------------------ */

static jrect_t JUI_RECT(float x, float y, float w, float h)
{
        jrect_t r; r.x = x; r.y = y; r.w = w; r.h = h; return r;
}

static bool jui_pt_in(float x, float y, jrect_t r)
{
        return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

/* ------------------------------ 生命周期 ------------------------------ */

jui_ctx_p jui_ctx_create(jr2d_p r2d, font_p font)
{
        jui_ctx_p ctx = (jui_ctx_p)calloc(1, sizeof(jui_ctx_t));
        if (!ctx) return NULL;
        ctx->r2d  = r2d;
        ctx->font = font;
        ctx->theme = jui_default_theme();
        return ctx;
}

void jui_ctx_destroy(jui_ctx_p ctx)
{
        if (!ctx) return;
        for (int i = 0; i < ctx->slot_count; i++) {
                jui_slot_t* s = &ctx->slots[i];
                if (s->btn) button_destroy(s->btn);
                if (s->cb)  checkbox_destroy(s->cb);
                if (s->sl)  slider_destroy(s->sl);
        }
        free(ctx->slots);
        free(ctx);
}

void jui_set_theme(jui_ctx_p ctx, const jui_theme_t* theme)
{
        if (ctx && theme) ctx->theme = *theme;
}

/* ------------------------------ 槽位池 ------------------------------ */

static jui_slot_t* jui_get_slot(jui_ctx_p ctx, int id)
{
        if (ctx->slot_count < 32) {
                if (ctx->slot_count >= ctx->slot_cap) {
                        int nc = ctx->slot_cap ? ctx->slot_cap * 2 : 64;
                        jui_slot_t* ns = (jui_slot_t*)realloc(ctx->slots, (size_t)nc * sizeof(jui_slot_t));
                        if (!ns) return NULL;
                        ctx->slots = ns;
                        ctx->slot_cap = nc;
                }
                jui_slot_t* s = &ctx->slots[ctx->slot_count++];
                memset(s, 0, sizeof(*s));
                s->id = id;
                return s;
        }
        /* 已有足够槽位：查找或回收空闲 */
        jui_slot_t* free_slot = NULL;
        for (int i = 0; i < ctx->slot_count; i++) {
                jui_slot_t* s = &ctx->slots[i];
                if (s->id == id) return s;
                if (s->id < 0 && !free_slot) free_slot = s;
        }
        if (free_slot) { free_slot->id = id; return free_slot; }
        return NULL;
}

/* 每帧结束后清空槽位 id，下帧按需重新分配 */
static void jui_reset_slots(jui_ctx_p ctx)
{
        for (int i = 0; i < ctx->slot_count; i++) ctx->slots[i].id = -1;
}

/* ------------------------------ 输入 ------------------------------ */

void jui_begin(jui_ctx_p ctx)
{
        if (!ctx) return;
        ctx->consumed = true;
        /* 由调用方在每帧开始喂入的窗口坐标已存于 ctx->winx/winy，
           这里转为逻辑坐标，供 jui_hit / 控件使用 */
        vec2_t p = r2d_coordinates_from_window(ctx->r2d, ctx->winx, ctx->winy);
        ctx->mx = p.x;
        ctx->my = p.y;
        ctx->logx = p.x;
        ctx->logy = p.y;
        ctx->widget_seq = 0;
        ctx->hot = 0;
}

void jui_end(jui_ctx_p ctx)
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

void jui_input_motion(jui_ctx_p ctx, float win_x, float win_y)
{
        if (!ctx) return;
        ctx->winx = win_x;
        ctx->winy = win_y;
}

void jui_input_down(jui_ctx_p ctx, int button)
{
        if (!ctx) return;
        if (button == 0) { ctx->ldown = true; ctx->lpressed = true; }
}

void jui_input_up(jui_ctx_p ctx, int button)
{
        if (!ctx) return;
        if (button == 0) { ctx->ldown = false; ctx->lreleased = true; }
}

void jui_input_wheel(jui_ctx_p ctx, float dy)
{
        if (!ctx) return;
        ctx->wheel += dy;
}

void jui_input_key(jui_ctx_p ctx, bool shift, bool ctrl)
{
        if (!ctx) return;
        ctx->shift = shift;
        ctx->ctrl  = ctrl;
}

void jui_get_mouse(jui_ctx_p ctx, float* out_x, float* out_y)
{
        if (out_x) *out_x = ctx ? ctx->mx : 0.0f;
        if (out_y) *out_y = ctx ? ctx->my : 0.0f;
}

bool jui_is_down(jui_ctx_p ctx)    { return ctx && ctx->ldown; }
bool jui_is_pressed(jui_ctx_p ctx) { return ctx && ctx->lpressed; }
bool jui_is_released(jui_ctx_p ctx){ return ctx && ctx->lreleased; }
float jui_take_wheel(jui_ctx_p ctx)
{
        if (!ctx || ctx->wheel_used) return 0.0f;
        ctx->wheel_used = true;
        return ctx->wheel;
}
bool jui_is_shift(jui_ctx_p ctx)   { return ctx && ctx->shift; }
bool jui_is_ctrl(jui_ctx_p ctx)    { return ctx && ctx->ctrl; }

/* ------------------------------ 裁剪 ------------------------------ */

void jui_push_clip(jui_ctx_p ctx, jrect_t rect)
{
        if (!ctx) return;
        if (ctx->clip_count >= 32) return;
        r2d_push_clip_rect(ctx->r2d, rect);
        ctx->clip_stack[ctx->clip_count++] = rect;
        ctx->has_clip = true;
}

void jui_pop_clip(jui_ctx_p ctx)
{
        if (!ctx || ctx->clip_count <= 0) return;
        ctx->clip_count--;
        r2d_pop_clip_rect(ctx->r2d);
        ctx->has_clip = ctx->clip_count > 0;
}

void jui_reset_clip(jui_ctx_p ctx)
{
        if (!ctx) return;
        ctx->clip_count = 0;
        ctx->has_clip = false;
        r2d_reset_clip_stack(ctx->r2d);
}

bool jui_hit(jui_ctx_p ctx, jrect_t rect)
{
        if (!ctx) return false;
        if (!jui_pt_in(ctx->mx, ctx->my, rect)) return false;
        if (ctx->has_clip) {
                /* 只需检查最内层裁剪 */
                jrect_t c = ctx->clip_stack[ctx->clip_count - 1];
                if (!jui_pt_in(ctx->mx, ctx->my, c)) return false;
        }
        return true;
}

/* ------------------------------ 绘制辅助 ------------------------------ */

void jui_fill_rect(jui_ctx_p ctx, jrect_t r, jcolor_t c)
{
        if (!ctx) return;
        r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        shape_draw_rectangle(ctx->r2d, "fill", r);
}

void jui_stroke_rect(jui_ctx_p ctx, jrect_t r, jcolor_t c)
{
        if (!ctx) return;
        r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        shape_draw_rectangle(ctx->r2d, "line", r);
}

void jui_line(jui_ctx_p ctx, float x1, float y1, float x2, float y2, jcolor_t c)
{
        if (!ctx) return;
        r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        shape_draw_line(ctx->r2d, x1, y1, x2, y2);
}

int jui_text(jui_ctx_p ctx, float x, float y, const char* s, jcolor_t c)
{
        if (!ctx || !ctx->font || !s) return 0;
        int w, h;
        if (!jtext_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, &w, &h)) return 0;
        jtext_printx(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, x, y);
        return w;
}

bool jui_text_measure(jui_ctx_p ctx, const char* s, jcolor_t c, int* out_w, int* out_h)
{
        if (!ctx || !ctx->font || !s) return false;
        return jtext_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, out_w, out_h);
}

int jui_text_centered(jui_ctx_p ctx, jrect_t r, const char* s, jcolor_t c)
{
        if (!ctx || !ctx->font || !s) return 0;
        int w = 0, h = 0;
        if (!jtext_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, &w, &h)) return 0;
        jtext_printx(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c,
                     r.x + (r.w - w) * 0.5f, r.y + (r.h - h) * 0.5f);
        return w;
}

/* ------------------------------ 面板 ------------------------------ */

void jui_panel_begin(jui_ctx_p ctx, jrect_t r, const char* title)
{
        if (!ctx) return;
        jui_fill_rect(ctx, r, ctx->theme.panel);
        jrect_t th = JUI_RECT(r.x, r.y, r.w, 22.0f);
        jui_fill_rect(ctx, th, ctx->theme.panel_title);
        if (title) jui_text(ctx, r.x + 8.0f, r.y + 5.0f, title, (jcolor_t){ 133, 133, 133, 255 });
        jui_line(ctx, r.x, r.y + 22.0f, r.x + r.w, r.y + 22.0f, ctx->theme.border);
        jui_push_clip(ctx, JUI_RECT(r.x + 1.0f, r.y + 23.0f, r.w - 2.0f, r.h - 24.0f));
}

void jui_panel_end(jui_ctx_p ctx)
{
        if (ctx) jui_pop_clip(ctx);
}

void jui_panel_frame(jui_ctx_p ctx, jrect_t r)
{
        if (ctx) jui_stroke_rect(ctx, r, ctx->theme.border);
}

/* ------------------------------ 滚动 ------------------------------ */

float jui_scroll_begin(jui_ctx_p ctx, int id, jrect_t view, float content_h)
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
        float dy = jui_take_wheel(ctx);
        if (dy != 0.0f && jui_hit(ctx, view)) {
                *off -= dy * 46.0f;
        }
        if (*off < 0) *off = 0;
        if (*off > max_off) *off = max_off;
        return *off;
}

void jui_scroll_end(jui_ctx_p ctx)
{
        /* 预留：滚动区结束后若需要恢复裁剪，可在这里处理 */
}

void jui_scroll_set_offset(jui_ctx_p ctx, int id, float off, float content_h, float view_h)
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

void jui_scrollbar(jui_ctx_p ctx, jrect_t view, float content_h, float off)
{
        if (!ctx || content_h <= view.h) return;
        float thumb_h = view.h * (view.h / content_h);
        if (thumb_h < 24.0f) thumb_h = 24.0f;
        float t = content_h > view.h ? off / (content_h - view.h) : 0.0f;
        float ty = view.y + (view.h - thumb_h) * t;
        jui_fill_rect(ctx, JUI_RECT(view.x + view.w - 6.0f, view.y, 5.0f, view.h),
                      ctx->theme.panel_title);
        jui_fill_rect(ctx, JUI_RECT(view.x + view.w - 6.0f, ty, 5.0f, thumb_h),
                      (jcolor_t){ 110, 110, 116, 255 });
}

/* ------------------------------ 控件 ------------------------------ */

bool jui_button(jui_ctx_p ctx, int id, jrect_t r, const char* text)
{
        if (!ctx || !ctx->font) return false;
        jui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        if (!s->btn) {
                s->btn = button_create(ctx->r2d, 0.0f, 0.0f, r.w / 480.0f, r.h / 320.0f);
                if (!s->btn) return false;
                button_set_textx(s->btn, ctx->font, text, (int)SDL_strlen(text), ctx->theme.btn_text);
        }
        button_p b = s->btn;
        button_set_position(b, r.x, r.y);
        button_set_size(b, r.w, r.h);
        button_set_normal_color(b, ctx->theme.btn_normal);
        button_set_hover_color(b, ctx->theme.btn_hover);
        button_set_pressed_color(b, ctx->theme.btn_pressed);

        bool hovered = jui_hit(ctx, r);
        button_set_hovered(b, hovered);
        bool clicked = false;
        if (hovered && ctx->lpressed) { ctx->active = id; ctx->hot = id; }
        if (ctx->active == id && ctx->lreleased) {
                if (hovered) clicked = true;
                ctx->active = 0;
        }
        button_set_pressed(b, ctx->active == id);

        button_draw(b);
        return clicked;
}

bool jui_checkbox(jui_ctx_p ctx, int id, jrect_t r, bool* value, const char* label)
{
        if (!ctx || !ctx->font) return false;
        jui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        /* 复用 checkbox：label 只在创建时定，之后位置由 set_position 移动 */
        if (!s->cb) {
                s->cb = checkbox_create(r.x / 480.0f, r.y / 320.0f, 14.0f / 480.0f, label);
                if (!s->cb) return false;
        }
        checkbox_p cb = s->cb;
        checkbox_set_r2d(cb, ctx->r2d);
        checkbox_set_position(cb, (int)r.x, (int)r.y);
        checkbox_set_colors(cb, ctx->theme.cb_box, ctx->theme.cb_check, ctx->theme.cb_hover);
        checkbox_set_text_color(cb, ctx->theme.cb_text);

        checkbox_state_t cur = *value ? JOY_CHECKBOX_CHECKED : JOY_CHECKBOX_UNCHECKED;
        if (checkbox_get_state(cb) != cur) checkbox_set_state(cb, cur);

        bool hovered = jui_hit(ctx, r);
        checkbox_set_hovered(cb, hovered);

        bool changed = false;
        if (hovered && ctx->lpressed) ctx->active = id;
        if (ctx->active == id && ctx->lreleased) {
                if (hovered) { *value = !*value; changed = true; }
                ctx->active = 0;
        }

        checkbox_draw(cb, ctx->r2d, ctx->font);
        return changed;
}

bool jui_slider(jui_ctx_p ctx, int id, jrect_t track, float* value, float minv, float maxv)
{
        if (!ctx) return false;
        jui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        if (!s->sl) {
                s->sl = slider_create(ctx->r2d, track.x / 480.0f, track.y / 320.0f, track.w / 480.0f, track.h / 320.0f,
                                      minv, maxv, *value);
                if (!s->sl) return false;
        }
        slider_p sl = s->sl;
        slider_set_rect(sl, track.x, track.y, track.w, track.h);
        slider_set_range(sl, minv, maxv);
        slider_set_colors(sl, ctx->theme.sl_track, ctx->theme.sl_fill,
                          ctx->theme.sl_thumb, ctx->theme.sl_thumb_hover);

        /* 反向同步：外部可能改了 value */
        if (slider_get_value(sl) != *value) slider_set_value(sl, *value);

        bool hovered = jui_hit(ctx, track);
        slider_set_hovered(sl, hovered);

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

        slider_draw(sl);
        return changed;
}
