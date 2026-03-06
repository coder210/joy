/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: grphics.h
Description: 图形库
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "SDL3/SDL.h"
#include "config.h"

typedef struct image image_t, *image_p;
typedef struct spritebatch spritebatch_t, * spritebatch_p;
typedef struct font_t  font_t, *font_p;
typedef struct text_texture text_texture_t, *text_texture_p;
typedef struct animation animation_t, * animation_p;
typedef struct codepoint_array {
	int* array;
	int length;
}codepoint_array_t, * codepoint_array_p;

typedef struct label {
	SDL_Renderer* renderer;
	SDL_FPoint position;
	text_texture_p text;
	SDL_Color color;
}label_t, * label_p;


typedef struct button {
	SDL_Renderer* renderer;
	SDL_FRect rect;           // 按钮的位置和大小
	SDL_Color normal_color;   // 正常状态颜色
	SDL_Color hover_color;    // 悬停状态颜色
	SDL_Color pressed_color;  // 按下状态颜色
	SDL_FPoint text_pos; // 文本相对位置
	image_p image;     // 按钮纹理（可选）
	text_texture_p text;	// 按钮文件纹理
	bool is_hovered;          // 鼠标悬停状态
	bool is_pressed;          // 按钮按下状态
} button_t, * button_p;

typedef struct combobox_item {
	SDL_FRect rect;
	SDL_Texture* text;
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
	SDL_Texture* headers[512];        // 表头
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

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////
//////////////////////////////shape//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

	JOY_API bool shape_draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2);
	JOY_API bool shape_draw_rectangle(SDL_Renderer* renderer, const char* mode, SDL_FRect rect);
	JOY_API bool shape_draw_polygon(SDL_Renderer* renderer, const char* mode, SDL_FPoint* points, int point_count, SDL_FColor color);
	JOY_API bool shape_draw_circle(SDL_Renderer* renderer, const char* mode, SDL_FPoint center, float radius, int segments);
	JOY_API bool shape_draw_grid(SDL_Renderer* renderer,
	SDL_FPoint start, SDL_FPoint end, float grid_size);
	JOY_API bool shape_draw_gridx(SDL_Renderer* renderer,
	SDL_FPoint position,
	int rows, int cols, float grid_size);

////////////////////////////////////////////////////////////////////////
//////////////////////////////image/////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	JOY_API image_p image_create(SDL_Renderer* renderer, const char* filename);
	JOY_API void image_destroy(image_p image);
	JOY_API void image_draw(image_p image, SDL_FPoint position);
	JOY_API void image_draw_ex(image_p image, const SDL_FRect* srcrect, SDL_FPoint position,
	float rotation, SDL_FPoint scale, SDL_FPoint origin);
	JOY_API spritebatch_p spritebatch_create(SDL_Renderer* renderer, const char* filename);
	JOY_API void spritebatch_destroy(spritebatch_p batch);
	JOY_API void spritebatch_add(spritebatch_p batch, SDL_FPoint position, float rotation, 
		float scale_x, float scale_y, SDL_FPoint origin, SDL_FRect src_rect);
	JOY_API float spritebatch_get_width(spritebatch_p batch);
	JOY_API float spritebatch_get_height(spritebatch_p batch);
	JOY_API void spritebatch_add_ex(spritebatch_p batch, 
		SDL_FRect src_rect, SDL_FRect dest_rect, 
		float rotation, SDL_FPoint origin);
	JOY_API void spritebatch_clear(spritebatch_p batch);
	JOY_API void spritebatch_set_image(spritebatch_p batch, const char* filename);
	JOY_API void spritebatch_draw(spritebatch_p batch);

	JOY_API animation_p animation_create(SDL_Renderer *renderer);
	JOY_API void animation_destroy(animation_p animation);
	JOY_API void animation_reset(animation_p animation);
	JOY_API void animation_add_clip(animation_p animation, 
		const char* image_path, float duration, SDL_FRect src_rect);
	JOY_API bool animation_is_finished(animation_p animation);
	JOY_API void animation_set_position(animation_p animation, float x, float y);
	JOY_API void animation_set_scale(animation_p animation, float x, float y);
	JOY_API void animation_set_rotation(animation_p animation, float rotation);
	JOY_API void animation_update(animation_p animation, float dt);
	JOY_API void animation_draw(animation_p animation, SDL_FRect* camera);

////////////////////////////////////////////////////////////////////////
//////////////////////////////font//////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	JOY_API font_p font_create(SDL_Renderer* renderer, const char* filename, int fontsize);
	JOY_API void font_destroy(font_p font);
	JOY_API text_texture_p text_create(font_p font, const int* codepoints, int num_codepoints, SDL_Color color);
	JOY_API void text_update(text_texture_p text, font_p font, 
		const int* codepoints, int num_codepoints, SDL_Color color);
	JOY_API void text_destroy(text_texture_p text_tex);
	JOY_API void text_print(SDL_Renderer* renderer, text_texture_p text_tex, float x, float y);

////////////////////////////////////////////////////////////////////////
//////////////////////////////ui////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

	JOY_API label_p label_create(SDL_Renderer* renderer, font_p fontinfo, const int* codepoints, int len);
	JOY_API void label_destroy(label_p label);
	JOY_API void label_set_text(label_p label, font_p fontinfo, const int* codepoints, int len, SDL_Color color);
	JOY_API void label_draw(label_p label);
	JOY_API void label_handle_event(label_p label, SDL_Event* event);

	JOY_API button_p button_create(SDL_Renderer* renderer, SDL_FRect rect);
	JOY_API void button_destroy(button_p btn);
	JOY_API void button_set_normal_color(button_p btn, SDL_Color color);
	JOY_API void button_set_hover_color(button_p btn, SDL_Color color);
	JOY_API void button_set_pressed_color(button_p btn, SDL_Color color);
	JOY_API void button_set_text(button_p btn, font_p fontinfo, const int* codepoints, int len, SDL_Color color);
	JOY_API void button_draw(button_p btn);
	JOY_API void button_handle_event(button_p btn, SDL_Event* event);

	JOY_API combobox_p combobox_create(SDL_Renderer* renderer, SDL_FRect rect);
	JOY_API void combobox_destroy(combobox_p combobox);
	JOY_API void combobox_set_text(combobox_p combobox, const char* filename, int fontsize,
		const int* codepoints, int len, SDL_Color color);
	JOY_API void combobox_add_item(combobox_p combobox, const char* filename, int fontsize, 
		const int* codepoints, int len, SDL_Color color);
	JOY_API bool combobox_handle_event(combobox_p combobox, SDL_Event* event);
	JOY_API void combobox_draw(combobox_p combobox);

	JOY_API datagrid_p datagrid_create(SDL_Renderer* renderer, SDL_Rect rect, int col_count, bool has_header, void* font);
	JOY_API void datagrid_setheaders(datagrid_p grid, const codepoint_array_p* headers);
	JOY_API void datagrid_setheader(datagrid_p grid, int index, const codepoint_array_p header);
	JOY_API void datagrid_addrow(datagrid_p grid, const char** row_data);
	JOY_API void datagrid_setcell(datagrid_p grid, int row_index, int col_index, const char* data);
	JOY_API void datagrid_destroy(datagrid_p grid);
	JOY_API void datagrid_draw(datagrid_p grid, SDL_Renderer* renderer);
	JOY_API void datagrid_handle_event(datagrid_p grid, SDL_Event* event, SDL_Renderer* renderer);

	JOY_API radio_button_p radio_button_create(int x, int y, int size, const char* label, int group_id);
	JOY_API void radio_button_destroy(radio_button_p radio);

	JOY_API radio_button_group_p radio_button_group_create();
	JOY_API void radio_button_group_destroy(radio_button_group_p group);
	JOY_API void radio_button_group_add_button(radio_button_group_p group, radio_button_p radio);
	JOY_API void radio_button_group_set_selected(radio_button_group_p group, int index);
	JOY_API void radio_button_group_draw(radio_button_group_p group, SDL_Renderer* renderer);
	JOY_API bool radio_button_group_handle_event(radio_button_group_p group, SDL_Event* event, SDL_Renderer* renderer);

	JOY_API checkbox_p checkbox_create(int x, int y, int size, const char* label);
	JOY_API void checkbox_destroy(checkbox_p checkbox);
	JOY_API void checkbox_set_state(checkbox_p checkbox, checkbox_state_t state);
	JOY_API void checkbox_toggle(checkbox_p checkbox);
	JOY_API bool checkbox_handle_event(checkbox_p checkbox, SDL_Event* event, SDL_Renderer* renderer);
	JOY_API void checkbox_draw(checkbox_p checkbox, SDL_Renderer* renderer, void* font);

#ifdef __cplusplus
}
#endif

#endif // !GRAPHICS_H
