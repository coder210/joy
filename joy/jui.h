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
#include "jtextures.h"
#include "jtext.h"

typedef struct label {
	SDL_Renderer* renderer;
	SDL_FPoint position;
	text_texture_p text;
	SDL_Color color;
}label_t, * label_p;


typedef struct button button_t, * button_p;

struct button {
	SDL_Renderer* renderer;   // 渲染器
	SDL_FPoint position;      // 位置
	float width;              // 宽度
	float height;             // 高度
	SDL_Color normal_color;   // 正常状态颜色
	SDL_Color hover_color;    // 悬停状态颜色
	SDL_Color pressed_color;  // 按下状态颜色
	SDL_FPoint text_pos;      // 文本相对位置
	image_p image;            // 按钮正常纹理（可选）
	image_p pressed_image;    // 按钮按下纹理（可选）
	text_texture_p text;      // 按钮文件纹理
	bool is_hovered;          // 内部状态：由 is_hovered() / is_pressed() 设置
	bool is_pressed;
};

typedef struct combobox_item {
	SDL_FRect rect;
	text_texture_p text;
	bool is_selected;
} combobox_item_t, * combobox_item_p;

typedef struct combobox {
	SDL_FRect main_rect;
	SDL_FRect dropdown_rect;
	combobox_item_t items[20];
	int item_count;
	int item_height;
	int selected_index;
	bool is_open;
	SDL_Texture* text;
	SDL_Color bg_color;
	SDL_Color text_color;
	SDL_Color hover_color;
	SDL_Renderer* renderer;
} combobox_t, * combobox_p;

typedef struct datagrid {
	SDL_Renderer* renderer;
	int x, y;              // 位置
	int width, height;     // 尺寸
	int row_height;        // 行高
	int col_count;         // 列数
	int row_count;         // 行数（包括表头）
	int visible_rows;      // 可见行数
	int scroll_offset;     // 滚动偏移
	int selected_row;      // 选中的行
	text_texture_p headers[512];        // 表头
	int*** data;          // 表格数据
	int* col_widths;       // 列宽
	bool has_header;   // 是否有表头
	font_p font;        // 字体
	SDL_Color grid_color;
	SDL_Color bg_color;
	SDL_Color text_color;
	SDL_Color header_bg_color;
	SDL_Color selected_color;
} datagrid_t, * datagrid_p;

typedef struct radio_button {
	int x, y;                  // 位置
	int size;                  // 大小
	bool checked;              // 是否选中
	char* label;               // 标签文本
	int group_id;              // 所属组ID
	SDL_FRect hit_area;         // 点击区域
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
	SDL_Color box_color;         // 框的颜色
	SDL_Color check_color;       // 勾选标记的颜色
	SDL_Color text_color;        // 文本颜色
	SDL_Color hover_color;       // 悬停颜色
	bool hover;                 // 悬停状态
	bool enabled;               // 是否启用
	SDL_FRect hit_area;           // 点击区域
} checkbox_t, * checkbox_p;

typedef struct joystick joystick_t, * joystick_p;

typedef struct textinput {
	SDL_Renderer* renderer;
	SDL_FRect rect;               // 位置和大小
	SDL_Color bg_color;           // 背景色
	SDL_Color border_color;       // 边框色
	SDL_Color text_color;         // 文字色
	SDL_Color cursor_color;       // 光标色
	SDL_Color focus_border_color; // 聚焦时边框色
	font_p font;                  // 字体
	char* buffer;                 // 文本缓冲区
	int buf_len;                  // 缓冲区大小
	int text_len;                 // 当前文本长度
	int cursor_pos;               // 光标位置（字节偏移）
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

	JOY_API label_p label_create(SDL_Renderer* renderer, font_p fontinfo, const int* codepoints, int len);
	JOY_API void label_destroy(label_p label);
	JOY_API void label_set_text(label_p label, font_p fontinfo, const int* codepoints, int len, SDL_Color color);
	JOY_API void label_draw(label_p label);

	JOY_API button_p button_create(SDL_Renderer* renderer, float width, float height);
	JOY_API void button_destroy(button_p btn);
	JOY_API void button_set_position(button_p btn, float x, float y);
	JOY_API void button_set_normal_color(button_p btn, SDL_Color color);
	JOY_API void button_set_hover_color(button_p btn, SDL_Color color);
	JOY_API void button_set_pressed_color(button_p btn, SDL_Color color);
	JOY_API void button_set_textx(button_p btn, font_p fontinfo, const char* str, int len, SDL_Color color);
	JOY_API void button_set_text(button_p btn, font_p fontinfo, const int* codepoints, int len, SDL_Color color);
	JOY_API bool button_is_hovered(button_p btn, float x, float y);
	JOY_API bool button_is_pressed(button_p btn, float x, float y);
	JOY_API void button_set_hovered(button_p btn, bool v);
	JOY_API void button_set_pressed(button_p btn, bool v);
	JOY_API void button_draw(button_p btn);

	JOY_API combobox_p combobox_create(SDL_Renderer* renderer, SDL_FRect rect);
	JOY_API void combobox_destroy(combobox_p combobox);
	JOY_API void combobox_set_text(combobox_p combobox, const char* filename, int fontsize,
		const int* codepoints, int len, SDL_Color color);
	JOY_API void combobox_add_item(combobox_p combobox, const char* filename, int fontsize, 
		const int* codepoints, int len, SDL_Color color);
	JOY_API bool combobox_is_pressed(combobox_p combobox, float x, float y);
	JOY_API bool combobox_is_open(combobox_p combobox);
	JOY_API void combobox_set_open(combobox_p combobox, bool v);
	JOY_API int combobox_hit_item(combobox_p combobox, float x, float y);
	JOY_API void combobox_set_selected(combobox_p combobox, int index);
	JOY_API int combobox_get_selected(combobox_p combobox);
	JOY_API void combobox_draw(combobox_p combobox);

	JOY_API datagrid_p datagrid_create(SDL_Renderer* renderer, SDL_Rect rect, int col_count, bool has_header, void* font);
	JOY_API void datagrid_setheaders(datagrid_p grid, const char* headers);
	JOY_API void datagrid_setheader(datagrid_p grid, int index, const char *header);
	JOY_API void datagrid_addrow(datagrid_p grid, const char** row_data);
	JOY_API void datagrid_setcell(datagrid_p grid, int row_index, int col_index, const char* data);
	JOY_API void datagrid_destroy(datagrid_p grid);
	JOY_API void datagrid_draw(datagrid_p grid, SDL_Renderer* renderer);
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
	JOY_API void radio_button_group_draw(radio_button_group_p group, SDL_Renderer* renderer);

	JOY_API checkbox_p checkbox_create(int x, int y, int size, const char* label);
	JOY_API void checkbox_destroy(checkbox_p checkbox);
	JOY_API void checkbox_set_state(checkbox_p checkbox, checkbox_state_t state);
	JOY_API void checkbox_toggle(checkbox_p checkbox);
	JOY_API bool checkbox_is_pressed(checkbox_p cb, float x, float y);
	JOY_API void checkbox_set_hovered(checkbox_p cb, bool v);
	JOY_API void checkbox_draw(checkbox_p cb, SDL_Renderer* renderer, void* font);

	JOY_API joystick_p joystick_create(SDL_Renderer* renderer, float x, float y, float radius);
	JOY_API void joystick_destroy(joystick_p joystick);
	JOY_API void joystick_draw(joystick_p joystick);
	JOY_API void joystick_reset(joystick_p joystick);
	JOY_API void joystick_set_position(joystick_p joystick, float x, float y);
	JOY_API bool joystick_is_dragging(joystick_p joystick);
	JOY_API void joystick_touch_down(joystick_p joystick, float x, float y);
	JOY_API void joystick_touch_move(joystick_p joystick, float x, float y);
	JOY_API void joystick_touch_up(joystick_p joystick);
	JOY_API SDL_FPoint joystick_get_direction(joystick_p joystick);
	JOY_API float joystick_get_magnitude(joystick_p joystick);

	JOY_API textinput_p textinput_create(SDL_Renderer* renderer, SDL_FRect rect, font_p font);
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
	JOY_API bool textinput_is_focused(textinput_p ti);

#ifdef __cplusplus
}
#endif

#endif // !UI_H
