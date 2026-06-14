/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: ui.h
Description: ui库
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#ifndef UI_H
#define UI_H
#include "external/klib/kvec.h"
#include "jr2d.h"
#include "jmath.h"
#include "joy.h"

typedef kvec_t(char*) dg_row_t;

typedef struct label {
	jr2d_p renderer;
	vec2_t position;
	text_texture_p text;
	jcolor_t color;
}label_t, * label_p;


typedef struct button button_t, * button_p;

struct button {
	jr2d_p renderer;   // 渲染器
	vec2_t position;      // 位置
	float width;              // 宽度
	float height;             // 高度
	jcolor_t normal_color;   // 正常状态颜色
	jcolor_t hover_color;    // 悬停状态颜色
	jcolor_t pressed_color;  // 按下状态颜色
	vec2_t text_pos;      // 文本相对位置
	image_p image;            // 按钮正常纹理（可选）
	image_p pressed_image;    // 按钮按下纹理（可选）
	text_texture_p text;      // 按钮文件纹理
	bool is_hovered;          // 内部状态：由 is_hovered() / is_pressed() 设置
	bool is_pressed;
};

typedef struct combobox_item {
	jrect_t rect;
	text_texture_p text;
	bool is_selected;
} combobox_item_t, * combobox_item_p;

typedef struct combobox {
	jrect_t main_rect;
	jrect_t dropdown_rect;
	combobox_item_t items[20];
	int item_count;
	int item_height;
	int selected_index;
	bool is_open;
	SDL_Texture* text;
	jcolor_t bg_color;
	jcolor_t text_color;
	jcolor_t hover_color;
	jr2d_p renderer;
} combobox_t, * combobox_p;

typedef struct datagrid {
	jr2d_p renderer;
	int x, y;              // 位置
	int width, height;     // 尺寸
	int row_height;        // 行高
	int col_count;         // 列数
	int row_count;         // 行数（包括表头）
	int visible_rows;      // 可见行数
	int scroll_offset;     // 滚动偏移
	int selected_row;      // 选中的行
	text_texture_p headers[512];        // 表头
	kvec_t(dg_row_t) data;              // 表格数据（klib 动态数组 of dg_row_t）
	int* col_widths;       // 列宽
	bool has_header;   // 是否有表头
	font_p font;        // 字体
	jcolor_t grid_color;
	jcolor_t bg_color;
	jcolor_t text_color;
	jcolor_t header_bg_color;
	jcolor_t selected_color;
} datagrid_t, * datagrid_p;

typedef struct radio_button {
	int x, y;                  // 位置
	int size;                  // 大小
	bool checked;              // 是否选中
	char* label;               // 标签文本
	int group_id;              // 所属组ID
	jrect_t hit_area;         // 点击区域
	bool hover;                // 鼠标悬停状态
	SDL_FColor r1_color;
	SDL_FColor r2_color;
	SDL_FColor r3_color;
} radio_button_t, * radio_button_p;

typedef struct radio_button_group {
	radio_button_t** buttons;     // 按钮数组
	int capacity; // 容量
	int count;                 // 按钮数量
	int selected_index;        // 当前选中的按钮索引
} radio_button_group_t, * radio_button_group_p;


// 复选框状态枚举
typedef enum checkbox_state {
	JOY_CHECKBOX_UNCHECKED,
	JOY_CHECKBOX_CHECKED,
	JOY_CHECKBOX_INDETERMINATE // 不确定状态（用于三态复选框）
} checkbox_state_t, * checkbox_state_p;

typedef struct checkbox {
	int x, y;                   // 位置
	int size;                   // 大小
	checkbox_state_t state;     // 当前状态
	char* label;                // 标签文本
	jcolor_t box_color;         // 框的颜色
	jcolor_t check_color;       // 勾选标记的颜色
	jcolor_t text_color;        // 文本颜色
	jcolor_t hover_color;       // 悬停颜色
	bool hover;                 // 悬停状态
	bool enabled;               // 是否启用
	jrect_t hit_area;           // 点击区域
} checkbox_t, * checkbox_p;

typedef struct joystick joystick_t, * joystick_p;

typedef struct textinput {
	jr2d_p renderer;
	jrect_t rect;               // 位置和大小
	jcolor_t bg_color;           // 背景色
	jcolor_t border_color;       // 边框色
	jcolor_t text_color;         // 文字色
	jcolor_t cursor_color;       // 光标色
	jcolor_t sel_color;          // 选中背景色
	jcolor_t focus_border_color; // 聚焦时边框色
	font_p font;                  // 字体
	char* buffer;                 // 文本缓冲区
	int buf_len;                  // 缓冲区大小
	int text_len;                 // 当前文本长度
	int cursor_pos;               // 光标位置（字节偏移）
	int sel_start;                // 选中起始（-1 表示无选中）
	int sel_end;                  // 选中结束
	int scroll_offset;            // 文本滚动偏移（像素）
	bool focused;                 // 是否聚焦
	bool hovered;                 // 鼠标悬停
	Uint64 cursor_timer;          // 光标闪烁计时
	bool cursor_visible;          // 光标是否可见
	void (*on_submit)(struct textinput* ti, void* userdata);
	void (*on_change)(struct textinput* ti, void* userdata);
	void* cb_userdata;
} textinput_t, * textinput_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API label_p label_create(jr2d_p renderer, font_p fontinfo, const int* codepoints, int len);
	JOY_API void label_destroy(label_p label);
	JOY_API void label_set_text(label_p label, font_p fontinfo, const int* codepoints, int len, jcolor_t color);
	JOY_API void label_draw(label_p label);

	JOY_API button_p button_create(jr2d_p renderer, float width, float height);
	JOY_API void button_destroy(button_p btn);
	JOY_API void button_set_position(button_p btn, float x, float y);
	JOY_API void button_set_normal_color(button_p btn, jcolor_t color);
	JOY_API void button_set_hover_color(button_p btn, jcolor_t color);
	JOY_API void button_set_pressed_color(button_p btn, jcolor_t color);
	JOY_API void button_set_textx(button_p btn, font_p fontinfo, const char* str, int len, jcolor_t color);
	JOY_API void button_set_text(button_p btn, font_p fontinfo, const int* codepoints, int len, jcolor_t color);
	JOY_API bool button_is_hovered(button_p btn, float x, float y);
	JOY_API bool button_is_pressed(button_p btn, float x, float y);
	JOY_API void button_set_hovered(button_p btn, bool v);
	JOY_API void button_set_pressed(button_p btn, bool v);
	JOY_API void button_draw(button_p btn);

	JOY_API combobox_p combobox_create(jr2d_p renderer, jrect_t rect);
	JOY_API void combobox_destroy(combobox_p combobox);
	JOY_API void combobox_set_text(combobox_p combobox, const char* filename, int fontsize,
		const int* codepoints, int len, jcolor_t color);
	JOY_API void combobox_add_item(combobox_p combobox, const char* filename, int fontsize, 
		const int* codepoints, int len, jcolor_t color);
	JOY_API bool combobox_is_pressed(combobox_p combobox, float x, float y);
	JOY_API bool combobox_is_open(combobox_p combobox);
	JOY_API void combobox_set_open(combobox_p combobox, bool v);
	JOY_API int combobox_hit_item(combobox_p combobox, float x, float y);
	JOY_API void combobox_set_selected(combobox_p combobox, int index);
	JOY_API int combobox_get_selected(combobox_p combobox);
	JOY_API void combobox_draw(combobox_p combobox);

	JOY_API datagrid_p datagrid_create(jr2d_p renderer, SDL_Rect rect, int col_count, bool has_header, void* font);
	JOY_API void datagrid_setheaders(datagrid_p grid, const char* headers);
	JOY_API void datagrid_setheader(datagrid_p grid, int index, const char *header);
	JOY_API void datagrid_addrow(datagrid_p grid, const char** row_data);
	JOY_API void datagrid_setcell(datagrid_p grid, int row_index, int col_index, const char* data);
	JOY_API void datagrid_destroy(datagrid_p grid);
	JOY_API void datagrid_draw(datagrid_p grid, jr2d_p renderer);
	JOY_API int datagrid_hit_row(datagrid_p grid, float x, float y);
	JOY_API void datagrid_scroll(datagrid_p grid, int lines);
	JOY_API void datagrid_set_selected_row(datagrid_p grid, int row);

	JOY_API radio_button_p radio_button_create(int x, int y, int size, const char* label, int group_id);
	JOY_API void radio_button_destroy(radio_button_p radio);

	JOY_API radio_button_group_p radio_button_group_create();
	JOY_API void radio_button_group_destroy(radio_button_group_p group);
	JOY_API void radio_button_group_add_button(radio_button_group_p group, radio_button_p radio);
	JOY_API void radio_button_group_set_selected(radio_button_group_p group, int index);
	JOY_API bool radio_button_is_pressed(radio_button_p radio, float x, float y);
	JOY_API void radio_button_set_hovered(radio_button_p radio, bool v);
	JOY_API void radio_button_group_draw(radio_button_group_p group, jr2d_p renderer);

	JOY_API checkbox_p checkbox_create(int x, int y, int size, const char* label);
	JOY_API void checkbox_destroy(checkbox_p checkbox);
	JOY_API void checkbox_set_state(checkbox_p checkbox, checkbox_state_t state);
	JOY_API void checkbox_toggle(checkbox_p checkbox);
	JOY_API bool checkbox_is_pressed(checkbox_p cb, float x, float y);
	JOY_API void checkbox_set_hovered(checkbox_p cb, bool v);
	JOY_API void checkbox_draw(checkbox_p cb, jr2d_p renderer, void* font);

	JOY_API joystick_p joystick_create(jr2d_p renderer, float x, float y, float radius);
	JOY_API void joystick_destroy(joystick_p joystick);
	JOY_API void joystick_draw(joystick_p joystick);
	JOY_API void joystick_reset(joystick_p joystick);
	JOY_API void joystick_set_position(joystick_p joystick, float x, float y);
	JOY_API bool joystick_is_dragging(joystick_p joystick);
	JOY_API void joystick_touch_down(joystick_p joystick, float x, float y);
	JOY_API void joystick_touch_move(joystick_p joystick, float x, float y);
	JOY_API void joystick_touch_up(joystick_p joystick);
	JOY_API vec2_t joystick_get_direction(joystick_p joystick);
	JOY_API float joystick_get_magnitude(joystick_p joystick);

	JOY_API textinput_p textinput_create(jr2d_p renderer, jrect_t rect, font_p font);
	JOY_API void textinput_destroy(textinput_p ti);
	JOY_API void textinput_set_text(textinput_p ti, const char* text);
	JOY_API const char* textinput_get_text(textinput_p ti);
	JOY_API void textinput_set_callback(textinput_p ti, void (*submit)(textinput_p, void*), void (*change)(textinput_p, void*), void* userdata);
	JOY_API void textinput_draw(textinput_p ti);
	JOY_API bool textinput_is_pressed(textinput_p ti, float x, float y);
	JOY_API bool textinput_is_hovered(textinput_p ti, float x, float y);
	JOY_API void textinput_focus(textinput_p ti);
	JOY_API void textinput_defocus(textinput_p ti);
	JOY_API void textinput_insert(textinput_p ti, const char* s, int len);
	JOY_API void textinput_backspace(textinput_p ti);
	JOY_API void textinput_delete(textinput_p ti);
	JOY_API void textinput_cursor_left(textinput_p ti);
	JOY_API void textinput_cursor_right(textinput_p ti);
	JOY_API void textinput_cursor_home(textinput_p ti);
	JOY_API void textinput_cursor_end(textinput_p ti);
	JOY_API void textinput_select_left(textinput_p ti);
	JOY_API void textinput_select_right(textinput_p ti);
	JOY_API void textinput_select_home(textinput_p ti);
	JOY_API void textinput_select_end(textinput_p ti);
	JOY_API void textinput_select_all(textinput_p ti);
	JOY_API void textinput_paste(textinput_p ti);
	JOY_API void textinput_copy(textinput_p ti);
	JOY_API void textinput_cut(textinput_p ti);
	JOY_API bool textinput_is_focused(textinput_p ti);

typedef struct slider {
	jr2d_p renderer;
	jrect_t track;             // 滑轨区域
	float min, max;              // 取值范围
	float value;                 // 当前值
	float thumb_width;           // 滑块宽度（高度 = track.h）
	bool is_dragging;            // 是否在拖动
	jcolor_t track_color;       // 滑轨背景色
	jcolor_t fill_color;        // 已填充部分颜色
	jcolor_t thumb_color;       // 滑块颜色
	jcolor_t thumb_hover_color; // 滑块悬停颜色
	bool hovered;                // 滑块是否悬停
} slider_t, * slider_p;

JOY_API slider_p slider_create(jr2d_p renderer, float x, float y, float w, float h, float min, float max, float initial);
JOY_API void slider_destroy(slider_p s);
JOY_API void slider_set_value(slider_p s, float v);
JOY_API float slider_get_value(slider_p s);
JOY_API void slider_set_range(slider_p s, float min, float max);
JOY_API void slider_draw(slider_p s);
JOY_API bool slider_is_pressed(slider_p s, float x, float y);
JOY_API void slider_set_pressed(slider_p s, bool v);
JOY_API void slider_drag_move(slider_p s, float x, float y);
JOY_API bool slider_is_hovered(slider_p s, float x, float y);
JOY_API void slider_set_hovered(slider_p s, bool v);

#define POPUP_MAX_TITLE 128
#define POPUP_MAX_CONTENT 2048

typedef struct popup {
	jr2d_p renderer;
	float x, y, w, h;
	float title_height;         // title bar height (set in draw)
	float padding;
	float close_size;
	jcolor_t bg_color;
	jcolor_t border_color;
	jcolor_t title_bg_color;
	jcolor_t content_color;
	jcolor_t close_color;
	jcolor_t close_hover_color;
	jcolor_t title_text_color;
	bool visible;
	bool closeable;
	char title[POPUP_MAX_TITLE];
	char content[POPUP_MAX_CONTENT];
	bool close_hovered;
	bool is_dragging;
	float drag_off_x, drag_off_y;
} popup_t, * popup_p;

JOY_API popup_p popup_create(jr2d_p renderer, float w, float h);
JOY_API void popup_destroy(popup_p p);
JOY_API void popup_set_title(popup_p p, const char* text, int len);
JOY_API void popup_set_content(popup_p p, const char* text, int len);
JOY_API void popup_set_position(popup_p p, float x, float y);
JOY_API void popup_set_visible(popup_p p, bool v);
JOY_API void popup_set_closeable(popup_p p, bool v);
JOY_API void popup_draw(popup_p p, font_p title_font, font_p content_font);
JOY_API bool popup_is_close_clicked(popup_p p, float mx, float my);
JOY_API bool popup_is_title_clicked(popup_p p, float mx, float my);
JOY_API void popup_drag_move(popup_p p, float mx, float my);
JOY_API bool popup_is_inside(popup_p p, float mx, float my);

#define TOOLTIP_MAX_TEXT 256

typedef struct tooltip {
	jr2d_p renderer;
	float x, y;
	char text[TOOLTIP_MAX_TEXT];
	int text_len;
	jcolor_t bg_color;
	jcolor_t border_color;
	jcolor_t text_color;
	float padding;
	bool visible;
} tooltip_t, * tooltip_p;

JOY_API tooltip_p tooltip_create(jr2d_p renderer, const char* text, int len);
JOY_API void tooltip_destroy(tooltip_p t);
JOY_API void tooltip_set_text(tooltip_p t, const char* text, int len);
JOY_API void tooltip_set_position(tooltip_p t, float x, float y);
JOY_API void tooltip_set_visible(tooltip_p t, bool v);
JOY_API void tooltip_draw(tooltip_p t, font_p font);

#ifdef __cplusplus
}
#endif

#endif // !UI_H
