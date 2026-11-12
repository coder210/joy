#include "external/klib/kvec.h"
#include <SDL3/SDL.h>
#include "joy.h"

#define JOYSTICK_BG_COLOR 0x555555FF
#define JOYSTICK_HANDLE_COLOR 0x888888FF
#define JOYSTICK_HANDLE_ACTIVE_COLOR 0x4CAF50FF

static joy_rect_t jui_frect(joy_r2d_p r2d, float fx, float fy, float fw, float fh)
{
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        joy_rect_t r = { fx * (float)lw, fy * (float)lh, fw * (float)lw, fh * (float)lh };
        return r;
}
static void jui_fpx(joy_r2d_p r2d, float fx, float fy, float fw, float fh, float* x, float* y, float* w, float* h)
{
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        *x = fx * (float)lw; *y = fy * (float)lh;
        *w = fw * (float)lw; *h = fh * (float)lh;
}

typedef kvec_t(char*) joy_dg_row_t;

struct joy_label {
        joy_r2d_p r2d;
        joy_vec2_t position;
        joy_text_p text;
        joy_color_t color;
};


struct joy_button {
        joy_r2d_p r2d;   // 渲染器
        joy_vec2_t position;      // 位置
        float width;              // 宽度
        float height;             // 高度
        joy_color_t normal_color;   // 正常状态颜色
        joy_color_t hover_color;    // 悬停状态颜色
        joy_color_t pressed_color;  // 按下状态颜色
        joy_color_t display_color;  // 实际渲染颜色（平滑过渡用）
        joy_vec2_t text_pos;      // 文本相对位置
        joy_image_p image;            // 按钮正常纹理（可选）
        joy_image_p pressed_image;    // 按钮按下纹理（可选）
        joy_text_p text;      // 按钮文件纹理
        bool is_hovered;          // 内部状态：由 is_hovered() / is_pressed() 设置
        bool is_pressed;
};

struct joy_combobox_item {
        joy_rect_t rect;
        joy_text_p text;
        bool is_selected;
};

struct joy_combobox {
        joy_rect_t main_rect;
        joy_rect_t dropdown_rect;
        joy_combobox_item_t items[20];
        int item_count;
        int item_height;
        float ui_scale;             /* 相对 480x320 设计稿的缩放，供绘制换算固定像素 */
        int selected_index;
        bool is_open;
        joy_text_p text;
        joy_color_t bg_color;
        joy_color_t text_color;
        joy_color_t hover_color;
        joy_r2d_p r2d;
};

struct joy_datagrid {
        joy_r2d_p r2d;
        int x, y;              // 位置
        int width, height;     // 尺寸
        int row_height;        // 行高
        int col_count;         // 列数
        int row_count;         // 行数（包括表头）
        int visible_rows;      // 可见行数
        int scroll_offset;     // 滚动偏移
        int selected_row;      // 选中的行
        joy_text_p headers[512];        // 表头
        kvec_t(joy_dg_row_t) data;              // 表格数据（klib 动态数组 of joy_dg_row_t）
        int* col_widths;       // 列宽
        bool has_header;   // 是否有表头
        joy_font_p font;        // 字体
        joy_color_t grid_color;
        joy_color_t bg_color;
        joy_color_t text_color;
        joy_color_t header_bg_color;
        joy_color_t selected_color;
};

struct joy_radio {
        joy_r2d_p r2d;                // 渲染器（布局换算用）
        int x, y;                  // 位置
        int size;                  // 大小
        /* 创建时传入的比例（fx=较宽, fy=较高, size_frac=较宽）。保存下来，
         * 渲染/命中前按"当前"逻辑分辨率重算（同 checkbox），否则分辨率改变后
         * 单选框会停在旧像素位置且相对屏幕偏小。 */
        float fx, fy, size_frac;
        bool checked;              // 是否选中
        char* label;               // 标签文本
        int group_id;              // 所属组ID
        joy_rect_t hit_area;         // 点击区域
        bool hover;                // 鼠标悬停状态
        joy_color_t r1_color;
        joy_color_t r2_color;
        joy_color_t r3_color;
};

/* 布局重算（实现见下）：把创建时保存的比例按"当前"逻辑分辨率换算成像素，
 * 使逻辑分辨率改变后控件仍保持原有的相对位置与大小。 */
static void checkbox_update_layout(joy_checkbox_p cb);
static void radio_update_layout(joy_radio_p rb);

struct joy_checkbox {
        joy_r2d_p r2d;                 // 渲染器（坐标转换用）
        int x, y;                   // 位置
        int size;                   // 大小
        /* 创建时传入的比例（fx=较宽, fy=较高, size_frac=较宽）。
         * 保存下来，在绘制/命中前用"当前"逻辑分辨率重算 x/y/size —— 否则逻辑分辨率
         * 改变（如 480x320 → 960x640）后复选框会停在旧像素位置且相对屏幕偏小。 */
        float fx, fy, size_frac;
        joy_checkbox_state_t state;     // 当前状态
        char* label;                // 标签文本
        joy_color_t box_color;         // 框的颜色
        joy_color_t check_color;       // 勾选标记的颜色
        joy_color_t text_color;        // 文本颜色
        joy_color_t hover_color;       // 悬停颜色
        bool hover;                 // 悬停状态
        bool enabled;               // 是否启用
        joy_rect_t hit_area;           // 点击区域
};

struct joy_inputbox {
        joy_r2d_p r2d;
        joy_rect_t rect;               // 位置和大小
        joy_color_t bg_color;           // 背景色
        joy_color_t border_color;       // 边框色
        joy_color_t text_color;         // 文字色
        joy_color_t cursor_color;       // 光标色
        joy_color_t sel_color;          // 选中背景色
        joy_color_t focus_border_color; // 聚焦时边框色
        joy_font_p font;                  // 字体
        char* buffer;                 // 文本缓冲区
        int buf_len;                  // 缓冲区大小
        int text_len;                 // 当前文本长度
        int cursor_pos;               // 光标位置（字节偏移）
        int sel_start;                // 选中起始（-1 表示无选中）
        int sel_end;                  // 选中结束
        int scroll_offset;            // 文本滚动偏移（像素）
        bool focused;                 // 是否聚焦
        bool hovered;                 // 鼠标悬停
        uint64_t cursor_timer;          // 光标闪烁计时
        bool cursor_visible;          // 光标是否可见
        void (*on_submit)(struct joy_inputbox* ti, void* userdata);
        void (*on_change)(struct joy_inputbox* ti, void* userdata);
        void* cb_userdata;
};

struct joy_slider {
        joy_r2d_p r2d;
        joy_rect_t track;             // 滑轨区域
        float min, max;              // 取值范围
        float value;                 // 当前值
        float thumb_width;           // 滑块宽度（高度 = track.h）
        bool is_dragging;            // 是否在拖动
        joy_color_t track_color;       // 滑轨背景色
        joy_color_t fill_color;        // 已填充部分颜色
        joy_color_t thumb_color;       // 滑块颜色
        joy_color_t thumb_hover_color; // 滑块悬停颜色
        bool hovered;                // 滑块是否悬停
};



struct joy_popup {
        joy_r2d_p r2d;
        float x, y, w, h;
        float title_height;         // title bar height (set in draw)
        float padding;
        float close_size;
        joy_color_t bg_color;
        joy_color_t border_color;
        joy_color_t title_bg_color;
        joy_color_t content_color;
        joy_color_t close_color;
        joy_color_t close_hover_color;
        joy_color_t title_text_color;
        bool visible;
        bool closeable;
        char title[JOY_POPUP_MAX_TITLE];
        char content[JOY_POPUP_MAX_CONTENT];
        bool close_hovered;
        bool is_dragging;
        float drag_off_x, drag_off_y;
};


struct joy_tooltip {
        joy_r2d_p r2d;
        float x, y;
        char text[JOY_TOOLTIP_MAX_TEXT];
        int text_len;
        joy_color_t bg_color;
        joy_color_t border_color;
        joy_color_t text_color;
        float padding;
        bool visible;
};


struct joy_joystick {
        joy_r2d_p r2d;
        joy_rect_t background;
        joy_rect_t handle;
        joy_vec2_t center;
        float radius;
        float handle_radius;
        joy_vec2_t direction;
        float magnitude;
        float deadzone;
        bool is_dragging;
        bool is_touch_active;
        int touch_id;
        joy_image_p base_image;     // 当前显示的底圈纹理（指向 default 或某 mode 的纹理，不单独拥有）
        joy_image_p thumb_image;    // 当前显示的摇杆头纹理
        joy_image_p base_default;   // 默认底圈（joy_input_joystick_set_textures 设置）
        joy_image_p thumb_default;  // 默认摇杆头
        joy_image_p mode_base[5];   // 各方向模式专属底圈（NULL=回退默认）
        joy_image_p mode_thumb[5];  // 各方向模式专属摇杆头
        joystick_dir_mode dir_mode;  // 方向约束模式（默认全方向）
};

static float distance_squared(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return dx * dx + dy * dy;
}

joy_label_p
joy_ui_label_create(joy_r2d_p r2d, joy_font_p fontinfo, const int* codepoints, int num_codepoints)
{
        joy_label_p label = (joy_label_p)malloc(sizeof(joy_label_t));
        label->r2d = r2d;
        label->position = (joy_vec2_t){ 0, 0 };
        label->color = (joy_color_t){ 255, 255,255, 255 };
        label->text = joy_ui_text_create(r2d, fontinfo, codepoints, num_codepoints, label->color);
        return label;
}

void
joy_ui_label_destroy(joy_label_p label)
{
        if (!label) return;
        if (label->text) joy_ui_text_destroy(label->text);
        free(label);
}

void
joy_ui_label_set_text(joy_label_p label, joy_font_p font,
        const int* codepoints, int num_codepoints,
        joy_color_t color)
{
        if (label->text) {
                joy_ui_text_update(label->text, font, codepoints, num_codepoints, color);
        }
        else {
                label->text = joy_ui_text_create(label->r2d, font, codepoints, num_codepoints, color);
        }
}

void joy_ui_label_draw(joy_label_p label)
{
        //joy_rect_t dest_rect;
        if (!label) return;

        //r2d = label->r2d;
        //dest_rect.x = label->position.x;
        //dest_rect.y = label->position.y;
        //dest_rect.w = label->text->w;
        //dest_rect.h = label->text->h;
        //SDL_RenderTexture(r2d, label->text, NULL, &dest_rect);
        joy_r2d_jtext_print(label->r2d, label->text, label->position.x, label->position.y);
}

void joy_label_handle_event(joy_label_p label, void* event)
{
        (void)label;
        (void)event;
}

joy_button_p
joy_ui_button_create(joy_r2d_p r2d, float cx, float cy, float wf, float hf)
{
        joy_button_p btn = (joy_button_p)malloc(sizeof(joy_button_t));
        if (!btn) return NULL;
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        float w = wf * (float)lw, h = hf * (float)lh;
        float x = (cx - wf * 0.5f) * (float)lw, y = (cy - hf * 0.5f) * (float)lh;
        btn->r2d = r2d;
        btn->position = (joy_vec2_t){ x, y };
        btn->width = w;
        btn->height = h;
        btn->normal_color = (joy_color_t){ 100, 100, 200, 255 };
        btn->hover_color = (joy_color_t){ 150, 150, 250, 255 };
        btn->pressed_color = (joy_color_t){ 50, 50, 150, 255 };
        btn->text_pos = (joy_vec2_t){ 0, 0 };
        btn->image = NULL;
        btn->pressed_image = NULL;
        btn->text = NULL;
        btn->is_hovered = btn->is_pressed = false;
        btn->display_color = btn->normal_color;
        return btn;
}

void joy_ui_button_set_normal_color(joy_button_p btn, joy_color_t color)
{
        if (!btn) return;
        btn->normal_color = color;
}

void joy_ui_button_set_hover_color(joy_button_p btn, joy_color_t color)
{
        if (!btn) return;
        btn->hover_color = color;
}

void joy_ui_button_set_pressed_color(joy_button_p btn, joy_color_t color)
{
        if (!btn) return;
        btn->pressed_color = color;
}

void joy_ui_button_set_image(joy_button_p btn, joy_image_p image)
{
        if (!btn) return;
        btn->image = image;
}

void joy_ui_button_set_pressed_image(joy_button_p btn, joy_image_p image)
{
        if (!btn) return;
        btn->pressed_image = image;
}



void joy_ui_button_set_textx(joy_button_p btn, joy_font_p font,
        const char* str, int len, joy_color_t color)
{
        if (!btn) return;
        if (btn->text) {
                joy_ui_text_updatex(btn->text, font, str, len, color);
        }
        else {
                btn->text = joy_ui_text_createx(btn->r2d, font, str, len, color);
        }
}

void
joy_ui_button_set_text(joy_button_p btn, joy_font_p font,
        const int* codepoints, int num_codepoints, joy_color_t color)
{
        if (!btn) return;
        if (btn->text) {
                joy_ui_text_update(btn->text, font, codepoints, num_codepoints, color);
        }
        else {
                btn->text = joy_ui_text_create(btn->r2d, font, codepoints, num_codepoints, color);
        }
}

void joy_ui_button_set_position(joy_button_p btn, float x, float y)
{
        if (!btn) return;
        btn->position.x = x;
        btn->position.y = y;
}

joy_vec2_t joy_ui_button_get_position(joy_button_p btn)
{
        return btn->position;
}

void joy_ui_button_set_size(joy_button_p btn, float width, float height)
{
        if (!btn) return;
        btn->width = width;
        btn->height = height;
}

static bool button_hit_test(joy_button_p btn, float x, float y)
{
        joy_vec2_t logic = joy_r2d_coordinates_from_window(btn->r2d, x, y);
        return logic.x >= btn->position.x && logic.x <= btn->position.x + btn->width
                && logic.y >= btn->position.y && logic.y <= btn->position.y + btn->height;
}

bool joy_ui_button_is_hovered(joy_button_p btn, float x, float y)
{
        if (!btn) return false;
        btn->is_hovered = button_hit_test(btn, x, y);
        return btn->is_hovered;
}

bool joy_ui_button_is_pressed(joy_button_p btn, float x, float y)
{
        if (!btn) return false;
        btn->is_pressed = button_hit_test(btn, x, y);
        return btn->is_pressed;
}

void joy_ui_button_set_hovered(joy_button_p btn, bool v)
{
        if (btn) btn->is_hovered = v;
}

void joy_ui_button_set_text_offset(joy_button_p btn, float x, float y)
{
        if (!btn) return;
        btn->text_pos.x = x;
        btn->text_pos.y = y;
}

void joy_ui_button_set_pressed(joy_button_p btn, bool v)
{
        if (btn) btn->is_pressed = v;
}

bool joy_ui_button_get_pressed(joy_button_p btn)
{
        return btn ? btn->is_pressed : false;
}

void joy_ui_button_destroy(joy_button_p btn)
{
        if (!btn) return;
        if (btn->text) joy_ui_text_destroy(btn->text);
        if (btn->image) joy_ui_image_destroy(btn->image);
        if (btn->pressed_image) joy_ui_image_destroy(btn->pressed_image);
        free(btn);
}


void joy_ui_button_draw(joy_button_p btn)
{
        joy_r2d_p r2d;
        joy_rect_t dest_rect, rect, inner;
        joy_color_t color;
        if (!btn) return;

        color = btn->normal_color;
        if (btn->is_pressed) {
                color = btn->pressed_color;
        }
        else if (btn->is_hovered) {
                color = btn->hover_color;
        }

        // 平滑过渡：display_color 每帧向目标颜色靠拢 15%
        #define LERP_CHANNEL(a, b, t) ((uint8_t)((float)(a) + ((float)(b) - (float)(a)) * (t)))
        float t = 0.15f;
        btn->display_color.r = LERP_CHANNEL(btn->display_color.r, color.r, t);
        btn->display_color.g = LERP_CHANNEL(btn->display_color.g, color.g, t);
        btn->display_color.b = LERP_CHANNEL(btn->display_color.b, color.b, t);
        btn->display_color.a = LERP_CHANNEL(btn->display_color.a, color.a, t);
        #undef LERP_CHANNEL

        rect.x = btn->position.x;
        rect.y = btn->position.y;
        rect.w = btn->width;
        rect.h = btn->height;

        r2d = btn->r2d;
        joy_r2d_set_color(r2d, btn->display_color.r, btn->display_color.g, btn->display_color.b, btn->display_color.a);
        joy_math_shape_draw_rectangle(r2d, "fill", rect);
        joy_r2d_set_color(r2d, 0, 0, 0, 255);
        joy_math_shape_draw_rectangle(r2d, "line", rect);
        /* 加粗边框：内缩 1 逻辑像素再描一圈，形成约 2px 黑框（窗口缩小时仍可见） */
        inner = rect;
        inner.x += 1;
        inner.y += 1;
        inner.w -= 2;
        inner.h -= 2;
        if (inner.w > 0 && inner.h > 0) {
                joy_math_shape_draw_rectangle(r2d, "line", inner);
        }

        if (btn->is_pressed && btn->pressed_image) {
                joy_ui_image_draw1(btn->pressed_image, NULL, &rect);
        }
        else if (btn->image) {
                joy_ui_image_draw1(btn->image, NULL, &rect);
        }

        if (btn->text) {
                float widht = joy_ui_text_get_width(btn->text);
                float height = joy_ui_text_get_height(btn->text);
                dest_rect.x = rect.x + (rect.w - widht) / 2.0f + btn->text_pos.x;
                dest_rect.y = rect.y + (rect.h - height) / 2.0f + btn->text_pos.y;
                joy_r2d_jtext_print(r2d, btn->text, dest_rect.x, dest_rect.y);
        }
}

/* 事件处理已移至 Lua 层，见 joy.lua 中的 joy.mousepressed/mousereleased/mousemoved */

joy_combobox_p joy_ui_combobox_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh)
{
        joy_rect_t rect = jui_frect(r2d, fx, fy, fw, fh);
        joy_combobox_p combobox;
        combobox = (joy_combobox_p)malloc(sizeof(joy_combobox_t));
        combobox->r2d = r2d;
        combobox->text = NULL;
        combobox->main_rect = rect;
        combobox->dropdown_rect.x = rect.x;
        combobox->dropdown_rect.y = rect.y + rect.h;
        combobox->dropdown_rect.w = rect.w;
        combobox->dropdown_rect.h = 0;
        // item_height 原为 480x320 设计稿下的固定 30px，不随逻辑分辨率换算 ——
        // 高分辨率下会出现"文字比列表项还高、溢出到列表背景外"，这里按分辨率缩放。
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        const float s = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;
        combobox->ui_scale = s;
        combobox->item_count = 0;
        combobox->item_height = (int)(30.0f * s);
        combobox->selected_index = -1;
        combobox->is_open = false;
        combobox->bg_color = (joy_color_t){ 240, 240, 240, 255 };
        combobox->text_color = (joy_color_t){ 0, 0, 0, 255 };
        combobox->hover_color = (joy_color_t){ 150, 150, 250, 255 };
        return combobox;
}

void joy_ui_combobox_destroy(joy_combobox_p combobox)
{
        if (combobox) {
                for (int i = 0; i < combobox->item_count; i++)
                        if (combobox->items[i].text) joy_ui_text_destroy(combobox->items[i].text);
                free(combobox);
        }
}

void
joy_ui_combobox_set_text(joy_combobox_p combobox, joy_font_p font,
        const int* codepoints, int len, joy_color_t color)
{
        if (combobox->text) {
                joy_ui_text_destroy(combobox->text);
        }
        combobox->text = joy_ui_text_create(combobox->r2d, font, codepoints, len, color);
}

void joy_ui_combobox_add_item(joy_combobox_p combobox, joy_font_p font, const char* str, int len, joy_color_t color)
{
        int capacity_codepoints = len * 4;
        int* codepoints = (int*)SDL_malloc(sizeof(int) * capacity_codepoints);
        int num_codepoints = joy_core_utf8_decode_all(str, len, codepoints, capacity_codepoints);
        joy_ui_combobox_add_itemx(combobox, font, codepoints, num_codepoints, color);
        SDL_free(codepoints);
}

void joy_ui_combobox_add_itemx(joy_combobox_p combobox, joy_font_p font, const int* codepoints, int len, joy_color_t color)
{
        joy_combobox_item_p item;
        joy_r2d_p r2d;
        if (combobox->item_count > 20) {
                return;
        }
        r2d = combobox->r2d;
        item = combobox->items + combobox->item_count;
        item->text = joy_ui_text_create(r2d, font, codepoints, len, color);
        item->is_selected = false;
        combobox->item_count++;
        combobox->dropdown_rect.h = combobox->item_count * combobox->item_height;
}

bool joy_ui_combobox_is_pressed(joy_combobox_p combobox, float x, float y)
{
        if (!combobox) return false;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(combobox->r2d, x, y);
        joy_rectangle_t rect;
        rect.x = combobox->main_rect.x;
        rect.y = combobox->main_rect.y;
        rect.width = combobox->main_rect.w;
        rect.height = combobox->main_rect.h;
        return joy_math_collision2d_check_point_rectangle(logic, rect);
}

bool joy_ui_combobox_is_open(joy_combobox_p combobox)
{
        return combobox ? combobox->is_open : false;
}

void joy_ui_combobox_set_open(joy_combobox_p combobox, bool v)
{
        if (combobox) combobox->is_open = v;
}

int joy_ui_combobox_hit_item(joy_combobox_p combobox, float x, float y)
{
        if (!combobox || !combobox->is_open) return -1;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(combobox->r2d, x, y);
        for (int i = 0; i < combobox->item_count; i++) {
                joy_rectangle_t item_rect = {
                    combobox->dropdown_rect.x,
                    combobox->dropdown_rect.y + i * combobox->item_height,
                    combobox->dropdown_rect.w,
                    combobox->item_height
                };
                if (joy_math_collision2d_check_point_rectangle(logic, item_rect)) return i;
        }
        return -1;
}

void joy_ui_combobox_set_selected(joy_combobox_p combobox, int index)
{
        if (combobox) combobox->selected_index = index;
}

int joy_ui_combobox_get_selected(joy_combobox_p combobox)
{
        return combobox ? combobox->selected_index : -1;
}

void joy_ui_combobox_draw(joy_combobox_p combobox)
{
        if (!combobox) return;
        joy_r2d_p r2d;
        r2d = combobox->r2d;
        joy_r2d_set_color(r2d, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
        joy_math_shape_draw_rectangle(r2d, "fill", combobox->main_rect);


        joy_r2d_set_color(r2d, 0, 0, 0, 255);
        joy_math_shape_draw_rectangle(r2d, "line", combobox->main_rect);

        const float s = (combobox->ui_scale > 0.0f) ? combobox->ui_scale : 1.0f;

        // 渲染当前选中的文本（左边距按分辨率换算，垂直居中）
        if (combobox->selected_index >= 0 && combobox->items[combobox->selected_index].text) {
                // 注：jtext 的内部结构（real_height）在 jui.c 不可见，无法精确垂直居中，
                // 这里用按分辨率换算的顶部偏移（设计稿 2px）。
                joy_text_p t = combobox->items[combobox->selected_index].text;
                joy_r2d_jtext_print(r2d, t, combobox->main_rect.x + 5.0f * s,
                            combobox->main_rect.y + 2.0f * s);
        }


        // 如果下拉框是打开的, 渲染选择列表
        if (combobox->is_open) {
                // 渲染下拉箭头
                joy_color_t color = { 0, 0, 0, 255 };
                joy_r2d_set_color(r2d, 0, 0, 0, 255);
                // 箭头各偏移是设计稿像素（25/10/5/15/20），按分辨率换算并垂直居中
                float ax = combobox->main_rect.x + combobox->main_rect.w;
                float ay = combobox->main_rect.y;
                float ah = 10.0f * s;
                float ao = (combobox->main_rect.h - ah) * 0.5f;
                if (ao < 0.0f) ao = 0.0f;
                joy_vec2_t arrow_points[3] = {
                    {ax - 25.0f * s, ay + ao},
                    {ax - 5.0f * s,  ay + ao},
                    {ax - 15.0f * s, ay + ao + ah}
                };
                joy_math_shape_draw_polygon(r2d, "line", arrow_points, 3, color);


                joy_r2d_set_color(r2d, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
                joy_math_shape_draw_rectangle(r2d, "fill", combobox->dropdown_rect);

                joy_r2d_set_color(r2d, 0, 0, 0, 255);
                joy_math_shape_draw_rectangle(r2d, "line", combobox->dropdown_rect);

                joy_combobox_item_p item;

                for (int i = 0; i < combobox->item_count; i++) {
                        joy_rect_t item_rect = {
                            combobox->dropdown_rect.x,
                            combobox->dropdown_rect.y + i * combobox->item_height,
                            combobox->dropdown_rect.w,
                            combobox->item_height
                        };

                        // 如果是当前选中的项, 使用高亮颜色
                        if (i == combobox->selected_index) {
                                joy_r2d_set_color(r2d, combobox->hover_color.r, combobox->hover_color.g, combobox->hover_color.b, combobox->hover_color.a);
                                joy_math_shape_draw_rectangle(r2d, "line", item_rect);
                        }

                        joy_r2d_set_color(r2d, 0, 0, 0, 255);
                        joy_math_shape_draw_rectangle(r2d, "line", item_rect);

                        item = combobox->items + i;
                        if (item->text) {
                                // 顶部偏移按分辨率换算（设计稿 2px）：
                                // item_height 已同步放大，文字不会溢出到下一项
                                joy_r2d_jtext_print(r2d, item->text, item_rect.x + 5.0f * s,
                                            item_rect.y + 2.0f * s);
                        }
                }
        }
        else {
                // 渲染下拉箭头
                joy_color_t color = { 0, 0, 0, 255 };
                float ax = combobox->main_rect.x + combobox->main_rect.w;
                float ay = combobox->main_rect.y;
                float ah = 10.0f * s;
                float ao = (combobox->main_rect.h - ah) * 0.5f;
                if (ao < 0.0f) ao = 0.0f;
                joy_vec2_t arrow_points[3] = {
                    {ax - 15.0f * s, ay + ao},
                    {ax - 25.0f * s, ay + ao + ah},
                    {ax - 5.0f * s,  ay + ao + ah}
                };
                joy_math_shape_draw_polygon(r2d, "line", arrow_points, 3, color);
        }
}


/* ===== 可输入下拉列表框 (combobox_input) ===== */

struct joy_combobox_input {
        joy_r2d_p r2d;
        joy_rect_t rect;               // 整体控件区域
        joy_color_t bg_color;          // 背景色
        joy_color_t border_color;      // 边框色
        joy_color_t text_color;        // 文字色
        joy_color_t cursor_color;      // 光标色
        joy_color_t sel_color;         // 选中背景色
        joy_color_t focus_border_color; // 聚焦时边框色
        joy_font_p font;

        // 文本输入
        char* buffer;
        int buf_len;
        int text_len;
        int cursor_pos;
        int sel_start, sel_end;
        int scroll_offset;
        bool focused;
        uint64_t cursor_timer;
        bool cursor_visible;

        // 下拉列表
        char items[20][JOY_MAX_IP];
        int item_count;
        int selected_index;
        bool is_open;
        int item_height;

        // 箭头按钮区域（右侧）
        joy_rect_t arrow_rect;
        bool arrow_hovered;

        // 回调
        void (*on_change)(struct joy_combobox_input* ci, void* userdata);
        void* cb_userdata;

        // 弹层节点（展开时自动挂载，关闭时自动卸载）
        joy_scene_node_p popup_node;
};

joy_combobox_input_p joy_ui_combobox_input_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, joy_font_p font, int buf_len)
{
        joy_rect_t rect = jui_frect(r2d, fx, fy, fw, fh);
        joy_combobox_input_p ci = (joy_combobox_input_p)calloc(1, sizeof(joy_combobox_input_t));
        if (!ci) return NULL;
        ci->r2d = r2d;
        ci->rect = rect;
        ci->font = font;
        ci->buf_len = buf_len > 0 ? buf_len : 256;
        ci->buffer = (char*)malloc(ci->buf_len);
        ci->buffer[0] = '\0';
        ci->text_len = 0;
        ci->cursor_pos = 0;
        ci->sel_start = -1;
        ci->sel_end = -1;
        ci->scroll_offset = 0;
        ci->focused = false;
        ci->cursor_timer = 0;
        ci->cursor_visible = false;
        ci->item_count = 0;
        ci->selected_index = -1;
        ci->is_open = false;
        /* item_height / 箭头宽度原本是 480x320 设计稿下的固定像素（24 / 20），
         * 逻辑分辨率提高后不跟着放大 —— 表现为"字号比列表项还高"、文字溢出到
         * 列表背景外面（与下方控件重叠看不清）。这里按逻辑分辨率换算。 */
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        const float s = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;

        ci->item_height = (int)(24.0f * s);
        ci->arrow_hovered = false;
        // 箭头按钮在右侧（设计稿 20px）
        ci->arrow_rect.w = 20.0f * s;
        ci->arrow_rect.x = rect.x + rect.w - ci->arrow_rect.w;
        ci->arrow_rect.y = rect.y;
        ci->arrow_rect.h = rect.h;
        // 配色
        ci->bg_color = (joy_color_t){ 255, 255, 255, 255 };
        ci->border_color = (joy_color_t){ 80, 80, 80, 255 };
        ci->text_color = (joy_color_t){ 0, 0, 0, 255 };
        ci->cursor_color = (joy_color_t){ 0, 0, 0, 255 };
        ci->sel_color = (joy_color_t){ 100, 150, 255, 100 };
        ci->focus_border_color = (joy_color_t){ 50, 100, 200, 255 };
        return ci;
}

void joy_ui_combobox_input_destroy(joy_combobox_input_p ci)
{
        if (ci) {
                free(ci->buffer);
                free(ci);
        }
}

void joy_ui_combobox_input_add_item(joy_combobox_input_p ci, const char* str, int len)
{
        if (!ci || ci->item_count >= 20) return;
        if (len <= 0) len = (int)strlen(str);
        if (len >= JOY_MAX_IP) len = JOY_MAX_IP - 1;
        memcpy(ci->items[ci->item_count], str, len);
        ci->items[ci->item_count][len] = '\0';
        ci->item_count++;
}

void joy_ui_combobox_input_clear_items(joy_combobox_input_p ci)
{
        if (ci) ci->item_count = 0;
}

const char* joy_ui_combobox_input_get_text(joy_combobox_input_p ci)
{
        return ci ? ci->buffer : "";
}

void joy_ui_combobox_input_set_text(joy_combobox_input_p ci, const char* str)
{
        if (!ci) return;
        int len = (int)strlen(str);
        if (len >= ci->buf_len) len = ci->buf_len - 1;
        memcpy(ci->buffer, str, len);
        ci->buffer[len] = '\0';
        ci->text_len = len;
        ci->cursor_pos = len;
        ci->scroll_offset = 0;
}

static void ci_inputbox_focus(joy_combobox_input_p ci)
{
        if (!ci || ci->focused) return;
        ci->focused = true;
        ci->cursor_timer = joy_core_get_ticks();
        ci->cursor_visible = true;
        SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)joy_r2d_unwrap(ci->r2d));
        if (win) SDL_StartTextInput(win);
}

static void ci_inputbox_defocus(joy_combobox_input_p ci)
{
        if (!ci || !ci->focused) return;
        ci->focused = false;
        ci->sel_start = -1;
        ci->sel_end = -1;
        SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)joy_r2d_unwrap(ci->r2d));
        if (win) SDL_StopTextInput(win);
}

static int ci_cursor_x(joy_combobox_input_p ci)
{
        if (ci->cursor_pos <= 0 || !ci->font) return 0;
        joy_color_t c = { 0,0,0,255 };
        joy_text_p tex = joy_ui_text_createx(ci->r2d, ci->font, ci->buffer, ci->cursor_pos, c);
        if (!tex) return 0;
        int w = joy_ui_text_get_width(tex);
        joy_ui_text_destroy(tex);
        return w;
}

static int ci_cursor_x_at(joy_combobox_input_p ci, int pos)
{
        if (pos <= 0 || !ci->font) return 0;
        joy_color_t c = { 0,0,0,255 };
        joy_text_p tex = joy_ui_text_createx(ci->r2d, ci->font, ci->buffer, pos, c);
        if (!tex) return 0;
        int w = joy_ui_text_get_width(tex);
        joy_ui_text_destroy(tex);
        return w;
}

static int ci_pos_from_x(joy_combobox_input_p ci, float local_x)
{
        if (!ci || ci->text_len <= 0 || local_x <= 0) return 0;
        int total_w = ci_cursor_x_at(ci, ci->text_len);
        if (local_x >= total_w) return ci->text_len;
        for (int i = 0; i < ci->text_len; ) {
                int next = i + 1;
                while (next < ci->text_len && ((unsigned char)ci->buffer[next] & 0xC0) == 0x80)
                        next++;
                int w = ci_cursor_x_at(ci, next);
                if (w >= local_x) {
                        int prev_w = ci_cursor_x_at(ci, i);
                        return (local_x - prev_w < w - local_x) ? i : next;
                }
                i = next;
        }
        return ci->text_len;
}

static void ci_scroll_to_cursor(joy_combobox_input_p ci)
{
        if (!ci) return;
        int cx = ci_cursor_x(ci);
        // 文字区宽度 = 总宽 - 箭头按钮区（箭头宽度已按分辨率缩放，不再写死 24）
        float text_area_w = ci->rect.w - ci->arrow_rect.w - 4.0f;
        if (cx - ci->scroll_offset < 0)
                ci->scroll_offset = cx;
        else if (cx - ci->scroll_offset > text_area_w)
                ci->scroll_offset = cx - (int)text_area_w;
}

static void ci_insert(joy_combobox_input_p ci, const char* text, int len)
{
        if (!ci || !text || len <= 0) return;
        if (ci->text_len + len >= ci->buf_len) return;
        memmove(ci->buffer + ci->cursor_pos + len, ci->buffer + ci->cursor_pos,
                ci->text_len - ci->cursor_pos);
        memcpy(ci->buffer + ci->cursor_pos, text, len);
        ci->cursor_pos += len;
        ci->text_len += len;
        ci->buffer[ci->text_len] = '\0';
        ci_scroll_to_cursor(ci);
        if (ci->on_change) ci->on_change(ci, ci->cb_userdata);
}

static void ci_backspace(joy_combobox_input_p ci)
{
        if (ci->cursor_pos <= 0) return;
        int pos = ci->cursor_pos - 1;
        while (pos > 0 && ((unsigned char)ci->buffer[pos] & 0xC0) == 0x80)
                pos--;
        int ch_len = ci->cursor_pos - pos;
        memmove(ci->buffer + pos, ci->buffer + ci->cursor_pos,
                ci->text_len - ci->cursor_pos);
        ci->text_len -= ch_len;
        ci->cursor_pos = pos;
        ci->buffer[ci->text_len] = '\0';
        ci_scroll_to_cursor(ci);
        if (ci->on_change) ci->on_change(ci, ci->cb_userdata);
}

void joy_ui_combobox_input_draw(joy_combobox_input_p ci)
{
        if (!ci) return;
        joy_r2d_p r = ci->r2d;
        joy_rect_t rect = ci->rect;

        // 设计稿像素（文字左边距 4、光标边界、分隔线等）按逻辑分辨率换算，
        // 理由同 item_height：不换算在 960x640 下会偏小/偏移。
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r, &lw, &lh, NULL);
        const float s = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;

        // 背景
        joy_r2d_set_color(r, ci->bg_color.r, ci->bg_color.g, ci->bg_color.b, ci->bg_color.a);
        joy_math_shape_draw_rectangle(r, "fill", rect);

        // 边框（聚焦时高亮）
        joy_color_t border = ci->focused ? ci->focus_border_color : ci->border_color;
        joy_r2d_set_color(r, border.r, border.g, border.b, border.a);
        joy_math_shape_draw_rectangle(r, "line", rect);

        // 裁剪区域（文字区域，排除箭头按钮）
        SDL_Renderer* sdl_r = (SDL_Renderer*)joy_r2d_unwrap(r);
        // 裁剪区排除箭头按钮（宽度随分辨率换算，原来写死 24）
        SDL_Rect clip_rect = { (int)rect.x + 2, (int)rect.y,
                (int)(rect.w - ci->arrow_rect.w - 4.0f), (int)rect.h };
        bool clip_was_enabled = SDL_RenderClipEnabled(sdl_r);
        SDL_Rect old_clip;
        if (clip_was_enabled) SDL_GetRenderClipRect(sdl_r, &old_clip);
        SDL_SetRenderClipRect(sdl_r, &clip_rect);

        // 文字
        if (ci->text_len > 0 && ci->font) {
                float text_x = rect.x + 4.0f * s;
                float text_y = rect.y + (rect.h - (float)joy_r2d_font_get_size(ci->font)) / 2.0f;
                float ox = text_x - (float)ci->scroll_offset;
                joy_r2d_jtext_printx(r, ci->font, ci->buffer, ci->text_len, ci->text_color, ox, text_y);
        }

        // 恢复裁剪
        if (clip_was_enabled)
                SDL_SetRenderClipRect(sdl_r, &old_clip);
        else
                SDL_SetRenderClipRect(sdl_r, NULL);

        // 光标
        if (ci->focused) {
                uint64_t now = joy_core_get_ticks();
                if (now - ci->cursor_timer >= 530) {
                        ci->cursor_visible = !ci->cursor_visible;
                        ci->cursor_timer = now;
                }
                if (ci->cursor_visible) {
                        int cx = ci_cursor_x(ci) - ci->scroll_offset + (int)rect.x + (int)(4.0f * s);
                        // 光标右边界 = 文字区右缘（总宽 - 箭头按钮区），原来写死 22
                        if (cx >= rect.x + 2 && cx <= rect.x + rect.w - (int)(ci->arrow_rect.w + 4.0f)) {
                                float cy = rect.y + 3.0f * s;
                                float ch = rect.h - 6.0f * s;
                                joy_r2d_set_color(r, ci->cursor_color.r, ci->cursor_color.g, ci->cursor_color.b, ci->cursor_color.a);
                                joy_math_shape_draw_line(r, (float)cx + 0.5f, cy, (float)cx + 0.5f, cy + ch);
                        }
                }
        }

        /* 箭头按钮：展开时朝上、收起时朝下。
         * 三角原本是设计稿下的固定像素（半宽 5、半高 3），逻辑分辨率提高后不换算
         * —— 960x640 下只有 10x6 px，小到几乎看不见（像是"没有展开/收起标识"）。
         * 这里按逻辑分辨率缩放（s 已在函数开头算好），并跟随箭头区大小。 */
        float arrow_cx = ci->arrow_rect.x + ci->arrow_rect.w / 2.0f;
        float arrow_cy = ci->arrow_rect.y + ci->arrow_rect.h / 2.0f;
        float aw = 5.0f * s, ah = 3.0f * s;          // 半宽 / 半高（设计稿 5 / 3）
        // 用箭头区实际高度再兜一层，避免控件本身很高时三角仍偏小
        float min_h = ci->arrow_rect.h * 0.18f;
        if (ah < min_h) { ah = min_h; aw = ah * (5.0f / 3.0f); }

        if (ci->is_open) {
                // 上箭头
                joy_vec2_t pts[3] = {
                    {arrow_cx - aw, arrow_cy + ah},
                    {arrow_cx + aw, arrow_cy + ah},
                    {arrow_cx,      arrow_cy - ah}
                };
                joy_math_shape_draw_polygon(r, "fill", pts, 3, ci->text_color);
        } else {
                // 下箭头
                joy_vec2_t pts[3] = {
                    {arrow_cx - aw, arrow_cy - ah},
                    {arrow_cx + aw, arrow_cy - ah},
                    {arrow_cx,      arrow_cy + ah}
                };
                joy_math_shape_draw_polygon(r, "fill", pts, 3, ci->text_color);
        }

        // 竖线分隔箭头区域
        joy_r2d_set_color(r, ci->border_color.r, ci->border_color.g, ci->border_color.b, ci->border_color.a);
        joy_math_shape_draw_line(r, ci->arrow_rect.x, rect.y + 2.0f * s, ci->arrow_rect.x, rect.y + rect.h - 2.0f * s);
}

void joy_ui_combobox_input_draw_dropdown(joy_combobox_input_p ci)
{
        if (!ci || !ci->is_open || ci->item_count <= 0) return;
        joy_r2d_p r = ci->r2d;
        joy_rect_t rect = ci->rect;
        /* 设计稿像素（文字左边距等）按逻辑分辨率换算，理由同 create 中的 item_height */
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r, &lw, &lh, NULL);
        const float s = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;
        joy_rect_t drop_rect = {
            rect.x, rect.y + rect.h,
            rect.w, (float)(ci->item_count * ci->item_height)
        };
        // 背景
        joy_color_t drop_bg = { 255, 255, 255, 255 };
        joy_r2d_set_color(r, drop_bg.r, drop_bg.g, drop_bg.b, drop_bg.a);
        joy_math_shape_draw_rectangle(r, "fill", drop_rect);
        // 边框
        joy_r2d_set_color(r, 0, 0, 0, 255);
        joy_math_shape_draw_rectangle(r, "line", drop_rect);

        for (int i = 0; i < ci->item_count; i++) {
                joy_rect_t item_rect = {
                    drop_rect.x, drop_rect.y + i * ci->item_height,
                    drop_rect.w, (float)ci->item_height
                };
                // 选中项高亮
                if (i == ci->selected_index) {
                        joy_color_t hl = { 200, 220, 255, 255 };
                        joy_r2d_set_color(r, hl.r, hl.g, hl.b, hl.a);
                        joy_math_shape_draw_rectangle(r, "fill", item_rect);
                }
                // 分隔线
                joy_r2d_set_color(r, 220, 220, 220, 255);
                joy_math_shape_draw_line(r, item_rect.x, item_rect.y + item_rect.h,
                        item_rect.x + item_rect.w, item_rect.y + item_rect.h);
                // 文字：垂直居中 + 左边距按分辨率换算
                //（原来是固定 +4 / +2，字号变大后会顶出列表项、溢出到列表外）
                float th = (float)joy_r2d_font_get_size(ci->font);
                float ty = item_rect.y + (item_rect.h - th) * 0.5f;
                if (ty < item_rect.y) ty = item_rect.y;
                joy_r2d_jtext_printx(r, ci->font, ci->items[i], (int)strlen(ci->items[i]),
                        ci->text_color, item_rect.x + 4.0f * s, ty);
        }
}

bool joy_ui_combobox_input_is_open(joy_combobox_input_p ci)
{
        return ci ? ci->is_open : false;
}

/* 下面两个内部函数在后面才定义，这里提前声明以便 joy_ui_combobox_input_set_open 使用 */
static void ci_open_dropdown(joy_combobox_input_p ci);
static void ci_close_dropdown(joy_combobox_input_p ci);

/* 主动展开/收起下拉列表：调用方用 hit_item 选中某项后需要收起列表，
 * 原来没有这个接口（内部 ci_open_dropdown / ci_close_dropdown 是 static）。 */
void joy_ui_combobox_input_set_open(joy_combobox_input_p ci, bool v)
{
        if (!ci) return;
        if (v) ci_open_dropdown(ci);
        else   ci_close_dropdown(ci);
}

void joy_ui_combobox_input_set_popup_node(joy_combobox_input_p ci, joy_scene_node_p node)
{
        if (ci) ci->popup_node = node;
}

/* 弹层节点渲染回调：绘制下拉列表 */
static void ci_popup_render(joy_scene_node_p node, const void* arg)
{
        (void)arg;
        joy_combobox_input_p ci = (joy_combobox_input_p)joy_scene_node_get_userdata(node);
        if (ci) joy_ui_combobox_input_draw_dropdown(ci);
}

/* 展开下拉列表时挂载到弹层节点 */
static void ci_open_dropdown(joy_combobox_input_p ci)
{
        if (!ci || ci->is_open) return;
        ci->is_open = true;
        if (ci->popup_node) {
                joy_scene_node_set_userdata(ci->popup_node, ci);
                joy_scene_node_set_render_callback(ci->popup_node, ci_popup_render);
        }
}

/* 关闭下拉列表时从弹层节点卸载 */
static void ci_close_dropdown(joy_combobox_input_p ci)
{
        if (!ci || !ci->is_open) return;
        ci->is_open = false;
        if (ci->popup_node) {
                joy_scene_node_set_render_callback(ci->popup_node, NULL);
                joy_scene_node_set_userdata(ci->popup_node, NULL);
        }
}

void joy_ui_combobox_input_process_event(joy_combobox_input_p ci, const void* ev)
{
        if (!ci || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_vec2_t logic = joy_r2d_coordinates_from_window(ci->r2d, e->motion.x, e->motion.y);
                ci->arrow_hovered = (logic.x >= ci->arrow_rect.x && logic.x <= ci->arrow_rect.x + ci->arrow_rect.w
                        && logic.y >= ci->arrow_rect.y && logic.y <= ci->arrow_rect.y + ci->arrow_rect.h);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_vec2_t logic = joy_r2d_coordinates_from_window(ci->r2d, e->button.x, e->button.y);
                bool in_rect = (logic.x >= ci->rect.x && logic.x <= ci->rect.x + ci->rect.w
                        && logic.y >= ci->rect.y && logic.y <= ci->rect.y + ci->rect.h);
                bool in_arrow = (logic.x >= ci->arrow_rect.x && logic.x <= ci->arrow_rect.x + ci->arrow_rect.w
                        && logic.y >= ci->arrow_rect.y && logic.y <= ci->arrow_rect.y + ci->arrow_rect.h);

                if (in_arrow) {
                        // 点击箭头：切换下拉列表
                        if (ci->is_open)
                                ci_close_dropdown(ci);
                        else
                                ci_open_dropdown(ci);
                        ci_inputbox_focus(ci);
                }
                else if (in_rect) {
                        // 点击文字区域：聚焦并定位光标
                        ci_inputbox_focus(ci);
                        float local_x = logic.x - ci->rect.x - 2 + ci->scroll_offset;
                        ci->cursor_pos = ci_pos_from_x(ci, local_x);
                        ci_scroll_to_cursor(ci);
                        ci_close_dropdown(ci);
                }
                else {
                        // 点击外部：关闭列表并失焦
                        ci_close_dropdown(ci);
                        ci_inputbox_defocus(ci);
                }
        }
        else if (e->type == SDL_EVENT_TEXT_INPUT) {
                if (ci->focused) {
                        ci_insert(ci, e->text.text, (int)strlen(e->text.text));
                }
        }
        else if (e->type == SDL_EVENT_KEY_DOWN) {
                if (!ci->focused) return;
                switch (e->key.scancode) {
                case SDL_SCANCODE_BACKSPACE: ci_backspace(ci); break;
                case SDL_SCANCODE_LEFT:
                        if (ci->cursor_pos > 0) ci->cursor_pos--;
                        ci_scroll_to_cursor(ci);
                        break;
                case SDL_SCANCODE_RIGHT:
                        if (ci->cursor_pos < ci->text_len) ci->cursor_pos++;
                        ci_scroll_to_cursor(ci);
                        break;
                case SDL_SCANCODE_DOWN:
                        if (ci->is_open && ci->selected_index < ci->item_count - 1)
                                ci->selected_index++;
                        break;
                case SDL_SCANCODE_UP:
                        if (ci->is_open && ci->selected_index > 0)
                                ci->selected_index--;
                        break;
                case SDL_SCANCODE_RETURN:
                case SDL_SCANCODE_KP_ENTER:
                        if (ci->is_open && ci->selected_index >= 0 && ci->selected_index < ci->item_count) {
                                joy_ui_combobox_input_set_text(ci, ci->items[ci->selected_index]);
                                ci_close_dropdown(ci);
                        }
                        break;
                default: break;
                }
        }
}

int joy_ui_combobox_input_hit_item(joy_combobox_input_p ci, float x, float y)
{
        if (!ci || !ci->is_open) return -1;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(ci->r2d, x, y);
        for (int i = 0; i < ci->item_count; i++) {
                joy_rect_t item_rect = {
                    ci->rect.x, ci->rect.y + ci->rect.h + i * ci->item_height,
                    ci->rect.w, (float)ci->item_height
                };
                if (logic.x >= item_rect.x && logic.x <= item_rect.x + item_rect.w
                        && logic.y >= item_rect.y && logic.y <= item_rect.y + item_rect.h)
                        return i;
        }
        return -1;
}


joy_datagrid_p
joy_ui_datagrid_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, int col_count, bool has_header, void* font)
{
        joy_rect_t rect = jui_frect(r2d, fx, fy, fw, fh);
        joy_datagrid_p grid;
        grid = (joy_datagrid_p)malloc(sizeof(joy_datagrid_t));
        if (!grid) return NULL;
        memset(grid, 0, sizeof(joy_datagrid_t));

        grid->r2d = r2d;
        grid->x = rect.x;
        grid->y = rect.y;
        grid->width = rect.w;
        grid->height = rect.h;
        grid->row_height = 30;
        grid->col_count = col_count;
        grid->row_count = 0;
        grid->visible_rows = (rect.h - grid->row_height) / grid->row_height;
        grid->scroll_offset = 0;
        grid->selected_row = -1;
        grid->has_header = has_header;
        grid->font = (joy_font_p)font;
        kv_init(grid->data);
        grid->col_widths = (int*)malloc(col_count * sizeof(int));
        grid->bg_color = (joy_color_t){ 0xFF, 0xFF, 0xFF, 0xFF };
        grid->grid_color = (joy_color_t){ 0xCC, 0xCC, 0xCC, 0xFF };
        grid->text_color = (joy_color_t){ 0x00, 0x00, 0x00, 0xFF };
        grid->header_bg_color = (joy_color_t){ 0xEE, 0xEE, 0xEE, 0xFF };
        grid->selected_color = (joy_color_t){ 0x33, 0x99, 0xFF, 0xFF };
        memset(grid->headers, 0, 512 * sizeof(char));
        int default_width = grid->width / col_count;
        for (int i = 0; i < col_count; i++) {
                grid->col_widths[i] = default_width;
        }
        return grid;
}


void joy_ui_datagrid_setheaders(joy_datagrid_p grid, const char* headers)
{
        joy_font_p fontinfo;
        joy_color_t color;
        color = (joy_color_t){ 0x0, 0x0, 0x0, 0xFF };
        if (!grid->has_header) return;
        fontinfo = grid->font;
        for (int i = 0; i < grid->col_count; i++) {
                //grid->headers[i] = joy_ui_text_createx(grid->r2d, fontinfo, headers[i], SDL_strlen(headers[i]), color);
        }
}

void joy_ui_datagrid_setheader(joy_datagrid_p grid, int index, const char* header)
{
        joy_font_p fontinfo;
        joy_color_t color;
        if (!grid->has_header) return;
        if (index < grid->col_count) return;
        color = (joy_color_t){ 0x0, 0x0, 0x0, 0xFF };
        fontinfo = grid->font;
        if (grid->headers[index]) {
                /* 由外部管理 text 更新 */
        }
        else {
                /* 由外部设置 header text */
        }
}

void joy_ui_datagrid_addrow(joy_datagrid_p grid, const char** row_data)
{
        if (!grid) return;
        joy_dg_row_t row;
        kv_init(row);
        for (int i = 0; i < grid->col_count; i++) {
                kv_push(char*, row, row_data && row_data[i] ? joy_core_utils_strdup(row_data[i], NULL) : NULL);
        }
        kv_push(joy_dg_row_t, grid->data, row);
        grid->row_count = (int)kv_size(grid->data);
}

void joy_ui_datagrid_setcell(joy_datagrid_p grid, int row_index, int col_index, const char* data)
{
        if (!grid) return;
        if (row_index < (int)kv_size(grid->data) && col_index < grid->col_count) {
                joy_dg_row_t* row = &kv_A(grid->data, row_index);
                if (kv_A(*row, col_index)) free(kv_A(*row, col_index));
                kv_A(*row, col_index) = data ? joy_core_utils_strdup(data, NULL) : NULL;
        }
}

static void datagrid_cleardata(joy_datagrid_p grid)
{
        for (size_t i = 0; i < kv_size(grid->data); i++) {
                joy_dg_row_t* row = &kv_A(grid->data, i);
                for (size_t j = 0; j < kv_size(*row); j++)
                        if (kv_A(*row, j)) { free(kv_A(*row, j)); kv_A(*row, j) = NULL; }
                kv_destroy(*row);
        }
        kv_destroy(grid->data);
        grid->row_count = grid->has_header ? 1 : 0;
}


void joy_ui_datagrid_destroy(joy_datagrid_p grid)
{
        if (!grid) return;
        if (grid->has_header) {
                for (int i = 0; i < grid->col_count; i++)
                        if (grid->headers[i]) joy_ui_text_destroy(grid->headers[i]);
        }
        datagrid_cleardata(grid);
        free(grid->col_widths);
        free(grid);
}

void joy_ui_datagrid_draw(joy_datagrid_p grid, joy_r2d_p r2d)
{
        // 计算可见行范围
        int start_row = grid->scroll_offset;
        int end_row = start_row + grid->visible_rows;
        if (end_row > grid->row_count) end_row = grid->row_count;

        // 绘制背景
        joy_color_t bg_color = grid->bg_color;
        joy_r2d_set_color(r2d, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        joy_rect_t bg_rect = { grid->x, grid->y, grid->width, grid->height };
        joy_math_shape_draw_rectangle(r2d, "fill", bg_rect);

        // 获取网格线和文字颜色
        joy_color_t grid_color = grid->grid_color;
        joy_color_t text_color = grid->text_color;
        joy_color_t header_bg_color = grid->header_bg_color;
        joy_color_t selected_color = grid->selected_color;

        // 绘制表头
        joy_r2d_set_color(r2d, header_bg_color.r, header_bg_color.g, header_bg_color.b, header_bg_color.a);
        joy_rect_t header_rect = { grid->x, grid->y, grid->width, grid->row_height };
        joy_math_shape_draw_rectangle(r2d, "fill", header_rect);


        {
                int x = grid->x;
                int y = grid->y;
                for (int col = 0; col < grid->col_count; col++) {
                        joy_text_p tex = grid->headers[col];
                        joy_r2d_jtext_print(r2d, tex, x, y);

                        // 绘制垂直分隔线
                        joy_r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
                        joy_math_shape_draw_line(r2d, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

                        x += grid->col_widths[col];
                }
        }

        // 绘制数据行
        for (int row = start_row; row < end_row; row++) {
                int y = grid->y + (row - start_row) * grid->row_height + grid->row_height;

                // 绘制选中行背景
                if (row == grid->selected_row) {
                        joy_r2d_set_color(r2d, selected_color.r, selected_color.g, selected_color.b, selected_color.a);
                        joy_rect_t selected_rect = { grid->x, y, grid->width, grid->row_height };
                        joy_math_shape_draw_rectangle(r2d, "fill", selected_rect);
                }
                else {
                        /* 非选中行默认无背景 */
                }

                // 绘制单元格内容和分隔线
                int x = grid->x;
                for (int col = 0; col < grid->col_count; col++) {
                        if ((size_t)row < kv_size(grid->data) && (size_t)col < kv_size(kv_A(grid->data, row))) {
                                char* text = kv_A(kv_A(grid->data, row), col);
                                if (text) {
                                        joy_color_t c = { 0,0,0,255 };
                                        joy_r2d_jtext_printx(r2d, grid->font, text, (int)strlen(text), c, (float)x + 2, (float)y);
                                }
                        }

                        // 绘制垂直分隔线
                        joy_r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
                        joy_math_shape_draw_line(r2d, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

                        x += grid->col_widths[col];
                }

                // 绘制水平分隔线
                joy_r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
                joy_math_shape_draw_line(r2d, grid->x, y + grid->row_height, grid->x + grid->width, y + grid->row_height);
        }

        // 绘制外边框
        joy_r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
        joy_math_shape_draw_rectangle(r2d, "line", bg_rect);
}

int joy_ui_datagrid_hit_row(joy_datagrid_p grid, float x, float y)
{
        if (!grid) return -1;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(grid->r2d, x, y);
        joy_rectangle_t rect = { (float)grid->x, (float)(grid->y + grid->row_height), (float)grid->width, (float)grid->height };
        if (!joy_math_collision2d_check_point_rectangle(logic, rect))
                return -1;
        int clicked_row = grid->scroll_offset + ((int)(logic.y - rect.y)) / grid->row_height;
        if (clicked_row < grid->row_count) return clicked_row;
        return -1;
}

void joy_ui_datagrid_scroll(joy_datagrid_p grid, int lines)
{
        if (!grid) return;
        grid->scroll_offset -= lines;
        int max_scroll = grid->row_count - grid->visible_rows;
        if (max_scroll < 0) max_scroll = 0;
        if (grid->scroll_offset < 0) grid->scroll_offset = 0;
        if (grid->scroll_offset > max_scroll) grid->scroll_offset = max_scroll;
}

void joy_ui_datagrid_set_selected_row(joy_datagrid_p grid, int row)
{
        if (grid) grid->selected_row = row;
}

joy_radio_p joy_ui_radio_create(float fx, float fy, float size_frac, const char* label, int group_id)
{
        int lw = 480, lh = 320;
        int x = (int)(fx * (float)lw), y = (int)(fy * (float)lh), size = (int)(size_frac * (float)lw);
        joy_radio_p radio;
        radio = (joy_radio_p)malloc(sizeof(joy_radio_t));
        if (!radio) return NULL;

        radio->x = x;
        radio->y = y;
        radio->size = size;
        // 记住比例：拿到 r2d 后按真实逻辑分辨率重算（见 radio_update_layout）
        radio->fx = fx;
        radio->fy = fy;
        radio->size_frac = size_frac;
        radio->r2d = NULL;
        radio->checked = false;
        radio->group_id = group_id;
        radio->hover = false;

        // 复制标签文本
        if (label) {
                radio->label = joy_core_utils_strdup(label, NULL);
        }
        else {
                radio->label = NULL;
        }

        // 设置点击区域(包含标签宽度)
        int label_width = label ? strlen(label) * 10 : 0; // 估算标签宽度
        radio->hit_area.x = x;
        radio->hit_area.y = y;
        radio->hit_area.w = size + 5 + label_width;
        radio->hit_area.h = size;

        return radio;
}

void joy_ui_radio_destroy(joy_radio_p radio)
{
        if (!radio) return;

        if (radio->label) {
                free(radio->label);
        }

        free(radio);
}


/* 按"当前"逻辑分辨率把比例换算成像素（位置/尺寸/命中区），同 checkbox_update_layout。
 * joy_ui_radio_create 拿不到 r2d（只能拿到比例），真正的换算推迟到这里。 */
static void radio_update_layout(joy_radio_p rb)
{
        if (!rb || !rb->r2d) return;

        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(rb->r2d, &lw, &lh, NULL);
        if (lw <= 0 || lh <= 0) return;

        const float scale = (float)lw / 480.0f;      // 相对设计稿 480x320 的缩放
        rb->x    = (int)(rb->fx * (float)lw);
        rb->y    = (int)(rb->fy * (float)lh);
        rb->size = (int)(rb->size_frac * (float)lw);

        int label_width = rb->label ? (int)(strlen(rb->label) * 10 * scale) : 0;
        rb->hit_area.x = rb->x;
        rb->hit_area.y = rb->y;
        rb->hit_area.w = rb->size + (int)(5.0f * scale) + label_width;
        rb->hit_area.h = rb->size;
}

void joy_radio_render(joy_radio_p radio, joy_r2d_p r2d)
{
        if (!radio) return;

        // 可能刚拿到 r2d：先按当前逻辑分辨率重算布局
        if (r2d) radio->r2d = r2d;
        radio_update_layout(radio);

        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        const float scale = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;

        float center_x = radio->x + radio->size / 2;
        float center_y = radio->y + radio->size / 2;
        float r1 = radio->size / 2;
        float r2 = r1 - 2.0f * scale;    // 描边/内圈厚度是设计稿像素，按分辨率换算
        float r3 = r2 - 4.0f * scale;

        // 使用足够大的分段数绘制平滑圆形
        const int segments = 99; // 增加分段数使圆形平滑
        radio->r1_color.r = 0;
        radio->r1_color.g = 0;
        radio->r1_color.b = 255;
        radio->r1_color.a = 255;

        radio->r2_color.r = 255;
        radio->r2_color.g = 255;
        radio->r2_color.b = 255;
        radio->r2_color.a = 255;

        radio->r3_color.r = 0;
        radio->r3_color.g = 0;
        radio->r3_color.b = 255;
        radio->r3_color.a = 255;


        /* 外圈 */
        joy_r2d_set_color(r2d, radio->r1_color.r, radio->r1_color.g, radio->r1_color.b, radio->r1_color.a);
        joy_math_shape_draw_circle(r2d, "fill", (joy_vec2_t) { center_x, center_y }, r1, 32);

        /* 内圈 */
        joy_r2d_set_color(r2d, radio->r2_color.r, radio->r2_color.g, radio->r2_color.b, radio->r2_color.a);
        joy_math_shape_draw_circle(r2d, "fill", (joy_vec2_t) { center_x, center_y }, r2, 32);

        /* 选中标记 */
        if (radio->checked) {
                joy_r2d_set_color(r2d, radio->r3_color.r, radio->r3_color.g, radio->r3_color.b, radio->r3_color.a);
                joy_math_shape_draw_circle(r2d, "fill", (joy_vec2_t) { center_x, center_y }, r3, 32);
        }

        // 渲染标签文本
        if (radio->label) {
                /* 标签文本 — 由外部传入 font 后使用 joy_r2d_jtext_printx 渲染 */
        }
}

bool joy_ui_radio_is_pressed(joy_radio_p radio, float x, float y)
{
        if (!radio)
                return false;
        joy_rectangle_t hit_area;
        joy_vec2_t pt = { x, y };

        hit_area.x = radio->hit_area.x;
        hit_area.y = radio->hit_area.y;
        hit_area.width = radio->hit_area.w;
        hit_area.height = radio->hit_area.h;

        if (joy_math_collision2d_check_point_rectangle(pt, hit_area)) {
                radio->checked = !radio->checked;
                return true;
        }
        return false;
}

void joy_ui_radio_set_hovered(joy_radio_p radio, bool v)
{
        if (radio) radio->hover = v;
}


joy_checkbox_p joy_ui_checkbox_create(float fx, float fy, float size_frac, const char* label)
{
        int lw = 480, lh = 320;
        int x = (int)(fx * (float)lw), y = (int)(fy * (float)lh), size = (int)(size_frac * (float)lw);
        joy_checkbox_p cb;
        cb = malloc(sizeof(joy_checkbox_t));
        if (!cb) return NULL;

        cb->x = x;
        cb->y = y;
        cb->size = size;
        // 记住比例：拿到 r2d 后会按真实逻辑分辨率重算（见 checkbox_update_layout）
        cb->fx = fx;
        cb->fy = fy;
        cb->size_frac = size_frac;
        cb->r2d = NULL;
        cb->state = JOY_CHECKBOX_UNCHECKED;
        cb->label = label ? joy_core_utils_strdup(label, NULL) : NULL;
        cb->hover = false;
        cb->enabled = true;

        // 设置默认颜色
        cb->box_color = (joy_color_t){ 0, 0, 0, 255 };        // 黑色边框
        cb->check_color = (joy_color_t){ 0, 120, 0, 255 };    // 绿色勾选
        cb->text_color = (joy_color_t){ 0, 0, 0, 255 };       // 黑色文本
        cb->hover_color = (joy_color_t){ 200, 200, 200, 255 };// 浅灰色悬停

        // 设置点击区域(包含标签)
        int label_width = 0;
        if (label) {
                // 估算标签宽度(实际渲染时需要精确计算)
                label_width = strlen(label) * 8;
        }
        cb->hit_area.x = x;
        cb->hit_area.y = y;
        cb->hit_area.w = size + 5 + label_width;
        cb->hit_area.h = size;
        return cb;
}


void joy_ui_checkbox_destroy(joy_checkbox_p checkbox)
{
        if (!checkbox) return;

        if (checkbox->label) {
                free(checkbox->label);
        }

        free(checkbox);
}

void joy_ui_checkbox_set_state(joy_checkbox_p checkbox, joy_checkbox_state_t state)
{
        if (!checkbox) return;
        checkbox->state = state;
}

joy_checkbox_state_t joy_ui_checkbox_get_state(joy_checkbox_p checkbox)
{
        return checkbox ? checkbox->state : JOY_CHECKBOX_UNCHECKED;
}

void joy_ui_checkbox_set_r2d(joy_checkbox_p checkbox, joy_r2d_p r2d)
{
        if (!checkbox) return;
        checkbox->r2d = r2d;
}

void joy_ui_checkbox_set_position(joy_checkbox_p checkbox, int x, int y)
{
        if (!checkbox) return;
        int dx = x - checkbox->x;
        int dy = y - checkbox->y;
        checkbox->x = x;
        checkbox->y = y;
        checkbox->hit_area.x += dx;
        checkbox->hit_area.y += dy;
}

void joy_ui_checkbox_set_text_color(joy_checkbox_p checkbox, joy_color_t color)
{
        if (!checkbox) return;
        checkbox->text_color = color;
}

void joy_ui_checkbox_toggle(joy_checkbox_p checkbox)
{
        if (!checkbox || !checkbox->enabled) return;

        if (checkbox->state == JOY_CHECKBOX_UNCHECKED) {
                checkbox->state = JOY_CHECKBOX_CHECKED;
        }
        else {
                checkbox->state = JOY_CHECKBOX_UNCHECKED;
        }
}


// 处理复选框点击事件
bool joy_ui_checkbox_is_pressed(joy_checkbox_p cb, float x, float y)
{
        if (!cb || !cb->enabled) return false;
        // 命中前先按当前逻辑分辨率重算命中区，否则分辨率改变后点击位置对不上
        checkbox_update_layout(cb);
        joy_vec2_t pt = { x, y };
        // 与 button/slider/inputbox 一致：窗口坐标转逻辑坐标，保证全屏下命中正确
        if (cb->r2d) pt = joy_r2d_coordinates_from_window(cb->r2d, x, y);
        joy_rectangle_t hit_area;
        hit_area.x = cb->hit_area.x;
        hit_area.y = cb->hit_area.y;
        hit_area.width = cb->hit_area.w;
        hit_area.height = cb->hit_area.h;
        if (joy_math_collision2d_check_point_rectangle(pt, hit_area)) {
                joy_ui_checkbox_toggle(cb);
                return true;
        }
        return false;
}

void joy_ui_checkbox_set_hovered(joy_checkbox_p cb, bool v)
{
        if (cb) cb->hover = v;
}

void joy_ui_checkbox_set_colors(joy_checkbox_p checkbox, joy_color_t box, joy_color_t check, joy_color_t hover)
{
        if (!checkbox) return;
        checkbox->box_color = box;
        checkbox->check_color = check;
        checkbox->hover_color = hover;
}

/* 按"当前"逻辑分辨率把比例换算成像素（位置/尺寸/命中区）。
 * 创建时只能拿到比例（没有 r2d），所以真正的换算推迟到这里。
 * 逻辑分辨率改变后，复选框仍保持原有的相对位置与大小。 */
static void checkbox_update_layout(joy_checkbox_p cb)
{
        if (!cb || !cb->r2d) return;

        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(cb->r2d, &lw, &lh, NULL);
        if (lw <= 0 || lh <= 0) return;

        const float scale = (float)lw / 480.0f;      // 相对设计稿 480x320 的缩放
        cb->x    = (int)(cb->fx * (float)lw);
        cb->y    = (int)(cb->fy * (float)lh);
        cb->size = (int)(cb->size_frac * (float)lw);

        int label_width = 0;
        if (cb->label) label_width = (int)(strlen(cb->label) * 8 * scale);
        cb->hit_area.x = cb->x;
        cb->hit_area.y = cb->y;
        cb->hit_area.w = cb->size + (int)(5.0f * scale) + label_width;
        cb->hit_area.h = cb->size;
}

void joy_ui_checkbox_draw(joy_checkbox_p checkbox, joy_r2d_p r2d, void* font)
{
        if (!checkbox) return;

        // 可能刚 set_r2d：先按当前逻辑分辨率重算布局
        if (r2d) checkbox->r2d = r2d;
        checkbox_update_layout(checkbox);

        float cx = (float)checkbox->x;
        float cy = (float)checkbox->y;
        float sz = (float)checkbox->size;

        uint8_t alpha = checkbox->enabled ? 255 : 100;

        // === 框体背景 ===
        joy_r2d_set_color(r2d, checkbox->box_color.r, checkbox->box_color.g, checkbox->box_color.b, alpha);
        joy_rect_t fill_rect = { cx, cy, sz, sz };
        joy_math_shape_draw_rectangle(r2d, "fill", fill_rect);

        // === 悬停时变色 ===
        if (checkbox->hover && checkbox->enabled) {
                joy_r2d_set_color(r2d, checkbox->hover_color.r, checkbox->hover_color.g, checkbox->hover_color.b, 255);
                joy_math_shape_draw_rectangle(r2d, "fill", fill_rect);
        }

        // === 边框 ===
        joy_r2d_set_color(r2d, 255, 255, 255, alpha);
        joy_math_shape_draw_rectangle(r2d, "line", fill_rect);

        // === 选中状态 ===
        if (checkbox->state == JOY_CHECKBOX_CHECKED && checkbox->enabled) {
                joy_r2d_set_color(r2d, checkbox->check_color.r, checkbox->check_color.g, checkbox->check_color.b, 255);
                float p = sz * 0.18f;
                float m = sz * 0.5f;
                float e = sz - p;
                float h1 = sz * 0.38f;
                float h2 = sz - p;
                joy_vec2_t check[3] = {
                    {cx + p,      cy + m},
                    {cx + h1,     cy + h2},
                    {cx + e,      cy + p}
                };
                // 加粗：画 4 条偏移线（-1.5, -0.5, 0.5, 1.5）
                for (float off = -1.5f; off <= 1.5f; off += 1.0f) {
                        joy_vec2_t thick[3] = {
                            {check[0].x + off * 0.4f, check[0].y + off * 0.4f},
                            {check[1].x + off * 0.4f, check[1].y + off * 0.4f},
                            {check[2].x + off * 0.4f, check[2].y + off * 0.4f}
                        };
                        joy_r2d_draw_lines(r2d, thick, 3);
                }
                joy_r2d_draw_lines(r2d, check, 3);
        }
        else if (checkbox->state == JOY_CHECKBOX_INDETERMINATE && checkbox->enabled) {
                // 不确定状态：白色短横线
                joy_r2d_set_color(r2d, 255, 255, 255, 255);
                float p = sz * 0.25f;
                joy_vec2_t dash[2] = {
                    {cx + p, cy + sz * 0.5f},
                    {cx + sz - p, cy + sz * 0.5f}
                };
                joy_math_shape_draw_line(r2d, dash[0].x, dash[0].y, dash[1].x, dash[1].y);
        }

        // === 禁用遮罩 ===
        if (!checkbox->enabled) {
                joy_r2d_set_color(r2d, 200, 200, 200, 120);
                joy_math_shape_draw_rectangle(r2d, "fill", fill_rect);
        }

        // === 标签文本 ===
        if (checkbox->label && font) {
                joy_font_p f = (joy_font_p)font;
                joy_color_t tc = checkbox->enabled ? checkbox->text_color : (joy_color_t){ 160, 160, 160, 255 };
                // 标签与框的间距 8 是设计稿像素，按分辨率换算
                int lw = 0, lh = 0;
                joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
                const float scale = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;
                joy_r2d_jtext_printx(r2d, f, checkbox->label, (int)strlen(checkbox->label), tc,
                        cx + sz + 8.0f * scale,
                        cy + (sz - (float)joy_r2d_font_get_size(f)) / 2);
        }
}




// 初始化摇杆
joy_joystick_p joy_input_joystick_create(joy_r2d_p r2d, float cx, float cy, float radius_frac)
{
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        float x = cx * (float)lw, y = cy * (float)lh, radius = radius_frac * (float)lw;
        joy_joystick_p joystick;
        joystick = (joy_joystick_p)malloc(sizeof(joy_joystick_t));

        joystick->r2d = r2d;
        joystick->center.x = x;
        joystick->center.y = y;
        joystick->radius = radius;
        joystick->handle_radius = radius * 0.4f;

        joystick->background.x = x - radius;
        joystick->background.y = y - radius;
        joystick->background.w = radius * 2;
        joystick->background.h = radius * 2;

        joystick->handle.x = x - joystick->handle_radius;
        joystick->handle.y = y - joystick->handle_radius;
        joystick->handle.w = joystick->handle_radius * 2;
        joystick->handle.h = joystick->handle_radius * 2;

        joystick->direction.x = 0.0f;
        joystick->direction.y = 0.0f;
        joystick->magnitude = 0.0f;
        joystick->deadzone = 0.1f;
        joystick->is_dragging = false;
        joystick->is_touch_active = false;
        joystick->touch_id = -1;
        joystick->base_image = NULL;
        joystick->thumb_image = NULL;
        joystick->base_default = NULL;
        joystick->thumb_default = NULL;
        for (int i = 0; i < 5; i++) { joystick->mode_base[i] = NULL; joystick->mode_thumb[i] = NULL; }
        joystick->dir_mode = JOYSTICK_DIR_OMNI;
        return joystick;
}

void joy_input_joystick_destroy(joy_joystick_p joystick)
{
        if (!joystick) return;
        // base_image/thumb_image 只是指向 default 或某 mode 纹理的指针，不单独释放，避免重复释放
        if (joystick->base_default) joy_ui_image_destroy(joystick->base_default);
        if (joystick->thumb_default) joy_ui_image_destroy(joystick->thumb_default);
        for (int i = 0; i < 5; i++) {
                if (joystick->mode_base[i]) joy_ui_image_destroy(joystick->mode_base[i]);
                if (joystick->mode_thumb[i]) joy_ui_image_destroy(joystick->mode_thumb[i]);
        }
        free(joystick);
}

// 设置摇杆默认纹理：底圈(base) 与摇杆头(thumb)。传空串 / NULL 则清除（退回纯绘制）。
// 设为默认集合，并立即作为当前显示（除非后续有模式专属纹理）。
void joy_input_joystick_set_textures(joy_joystick_p joystick, const char* base_path, const char* thumb_path)
{
        if (!joystick) return;
        if (joystick->base_default) { joy_ui_image_destroy(joystick->base_default); joystick->base_default = NULL; }
        if (joystick->thumb_default) { joy_ui_image_destroy(joystick->thumb_default); joystick->thumb_default = NULL; }
        if (base_path && base_path[0])
                joystick->base_default = joy_ui_image_create(joystick->r2d, base_path);
        if (thumb_path && thumb_path[0])
                joystick->thumb_default = joy_ui_image_create(joystick->r2d, thumb_path);
        // 立即作为当前显示（若当前模式无专属纹理，则显示默认）
        joystick->base_image = joystick->base_default;
        joystick->thumb_image = joystick->thumb_default;
}

// 为某个方向模式注册专属纹理；切换该模式时自动换上，未注册则回退默认纹理。
void joy_input_joystick_set_textures_for_mode(joy_joystick_p joystick, joystick_dir_mode mode, const char* base_path, const char* thumb_path)
{
        if (!joystick) return;
        if (mode < 0 || mode > JOYSTICK_DIR_HORIZONTAL) return;
        if (joystick->mode_base[(int)mode]) { joy_ui_image_destroy(joystick->mode_base[(int)mode]); joystick->mode_base[(int)mode] = NULL; }
        if (joystick->mode_thumb[(int)mode]) { joy_ui_image_destroy(joystick->mode_thumb[(int)mode]); joystick->mode_thumb[(int)mode] = NULL; }
        if (base_path && base_path[0])
                joystick->mode_base[(int)mode] = joy_ui_image_create(joystick->r2d, base_path);
        if (thumb_path && thumb_path[0])
                joystick->mode_thumb[(int)mode] = joy_ui_image_create(joystick->r2d, thumb_path);
        // 若当前正好是该模式，立即切换显示
        if (joystick->dir_mode == mode) {
                joystick->base_image = joystick->mode_base[(int)mode] ? joystick->mode_base[(int)mode] : joystick->base_default;
                joystick->thumb_image = joystick->mode_thumb[(int)mode] ? joystick->mode_thumb[(int)mode] : joystick->thumb_default;
        }
}

// 设置摇杆位置
void joy_input_joystick_set_position(joy_joystick_p joystick, float x, float y)
{
        joystick->center.x = x;
        joystick->center.y = y;

        joystick->background.x = x - joystick->radius;
        joystick->background.y = y - joystick->radius;

        if (!joystick->is_dragging) {
                joystick->handle.x = x - joystick->handle_radius;
                joystick->handle.y = y - joystick->handle_radius;
        }
}

// 设置摇杆半径（底圈与手柄同步缩放）。用于逻辑分辨率变化时保持设计尺寸。
void joy_input_joystick_set_radius(joy_joystick_p joystick, float radius)
{
        if (!joystick || radius <= 0.0f) return;
        joystick->radius = radius;
        joystick->handle_radius = radius * 0.4f;

        joystick->background.x = joystick->center.x - radius;
        joystick->background.y = joystick->center.y - radius;
        joystick->background.w = radius * 2.0f;
        joystick->background.h = radius * 2.0f;

        if (!joystick->is_dragging) {
                joystick->handle.x = joystick->center.x - joystick->handle_radius;
                joystick->handle.y = joystick->center.y - joystick->handle_radius;
                joystick->handle.w = joystick->handle_radius * 2.0f;
                joystick->handle.h = joystick->handle_radius * 2.0f;
        }
}

// 设置方向约束模式（全方向/四向/八向/仅上/下/左/右）
void joy_input_joystick_set_dir_mode(joy_joystick_p joystick, joystick_dir_mode mode)
{
        if (!joystick) return;
        joystick->dir_mode = mode;
        // 切换显示纹理：优先该模式专属，否则默认
        joystick->base_image = joystick->mode_base[(int)mode] ? joystick->mode_base[(int)mode] : joystick->base_default;
        joystick->thumb_image = joystick->mode_thumb[(int)mode] ? joystick->mode_thumb[(int)mode] : joystick->thumb_default;
}

// 前向声明（定义见下方）；供 joy_input_joystick_set_vector 在定义前调用，避免 static 函数“定义前使用”被当成隐式声明
static void joystick_apply_dir_mode(joystick_dir_mode mode, joy_vec2_t* d);

// 以方向向量驱动摇杆（无需触摸），用于键盘/外部输入同步显示。
// (dx,dy) 为方向（屏幕坐标，y 向下）；长度>0 即视为按下（满量程），为 0 则回中。
// 同样受 dir_mode 约束（小圆只沿允许方向滑动），视觉与触摸一致。
void joy_input_joystick_set_vector(joy_joystick_p j, float dx, float dy)
{
        if (!j) return;
        if (dx == 0.0f && dy == 0.0f) { joy_input_joystick_reset(j); return; }
        j->is_dragging = true;
        j->is_touch_active = true;
        float len = sqrtf(dx * dx + dy * dy);
        joy_vec2_t d = { dx / len, dy / len };
        joystick_apply_dir_mode(j->dir_mode, &d);   // 同样受方向约束
        j->direction.x = d.x;
        j->direction.y = d.y;
        j->magnitude = 1.0f;
        j->handle.x = j->center.x + d.x * j->radius - j->handle_radius;
        j->handle.y = j->center.y + d.y * j->radius - j->handle_radius;
}

// 按方向模式吸附/约束方向向量 d（屏幕坐标，y 向下；保留其长度=强度）
static void joystick_apply_dir_mode(joystick_dir_mode mode, joy_vec2_t* d)
{
        float len = sqrtf(d->x * d->x + d->y * d->y);
        if (len < 1e-6f) return;
        if (mode == JOYSTICK_DIR_OMNI) return;
        float nx = d->x / len, ny = d->y / len;          // 单位方向
        if (mode == JOYSTICK_DIR_VERTICAL) {
                nx = 0.0f; ny = (ny < 0.0f) ? -1.0f : 1.0f;
        } else if (mode == JOYSTICK_DIR_HORIZONTAL) {
                ny = 0.0f; nx = (nx < 0.0f) ? -1.0f : 1.0f;
        } else {
                int steps = (mode == JOYSTICK_DIR_4WAY) ? 4 : 8;
                float step = 2.0f * 3.1415926f / (float)steps;
                float ang = atan2f(ny, nx);
                float snap = roundf(ang / step) * step;   // 对齐到最近离散方向
                nx = cosf(snap); ny = sinf(snap);
        }
        d->x = nx * len;                                  // 保留原强度
        d->y = ny * len;
}

// 更新摇杆拖拽逻辑
static void joystick_update_drag(joy_joystick_p joystick, float touch_x, float touch_y)
{
        // 计算手指到中心的偏移量
        float dx = touch_x - joystick->center.x;
        float dy = touch_y - joystick->center.y;

        // 计算距离
        float distance = sqrtf(dx * dx + dy * dy);

        // 控制距离不超过摇杆半径
        if (distance > joystick->radius) {
                dx = (dx / distance) * joystick->radius;
                dy = (dy / distance) * joystick->radius;
                distance = joystick->radius;
        }

        // 计算方向和强度
        joystick->magnitude = distance / joystick->radius;

        // 应用死区
        if (joystick->magnitude < joystick->deadzone) {
                joystick->direction.x = 0.0f;
                joystick->direction.y = 0.0f;
                joystick->magnitude = 0.0f;

                // 手柄回到中心
                joystick->handle.x = joystick->center.x - joystick->handle_radius;
                joystick->handle.y = joystick->center.y - joystick->handle_radius;
        }
        else {
                // 非死区时更新方向（并应用方向约束模式）
                joystick->direction.x = dx / joystick->radius;
                joystick->direction.y = dy / joystick->radius;
                joystick_apply_dir_mode(joystick->dir_mode, &joystick->direction);

                // 更新手柄位置（同样受方向约束：由受限后的方向反推，小圆只沿允许方向滑动）
                joystick->handle.x = joystick->center.x + joystick->direction.x * joystick->radius - joystick->handle_radius;
                joystick->handle.y = joystick->center.y + joystick->direction.y * joystick->radius - joystick->handle_radius;
        }
}

// 处理事件和响应逻辑(分辨率)
bool joy_input_joystick_is_dragging(joy_joystick_p j)
{
        return j ? j->is_dragging : false;
}

void joy_input_joystick_touch_down(joy_joystick_p j, float x, float y)
{
        if (!j) return;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(j->r2d, x, y);
        float dist_sq = distance_squared(logic.x, logic.y, j->center.x, j->center.y);
        if (dist_sq <= (j->radius * j->radius)) {
                j->is_dragging = true;
                j->is_touch_active = true;
                j->touch_id = -1;
                joystick_update_drag(j, logic.x, logic.y);
        }
}

void joy_input_joystick_touch_move(joy_joystick_p j, float x, float y)
{
        if (!j || !j->is_dragging) return;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(j->r2d, x, y);
        joystick_update_drag(j, logic.x, logic.y);
}

void joy_input_joystick_touch_up(joy_joystick_p j)
{
        if (!j) return;
        joy_input_joystick_reset(j);
}

// 重置摇杆
void joy_input_joystick_reset(joy_joystick_p joystick)
{
        joystick->direction.x = 0.0f;
        joystick->direction.y = 0.0f;
        joystick->magnitude = 0.0f;
        joystick->is_dragging = false;
        joystick->is_touch_active = false;
        joystick->touch_id = -1;

        // 手柄回到中心
        joystick->handle.x = joystick->center.x - joystick->handle_radius;
        joystick->handle.y = joystick->center.y - joystick->handle_radius;
}

// 绘制摇杆：优先使用纹理（底圈 base_image + 摇杆头 thumb_image），
// 未设置纹理时退回原纯绘制（圆形背景 + 十字 + 圆形手柄），保持向后兼容。
void joy_input_joystick_draw(joy_joystick_p joystick)
{
        if (!joystick) return;
        joy_r2d_p r2d = joystick->r2d;

        // ---- 底圈 ----
        if (joystick->base_image) {
                joy_ui_image_draw1(joystick->base_image, NULL, &joystick->background);
        } else {
                joy_r2d_set_color(r2d,
                        (JOYSTICK_BG_COLOR >> 24) & 0xFF,
                        (JOYSTICK_BG_COLOR >> 16) & 0xFF,
                        (JOYSTICK_BG_COLOR >> 8) & 0xFF,
                        JOYSTICK_BG_COLOR & 0xFF);
                // 绘制圆形背景, 使用多段模拟圆形
                joy_math_shape_draw_circle(r2d, "line", joystick->center, joystick->radius, 32);

                // 绘制十字线
                joy_r2d_set_color(r2d, 0x66, 0x66, 0x66, 0xFF);
                joy_math_shape_draw_line(r2d,
                        joystick->center.x - joystick->radius, joystick->center.y,
                        joystick->center.x + joystick->radius, joystick->center.y);
                joy_math_shape_draw_line(r2d,
                        joystick->center.x, joystick->center.y - joystick->radius,
                        joystick->center.x, joystick->center.y + joystick->radius);
        }

        // ---- 摇杆头 ----
        if (joystick->thumb_image) {
                joy_ui_image_draw1(joystick->thumb_image, NULL, &joystick->handle);
        } else {
                joy_color_t handle_color = joystick->is_dragging ?
                        (joy_color_t) { 0x4C, 0xAF, 0x50, 0xFF } :
                        (joy_color_t) { 0x88, 0x88, 0x88, 0xFF };
                joy_r2d_set_color(r2d, handle_color.r, handle_color.g, handle_color.b, handle_color.a);

                // 绘制圆形手柄
                joy_vec2_t handle_center = {
                        joystick->handle.x + joystick->handle_radius,
                        joystick->handle.y + joystick->handle_radius
                };
                joy_math_shape_draw_circle(r2d, "fill", handle_center, joystick->radius * 0.5f, 24);

                // 绘制手柄内框
                joy_r2d_set_color(r2d, handle_color.r, handle_color.g, handle_color.b, handle_color.a - 0x40);
                joy_rect_t handle_fill = {
                        joystick->handle.x + 2, joystick->handle.y + 2,
                        joystick->handle.w - 4, joystick->handle.h - 4
                };
                joy_math_shape_draw_rectangle(r2d, "fill", handle_fill);
        }

        // ---- 方向指示线（仅纯绘制时；纹理已用摇杆头位移表达方向）----
        if (!joystick->thumb_image && joy_input_joystick_get_magnitude(joystick) > 0) {
                joy_vec2_t dir = joy_input_joystick_get_direction(joystick);
                joy_r2d_set_color(r2d, 0, 0, 0, 100);
                joy_math_shape_draw_line(r2d,
                        joystick->center.x,
                        joystick->center.y,
                        joystick->center.x + dir.x * joystick->radius,
                        joystick->center.y + dir.y * joystick->radius);
        }
}

// 获取摇杆方向
joy_vec2_t joy_input_joystick_get_direction(joy_joystick_p joystick)
{
        return joystick->direction;
}

// 获取摇杆强度
float joy_input_joystick_get_magnitude(joy_joystick_p joystick)
{
        return joystick->magnitude;
}

/* =================================================================
 * InputBox
 * ================================================================= */

#define INPUTBOX_BUF_INIT 256

joy_inputbox_p joy_ui_inputbox_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, joy_font_p font)
{
        joy_rect_t rect = jui_frect(r2d, fx, fy, fw, fh);
        joy_inputbox_p ti = (joy_inputbox_p)calloc(1, sizeof(joy_inputbox_t));
        if (!ti) return NULL;
        ti->r2d = r2d;
        ti->rect = rect;
        ti->font = font;
        ti->bg_color = (joy_color_t){ 255, 255, 255, 255 };
        ti->border_color = (joy_color_t){ 180, 180, 180, 255 };
        ti->focus_border_color = (joy_color_t){ 66, 133, 244, 255 };
        ti->text_color = (joy_color_t){ 0, 0, 0, 255 };
        ti->cursor_color = (joy_color_t){ 0, 0, 0, 255 };
        ti->sel_color = (joy_color_t){ 100, 150, 255, 255 };
        ti->buf_len = INPUTBOX_BUF_INIT;
        ti->buffer = (char*)calloc(1, ti->buf_len);
        ti->text_len = 0;
        ti->cursor_pos = 0;
        ti->sel_start = -1;
        ti->sel_end = -1;
        ti->focused = false;
        ti->cursor_timer = joy_core_get_ticks();
        ti->cursor_visible = true;
        return ti;
}

void joy_ui_inputbox_destroy(joy_inputbox_p ti)
{
        if (!ti) return;
        free(ti->buffer);
        free(ti);
}


/* ---- 选中支持 ---- */

static void inputbox_clear_sel(joy_inputbox_p ti)
{
        ti->sel_start = ti->sel_end = -1;
}

static bool inputbox_has_sel(joy_inputbox_p ti)
{
        return ti->sel_start >= 0 && ti->sel_end >= 0 && ti->sel_start != ti->sel_end;
}

/* 删除选中文本，将光标置于选中起始 */
static void inputbox_delete_sel(joy_inputbox_p ti)
{
        if (!inputbox_has_sel(ti)) return;
        int a = ti->sel_start, b = ti->sel_end;
        if (a > b) { int t = a; a = b; b = t; }
        memmove(ti->buffer + a, ti->buffer + b, ti->text_len - b);
        ti->text_len -= (b - a);
        ti->cursor_pos = a;
        inputbox_clear_sel(ti);
        ti->buffer[ti->text_len] = '\0';
}


void joy_ui_inputbox_set_text(joy_inputbox_p ti, const char* text)
{
        if (!ti || !text) return;
        int len = (int)strlen(text);
        if (len >= ti->buf_len) {
                ti->buf_len = len + 64;
                ti->buffer = (char*)realloc(ti->buffer, ti->buf_len);
        }
        memcpy(ti->buffer, text, len);
        ti->buffer[len] = '\0';
        ti->text_len = len;
        ti->cursor_pos = len;
        inputbox_clear_sel(ti);
        ti->scroll_offset = 0;
        if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
}

const char* joy_ui_inputbox_get_text(joy_inputbox_p ti)
{
        return ti ? ti->buffer : "";
}

void joy_ui_inputbox_set_callback(joy_inputbox_p ti,
        void (*submit)(joy_inputbox_p, void*),
        void (*change)(joy_inputbox_p, void*),
        void* userdata)
{
        if (!ti) return;
        ti->on_submit = submit;
        ti->on_change = change;
        ti->cb_userdata = userdata;
}

static int inputbox_cursor_x(joy_inputbox_p ti);
static int inputbox_cursor_x_at(joy_inputbox_p ti, int pos);

/* 滚动到光标可见，并尽量填充右侧空白 */
static void inputbox_scroll_to_cursor(joy_inputbox_p ti)
{
        int cw = inputbox_cursor_x(ti);
        int visible_w = (int)ti->rect.w - 8;

        /* 1. 保证光标可见 */
        if (cw - ti->scroll_offset > visible_w)
                ti->scroll_offset = cw - visible_w;
        else if (cw - ti->scroll_offset < 0)
                ti->scroll_offset = cw;

        /* 2. 如果文字总宽度小于可见区域，直接左对齐 */
        int total_w = inputbox_cursor_x_at(ti, ti->text_len);
        if (total_w < visible_w)
                ti->scroll_offset = 0;

        /* 3. 如果右侧出现空白（删除文字后），把隐藏的左侧文字拉出来 */
        int rightmost_visible = ti->scroll_offset + visible_w;
        if (rightmost_visible > total_w && total_w > visible_w)
                ti->scroll_offset = total_w - visible_w;

        if (ti->scroll_offset < 0)
                ti->scroll_offset = 0;
}

/* 在 buffer 的 cursor_pos 处插入 len 字节 */
void joy_ui_inputbox_insert(joy_inputbox_p ti, const char* s, int len)
{
        /* 有选中时先删除选中文本 */
        if (inputbox_has_sel(ti)) inputbox_delete_sel(ti);

        if (ti->text_len + len >= ti->buf_len) {
                while (ti->text_len + len >= ti->buf_len)
                        ti->buf_len *= 2;
                ti->buffer = (char*)realloc(ti->buffer, ti->buf_len);
        }
        memmove(ti->buffer + ti->cursor_pos + len,
                ti->buffer + ti->cursor_pos,
                ti->text_len - ti->cursor_pos);
        memcpy(ti->buffer + ti->cursor_pos, s, len);
        ti->text_len += len;
        ti->cursor_pos += len;
        ti->buffer[ti->text_len] = '\0';
        if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
        inputbox_scroll_to_cursor(ti);
}

/* 移动光标并清除选中 */
static void inputbox_move_cursor(joy_inputbox_p ti, int pos)
{
        ti->cursor_pos = pos;
        inputbox_clear_sel(ti);
        inputbox_scroll_to_cursor(ti);
}

/* 扩展选中到光标位置 */
static void inputbox_update_sel(joy_inputbox_p ti)
{
        if (ti->sel_start < 0) ti->sel_start = ti->cursor_pos;
        ti->sel_end = ti->cursor_pos;
}

/* ---- end 选中支持 ---- */

void joy_ui_inputbox_cursor_left(joy_inputbox_p ti)
{
        if (!ti || ti->cursor_pos <= 0) return;
        inputbox_move_cursor(ti, ti->cursor_pos - 1);
}

void joy_ui_inputbox_cursor_right(joy_inputbox_p ti)
{
        if (!ti || ti->cursor_pos >= ti->text_len) return;
        inputbox_move_cursor(ti, ti->cursor_pos + 1);
}

void joy_ui_inputbox_cursor_home(joy_inputbox_p ti)
{
        if (!ti) return;
        inputbox_move_cursor(ti, 0);
        ti->scroll_offset = 0;
}

void joy_ui_inputbox_cursor_end(joy_inputbox_p ti)
{
        if (!ti) return;
        inputbox_move_cursor(ti, ti->text_len);
        inputbox_scroll_to_cursor(ti);
}

void joy_ui_inputbox_select_left(joy_inputbox_p ti)
{
        if (!ti || ti->cursor_pos <= 0) return;
        ti->cursor_pos--;
        inputbox_update_sel(ti);
        inputbox_scroll_to_cursor(ti);
}

void joy_ui_inputbox_select_right(joy_inputbox_p ti)
{
        if (!ti || ti->cursor_pos >= ti->text_len) return;
        ti->cursor_pos++;
        inputbox_update_sel(ti);
        inputbox_scroll_to_cursor(ti);
}

void joy_ui_inputbox_select_home(joy_inputbox_p ti)
{
        if (!ti) return;
        inputbox_update_sel(ti);
        ti->cursor_pos = 0;
        ti->scroll_offset = 0;
        ti->sel_end = 0;
}

void joy_ui_inputbox_select_end(joy_inputbox_p ti)
{
        if (!ti) return;
        inputbox_update_sel(ti);
        ti->cursor_pos = ti->text_len;
        ti->sel_end = ti->text_len;
        inputbox_scroll_to_cursor(ti);
}

void joy_ui_inputbox_select_all(joy_inputbox_p ti)
{
        if (!ti) return;
        ti->sel_start = 0;
        ti->sel_end = ti->text_len;
        ti->cursor_pos = ti->text_len;
        inputbox_scroll_to_cursor(ti);
}

/* 删除光标前一个 UTF-8 字符（退格） */
void joy_ui_inputbox_backspace(joy_inputbox_p ti)
{
        if (inputbox_has_sel(ti)) {
                inputbox_delete_sel(ti);
                inputbox_scroll_to_cursor(ti);
                if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
                return;
        }
        if (ti->cursor_pos <= 0) return;
        int pos = ti->cursor_pos - 1;
        while (pos > 0 && ((unsigned char)ti->buffer[pos] & 0xC0) == 0x80)
                pos--;
        int ch_len = ti->cursor_pos - pos;
        memmove(ti->buffer + pos, ti->buffer + ti->cursor_pos,
                ti->text_len - ti->cursor_pos);
        ti->text_len -= ch_len;
        ti->cursor_pos = pos;
        ti->buffer[ti->text_len] = '\0';
        if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
        inputbox_scroll_to_cursor(ti);
}

/* 删除光标后一个 UTF-8 字符（Delete） */
void joy_ui_inputbox_delete(joy_inputbox_p ti)
{
        if (inputbox_has_sel(ti)) {
                inputbox_delete_sel(ti);
                inputbox_scroll_to_cursor(ti);
                if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
                return;
        }
        if (ti->cursor_pos >= ti->text_len) return;
        int pos = ti->cursor_pos;
        int ch_len = 1;
        while (pos + ch_len < ti->text_len && ((unsigned char)ti->buffer[pos + ch_len] & 0xC0) == 0x80)
                ch_len++;
        memmove(ti->buffer + pos, ti->buffer + pos + ch_len,
                ti->text_len - pos - ch_len);
        ti->text_len -= ch_len;
        ti->buffer[ti->text_len] = '\0';
        if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
        inputbox_scroll_to_cursor(ti);
}

void joy_ui_inputbox_paste(joy_inputbox_p ti)
{
        if (!ti) return;
        const char* clip = joy_core_get_clipboard_text();
        if (clip) joy_ui_inputbox_insert(ti, clip, (int)strlen(clip));
}

void joy_ui_inputbox_copy(joy_inputbox_p ti)
{
        if (!ti || ti->text_len <= 0) return;
        if (inputbox_has_sel(ti)) {
                int a = ti->sel_start, b = ti->sel_end;
                if (a > b) { int t = a; a = b; b = t; }
                int len = b - a;
                char* buf = (char*)malloc(len + 1);
                memcpy(buf, ti->buffer + a, len);
                buf[len] = '\0';
                joy_core_set_clipboard_text(buf);
                free(buf);
        }
        else {
                joy_core_set_clipboard_text(ti->buffer);
        }
}

void joy_ui_inputbox_cut(joy_inputbox_p ti)
{
        if (!ti || ti->text_len <= 0) return;
        if (inputbox_has_sel(ti)) {
                int a = ti->sel_start, b = ti->sel_end;
                if (a > b) { int t = a; a = b; b = t; }
                int len = b - a;
                char* buf = (char*)malloc(len + 1);
                memcpy(buf, ti->buffer + a, len);
                buf[len] = '\0';
                joy_core_set_clipboard_text(buf);
                free(buf);
                inputbox_delete_sel(ti);
                ti->buffer[ti->text_len] = '\0';
                if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
                inputbox_scroll_to_cursor(ti);
        }
        else {
                joy_core_set_clipboard_text(ti->buffer);
                ti->text_len = 0;
                ti->cursor_pos = 0;
                ti->scroll_offset = 0;
                ti->buffer[0] = '\0';
                if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
        }
}

/* 获取光标之前的文本宽度（像素） */
static int inputbox_cursor_x(joy_inputbox_p ti)
{
        if (ti->cursor_pos <= 0 || !ti->font) return 0;
        /* 用 joy_ui_text_createx 生成光标前的子串，量宽度 */
        joy_color_t c = { 0,0,0,255 };
        joy_text_p tex = joy_ui_text_createx(ti->r2d, ti->font, ti->buffer, ti->cursor_pos, c);
        if (!tex) return 0;
        int w = joy_ui_text_get_width(tex);
        joy_ui_text_destroy(tex);
        return w;
}

/* 获取 buffer 中从开头到 pos 字节位置的文本宽度（像素） */
static int inputbox_cursor_x_at(joy_inputbox_p ti, int pos)
{
        if (pos <= 0 || !ti->font) return 0;
        joy_color_t c = { 0,0,0,255 };
        joy_text_p tex = joy_ui_text_createx(ti->r2d, ti->font, ti->buffer, pos, c);
        if (!tex) return 0;
        int w = joy_ui_text_get_width(tex);
        joy_ui_text_destroy(tex);
        return w;
}

/* 根据点击的局部 x 坐标计算光标位置 */
static int inputbox_pos_from_x(joy_inputbox_p ti, float local_x)
{
        if (!ti || ti->text_len <= 0 || local_x <= 0) return 0;
        int total_w = inputbox_cursor_x_at(ti, ti->text_len);
        if (local_x >= total_w) return ti->text_len;
        for (int i = 0; i < ti->text_len; ) {
                int next = i + 1;
                while (next < ti->text_len && ((unsigned char)ti->buffer[next] & 0xC0) == 0x80)
                        next++;
                int w = inputbox_cursor_x_at(ti, next);
                if (w >= local_x) {
                        int prev_w = inputbox_cursor_x_at(ti, i);
                        return (local_x - prev_w < w - local_x) ? i : next;
                }
                i = next;
        }
        return ti->text_len;
}

void joy_ui_inputbox_focus(joy_inputbox_p ti)
{
        if (!ti) return;
        ti->focused = true;
        ti->cursor_timer = joy_core_get_ticks();
        ti->cursor_visible = true;
        /* 从 r2d 获取 window 并启用文本输入。
         * 注意：即使已经处于聚焦态也要调用 SDL_StartTextInput——用户可能用系统返回键
         * 手动收起了输入法（此时 SDL 会把 text_input_active / screen_keyboard_shown
         * 复位为 false，但 inputbox 自身的 focused 仍是 true）。再次点击该框时需要重新
         * 拉起键盘，否则表现为"点一下没反应、要再点一下"。
         * 键盘本来就开着时，SDL_StartTextInput 内部是 no-op（不会重复 show/start）。 */
        SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)joy_r2d_unwrap(ti->r2d));
        if (win) SDL_StartTextInput(win);
}

void joy_ui_inputbox_defocus(joy_inputbox_p ti)
{
        if (!ti || !ti->focused) return;
        ti->focused = false;
        inputbox_clear_sel(ti);
        SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)joy_r2d_unwrap(ti->r2d));
        if (win) SDL_StopTextInput(win);
}

/* 仅清除焦点，不调用 SDL_StopTextInput。
 * 用于在两个输入框之间切换焦点：保持软键盘/IME 不收起。
 * 为什么不能直接走 joy_ui_inputbox_defocus：
 *   切换焦点时若先 SDL_StopTextInput(隐藏键盘)、同一帧内紧接着另一框
 *   SDL_StartTextInput(显示键盘)，Android 的 IME hide/show 是异步投递到 UI 线程的，
 *   show 会与尚未完成的 hide 竞态被吞掉，表现为"从 A 直接点 B 唤不起输入法，
 *   必须先点输入框外让 A 失焦、隔一帧再点 B 才行"。
 * 因此：框间切换用本函数只转移焦点（键盘保持），点到所有框之外才用
 * joy_ui_inputbox_defocus 真正收起键盘。 */
void joy_ui_inputbox_blur(joy_inputbox_p ti)
{
        if (!ti || !ti->focused) return;
        ti->focused = false;
        inputbox_clear_sel(ti);
}

void joy_ui_inputbox_draw(joy_inputbox_p ti)
{
        if (!ti) return;
        joy_r2d_p r = ti->r2d;
        joy_rect_t rect = ti->rect;

        /* 背景 */
        joy_r2d_set_color(r, ti->bg_color.r, ti->bg_color.g, ti->bg_color.b, ti->bg_color.a);
        joy_math_shape_draw_rectangle(r, "fill", rect);

        /* 边框（聚焦时高亮） */
        joy_color_t border = ti->focused ? ti->focus_border_color : ti->border_color;
        joy_r2d_set_color(r, border.r, border.g, border.b, border.a);
        joy_math_shape_draw_rectangle(r, "line", rect);

        /* 设置裁剪区域，防止文字绘制到 inputbox 矩形之外 */
        SDL_Renderer* sdl_r = (SDL_Renderer*)joy_r2d_unwrap(r);
        SDL_Rect clip_rect = { (int)rect.x + 2, (int)rect.y, (int)rect.w - 4, (int)rect.h };
        bool clip_was_enabled = SDL_RenderClipEnabled(sdl_r);
        SDL_Rect old_clip;
        if (clip_was_enabled)
                SDL_GetRenderClipRect(sdl_r, &old_clip);
        SDL_SetRenderClipRect(sdl_r, &clip_rect);

        /* 选中高亮（绘制在文字下方） */
        if (inputbox_has_sel(ti) && ti->font) {
                int a = ti->sel_start, b = ti->sel_end;
                if (a > b) { int t = a; a = b; b = t; }
                joy_color_t c = { 0,0,0,255 };
                joy_text_p ta = joy_ui_text_createx(r, ti->font, ti->buffer, a, c);
                joy_text_p tb = joy_ui_text_createx(r, ti->font, ti->buffer + a, b - a, c);
                if (ta && tb) {
                        float sx = rect.x + 4 + (float)(joy_ui_text_get_width(ta) - ti->scroll_offset);
                        float sw = (float)joy_ui_text_get_width(tb);
                        joy_rect_t sel_rect = { sx, rect.y + 1, sw, rect.h - 2 };
                        joy_r2d_set_color(r, ti->sel_color.r, ti->sel_color.g, ti->sel_color.b, ti->sel_color.a);
                        joy_math_shape_draw_rectangle(r, "fill", sel_rect);
                }
                if (ta) joy_ui_text_destroy(ta);
                if (tb) joy_ui_text_destroy(tb);
        }

        /* 文字 */
        if (ti->text_len > 0 && ti->font) {
                float text_x = rect.x + 4;
                float text_y = rect.y + (rect.h - (float)joy_r2d_font_get_size(ti->font)) / 2.0f;
                float ox = text_x - (float)ti->scroll_offset;

                if (inputbox_has_sel(ti)) {
                        int a = ti->sel_start, b = ti->sel_end;
                        if (a > b) { int t = a; a = b; b = t; }
                        joy_color_t white = { 255, 255, 255, 255 };
                        /* 选中前部分 */
                        if (a > 0)
                                joy_r2d_jtext_printx(r, ti->font, ti->buffer, a, ti->text_color, ox, text_y);
                        /* 选中部分（白色） */
                        if (b > a) {
                                float offset_before = (float)joy_ui_text_get_width(joy_ui_text_createx(r, ti->font, ti->buffer, a, ti->text_color));
                                joy_r2d_jtext_printx(r, ti->font, ti->buffer + a, b - a, white, ox + offset_before, text_y);
                        }
                        /* 选中后部分 */
                        if (b < ti->text_len) {
                                float offset_sel = (float)joy_ui_text_get_width(joy_ui_text_createx(r, ti->font, ti->buffer, b, ti->text_color));
                                joy_r2d_jtext_printx(r, ti->font, ti->buffer + b, ti->text_len - b, ti->text_color, ox + offset_sel, text_y);
                        }
                }
                else {
                        joy_r2d_jtext_printx(r, ti->font, ti->buffer, ti->text_len, ti->text_color, ox, text_y);
                }
        }

        /* 恢复裁剪区域 */
        if (clip_was_enabled)
                SDL_SetRenderClipRect(sdl_r, &old_clip);
        else
                SDL_SetRenderClipRect(sdl_r, NULL);

        /* 光标（聚焦时绘制） */
        if (ti->focused) {
                uint64_t now = joy_core_get_ticks();
                if (now - ti->cursor_timer >= 530) {
                        ti->cursor_visible = !ti->cursor_visible;
                        ti->cursor_timer = now;
                }
                if (ti->cursor_visible) {
                        int cx = inputbox_cursor_x(ti) - ti->scroll_offset + (int)rect.x + 4;
                        if (cx >= rect.x + 2 && cx <= rect.x + rect.w - 2) {
                                float cy = rect.y + 3;
                                float ch = rect.h - 6;
                                joy_r2d_set_color(r, ti->cursor_color.r, ti->cursor_color.g, ti->cursor_color.b, ti->cursor_color.a);
                                joy_math_shape_draw_line(r, (float)cx + 0.5f, cy, (float)cx + 0.5f, cy + ch);
                        }
                }
        }
}

bool joy_ui_inputbox_is_pressed(joy_inputbox_p ti, float x, float y)
{
        if (!ti) return false;
        joy_vec2_t logic;
        joy_rectangle_t rect;
        rect.x = ti->rect.x;
        rect.y = ti->rect.y;
        rect.width = ti->rect.w;
        rect.height = ti->rect.h;
        logic = joy_r2d_coordinates_from_window(ti->r2d, x, y);
        if (joy_math_collision2d_check_point_rectangle(logic, rect)) {
                joy_ui_inputbox_focus(ti);
                inputbox_clear_sel(ti);
                float local_x = logic.x - ti->rect.x - 2 + ti->scroll_offset;
                ti->cursor_pos = inputbox_pos_from_x(ti, local_x);
                inputbox_scroll_to_cursor(ti);
                return true;
        }
        joy_ui_inputbox_defocus(ti);
        return false;
}

bool joy_ui_inputbox_is_hovered(joy_inputbox_p ti, float x, float y)
{
        if (!ti) return false;
        joy_vec2_t logic;
        joy_rectangle_t rect;
        rect.x = ti->rect.x;
        rect.y = ti->rect.y;
        rect.width = ti->rect.w;
        rect.height = ti->rect.h;
        logic = joy_r2d_coordinates_from_window(ti->r2d, x, y);
        ti->hovered = joy_math_collision2d_check_point_rectangle(logic, rect);
        return ti->hovered;
}

bool joy_ui_inputbox_is_focused(joy_inputbox_p ti)
{
        return ti ? ti->focused : false;
}

/* =================================================================
 * Slider
 * ================================================================= */

static float slider_thumb_x(joy_slider_p s)
{
        float range = s->max - s->min;
        if (range <= 0) return s->track.x;
        float ratio = (s->value - s->min) / range;
        return s->track.x + ratio * s->track.w;
}

static joy_rect_t slider_thumb_rect(joy_slider_p s)
{
        float tx = slider_thumb_x(s);
        float half = s->thumb_width * 0.5f;
        // 限制滑块方块不超出滑轨边界
        if (tx - half < s->track.x) tx = s->track.x + half;
        if (tx + half > s->track.x + s->track.w) tx = s->track.x + s->track.w - half;
        joy_rect_t r;
        r.x = tx - half;
        r.y = s->track.y;
        r.w = s->thumb_width;
        r.h = s->track.h;
        return r;
}

static void slider_update_value_from_x(joy_slider_p s, float logic_x)
{
        float ratio = (logic_x - s->track.x) / s->track.w;
        float v = s->min + ratio * (s->max - s->min);
        if (v < s->min) v = s->min;
        if (v > s->max) v = s->max;
        s->value = v;
}

static bool slider_thumb_hit(joy_slider_p s, float logic_x, float logic_y)
{
        joy_rect_t tr = slider_thumb_rect(s);
        return logic_x >= tr.x && logic_x <= tr.x + tr.w
                && logic_y >= tr.y && logic_y <= tr.y + tr.h;
}

joy_slider_p joy_ui_slider_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, float min, float max, float initial)
{
        float x, y, w, h;
        jui_fpx(r2d, fx, fy, fw, fh, &x, &y, &w, &h);
        joy_slider_p s = (joy_slider_p)calloc(1, sizeof(joy_slider_t));
        if (!s) return NULL;
        s->r2d = r2d;
        s->track.x = x;
        s->track.y = y;
        s->track.w = w;
        s->track.h = h;
        s->min = min;
        s->max = max;
        s->value = initial;
        if (s->value < min) s->value = min;
        if (s->value > max) s->value = max;
        s->thumb_width = h;
        s->is_dragging = false;
        s->hovered = false;
        s->track_color = (joy_color_t){ 80,  80,  80,  255 };
        s->fill_color = (joy_color_t){ 66,  133, 244, 255 };
        s->thumb_color = (joy_color_t){ 200, 200, 200, 255 };
        s->thumb_hover_color = (joy_color_t){ 240, 240, 240, 255 };
        return s;
}

void joy_ui_slider_destroy(joy_slider_p s)
{
        free(s);
}

void joy_ui_slider_set_value(joy_slider_p s, float v)
{
        if (!s) return;
        if (v < s->min) v = s->min;
        if (v > s->max) v = s->max;
        s->value = v;
}

float joy_ui_slider_get_value(joy_slider_p s)
{
        return s ? s->value : 0;
}

void joy_ui_slider_set_range(joy_slider_p s, float min, float max)
{
        if (!s) return;
        s->min = min;
        s->max = max;
        if (s->value < min) s->value = min;
        if (s->value > max) s->value = max;
}

void joy_ui_slider_set_colors(joy_slider_p s, joy_color_t track, joy_color_t fill, joy_color_t thumb, joy_color_t thumb_hover)
{
        if (!s) return;
        s->track_color = track;
        s->fill_color = fill;
        s->thumb_color = thumb;
        s->thumb_hover_color = thumb_hover;
}

void joy_ui_slider_set_rect(joy_slider_p s, float x, float y, float w, float h)
{
        if (!s) return;
        s->track.x = x;
        s->track.y = y;
        s->track.w = w;
        s->track.h = h;
        s->thumb_width = h;
}

void joy_ui_slider_draw(joy_slider_p s)
{
        if (!s) return;
        joy_r2d_p r = s->r2d;

        /* 滑轨背景 */
        joy_r2d_set_color(r, s->track_color.r, s->track_color.g, s->track_color.b, s->track_color.a);
        joy_math_shape_draw_rectangle(r, "fill", s->track);

        /* 已填充部分（从 track.x 到 thumb 中心） */
        float tx = slider_thumb_x(s);
        float fill_w = tx - s->track.x;
        if (fill_w > 0) {
                joy_r2d_set_color(r, s->fill_color.r, s->fill_color.g, s->fill_color.b, s->fill_color.a);
                joy_rect_t fill_rect = { s->track.x, s->track.y, fill_w, s->track.h };
                joy_math_shape_draw_rectangle(r, "fill", fill_rect);
        }

        /* 滑块 */
        joy_rect_t tr = slider_thumb_rect(s);
        joy_color_t tc = s->hovered ? s->thumb_hover_color : s->thumb_color;
        joy_r2d_set_color(r, tc.r, tc.g, tc.b, tc.a);
        joy_math_shape_draw_rectangle(r, "fill", tr);
        joy_r2d_set_color(r, 0, 0, 0, 255);
        joy_math_shape_draw_rectangle(r, "line", tr);
}

bool joy_ui_slider_is_pressed(joy_slider_p s, float x, float y)
{
        if (!s) return false;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(s->r2d, x, y);
        if (slider_thumb_hit(s, logic.x, logic.y)) {
                s->is_dragging = true;
                return true;
        }
        /* 点击滑轨时跳转到该位置 */
        if (logic.x >= s->track.x && logic.x <= s->track.x + s->track.w
                && logic.y >= s->track.y && logic.y <= s->track.y + s->track.h) {
                slider_update_value_from_x(s, logic.x);
                s->is_dragging = true;
                return true;
        }
        return false;
}

void joy_ui_slider_set_pressed(joy_slider_p s, bool v)
{
        if (s) s->is_dragging = v;
}

void joy_ui_slider_drag_move(joy_slider_p s, float x, float y)
{
        if (!s || !s->is_dragging) return;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(s->r2d, x, y);
        slider_update_value_from_x(s, logic.x);
}

bool joy_ui_slider_is_hovered(joy_slider_p s, float x, float y)
{
        if (!s) return false;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(s->r2d, x, y);
        s->hovered = slider_thumb_hit(s, logic.x, logic.y);
        return s->hovered;
}

void joy_ui_slider_set_hovered(joy_slider_p s, bool v)
{
        if (s) s->hovered = v;
}

/* ===== Popup ===== */
#define POPUP_CLOSE_SIZE 24
#define POPUP_PADDING 12
#define POPUP_TITLE_HEIGHT 32

joy_popup_p joy_ui_popup_create(joy_r2d_p r2d, float fw, float fh)
{
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        float w = fw * (float)lw, h = fh * (float)lh;
        joy_popup_p p = (joy_popup_p)calloc(1, sizeof(joy_popup_t));
        if (!p) return NULL;
        p->r2d = r2d;
        p->w = w; p->h = h;
        p->x = 0; p->y = 0;
        p->title_height = POPUP_TITLE_HEIGHT;
        p->padding = POPUP_PADDING;
        p->close_size = POPUP_CLOSE_SIZE;
        p->visible = true;
        p->closeable = true;
        /* Default colors */
        p->bg_color = (joy_color_t){ 45, 45, 55, 240 };
        p->border_color = (joy_color_t){ 80, 80, 100, 255 };
        p->title_bg_color = (joy_color_t){ 55, 55, 70, 240 };
        p->content_color = (joy_color_t){ 220, 220, 230, 255 };
        p->close_color = (joy_color_t){ 200, 60, 60, 255 };
        p->close_hover_color = (joy_color_t){ 255, 80, 80, 255 };
        p->title_text_color = (joy_color_t){ 255, 255, 255, 255 };
        p->title[0] = '\0';
        p->content[0] = '\0';
        return p;
}

void joy_ui_popup_destroy(joy_popup_p p)
{
        free(p);
}

void joy_ui_popup_set_title(joy_popup_p p, const char* text, int len)
{
        if (!p || !text) return;
        if (len < 0) len = (int)strlen(text);
        if (len >= JOY_POPUP_MAX_TITLE) len = JOY_POPUP_MAX_TITLE - 1;
        memcpy(p->title, text, len);
        p->title[len] = '\0';
}

void joy_ui_popup_set_content(joy_popup_p p, const char* text, int len)
{
        if (!p || !text) return;
        if (len < 0) len = (int)strlen(text);
        if (len >= JOY_POPUP_MAX_CONTENT) len = JOY_POPUP_MAX_CONTENT - 1;
        memcpy(p->content, text, len);
        p->content[len] = '\0';
}

void joy_ui_popup_set_position(joy_popup_p p, float x, float y)
{
        if (p) { p->x = x; p->y = y; }
}

void joy_ui_popup_set_visible(joy_popup_p p, bool v)
{
        if (p) p->visible = v;
}

void joy_ui_popup_set_closeable(joy_popup_p p, bool v)
{
        if (p) p->closeable = v;
}

static void popup_draw_text_lines(joy_r2d_p r2d, joy_font_p font, const char* text, float x, float y, float max_w, joy_color_t color)
{
        if (!text || text[0] == '\0') return;
        /* 按行分割渲染 */
        const char* start = text;
        while (*start) {
                const char* end = strchr(start, '\n');
                if (!end) end = start + strlen(start);
                int line_len = (int)(end - start);
                if (line_len > 0) {
                        joy_r2d_jtext_printx(r2d, font, start, line_len, color, x, y);
                        y += (float)joy_r2d_font_get_size(font) + 4;
                }
                if (*end == '\n') end++;
                start = end;
        }
}

void joy_ui_popup_draw(joy_popup_p p, joy_font_p title_font, joy_font_p content_font)
{
        if (!p || !p->visible) return;
        joy_r2d_p r = p->r2d;
        float x = p->x, y = p->y, w = p->w, h = p->h;

        /* 半透明背景遮罩（使用 blend 模式） */
        joy_r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
        joy_r2d_set_color(r, 0, 0, 0, 120);
        joy_rect_t full = { 0, 0, 1920, 1080 };
        joy_math_shape_draw_rectangle(r, "fill", full);
        joy_r2d_set_blend_mode(r, JR2D_BLEND_NONE);

        /* 边框 */
        joy_r2d_set_color(r, p->border_color.r, p->border_color.g, p->border_color.b, p->border_color.a);
        joy_math_shape_draw_rectangle(r, "line", (joy_rect_t) { x - 1, y - 1, w + 2, h + 2 });

        /* 背景 */
        joy_r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
        joy_r2d_set_color(r, p->bg_color.r, p->bg_color.g, p->bg_color.b, p->bg_color.a);
        joy_math_shape_draw_rectangle(r, "fill", (joy_rect_t) { x, y, w, h });

        /* 标题栏背景 */
        joy_r2d_set_color(r, p->title_bg_color.r, p->title_bg_color.g, p->title_bg_color.b, p->title_bg_color.a);
        joy_math_shape_draw_rectangle(r, "fill", (joy_rect_t) { x, y, w, p->title_height });

        /* 标题栏分隔线 */
        joy_r2d_set_color(r, p->border_color.r, p->border_color.g, p->border_color.b, p->border_color.a);
        joy_math_shape_draw_line(r, x, y + p->title_height, x + w, y + p->title_height);

        /* 标题文字 */
        if (p->title[0] && title_font) {
                int title_len = (int)strlen(p->title);
                joy_r2d_jtext_printx(r, title_font, p->title, title_len, p->title_text_color,
                        x + p->padding, y + (p->title_height - (float)joy_r2d_font_get_size(title_font)) / 2);
        }

        /* 关闭按钮 */
        if (p->closeable) {
                float cx = x + w - p->close_size - 4;
                float cy = y + (p->title_height - p->close_size) / 2;
                if (p->close_hovered)
                        joy_r2d_set_color(r, p->close_hover_color.r, p->close_hover_color.g, p->close_hover_color.b, p->close_hover_color.a);
                else
                        joy_r2d_set_color(r, p->close_color.r, p->close_color.g, p->close_color.b, p->close_color.a);
                joy_math_shape_draw_rectangle(r, "fill", (joy_rect_t) { cx, cy, p->close_size, p->close_size });
                /* X 标记 */
                joy_r2d_set_color(r, 255, 255, 255, 255);
                joy_math_shape_draw_line(r, cx + 6, cy + 6, cx + p->close_size - 6, cy + p->close_size - 6);
                joy_math_shape_draw_line(r, cx + p->close_size - 6, cy + 6, cx + 6, cy + p->close_size - 6);
        }

        /* 内容文字 */
        if (p->content[0] && content_font) {
                joy_r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
                popup_draw_text_lines(r, content_font, p->content,
                        x + p->padding, y + p->title_height + p->padding,
                        w - p->padding * 2, p->content_color);
        }
        joy_r2d_set_blend_mode(r, JR2D_BLEND_NONE);
}

bool joy_ui_popup_is_close_clicked(joy_popup_p p, float mx, float my)
{
        if (!p || !p->visible || !p->closeable) return false;
        float cx = p->x + p->w - p->close_size - 4;
        float cy = p->y + (p->title_height - p->close_size) / 2;
        return mx >= cx && mx <= cx + p->close_size &&
                my >= cy && my <= cy + p->close_size;
}

bool joy_ui_popup_is_title_clicked(joy_popup_p p, float mx, float my)
{
        if (!p || !p->visible) return false;
        return mx >= p->x && mx <= p->x + p->w &&
                my >= p->y && my <= p->y + p->title_height;
        /* 关闭按钮区域单独处理，但 title 区域也包含它 */
}

void joy_ui_popup_drag_move(joy_popup_p p, float mx, float my)
{
        if (!p || !p->is_dragging) return;
        p->x = mx - p->drag_off_x;
        p->y = my - p->drag_off_y;
}

bool joy_ui_popup_is_inside(joy_popup_p p, float mx, float my)
{
        if (!p || !p->visible) return false;
        return mx >= p->x && mx <= p->x + p->w &&
                my >= p->y && my <= p->y + p->h;
}

/* ===== Tooltip ===== */
joy_tooltip_p joy_ui_tooltip_create(joy_r2d_p r2d, const char* text, int len)
{
        joy_tooltip_p t = (joy_tooltip_p)calloc(1, sizeof(joy_tooltip_t));
        if (!t) return NULL;
        t->r2d = r2d;
        t->x = 0; t->y = 0;
        t->padding = 8;
        t->visible = true;
        t->bg_color = (joy_color_t){ 40, 40, 55, 220 };
        t->border_color = (joy_color_t){ 100, 100, 130, 255 };
        t->text_color = (joy_color_t){ 255, 255, 255, 255 };
        joy_ui_tooltip_set_text(t, text, len);
        return t;
}

void joy_ui_tooltip_destroy(joy_tooltip_p t)
{
        free(t);
}

void joy_ui_tooltip_set_text(joy_tooltip_p t, const char* text, int len)
{
        if (!t || !text) return;
        if (len < 0) len = (int)strlen(text);
        if (len >= JOY_TOOLTIP_MAX_TEXT) len = JOY_TOOLTIP_MAX_TEXT - 1;
        memcpy(t->text, text, len);
        t->text[len] = '\0';
        t->text_len = len;
}

void joy_ui_tooltip_set_position(joy_tooltip_p t, float x, float y)
{
        if (t) { t->x = x; t->y = y; }
}

void joy_ui_tooltip_set_visible(joy_tooltip_p t, bool v)
{
        if (t) t->visible = v;
}

void joy_ui_tooltip_draw(joy_tooltip_p t, joy_font_p font)
{
        if (!t || !t->visible || !t->text_len) return;
        joy_r2d_p r = t->r2d;

        /* 估算宽高：按字符数 * fontsize/2 粗略估算 */
        float fh = (float)joy_r2d_font_get_size(font);
        float tw = t->text_len * fh * 0.55f + t->padding * 2;
        float th = fh + t->padding;

        /* 背景 */
        joy_r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
        joy_r2d_set_color(r, t->bg_color.r, t->bg_color.g, t->bg_color.b, t->bg_color.a);
        joy_math_shape_draw_rectangle(r, "fill", (joy_rect_t) { t->x, t->y, tw, th });

        /* 边框 */
        joy_r2d_set_blend_mode(r, JR2D_BLEND_NONE);
        joy_r2d_set_color(r, t->border_color.r, t->border_color.g, t->border_color.b, t->border_color.a);
        joy_math_shape_draw_rectangle(r, "line", (joy_rect_t) { t->x, t->y, tw, th });

        /* 文字 */
        joy_r2d_jtext_printx(r, font, t->text, t->text_len, t->text_color,
                t->x + t->padding, t->y + t->padding / 2);
}

/* =================================================================
 * 通用事件处理函数
 * 将 joy_label_handle_event 重命名为 joy_ui_label_process_event，风格统一
 * ================================================================= */

void joy_ui_label_process_event(joy_label_p label, const void* ev)
{
        (void)label;
        (void)ev;
}

void joy_ui_button_process_event(joy_button_p btn, const void* ev)
{
        if (!btn || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_ui_button_is_hovered(btn, e->motion.x, e->motion.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_ui_button_is_pressed(btn, e->button.x, e->button.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                btn->is_pressed = false;
        }
}

/* ===================== 滚动层 scrollview =====================
 * 见 joy.h 中的用法说明。要点：
 *  - 事件坐标是"窗口坐标"，拖拽增量必须经 joy_r2d_coordinates_from_window 转逻辑坐标，
 *    否则窗口与逻辑分辨率不同（letterbox 缩放）时滚动速度和手指不一致；
 *  - 点击/滑动用"本次按下累计位移"区分，位移小=点击、大=滑动；
 *  - 裁剪用 joy_r2d_set_clip_rect（替换语义，非入栈），故 begin 时保存旧裁剪、end 时还原。
 */
struct joy_scrollview {
        joy_r2d_p r2d;
        joy_rect_t view;               // 视口（逻辑像素）
        float content_w, content_h; // 内容尺寸（逻辑像素）
        float off_x, off_y;         // 滚动偏移（≤0）
        bool  dragging;             // 正在拖拽
        float last_x, last_y;       // 上一帧拖拽点（逻辑坐标）
        float moved;                // 本次按下累计位移（点击/滑动判定）
        bool  show_bar;             // 是否绘制滚动条
        joy_color_t bar_bg, bar_fg;
        bool  saved_had_clip;       // begin/end 保存&还原裁剪
        joy_rect_t saved_clip;
};

#define SCROLLVIEW_BAR_T      3.0f    // 滚动条厚度（像素）
#define SCROLLVIEW_BAR_MIN    24.0f   // 滑块最小长度（像素）
#define SCROLLVIEW_DRAG_EPS   6.0f    // 拖拽判定阈值（逻辑像素）
#define SCROLLVIEW_WHEEL_STEP 48.0f   // 滚轮每格滚动距离（像素）

joy_scrollview_p joy_ui_scrollview_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh)
{
        if (!r2d) return NULL;
        joy_scrollview_p v = (joy_scrollview_p)malloc(sizeof(joy_scrollview_t));
        if (!v) return NULL;
        memset(v, 0, sizeof(joy_scrollview_t));
        v->r2d      = r2d;
        v->view     = jui_frect(r2d, fx, fy, fw, fh);
        v->show_bar = true;
        v->bar_bg   = (joy_color_t){ 205, 205, 212, 255 };
        v->bar_fg   = (joy_color_t){ 90, 90, 98, 255 };
        return v;
}

void joy_ui_scrollview_destroy(joy_scrollview_p v)
{
        if (v) free(v);
}

void joy_ui_scrollview_set_view_px(joy_scrollview_p v, float x, float y, float w, float h)
{
        if (!v) return;
        v->view = (joy_rect_t){ x, y, w, h };
        joy_ui_scrollview_clamp(v);
}

void joy_ui_scrollview_set_content(joy_scrollview_p v, float content_w, float content_h)
{
        if (!v) return;
        v->content_w = content_w;
        v->content_h = content_h;
        joy_ui_scrollview_clamp(v);
}

void joy_ui_scrollview_clamp(joy_scrollview_p v)
{
        if (!v) return;
        const float minx = (v->content_w > v->view.w) ? -(v->content_w - v->view.w) : 0.0f;
        const float miny = (v->content_h > v->view.h) ? -(v->content_h - v->view.h) : 0.0f;
        if (v->off_x > 0.0f) v->off_x = 0.0f;
        if (v->off_x < minx) v->off_x = minx;
        if (v->off_y > 0.0f) v->off_y = 0.0f;
        if (v->off_y < miny) v->off_y = miny;
}

void joy_ui_scrollview_set_offset(joy_scrollview_p v, float ox, float oy)
{
        if (!v) return;
        v->off_x = ox;
        v->off_y = oy;
        joy_ui_scrollview_clamp(v);
}

float joy_ui_scrollview_get_offset_x(joy_scrollview_p v) { return v ? v->off_x : 0.0f; }
float joy_ui_scrollview_get_offset_y(joy_scrollview_p v) { return v ? v->off_y : 0.0f; }

void joy_ui_scrollview_set_show_bar(joy_scrollview_p v, bool show) { if (v) v->show_bar = show; }
bool joy_ui_scrollview_dragging(joy_scrollview_p v) { return v ? v->dragging : false; }

bool joy_ui_scrollview_hit(joy_scrollview_p v, float win_x, float win_y)
{
        if (!v) return false;
        joy_vec2_t p = joy_r2d_coordinates_from_window(v->r2d, win_x, win_y);
        return p.x >= v->view.x && p.x < v->view.x + v->view.w
            && p.y >= v->view.y && p.y < v->view.y + v->view.h;
}

void joy_ui_scrollview_begin(joy_scrollview_p v)
{
        if (!v) return;
        v->saved_had_clip = joy_r2d_get_clip_rect(v->r2d, &v->saved_clip);
        joy_r2d_set_clip_rect(v->r2d, v->view);
}

void joy_ui_scrollview_end(joy_scrollview_p v)
{
        if (!v) return;
        if (v->saved_had_clip) joy_r2d_set_clip_rect(v->r2d, v->saved_clip);
        else                    joy_r2d_reset_clip_stack(v->r2d);
        if (!v->show_bar) return;

        const bool hbar = (v->content_w > v->view.w);
        const bool vbar = (v->content_h > v->view.h);
        // 横条贴视口底边、竖条贴右边；两者同存时各让出对方厚度，避免角上重叠
        if (hbar) {
                const float x0 = v->view.x;
                const float w0 = v->view.w - (vbar ? (SCROLLVIEW_BAR_T + 1.0f) : 0.0f);
                const float y0 = v->view.y + v->view.h - SCROLLVIEW_BAR_T - 1.0f;
                joy_r2d_set_color(v->r2d, v->bar_bg.r, v->bar_bg.g, v->bar_bg.b, v->bar_bg.a);
                joy_math_shape_draw_rectangle(v->r2d, "fill", (joy_rect_t){ x0, y0, w0, SCROLLVIEW_BAR_T });
                float bw = w0 * (v->view.w / v->content_w);
                if (bw < SCROLLVIEW_BAR_MIN) bw = SCROLLVIEW_BAR_MIN;
                if (bw > w0) bw = w0;
                const float k = -v->off_x / (v->content_w - v->view.w);   // 0..1
                joy_r2d_set_color(v->r2d, v->bar_fg.r, v->bar_fg.g, v->bar_fg.b, v->bar_fg.a);
                joy_math_shape_draw_rectangle(v->r2d, "fill",
                        (joy_rect_t){ x0 + k * (w0 - bw), y0, bw, SCROLLVIEW_BAR_T });
        }
        if (vbar) {
                const float y0 = v->view.y;
                const float h0 = v->view.h - (hbar ? (SCROLLVIEW_BAR_T + 1.0f) : 0.0f);
                const float x0 = v->view.x + v->view.w - SCROLLVIEW_BAR_T - 1.0f;
                joy_r2d_set_color(v->r2d, v->bar_bg.r, v->bar_bg.g, v->bar_bg.b, v->bar_bg.a);
                joy_math_shape_draw_rectangle(v->r2d, "fill", (joy_rect_t){ x0, y0, SCROLLVIEW_BAR_T, h0 });
                float bh = h0 * (v->view.h / v->content_h);
                if (bh < SCROLLVIEW_BAR_MIN) bh = SCROLLVIEW_BAR_MIN;
                if (bh > h0) bh = h0;
                const float k = -v->off_y / (v->content_h - v->view.h);   // 0..1
                joy_r2d_set_color(v->r2d, v->bar_fg.r, v->bar_fg.g, v->bar_fg.b, v->bar_fg.a);
                joy_math_shape_draw_rectangle(v->r2d, "fill",
                        (joy_rect_t){ x0, y0 + k * (h0 - bh), SCROLLVIEW_BAR_T, bh });
        }
}

bool joy_ui_scrollview_process_event(joy_scrollview_p v, const void* ev)
{
        if (!v || !ev) return false;
        const SDL_Event* e = (const SDL_Event*)ev;

        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (!joy_ui_scrollview_hit(v, e->button.x, e->button.y)) {
                        v->dragging = false;              // 视口外按下：不接管
                        return false;
                }
                joy_vec2_t p = joy_r2d_coordinates_from_window(v->r2d, e->button.x, e->button.y);
                v->dragging = true;
                v->last_x  = p.x;
                v->last_y  = p.y;
                v->moved   = 0.0f;
                return false;                             // 不消费：子控件仍可拿到按下反馈
        }
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                if (!v->dragging) return false;
                joy_vec2_t p = joy_r2d_coordinates_from_window(v->r2d, e->motion.x, e->motion.y);
                const float dx = p.x - v->last_x;
                const float dy = p.y - v->last_y;
                v->last_x = p.x;
                v->last_y = p.y;
                v->off_x += dx;
                v->off_y += dy;
                joy_ui_scrollview_clamp(v);
                v->moved += (dx < 0.0f ? -dx : dx) + (dy < 0.0f ? -dy : dy);
                return v->moved > SCROLLVIEW_DRAG_EPS;     // 已进入滚动 → 调用方忽略点击
        }
        if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                const bool was_drag = v->dragging && (v->moved > SCROLLVIEW_DRAG_EPS);
                v->dragging = false;
                return was_drag;                          // 本次是滑动 → 调用方不要当点击
        }
        if (e->type == SDL_EVENT_MOUSE_WHEEL) {
                if (!joy_ui_scrollview_hit(v, e->wheel.mouse_x, e->wheel.mouse_y)) return false;
                if (v->content_h > v->view.h)
                        v->off_y -= e->wheel.y * SCROLLVIEW_WHEEL_STEP;   // 纵向优先
                else if (v->content_w > v->view.w)
                        v->off_x -= e->wheel.x * SCROLLVIEW_WHEEL_STEP;
                else
                        return false;                     // 没有可滚方向：不消费
                joy_ui_scrollview_clamp(v);
                return true;
        }
        return false;
}

void joy_ui_inputbox_process_event(joy_inputbox_p ti, const void* ev)
{
        if (!ti || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_ui_inputbox_is_hovered(ti, e->motion.x, e->motion.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_ui_inputbox_is_pressed(ti, e->button.x, e->button.y);
        }
        else if (e->type == SDL_EVENT_TEXT_INPUT) {
                if (ti->focused) {
                        joy_ui_inputbox_insert(ti, e->text.text, (int)strlen(e->text.text));
                }
        }
        else if (e->type == SDL_EVENT_KEY_DOWN) {
                if (!ti->focused) return;
                switch (e->key.scancode) {
                case SDL_SCANCODE_BACKSPACE: joy_ui_inputbox_backspace(ti); break;
                case SDL_SCANCODE_DELETE:    joy_ui_inputbox_delete(ti); break;
                case SDL_SCANCODE_LEFT:
                        if (e->key.mod & SDL_KMOD_SHIFT) joy_ui_inputbox_select_left(ti);
                        else joy_ui_inputbox_cursor_left(ti);
                        break;
                case SDL_SCANCODE_RIGHT:
                        if (e->key.mod & SDL_KMOD_SHIFT) joy_ui_inputbox_select_right(ti);
                        else joy_ui_inputbox_cursor_right(ti);
                        break;
                case SDL_SCANCODE_HOME:
                        if (e->key.mod & SDL_KMOD_SHIFT) joy_ui_inputbox_select_home(ti);
                        else joy_ui_inputbox_cursor_home(ti);
                        break;
                case SDL_SCANCODE_END:
                        if (e->key.mod & SDL_KMOD_SHIFT) joy_ui_inputbox_select_end(ti);
                        else joy_ui_inputbox_cursor_end(ti);
                        break;
                case SDL_SCANCODE_A:
                        if (e->key.mod & SDL_KMOD_CTRL) joy_ui_inputbox_select_all(ti);
                        break;
                case SDL_SCANCODE_V:
                        if (e->key.mod & SDL_KMOD_CTRL) joy_ui_inputbox_paste(ti);
                        break;
                case SDL_SCANCODE_C:
                        if (e->key.mod & SDL_KMOD_CTRL) joy_ui_inputbox_copy(ti);
                        break;
                case SDL_SCANCODE_X:
                        if (e->key.mod & SDL_KMOD_CTRL) joy_ui_inputbox_cut(ti);
                        break;
                default: break;
                }
        }
}

void joy_ui_slider_process_event(joy_slider_p s, const void* ev)
{
        if (!s || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_ui_slider_is_hovered(s, e->motion.x, e->motion.y);
                if (s->is_dragging) joy_ui_slider_drag_move(s, e->motion.x, e->motion.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_ui_slider_is_pressed(s, e->button.x, e->button.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                s->is_dragging = false;
        }
}

void joy_ui_tooltip_process_event(joy_tooltip_p t, const void* ev)
{
        if (!t || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_ui_tooltip_set_position(t, e->motion.x + 12, e->motion.y - 10);
        }
}

void joy_ui_popup_process_event(joy_popup_p p, const void* ev)
{
        if (!p || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = e->button.x, my = e->button.y;
                if (joy_ui_popup_is_close_clicked(p, mx, my)) {
                        joy_ui_popup_set_visible(p, false);
                } else if (joy_ui_popup_is_title_clicked(p, mx, my)) {
                        p->is_dragging = true;
                }
        }
        else if (e->type == SDL_EVENT_MOUSE_MOTION) {
                if (p->is_dragging) {
                        joy_ui_popup_drag_move(p, e->motion.x, e->motion.y);
                }
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                p->is_dragging = false;
        }
}

void joy_ui_combobox_process_event(joy_combobox_p cb, const void* ev)
{
        if (!cb || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = e->button.x, my = e->button.y;
                if (joy_ui_combobox_is_pressed(cb, mx, my)) {
                        // 点击主框 → 切换展开
                        joy_ui_combobox_set_open(cb, !joy_ui_combobox_is_open(cb));
                } else if (joy_ui_combobox_is_open(cb)) {
                        // 下拉已展开：检查是否点击了某个 item
                        int hit = joy_ui_combobox_hit_item(cb, mx, my);
                        if (hit >= 0) {
                                joy_ui_combobox_set_selected(cb, hit);
                                joy_ui_combobox_set_open(cb, false);
                        } else {
                                // 点击下拉外部 → 收起
                                joy_ui_combobox_set_open(cb, false);
                        }
                }
        }
}

void joy_ui_checkbox_process_event(joy_checkbox_p cb, const void* ev)
{
        if (!cb || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_ui_checkbox_is_pressed(cb, e->button.x, e->button.y);
        }
}

void joy_ui_radio_process_event(joy_radio_p rb, const void* ev)
{
        if (!rb || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_ui_radio_is_pressed(rb, e->button.x, e->button.y);
        }
}

void joy_ui_datagrid_process_event(joy_datagrid_p dg, const void* ev)
{
        (void)dg;
        (void)ev;
        /* datagrid 需要配合外部状态管理 */
}

void joy_input_joystick_process_event(joy_joystick_p js, const void* ev)
{
        if (!js || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_FINGER_DOWN || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float x = (e->type == SDL_EVENT_FINGER_DOWN) ? e->tfinger.x * 800 : e->button.x;
                float y = (e->type == SDL_EVENT_FINGER_DOWN) ? e->tfinger.y * 600 : e->button.y;
                joy_input_joystick_touch_down(js, x, y);
        }
        else if (e->type == SDL_EVENT_FINGER_MOTION || e->type == SDL_EVENT_MOUSE_MOTION) {
                if (joy_input_joystick_is_dragging(js)) {
                        float x = (e->type == SDL_EVENT_FINGER_MOTION) ? e->tfinger.x * 800 : e->motion.x;
                        float y = (e->type == SDL_EVENT_FINGER_MOTION) ? e->tfinger.y * 600 : e->motion.y;
                        joy_input_joystick_touch_move(js, x, y);
                }
        }
        else if (e->type == SDL_EVENT_FINGER_UP || e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                joy_input_joystick_touch_up(js);
        }
}

/* ===== 通用函数：将 SDL 事件路由到控件数组 ===== */
void joy_ui_process_widget_event(void** widgets, int count, const void* event)
{
        if (!widgets || !event) return;
        const SDL_Event* e = (const SDL_Event*)event;
        for (int i = 0; i < count; i++) {
                if (!widgets[i]) continue;
                /* 通过指针的指针判断控件类型（取前 4 字节作为类型标记） */
                /* 或者由调用方传入类型数组，更安全的方式是由脚本层直接调用各自 process_event */
                /* 此函数保持通用，实际分发给脚本层 */
                (void)e;
        }
}

/* =================================================================
 * 栅格布局 jlayout
 * 纯几何计算 + 可选标签绘制，不持有任何控件；矩形以“逻辑像素”表示
 * （场景用 s = lh/320 把设计像素换算成逻辑像素后传入）。
 * ================================================================= */
struct joy_layout_grid {
        float ox, oy, width, row_h, pad;
        int   ncols;
        float* col_fracs;
        int   row;
};

joy_layout_grid_t* joy_ui_layout_grid_create(float ox, float oy, float width, float row_h,
                                    int ncols, const float* col_fracs, float pad)
{
        if (ncols <= 0 || !col_fracs) return NULL;
        joy_layout_grid_t* g = (joy_layout_grid_t*)malloc(sizeof(joy_layout_grid_t));
        if (!g) return NULL;
        g->col_fracs = (float*)malloc(sizeof(float) * (size_t)ncols);
        if (!g->col_fracs) { free(g); return NULL; }
        for (int i = 0; i < ncols; i++) g->col_fracs[i] = col_fracs[i];
        g->ox = ox; g->oy = oy; g->width = width; g->row_h = row_h; g->pad = pad;
        g->ncols = ncols; g->row = 0;
        return g;
}

void joy_ui_layout_grid_destroy(joy_layout_grid_t* g)
{
        if (!g) return;
        if (g->col_fracs) free(g->col_fracs);
        free(g);
}

void joy_ui_layout_grid_next_row(joy_layout_grid_t* g)
{
        if (g) g->row++;
}

void joy_ui_layout_grid_set_row(joy_layout_grid_t* g, int row)
{
        if (g) g->row = row;
}

void joy_ui_layout_grid_cell(joy_layout_grid_t* g, int col, float* x, float* y, float* w, float* h)
{
        if (!g || col < 0 || col >= g->ncols) {
                if (x) *x = 0.0f; if (y) *y = 0.0f; if (w) *w = 0.0f; if (h) *h = 0.0f;
                return;
        }
        float colx = g->ox;
        for (int i = 0; i < col; i++) colx += g->col_fracs[i] * g->width;
        float colw = g->col_fracs[col] * g->width;
        if (x) *x = colx + g->pad;
        if (y) *y = g->oy + (float)g->row * g->row_h + g->pad;
        if (w) *w = colw - 2.0f * g->pad;
        if (h) *h = g->row_h - 2.0f * g->pad;
        if (w && *w < 0.0f) *w = 0.0f;
        if (h && *h < 0.0f) *h = 0.0f;
}

void joy_ui_layout_grid_label(joy_layout_grid_t* g, joy_r2d_p r2d, joy_font_p font,
                        const char* text, int n, joy_color_t c, int col,
                        float* cx, float* cy, float* cw, float* ch)
{
        float lx, ly, lw, lh;
        joy_ui_layout_grid_cell(g, col, &lx, &ly, &lw, &lh);
        int tw = 0, th = 0;
        joy_r2d_jtext_measurex(r2d, font, text, n, c, &tw, &th);
        int wl = 0, hl = 0;
        joy_r2d_get_logical_presentation(r2d, &wl, &hl, NULL);
        float text_x = lx;                                   // 左对齐
        float text_y = ly + ((lh - (float)th) * 0.5f);       // 垂直居中
        joy_r2d_jtext_printf(r2d, font, text, n, c,
                     (wl > 0 ? text_x / (float)wl : 0.0f),
                     (hl > 0 ? text_y / (float)hl : 0.0f));
        if (cx) *cx = lx; if (cy) *cy = ly; if (cw) *cw = lw; if (ch) *ch = lh;
}

/* 在当前行创建 n 个等宽按钮（横向按钮条/菜单），各占一列。
 * 按钮尺寸取单元格的 0.86×0.7 居中，避免贴边；指针写入 out[]（调用方分配 n 个）。
 * 列数受栅格 ncols 限制（多出的 label 被忽略）。创建后自动换到下一行。 */
void joy_ui_layout_grid_button_row(joy_layout_grid_t* g, joy_r2d_p r2d, joy_font_p font,
                             const char** labels, int n, joy_button_p* out)
{
        if (!g || !r2d || n <= 0 || !out) return;
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        const float LW = (lw > 0) ? (float)lw : 960.0f;
        const float LH = (lh > 0) ? (float)lh : 640.0f;
        const int cnt = (n < g->ncols) ? n : g->ncols;
        for (int i = 0; i < cnt; i++) {
                float cx, cy, cw, ch;
                joy_ui_layout_grid_cell(g, i, &cx, &cy, &cw, &ch);
                float bw = cw * 0.86f, bh = ch * 0.7f;
                float bx = cx + (cw - bw) * 0.5f, by = cy + (ch - bh) * 0.5f;
                joy_button_p b = joy_ui_button_create(r2d, bx / LW, by / LH, bw / LW, bh / LH);
                if (labels && labels[i])
                        joy_ui_button_set_textx(b, font, labels[i], (int)strlen(labels[i]), (joy_color_t){ 0, 0, 0, 255 });
                out[i] = b;
        }
        joy_ui_layout_grid_next_row(g);
}
