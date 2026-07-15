#include "external/klib/kvec.h"
#include <SDL3/SDL.h>
#include "joy.h"

#define JOYSTICK_BG_COLOR 0x555555FF
#define JOYSTICK_HANDLE_COLOR 0x888888FF
#define JOYSTICK_HANDLE_ACTIVE_COLOR 0x4CAF50FF

typedef kvec_t(char*) dg_row_t;

typedef struct label {
	jr2d_p r2d;
	vec2_t position;
	jtext_p text;
	jcolor_t color;
}label_t, * label_p;


struct button {
	jr2d_p r2d;   // 渲染器
	vec2_t position;      // 位置
	float width;              // 宽度
	float height;             // 高度
	jcolor_t normal_color;   // 正常状态颜色
	jcolor_t hover_color;    // 悬停状态颜色
	jcolor_t pressed_color;  // 按下状态颜色
	vec2_t text_pos;      // 文本相对位置
	image_p image;            // 按钮正常纹理（可选）
	image_p pressed_image;    // 按钮按下纹理（可选）
	jtext_p text;      // 按钮文件纹理
	bool is_hovered;          // 内部状态：由 is_hovered() / is_pressed() 设置
	bool is_pressed;
};

struct combobox_item {
	jrect_t rect;
	jtext_p text;
	bool is_selected;
};

struct combobox {
	jrect_t main_rect;
	jrect_t dropdown_rect;
	combobox_item_t items[20];
	int item_count;
	int item_height;
	int selected_index;
	bool is_open;
	jtext_p text;
	jcolor_t bg_color;
	jcolor_t text_color;
	jcolor_t hover_color;
	jr2d_p r2d;
};

struct datagrid {
	jr2d_p r2d;
	int x, y;              // 位置
	int width, height;     // 尺寸
	int row_height;        // 行高
	int col_count;         // 列数
	int row_count;         // 行数（包括表头）
	int visible_rows;      // 可见行数
	int scroll_offset;     // 滚动偏移
	int selected_row;      // 选中的行
	jtext_p headers[512];        // 表头
	kvec_t(dg_row_t) data;              // 表格数据（klib 动态数组 of dg_row_t）
	int* col_widths;       // 列宽
	bool has_header;   // 是否有表头
	font_p font;        // 字体
	jcolor_t grid_color;
	jcolor_t bg_color;
	jcolor_t text_color;
	jcolor_t header_bg_color;
	jcolor_t selected_color;
};

struct radio {
	int x, y;                  // 位置
	int size;                  // 大小
	bool checked;              // 是否选中
	char* label;               // 标签文本
	int group_id;              // 所属组ID
	jrect_t hit_area;         // 点击区域
	bool hover;                // 鼠标悬停状态
	jcolor_t r1_color;
	jcolor_t r2_color;
	jcolor_t r3_color;
};

struct checkbox {
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
};

struct inputbox {
	jr2d_p r2d;
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
	uint64_t cursor_timer;          // 光标闪烁计时
	bool cursor_visible;          // 光标是否可见
	void (*on_submit)(struct inputbox* ti, void* userdata);
	void (*on_change)(struct inputbox* ti, void* userdata);
	void* cb_userdata;
};

struct slider {
	jr2d_p r2d;
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
};



struct popup {
	jr2d_p r2d;
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
};


struct tooltip {
	jr2d_p r2d;
	float x, y;
	char text[TOOLTIP_MAX_TEXT];
	int text_len;
	jcolor_t bg_color;
	jcolor_t border_color;
	jcolor_t text_color;
	float padding;
	bool visible;
};


struct joystick {
	jr2d_p r2d;
	jrect_t background;
	jrect_t handle;
	vec2_t center;
	float radius;
	float handle_radius;
	vec2_t direction;
	float magnitude;
	float deadzone;
	bool is_dragging;
	bool is_touch_active;
	int touch_id;
};

static float distance_squared(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx * dx + dy * dy;
}

label_p
label_create(jr2d_p r2d, font_p fontinfo, const int* codepoints, int num_codepoints)
{
	label_p label = (label_p)malloc(sizeof(label_t));
	label->r2d = r2d;
	label->position = (vec2_t){ 0, 0 };
	label->color = (jcolor_t){ 255, 255,255, 255 };
	label->text = text_create(r2d, fontinfo, codepoints, num_codepoints, label->color);
	return label;
}

void
label_destroy(label_p label)
{
	if (!label) return;
	if (label->text) text_destroy(label->text);
	free(label);
}

void
label_set_text(label_p label, font_p font,
	const int* codepoints, int num_codepoints,
	jcolor_t color)
{
	if (label->text) {
		text_update(label->text, font, codepoints, num_codepoints, color);
	}
	else {
		label->text = text_create(label->r2d, font, codepoints, num_codepoints, color);
	}
}

void label_draw(label_p label)
{
	//jrect_t dest_rect;
	if (!label) return;

	//r2d = label->r2d;
	//dest_rect.x = label->position.x;
	//dest_rect.y = label->position.y;
	//dest_rect.w = label->text->w;
	//dest_rect.h = label->text->h;
	//SDL_RenderTexture(r2d, label->text, NULL, &dest_rect);
	jtext_print(label->r2d, label->text, label->position.x, label->position.y);
}

void label_handle_event(label_p label, void* event)
{
	(void)label;
	(void)event;
}

button_p
button_create(jr2d_p r2d, float width, float height)
{
	button_p btn = (button_p)malloc(sizeof(button_t));
	if (!btn) return NULL;
	btn->r2d = r2d;
	btn->position = (vec2_t){0, 0};
	btn->width = width;
	btn->height = height;
	btn->normal_color = (jcolor_t){ 100, 100, 200, 255 };
	btn->hover_color = (jcolor_t){ 150, 150, 250, 255 };
	btn->pressed_color = (jcolor_t){ 50, 50, 150, 255 };
	btn->text_pos = (vec2_t){0, 0};
	btn->image = NULL;
	btn->pressed_image = NULL;
	btn->text = NULL;
	btn->is_hovered = btn->is_pressed = false;
	return btn;
}

void button_set_normal_color(button_p btn, jcolor_t color)
{
	if (!btn) return;
	btn->normal_color = color;
}

void button_set_hover_color(button_p btn, jcolor_t color)
{
	if (!btn) return;
	btn->hover_color = color;
}

void button_set_pressed_color(button_p btn, jcolor_t color)
{
	if (!btn) return;
	btn->pressed_color = color;
}

void button_set_image(button_p btn, image_p image)
{
	if (!btn) return;
	btn->image = image;
}

void button_set_pressed_image(button_p btn, image_p image)
{
	if (!btn) return;
	btn->pressed_image = image;
}



void button_set_textx(button_p btn, font_p font,
	const char* str, int len, jcolor_t color)
{
	if (!btn) return;
	if (btn->text) {
		text_updatex(btn->text, font, str, len, color);
	}
	else {
		btn->text = text_createx(btn->r2d, font, str, len, color);
	}
}

void
button_set_text(button_p btn, font_p font,
	const int* codepoints, int num_codepoints, jcolor_t color)
{
	if (!btn) return;
	if (btn->text) {
		text_update(btn->text, font, codepoints, num_codepoints, color);
	}
	else {
		btn->text = text_create(btn->r2d, font, codepoints, num_codepoints, color);
	}
}

void button_set_position(button_p btn, float x, float y)
{
	if (!btn) return;
	btn->position.x = x;
	btn->position.y = y;
}

vec2_t button_get_position(button_p btn)
{
	return btn->position;
}

static bool button_hit_test(button_p btn, float x, float y)
{
	vec2_t logic = r2d_coordinates_from_window(btn->r2d, x, y);
	return logic.x >= btn->position.x && logic.x <= btn->position.x + btn->width
	    && logic.y >= btn->position.y && logic.y <= btn->position.y + btn->height;
}

bool button_is_hovered(button_p btn, float x, float y)
{
	if (!btn) return false;
	btn->is_hovered = button_hit_test(btn, x, y);
	return btn->is_hovered;
}

bool button_is_pressed(button_p btn, float x, float y)
{
	if (!btn) return false;
	btn->is_pressed = button_hit_test(btn, x, y);
	return btn->is_pressed;
}

void button_set_hovered(button_p btn, bool v)
{
	if (btn) btn->is_hovered = v;
}

void button_set_text_offset(button_p btn, float x, float y)
{
	if (!btn) return;
	btn->text_pos.x = x;
	btn->text_pos.y = y;
}

void button_set_pressed(button_p btn, bool v)
{
	if (btn) btn->is_pressed = v;
}

void button_destroy(button_p btn)
{
	if (!btn) return;
	if (btn->text) text_destroy(btn->text);
	if (btn->image) image_destroy(btn->image);
	if (btn->pressed_image) image_destroy(btn->pressed_image);
	free(btn);
}


void button_draw(button_p btn)
{
	jr2d_p r2d;
	jrect_t dest_rect, rect;
	jcolor_t color;
	if (!btn) return;

	color = btn->normal_color;
	if (btn->is_pressed) {
		color = btn->pressed_color;
	}
	else if (btn->is_hovered) {
		color = btn->hover_color;
	}

	rect.x = btn->position.x;
	rect.y = btn->position.y;
	rect.w = btn->width;
	rect.h = btn->height;

	r2d = btn->r2d;
	r2d_set_color(r2d, color.r, color.g, color.b, color.a);
	shape_draw_rectangle(r2d, "fill", rect);
	r2d_set_color(r2d, 0, 0, 0, 255);
	shape_draw_rectangle(r2d, "line", rect);

	if (btn->is_pressed && btn->pressed_image) {
		image_draw1(btn->pressed_image, NULL, &rect);
	} else if (btn->image) {
		image_draw1(btn->image, NULL, &rect);
	}

	if (btn->text) {
		float widht = text_get_width(btn->text);
		float height = text_get_height(btn->text);
		dest_rect.x = rect.x + (rect.w - widht) / 2.0f + btn->text_pos.x;
		dest_rect.y = rect.y + (rect.h - height) / 2.0f + btn->text_pos.y;
		jtext_print(r2d, btn->text, dest_rect.x, dest_rect.y);
	}
}

/* 事件处理已移至 Lua 层，见 joy.lua 中的 joy.mousepressed/mousereleased/mousemoved */

combobox_p combobox_create(jr2d_p r2d, jrect_t rect)
{
	combobox_p combobox;
	combobox = (combobox_p)malloc(sizeof(combobox_t));
	combobox->r2d = r2d;
	combobox->text = NULL;
	combobox->main_rect = rect;
	combobox->dropdown_rect.x = rect.x;
	combobox->dropdown_rect.y = rect.y + rect.h;
	combobox->dropdown_rect.w = rect.w;
	combobox->dropdown_rect.h = 0;
	combobox->item_count = 0;
	combobox->item_height = 30;
	combobox->selected_index = -1;
	combobox->is_open = false;
	combobox->bg_color = (jcolor_t){ 240, 240, 240, 255 };
	combobox->text_color = (jcolor_t){ 0, 0, 0, 255 };
	combobox->hover_color = (jcolor_t){ 150, 150, 250, 255 };
	return combobox;
}

void combobox_destroy(combobox_p combobox)
{
	if (combobox) {
		for (int i = 0; i < combobox->item_count; i++)
			if (combobox->items[i].text) text_destroy(combobox->items[i].text);
		free(combobox);
	}
}

void
combobox_set_text(combobox_p combobox, font_p font,
	const int* codepoints, int len, jcolor_t color)
{
	if (combobox->text) {
		text_destroy(combobox->text);
	}
	combobox->text = text_create(combobox->r2d, font, codepoints, len, color);
}

void combobox_add_item(combobox_p combobox, font_p font, const int* codepoints, int len, jcolor_t color)
{
	combobox_item_p item;
	jr2d_p r2d;
	if (combobox->item_count > 20) {
		return;
	}
	r2d = combobox->r2d;
	item = combobox->items + combobox->item_count;
	//item->text = create_jtext(r2d, filename, fontsize, codepoints, len, color);
	item->is_selected = false;
	combobox->item_count++;
	combobox->dropdown_rect.h = combobox->item_count * combobox->item_height;
}

bool combobox_is_pressed(combobox_p combobox, float x, float y)
{
	if (!combobox) return false;
	vec2_t logic = r2d_coordinates_from_window(combobox->r2d, x, y);
	rectangle_t rect;
	rect.x = combobox->main_rect.x;
	rect.y = combobox->main_rect.y;
	rect.width = combobox->main_rect.w;
	rect.height = combobox->main_rect.h;
	return collision2d_check_point_rectangle(logic, rect);
}

bool combobox_is_open(combobox_p combobox)
{
	return combobox ? combobox->is_open : false;
}

void combobox_set_open(combobox_p combobox, bool v)
{
	if (combobox) combobox->is_open = v;
}

int combobox_hit_item(combobox_p combobox, float x, float y)
{
	if (!combobox || !combobox->is_open) return -1;
	vec2_t logic = r2d_coordinates_from_window(combobox->r2d, x, y);
	for (int i = 0; i < combobox->item_count; i++) {
		rectangle_t item_rect = {
		    combobox->dropdown_rect.x,
		    combobox->dropdown_rect.y + i * combobox->item_height,
		    combobox->dropdown_rect.w,
		    combobox->item_height
		};
		if (collision2d_check_point_rectangle(logic, item_rect)) return i;
	}
	return -1;
}

void combobox_set_selected(combobox_p combobox, int index)
{
	if (combobox) combobox->selected_index = index;
}

int combobox_get_selected(combobox_p combobox)
{
	return combobox ? combobox->selected_index : -1;
}

void combobox_draw(combobox_p combobox)
{
	jr2d_p r2d;
	r2d = combobox->r2d;
	r2d_set_color(r2d, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
	shape_draw_rectangle(r2d, "fill", combobox->main_rect);
	

	r2d_set_color(r2d, 0, 0, 0, 255);
	shape_draw_rectangle(r2d, "line", combobox->main_rect);

	// ��Ⱦ��ǰѡ�е��ı�
	if (combobox->selected_index >= 0 && combobox->items[combobox->selected_index].text) {
		jtext_print(r2d, combobox->items[combobox->selected_index].text,
			combobox->main_rect.x + 5, combobox->main_rect.y);
	}


	// ����������Ǵ򿪵ģ���Ⱦѡ���б�
	if (combobox->is_open) {
		// ��Ⱦ������ͷ
		jcolor_t color = { 0, 0, 0, 255 };
		r2d_set_color(r2d, 0, 0, 0, 255);
		vec2_t arrow_points[3] = {
		    {combobox->main_rect.x + combobox->main_rect.w - 25, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 5, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 15, combobox->main_rect.y + 20}
		};
		shape_draw_polygon(r2d, "line", arrow_points, 3, color);


		r2d_set_color(r2d, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
		shape_draw_rectangle(r2d, "fill", combobox->dropdown_rect);

		r2d_set_color(r2d, 0, 0, 0, 255);
		shape_draw_rectangle(r2d, "line", combobox->dropdown_rect);

		combobox_item_p item;

		for (int i = 0; i < combobox->item_count; i++) {
			jrect_t item_rect = {
			    combobox->dropdown_rect.x,
			    combobox->dropdown_rect.y + i * combobox->item_height,
			    combobox->dropdown_rect.w,
			    combobox->item_height
			};

			// ����ǵ�ǰѡ�е��ʹ����ͣ��ɫ
			if (i == combobox->selected_index) {
				r2d_set_color(r2d, combobox->hover_color.r, combobox->hover_color.g, combobox->hover_color.b, combobox->hover_color.a);
				shape_draw_rectangle(r2d, "line", item_rect);
			}

			r2d_set_color(r2d, 0, 0, 0, 255);
			shape_draw_rectangle(r2d, "line", item_rect);

			item = combobox->items + i;
			if (item->text) {
				jtext_print(r2d, item->text, item_rect.x + 5, item_rect.y);
			}
		}
	}
	else {
		// ��Ⱦ������ͷ
		jcolor_t color = { 0, 0, 0, 255 };
		vec2_t arrow_points[3] = {
		    {combobox->main_rect.x + combobox->main_rect.w - 15, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 25, combobox->main_rect.y + 20},
		    {combobox->main_rect.x + combobox->main_rect.w - 5, combobox->main_rect.y + 20}
		};
		shape_draw_polygon(r2d, "line", arrow_points, 3, color);
	}
}


datagrid_p
datagrid_create(jr2d_p r2d, jrect_t rect, int col_count, bool has_header, void* font)
{
	datagrid_p grid;
	grid = (datagrid_p)malloc(sizeof(datagrid_t));
	if (!grid) return NULL;
	memset(grid, 0, sizeof(datagrid_t));

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
	grid->font = (font_p)font;
	kv_init(grid->data);
	grid->col_widths = (int*)malloc(col_count * sizeof(int));
	grid->bg_color = (jcolor_t){ 0xFF, 0xFF, 0xFF, 0xFF };
	grid->grid_color = (jcolor_t){ 0xCC, 0xCC, 0xCC, 0xFF };
	grid->text_color = (jcolor_t){ 0x00, 0x00, 0x00, 0xFF };
	grid->header_bg_color = (jcolor_t){ 0xEE, 0xEE, 0xEE, 0xFF };
	grid->selected_color = (jcolor_t){ 0x33, 0x99, 0xFF, 0xFF };
	memset(grid->headers, 0, 512 * sizeof(char));
	int default_width = grid->width / col_count;
	for (int i = 0; i < col_count; i++) {
		grid->col_widths[i] = default_width;
	}
	return grid;
}


void datagrid_setheaders(datagrid_p grid, const char* headers)
{
	font_p fontinfo;
	jcolor_t color;
	color = (jcolor_t){ 0x0, 0x0, 0x0, 0xFF };
	if (!grid->has_header) return;
	fontinfo = grid->font;
	for (int i = 0; i < grid->col_count; i++) {
		//grid->headers[i] = create_jtext(grid->r2d, fontinfo, headers[i]->array, headers[i]->length, color);
	}
}

void datagrid_setheader(datagrid_p grid, int index, const char* header)
{
	font_p fontinfo;
	jcolor_t color;
	if (!grid->has_header) return;
	if (index < grid->col_count) return;
	color = (jcolor_t){ 0x0, 0x0, 0x0, 0xFF };
	fontinfo = grid->font;
	if (grid->headers[index]) {
		/* 由外部管理 text 更新 */
	}
	else {
		/* 由外部设置 header text */
	}
}

void datagrid_addrow(datagrid_p grid, const char** row_data)
{
	if (!grid) return;
	dg_row_t row;
	kv_init(row);
	for (int i = 0; i < grid->col_count; i++) {
		kv_push(char*, row, row_data && row_data[i] ? utils_strdup(row_data[i], NULL) : NULL);
	}
	kv_push(dg_row_t, grid->data, row);
	grid->row_count = (int)kv_size(grid->data);
}

void datagrid_setcell(datagrid_p grid, int row_index, int col_index, const char* data)
{
	if (!grid) return;
	if (row_index < (int)kv_size(grid->data) && col_index < grid->col_count) {
		dg_row_t *row = &kv_A(grid->data, row_index);
		if (kv_A(*row, col_index)) free(kv_A(*row, col_index));
		kv_A(*row, col_index) = data ? utils_strdup(data, NULL) : NULL;
	}
}

static void datagrid_cleardata(datagrid_p grid)
{
	for (size_t i = 0; i < kv_size(grid->data); i++) {
		dg_row_t *row = &kv_A(grid->data, i);
		for (size_t j = 0; j < kv_size(*row); j++)
			if (kv_A(*row, j)) { free(kv_A(*row, j)); kv_A(*row, j) = NULL; }
		kv_destroy(*row);
	}
	kv_destroy(grid->data);
	grid->row_count = grid->has_header ? 1 : 0;
}


void datagrid_destroy(datagrid_p grid)
{
	if (!grid) return;
	if (grid->has_header) {
		for (int i = 0; i < grid->col_count; i++)
			if (grid->headers[i]) text_destroy(grid->headers[i]);
	}
	datagrid_cleardata(grid);
	free(grid->col_widths);
	free(grid);
}

void datagrid_draw(datagrid_p grid, jr2d_p r2d)
{
	// ����ɼ��з�Χ
	int start_row = grid->scroll_offset;
	int end_row = start_row + grid->visible_rows;
	if (end_row > grid->row_count) end_row = grid->row_count;

	// ���Ʊ���
	jcolor_t bg_color = grid->bg_color;
	r2d_set_color(r2d, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	jrect_t bg_rect = { grid->x, grid->y, grid->width, grid->height };
	shape_draw_rectangle(r2d, "fill", bg_rect);

	// ���������ߺ�����
	jcolor_t grid_color = grid->grid_color;
	jcolor_t text_color = grid->text_color;
	jcolor_t header_bg_color = grid->header_bg_color;
	jcolor_t selected_color = grid->selected_color;

	// ���Ʊ���
	r2d_set_color(r2d, header_bg_color.r, header_bg_color.g, header_bg_color.b, header_bg_color.a);
	jrect_t header_rect = { grid->x, grid->y, grid->width, grid->row_height };
	shape_draw_rectangle(r2d, "fill", header_rect);
	

	{
		int x = grid->x;
		int y = grid->y;
		for (int col = 0; col < grid->col_count; col++) {
			jtext_p tex = grid->headers[col];
			jtext_print(r2d, tex, x, y);

			// ���ƴ�ֱ��
			r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			shape_draw_line(r2d, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

			x += grid->col_widths[col];
		}
	}

	// ������������
	for (int row = start_row; row < end_row; row++) {
		int y = grid->y + (row - start_row) * grid->row_height + grid->row_height;

		// �����б���
		if (row == grid->selected_row) {
			r2d_set_color(r2d, selected_color.r, selected_color.g, selected_color.b, selected_color.a);
			jrect_t selected_rect = { grid->x, y, grid->width, grid->row_height };
			shape_draw_rectangle(r2d, "fill", selected_rect);
		}
		else {
			/* 非选中行默认无背景 */
		}

		// ���Ƶ�Ԫ�����ݺ�����
		int x = grid->x;
		for (int col = 0; col < grid->col_count; col++) {
			if ((size_t)row < kv_size(grid->data) && (size_t)col < kv_size(kv_A(grid->data, row))) {
				char* text = kv_A(kv_A(grid->data, row), col);
				if (text) {
					jcolor_t c = {0,0,0,255};
					jtext_printx(r2d, grid->font, text, (int)strlen(text), c, (float)x + 2, (float)y);
				}
			}

			// ���ƴ�ֱ��
			r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			shape_draw_line(r2d, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

			x += grid->col_widths[col];
		}

		// ����ˮƽ��
		r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
		shape_draw_line(r2d, grid->x, y + grid->row_height, grid->x + grid->width, y + grid->row_height);
	}

	// ������߿�
	r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
	shape_draw_rectangle(r2d, "line", bg_rect);
}

int datagrid_hit_row(datagrid_p grid, float x, float y)
{
	if (!grid) return -1;
	vec2_t logic = r2d_coordinates_from_window(grid->r2d, x, y);
	rectangle_t rect = { (float)grid->x, (float)(grid->y + grid->row_height), (float)grid->width, (float)grid->height };
	if (!collision2d_check_point_rectangle(logic, rect))
		return -1;
	int clicked_row = grid->scroll_offset + ((int)(logic.y - rect.y)) / grid->row_height;
	if (clicked_row < grid->row_count) return clicked_row;
	return -1;
}

void datagrid_scroll(datagrid_p grid, int lines)
{
	if (!grid) return;
	grid->scroll_offset -= lines;
	int max_scroll = grid->row_count - grid->visible_rows;
	if (max_scroll < 0) max_scroll = 0;
	if (grid->scroll_offset < 0) grid->scroll_offset = 0;
	if (grid->scroll_offset > max_scroll) grid->scroll_offset = max_scroll;
}

void datagrid_set_selected_row(datagrid_p grid, int row)
{
	if (grid) grid->selected_row = row;
}

radio_p radio_create(int x, int y, int size, const char* label, int group_id)
{
	radio_p radio;
	radio = (radio_p)malloc(sizeof(radio_t));
	if (!radio) return NULL;

	radio->x = x;
	radio->y = y;
	radio->size = size;
	radio->checked = false;
	radio->group_id = group_id;
	radio->hover = false;

	// ���Ʊ�ǩ�ı�
	if (label) {
		radio->label = utils_strdup(label, NULL);
	}
	else {
		radio->label = NULL;
	}

	// ���õ�����򣨰�����ǩ����
	int label_width = label ? strlen(label) * 10 : 0; // �����ǩ����
	radio->hit_area.x = x;
	radio->hit_area.y = y;
	radio->hit_area.w = size + 5 + label_width;
	radio->hit_area.h = size;

	return radio;
}

void radio_destroy(radio_p radio)
{
	if (!radio) return;

	if (radio->label) {
		free(radio->label);
	}

	free(radio);
}


void radio_render(radio_p radio, jr2d_p r2d)
{
	if (!radio) return;

	float center_x = radio->x + radio->size / 2;
	float center_y = radio->y + radio->size / 2;
	float r1 = radio->size / 2;
	float r2 = r1 - 2;
	float r3 = r2 - 4;

	// ʹ���㹻��ĵ�������ƽ����Բ
	const int segments = 99; // ���ӷֶ���ʹԲ��ƽ��
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
	r2d_set_color(r2d, radio->r1_color.r, radio->r1_color.g, radio->r1_color.b, radio->r1_color.a);
	shape_draw_circle(r2d, "fill", (vec2_t){ center_x, center_y }, r1, 32);

	/* 内圈 */
	r2d_set_color(r2d, radio->r2_color.r, radio->r2_color.g, radio->r2_color.b, radio->r2_color.a);
	shape_draw_circle(r2d, "fill", (vec2_t){ center_x, center_y }, r2, 32);

	/* 选中标记 */
	if (radio->checked) {
		r2d_set_color(r2d, radio->r3_color.r, radio->r3_color.g, radio->r3_color.b, radio->r3_color.a);
		shape_draw_circle(r2d, "fill", (vec2_t){ center_x, center_y }, r3, 32);
	}

	// ���Ʊ�ǩ�ı�
	if (radio->label) {
		/* 标签文本 — 由外部传入 font 后使用 jtext_printx 渲染 */
	}
}

bool radio_is_pressed(radio_p radio, float x, float y)
{
	if (!radio) 
		return false;
	rectangle_t hit_area;
	vec2_t pt = { x, y };

	hit_area.x = radio->hit_area.x;
	hit_area.y = radio->hit_area.y;
	hit_area.width = radio->hit_area.w;
	hit_area.height = radio->hit_area.h;

	if (collision2d_check_point_rectangle(pt, hit_area)) {
		radio->checked = !radio->checked;
		return true;
	}
	return false;
}

void radio_set_hovered(radio_p radio, bool v)
{
	if (radio) radio->hover = v;
}


checkbox_p checkbox_create(int x, int y, int size, const char* label)
{
	checkbox_p cb;
	cb = malloc(sizeof(checkbox_t));
	if (!cb) return NULL;

	cb->x = x;
	cb->y = y;
	cb->size = size;
	cb->state = JOY_CHECKBOX_UNCHECKED;
	cb->label = label ? utils_strdup(label, NULL) : NULL;
	cb->hover = false;
	cb->enabled = true;

	// ����Ĭ����ɫ
	cb->box_color = (jcolor_t){ 0, 0, 0, 255 };        // ��ɫ�߿�
	cb->check_color = (jcolor_t){ 0, 120, 0, 255 };    // ��ɫ��ѡ
	cb->text_color = (jcolor_t){ 0, 0, 0, 255 };       // ��ɫ�ı�
	cb->hover_color = (jcolor_t){ 200, 200, 200, 255 };// ǳ��ɫ��ͣ

	// ���õ�����򣨰�����ǩ��
	int label_width = 0;
	if (label) {
		// �����ǩ���ȣ�ʵ����Ⱦʱ��Ҫ��ȷ���㣩
		label_width = strlen(label) * 8;
	}
	cb->hit_area.x = x;
	cb->hit_area.y = y;
	cb->hit_area.w = size + 5 + label_width;
	cb->hit_area.h = size;
	return cb;
}


void checkbox_destroy(checkbox_p checkbox)
{
	if (!checkbox) return;

	if (checkbox->label) {
		free(checkbox->label);
	}

	free(checkbox);
}

void checkbox_set_state(checkbox_p checkbox, checkbox_state_t state)
{
	if (!checkbox) return;
	checkbox->state = state;
}

void checkbox_toggle(checkbox_p checkbox)
{
	if (!checkbox || !checkbox->enabled) return;

	if (checkbox->state == JOY_CHECKBOX_UNCHECKED) {
		checkbox->state = JOY_CHECKBOX_CHECKED;
	}
	else {
		checkbox->state = JOY_CHECKBOX_UNCHECKED;
	}
}


// ������ѡ���¼�
bool checkbox_is_pressed(checkbox_p cb, float x, float y)
{
	if (!cb || !cb->enabled) return false;
	vec2_t pt = { x, y };
	rectangle_t hit_area;
	hit_area.x = cb->hit_area.x;
	hit_area.y = cb->hit_area.y;
	hit_area.width = cb->hit_area.w;
	hit_area.height = cb->hit_area.h;
	if (collision2d_check_point_rectangle(pt, hit_area)) {
		checkbox_toggle(cb);
		return true;
	}
	return false;
}

void checkbox_set_hovered(checkbox_p cb, bool v)
{
	if (cb) cb->hover = v;
}

void checkbox_draw(checkbox_p checkbox, jr2d_p r2d, void* font)
{
	if (!checkbox) return;

	// ���Ƹ�ѡ�򱳾��������ͣ��
	if (checkbox->hover && checkbox->enabled) {
		r2d_set_color(r2d,
			checkbox->hover_color.r,
			checkbox->hover_color.g,
			checkbox->hover_color.b,
			checkbox->hover_color.a);
		jrect_t bgRect = {
		    checkbox->x - 2,
		    checkbox->y - 2,
		    checkbox->size + 4,
		    checkbox->size + 4
		};
		shape_draw_rectangle(r2d, "fill", bgRect);
		
	}

	// ���Ƹ�ѡ�����
	r2d_set_color(r2d,
		checkbox->box_color.r,
		checkbox->box_color.g,
		checkbox->box_color.b,
		checkbox->box_color.a);
	jrect_t boxRect = {
	    checkbox->x,
	    checkbox->y,
	    checkbox->size,
	    checkbox->size
	};
	shape_draw_rectangle(r2d, "line", boxRect);

	// �����ѡ�У����ƹ�ѡ���
	if (checkbox->state != JOY_CHECKBOX_UNCHECKED && checkbox->enabled) {
		r2d_set_color(r2d,
			checkbox->check_color.r,
			checkbox->check_color.g,
			checkbox->check_color.b,
			checkbox->check_color.a);

		if (checkbox->state == JOY_CHECKBOX_CHECKED) {
			// ���ƹ�ѡ��ǣ��Ժţ�
			const float padding = checkbox->size * 0.2f;
			vec2_t points[3] = {
			    {checkbox->x + padding, checkbox->y + checkbox->size * 0.5f},
			    {checkbox->x + checkbox->size * 0.4f, checkbox->y + checkbox->size - padding},
			    {checkbox->x + checkbox->size - padding, checkbox->y + padding}
			};
			r2d_draw_lines(r2d, points, 3);
		}
		else {
			// ���Ʋ�ȷ��״̬�����ߣ�
			const float padding = checkbox->size * 0.3f;
			vec2_t points[2] = {
			    {checkbox->x + padding, checkbox->y + checkbox->size * 0.5f},
			    {checkbox->x + checkbox->size - padding, checkbox->y + checkbox->size * 0.5f}
			};
			shape_draw_line(r2d, points[0].x, points[0].y, points[1].x, points[1].y);
		}
	}

	// ������ã����ư�͸��Ч��
	if (!checkbox->enabled) {
		r2d_set_color(r2d, 200, 200, 200, 128);
		shape_draw_rectangle(r2d, "fill", boxRect);
	}

	// ���Ʊ�ǩ�ı�
	if (checkbox->label && font) {
		font_p f = (font_p)font;
		jcolor_t tc = checkbox->enabled ? checkbox->text_color : (jcolor_t){ 128, 128, 128, 255 };
		jtext_printx(r2d, f, checkbox->label, (int)strlen(checkbox->label), tc,
			checkbox->x + checkbox->size + 5,
			checkbox->y + (checkbox->size - (float)font_get_size(f)) / 2);
	}
}




// ��ʼ��ҡ��
joystick_p joystick_create(jr2d_p r2d, float x, float y, float radius)
{
	joystick_p joystick;
	joystick = (joystick_p)malloc(sizeof(joystick_t));

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
	return joystick;
}

void joystick_destroy(joystick_p joystick)
{
	free(joystick);
}

// ����ҡ��λ��
void joystick_set_position(joystick_p joystick, float x, float y)
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

// ����ҡ����ק����
static void joystick_update_drag(joystick_p joystick, float touch_x, float touch_y)
{
	// ������������ĵ������
	float dx = touch_x - joystick->center.x;
	float dy = touch_y - joystick->center.y;

	// �������
	float distance = sqrtf(dx * dx + dy * dy);

	// ���ƾ��벻���������뾶
	if (distance > joystick->radius) {
		dx = (dx / distance) * joystick->radius;
		dy = (dy / distance) * joystick->radius;
		distance = joystick->radius;
	}

	// ���㷽���ǿ��
	joystick->magnitude = distance / joystick->radius;

	// Ӧ������
	if (joystick->magnitude < joystick->deadzone) {
		joystick->direction.x = 0.0f;
		joystick->direction.y = 0.0f;
		joystick->magnitude = 0.0f;

		// �ֱ��ص�����
		joystick->handle.x = joystick->center.x - joystick->handle_radius;
		joystick->handle.y = joystick->center.y - joystick->handle_radius;
	}
	else {
		// ��һ����������
		joystick->direction.x = dx / joystick->radius;
		joystick->direction.y = dy / joystick->radius;

		// �����ֱ�λ��
		joystick->handle.x = joystick->center.x + dx - joystick->handle_radius;
		joystick->handle.y = joystick->center.y + dy - joystick->handle_radius;
	}
}

// �����¼�������Ӧ�߼��ֱ��ʣ�
bool joystick_is_dragging(joystick_p j)
{
	return j ? j->is_dragging : false;
}

void joystick_touch_down(joystick_p j, float x, float y)
{
	if (!j) return;
	vec2_t logic = r2d_coordinates_from_window(j->r2d, x, y);
	float dist_sq = distance_squared(logic.x, logic.y, j->center.x, j->center.y);
	if (dist_sq <= (j->radius * j->radius)) {
		j->is_dragging = true;
		j->is_touch_active = true;
		j->touch_id = -1;
		joystick_update_drag(j, logic.x, logic.y);
	}
}

void joystick_touch_move(joystick_p j, float x, float y)
{
	if (!j || !j->is_dragging) return;
	vec2_t logic = r2d_coordinates_from_window(j->r2d, x, y);
	joystick_update_drag(j, logic.x, logic.y);
}

void joystick_touch_up(joystick_p j)
{
	if (!j) return;
	joystick_reset(j);
}

// ����ҡ��
void joystick_reset(joystick_p joystick)
{
	joystick->direction.x = 0.0f;
	joystick->direction.y = 0.0f;
	joystick->magnitude = 0.0f;
	joystick->is_dragging = false;
	joystick->is_touch_active = false;
	joystick->touch_id = -1;

	// �ֱ��ص�����
	joystick->handle.x = joystick->center.x - joystick->handle_radius;
	joystick->handle.y = joystick->center.y - joystick->handle_radius;
}

// ����ҡ��
void joystick_draw(joystick_p joystick)
{
	jr2d_p r2d = joystick->r2d;

	// ���Ʊ���Բ��
	r2d_set_color(r2d,
		(JOYSTICK_BG_COLOR >> 24) & 0xFF,
		(JOYSTICK_BG_COLOR >> 16) & 0xFF,
		(JOYSTICK_BG_COLOR >> 8) & 0xFF,
		JOYSTICK_BG_COLOR & 0xFF);
	// ����Բ�α�����ʹ�ö����ģ��Բ�Σ�
	shape_draw_circle(r2d, "line", joystick->center, joystick->radius, 32);

	// ����ʮ����
	r2d_set_color(r2d, 0x66, 0x66, 0x66, 0xFF);
	
	shape_draw_line(r2d,
		joystick->center.x - joystick->radius, joystick->center.y,
		joystick->center.x + joystick->radius, joystick->center.y);
	shape_draw_line(r2d,
		joystick->center.x, joystick->center.y - joystick->radius,
		joystick->center.x, joystick->center.y + joystick->radius);

	// �����ֱ�
	jcolor_t handle_color = joystick->is_dragging ?
		(jcolor_t) {
		0x4C, 0xAF, 0x50, 0xFF
	} :
		(jcolor_t) {
		0x88, 0x88, 0x88, 0xFF
	};

		r2d_set_color(r2d, handle_color.r, handle_color.g, handle_color.b, handle_color.a);

		// ����Բ���ֱ�

		vec2_t handle_center = {
		joystick->handle.x + joystick->handle_radius,
		joystick->handle.y + joystick->handle_radius
		};
		shape_draw_circle(r2d, "fill", handle_center, joystick->radius * 0.5f, 24);

		// ����ֱ�
		r2d_set_color(r2d, handle_color.r, handle_color.g, handle_color.b, handle_color.a - 0x40);

		jrect_t handle_fill = {
		    joystick->handle.x + 2,
		    joystick->handle.y + 2,
		    joystick->handle.w - 4,
		    joystick->handle.h - 4
		};
		shape_draw_rectangle(r2d, "fill", handle_fill);


		if (joystick_get_magnitude(joystick) > 0) {
			vec2_t dir = joystick_get_direction(joystick);
			r2d_set_color(r2d, 0, 0, 0, 100);
			shape_draw_line(r2d,
				joystick->center.x,
				joystick->center.y,
				joystick->center.x + dir.x * joystick->radius,
				joystick->center.y + dir.y * joystick->radius);
		}
}

// ��ȡҡ�˷���
vec2_t joystick_get_direction(joystick_p joystick)
{
	return joystick->direction;
}

// ��ȡҡ��ǿ��
float joystick_get_magnitude(joystick_p joystick)
{
	return joystick->magnitude;
}

/* =================================================================
 * InputBox
 * ================================================================= */

#define INPUTBOX_BUF_INIT 256

inputbox_p inputbox_create(jr2d_p r2d, jrect_t rect, font_p font)
{
	inputbox_p ti = (inputbox_p)calloc(1, sizeof(inputbox_t));
	if (!ti) return NULL;
	ti->r2d   = r2d;
	ti->rect       = rect;
	ti->font       = font;
	ti->bg_color   = (jcolor_t){255, 255, 255, 255};
	ti->border_color = (jcolor_t){180, 180, 180, 255};
	ti->focus_border_color = (jcolor_t){66, 133, 244, 255};
	ti->text_color = (jcolor_t){0, 0, 0, 255};
	ti->cursor_color = (jcolor_t){0, 0, 0, 255};
	ti->sel_color    = (jcolor_t){100, 150, 255, 255};
	ti->buf_len    = INPUTBOX_BUF_INIT;
	ti->buffer     = (char*)calloc(1, ti->buf_len);
	ti->text_len   = 0;
	ti->cursor_pos = 0;
	ti->sel_start  = -1;
	ti->sel_end    = -1;
	ti->focused    = false;
	ti->cursor_timer = get_ticks();
	ti->cursor_visible = true;
	return ti;
}

void inputbox_destroy(inputbox_p ti)
{
	if (!ti) return;
	free(ti->buffer);
	free(ti);
}


/* ---- 选中支持 ---- */

static void inputbox_clear_sel(inputbox_p ti)
{
	ti->sel_start = ti->sel_end = -1;
}

static bool inputbox_has_sel(inputbox_p ti)
{
	return ti->sel_start >= 0 && ti->sel_end >= 0 && ti->sel_start != ti->sel_end;
}

/* 删除选中文本，将光标置于选中起始 */
static void inputbox_delete_sel(inputbox_p ti)
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


void inputbox_set_text(inputbox_p ti, const char* text)
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

const char* inputbox_get_text(inputbox_p ti)
{
	return ti ? ti->buffer : "";
}

void inputbox_set_callback(inputbox_p ti,
	void (*submit)(inputbox_p, void*),
	void (*change)(inputbox_p, void*),
	void* userdata)
{
	if (!ti) return;
	ti->on_submit  = submit;
	ti->on_change  = change;
	ti->cb_userdata = userdata;
}

static int inputbox_cursor_x(inputbox_p ti);

/* 滚动到光标可见 */
static void inputbox_scroll_to_cursor(inputbox_p ti)
{
	int cw = inputbox_cursor_x(ti);
	int visible_w = (int)ti->rect.w - 8;
	if (cw - ti->scroll_offset > visible_w)
		ti->scroll_offset = cw - visible_w;
	else if (cw - ti->scroll_offset < 0)
		ti->scroll_offset = cw;
}

/* 在 buffer 的 cursor_pos 处插入 len 字节 */
void inputbox_insert(inputbox_p ti, const char* s, int len)
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
static void inputbox_move_cursor(inputbox_p ti, int pos)
{
	ti->cursor_pos = pos;
	inputbox_clear_sel(ti);
	inputbox_scroll_to_cursor(ti);
}

/* 扩展选中到光标位置 */
static void inputbox_update_sel(inputbox_p ti)
{
	if (ti->sel_start < 0) ti->sel_start = ti->cursor_pos;
	ti->sel_end = ti->cursor_pos;
}

/* ---- end 选中支持 ---- */

void inputbox_cursor_left(inputbox_p ti)
{
	if (!ti || ti->cursor_pos <= 0) return;
	inputbox_move_cursor(ti, ti->cursor_pos - 1);
}

void inputbox_cursor_right(inputbox_p ti)
{
	if (!ti || ti->cursor_pos >= ti->text_len) return;
	inputbox_move_cursor(ti, ti->cursor_pos + 1);
}

void inputbox_cursor_home(inputbox_p ti)
{
	if (!ti) return;
	inputbox_move_cursor(ti, 0);
	ti->scroll_offset = 0;
}

void inputbox_cursor_end(inputbox_p ti)
{
	if (!ti) return;
	inputbox_move_cursor(ti, ti->text_len);
	inputbox_scroll_to_cursor(ti);
}

void inputbox_select_left(inputbox_p ti)
{
	if (!ti || ti->cursor_pos <= 0) return;
	ti->cursor_pos--;
	inputbox_update_sel(ti);
	inputbox_scroll_to_cursor(ti);
}

void inputbox_select_right(inputbox_p ti)
{
	if (!ti || ti->cursor_pos >= ti->text_len) return;
	ti->cursor_pos++;
	inputbox_update_sel(ti);
	inputbox_scroll_to_cursor(ti);
}

void inputbox_select_home(inputbox_p ti)
{
	if (!ti) return;
	inputbox_update_sel(ti);
	ti->cursor_pos = 0;
	ti->scroll_offset = 0;
	ti->sel_end = 0;
}

void inputbox_select_end(inputbox_p ti)
{
	if (!ti) return;
	inputbox_update_sel(ti);
	ti->cursor_pos = ti->text_len;
	ti->sel_end = ti->text_len;
	inputbox_scroll_to_cursor(ti);
}

void inputbox_select_all(inputbox_p ti)
{
	if (!ti) return;
	ti->sel_start = 0;
	ti->sel_end = ti->text_len;
	ti->cursor_pos = ti->text_len;
	inputbox_scroll_to_cursor(ti);
}

/* 删除光标前一个 UTF-8 字符（退格） */
void inputbox_backspace(inputbox_p ti)
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
void inputbox_delete(inputbox_p ti)
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

void inputbox_paste(inputbox_p ti)
{
	if (!ti) return;
	const char* clip = get_clipboard_text();
	if (clip) inputbox_insert(ti, clip, (int)strlen(clip));
}

void inputbox_copy(inputbox_p ti)
{
	if (!ti || ti->text_len <= 0) return;
	if (inputbox_has_sel(ti)) {
		int a = ti->sel_start, b = ti->sel_end;
		if (a > b) { int t = a; a = b; b = t; }
		int len = b - a;
		char* buf = (char*)malloc(len + 1);
		memcpy(buf, ti->buffer + a, len);
		buf[len] = '\0';
		set_clipboard_text(buf);
		free(buf);
	} else {
		set_clipboard_text(ti->buffer);
	}
}

void inputbox_cut(inputbox_p ti)
{
	if (!ti || ti->text_len <= 0) return;
	if (inputbox_has_sel(ti)) {
		int a = ti->sel_start, b = ti->sel_end;
		if (a > b) { int t = a; a = b; b = t; }
		int len = b - a;
		char* buf = (char*)malloc(len + 1);
		memcpy(buf, ti->buffer + a, len);
		buf[len] = '\0';
		set_clipboard_text(buf);
		free(buf);
		inputbox_delete_sel(ti);
		ti->buffer[ti->text_len] = '\0';
		if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
	} else {
		set_clipboard_text(ti->buffer);
		ti->text_len = 0;
		ti->cursor_pos = 0;
		ti->buffer[0] = '\0';
		if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
	}
}

/* 获取光标之前的文本宽度（像素） */
static int inputbox_cursor_x(inputbox_p ti)
{
	if (ti->cursor_pos <= 0 || !ti->font) return 0;
	/* 用 text_createx 生成光标前的子串，量宽度 */
	jcolor_t c = {0,0,0,255};
	jtext_p tex = text_createx(ti->r2d, ti->font, ti->buffer, ti->cursor_pos, c);
	if (!tex) return 0;
	int w = text_get_width(tex);
	text_destroy(tex);
	return w;
}

void inputbox_focus(inputbox_p ti)
{
	if (!ti || ti->focused) return;
	ti->focused = true;
	ti->cursor_timer = get_ticks();
	ti->cursor_visible = true;
	/* 从 r2d 获取 window 并启用文本输入 */
	SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)r2d_unwrap(ti->r2d));
	if (win) SDL_StartTextInput(win);
}

void inputbox_defocus(inputbox_p ti)
{
	if (!ti || !ti->focused) return;
	ti->focused = false;
	inputbox_clear_sel(ti);
	SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)r2d_unwrap(ti->r2d));
	if (win) SDL_StopTextInput(win);
}

void inputbox_draw(inputbox_p ti)
{
	if (!ti) return;
	jr2d_p r = ti->r2d;
	jrect_t rect = ti->rect;

	/* 背景 */
	r2d_set_color(r, ti->bg_color.r, ti->bg_color.g, ti->bg_color.b, ti->bg_color.a);
	shape_draw_rectangle(r, "fill", rect);

	/* 边框（聚焦时高亮） */
	jcolor_t border = ti->focused ? ti->focus_border_color : ti->border_color;
	r2d_set_color(r, border.r, border.g, border.b, border.a);
	shape_draw_rectangle(r, "line", rect);

	/* 选中高亮（绘制在文字下方） */
	if (inputbox_has_sel(ti) && ti->font) {
		int a = ti->sel_start, b = ti->sel_end;
		if (a > b) { int t = a; a = b; b = t; }
		jcolor_t c = {0,0,0,255};
		jtext_p ta = text_createx(r, ti->font, ti->buffer, a, c);
		jtext_p tb = text_createx(r, ti->font, ti->buffer + a, b - a, c);
		if (ta && tb) {
			float sx = rect.x + 4 + (float)(text_get_width(ta) - ti->scroll_offset);
			float sw = (float)text_get_width(tb);
			jrect_t sel_rect = { sx, rect.y + 1, sw, rect.h - 2 };
			r2d_set_color(r, ti->sel_color.r, ti->sel_color.g, ti->sel_color.b, ti->sel_color.a);
			shape_draw_rectangle(r, "fill", sel_rect);
		}
		if (ta) text_destroy(ta);
		if (tb) text_destroy(tb);
	}

	/* 文字 */
	if (ti->text_len > 0 && ti->font) {
		float text_x = rect.x + 4;
		float text_y = rect.y + (rect.h - (float)font_get_size(ti->font)) / 2.0f;
		float ox = text_x - (float)ti->scroll_offset;

		if (inputbox_has_sel(ti)) {
			int a = ti->sel_start, b = ti->sel_end;
			if (a > b) { int t = a; a = b; b = t; }
			jcolor_t white = {255, 255, 255, 255};
			/* 选中前部分 */
			if (a > 0)
				jtext_printx(r, ti->font, ti->buffer, a, ti->text_color, ox, text_y);
			/* 选中部分（白色） */
			if (b > a) {
				float offset_before = (float)text_get_width(text_createx(r, ti->font, ti->buffer, a, ti->text_color));
				jtext_printx(r, ti->font, ti->buffer + a, b - a, white, ox + offset_before, text_y);
			}
			/* 选中后部分 */
			if (b < ti->text_len) {
				float offset_sel = (float)text_get_width(text_createx(r, ti->font, ti->buffer, b, ti->text_color));
				jtext_printx(r, ti->font, ti->buffer + b, ti->text_len - b, ti->text_color, ox + offset_sel, text_y);
			}
		} else {
			jtext_printx(r, ti->font, ti->buffer, ti->text_len, ti->text_color, ox, text_y);
		}
	}

	/* 光标（聚焦时绘制） */
	if (ti->focused) {
		uint64_t now = get_ticks();
		if (now - ti->cursor_timer >= 530) {
			ti->cursor_visible = !ti->cursor_visible;
			ti->cursor_timer = now;
		}
		if (ti->cursor_visible) {
			int cx = inputbox_cursor_x(ti) - ti->scroll_offset + (int)rect.x + 4;
			if (cx >= rect.x + 2 && cx <= rect.x + rect.w - 2) {
				float cy = rect.y + 3;
				float ch = rect.h - 6;
				r2d_set_color(r, ti->cursor_color.r, ti->cursor_color.g, ti->cursor_color.b, ti->cursor_color.a);
				shape_draw_line(r, (float)cx + 0.5f, cy, (float)cx + 0.5f, cy + ch);
			}
		}
	}
}

bool inputbox_is_pressed(inputbox_p ti, float x, float y)
{
	if (!ti) return false;
	vec2_t logic;
	rectangle_t rect;
	rect.x = ti->rect.x;
	rect.y = ti->rect.y;
	rect.width = ti->rect.w;
	rect.height = ti->rect.h;
	logic = r2d_coordinates_from_window(ti->r2d, x, y);
	if (collision2d_check_point_rectangle(logic, rect)) {
		inputbox_focus(ti);
		return true;
	}
	inputbox_defocus(ti);
	return false;
}

bool inputbox_is_hovered(inputbox_p ti, float x, float y)
{
	if (!ti) return false;
	vec2_t logic;
	rectangle_t rect;
	rect.x = ti->rect.x;
	rect.y = ti->rect.y;
	rect.width = ti->rect.w;
	rect.height = ti->rect.h;
	logic = r2d_coordinates_from_window(ti->r2d, x, y);
	ti->hovered = collision2d_check_point_rectangle(logic, rect);
	return ti->hovered;
}

bool inputbox_is_focused(inputbox_p ti)
{
	return ti ? ti->focused : false;
}

/* =================================================================
 * Slider
 * ================================================================= */

static float slider_thumb_x(slider_p s)
{
	float range = s->max - s->min;
	if (range <= 0) return s->track.x;
	float ratio = (s->value - s->min) / range;
	return s->track.x + ratio * s->track.w;
}

static jrect_t slider_thumb_rect(slider_p s)
{
	float tx = slider_thumb_x(s);
	jrect_t r;
	r.x = tx - s->thumb_width * 0.5f;
	r.y = s->track.y;
	r.w = s->thumb_width;
	r.h = s->track.h;
	return r;
}

static void slider_update_value_from_x(slider_p s, float logic_x)
{
	float ratio = (logic_x - s->track.x) / s->track.w;
	float v = s->min + ratio * (s->max - s->min);
	if (v < s->min) v = s->min;
	if (v > s->max) v = s->max;
	s->value = v;
}

static bool slider_thumb_hit(slider_p s, float logic_x, float logic_y)
{
	jrect_t tr = slider_thumb_rect(s);
	return logic_x >= tr.x && logic_x <= tr.x + tr.w
	    && logic_y >= tr.y && logic_y <= tr.y + tr.h;
}

slider_p slider_create(jr2d_p r2d, float x, float y, float w, float h, float min, float max, float initial)
{
	slider_p s = (slider_p)calloc(1, sizeof(slider_t));
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
	s->track_color   = (jcolor_t){ 80,  80,  80,  255 };
	s->fill_color    = (jcolor_t){ 66,  133, 244, 255 };
	s->thumb_color   = (jcolor_t){ 200, 200, 200, 255 };
	s->thumb_hover_color = (jcolor_t){ 240, 240, 240, 255 };
	return s;
}

void slider_destroy(slider_p s)
{
	free(s);
}

void slider_set_value(slider_p s, float v)
{
	if (!s) return;
	if (v < s->min) v = s->min;
	if (v > s->max) v = s->max;
	s->value = v;
}

float slider_get_value(slider_p s)
{
	return s ? s->value : 0;
}

void slider_set_range(slider_p s, float min, float max)
{
	if (!s) return;
	s->min = min;
	s->max = max;
	if (s->value < min) s->value = min;
	if (s->value > max) s->value = max;
}

void slider_draw(slider_p s)
{
	if (!s) return;
	jr2d_p r = s->r2d;

	/* 滑轨背景 */
	r2d_set_color(r, s->track_color.r, s->track_color.g, s->track_color.b, s->track_color.a);
	shape_draw_rectangle(r, "fill", s->track);

	/* 已填充部分（从 track.x 到 thumb 中心） */
	float tx = slider_thumb_x(s);
	float fill_w = tx - s->track.x;
	if (fill_w > 0) {
		r2d_set_color(r, s->fill_color.r, s->fill_color.g, s->fill_color.b, s->fill_color.a);
		jrect_t fill_rect = { s->track.x, s->track.y, fill_w, s->track.h };
		shape_draw_rectangle(r, "fill", fill_rect);
	}

	/* 滑块 */
	jrect_t tr = slider_thumb_rect(s);
	jcolor_t tc = s->hovered ? s->thumb_hover_color : s->thumb_color;
	r2d_set_color(r, tc.r, tc.g, tc.b, tc.a);
	shape_draw_rectangle(r, "fill", tr);
	r2d_set_color(r, 0, 0, 0, 255);
	shape_draw_rectangle(r, "line", tr);
}

bool slider_is_pressed(slider_p s, float x, float y)
{
	if (!s) return false;
	vec2_t logic = r2d_coordinates_from_window(s->r2d, x, y);
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

void slider_set_pressed(slider_p s, bool v)
{
	if (s) s->is_dragging = v;
}

void slider_drag_move(slider_p s, float x, float y)
{
	if (!s || !s->is_dragging) return;
	vec2_t logic = r2d_coordinates_from_window(s->r2d, x, y);
	slider_update_value_from_x(s, logic.x);
}

bool slider_is_hovered(slider_p s, float x, float y)
{
	if (!s) return false;
	vec2_t logic = r2d_coordinates_from_window(s->r2d, x, y);
	s->hovered = slider_thumb_hit(s, logic.x, logic.y);
	return s->hovered;
}

void slider_set_hovered(slider_p s, bool v)
{
	if (s) s->hovered = v;
}

/* ===== Popup ===== */
#define POPUP_CLOSE_SIZE 24
#define POPUP_PADDING 12
#define POPUP_TITLE_HEIGHT 32

popup_p popup_create(jr2d_p r2d, float w, float h)
{
	popup_p p = (popup_p)calloc(1, sizeof(popup_t));
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
	p->bg_color = (jcolor_t){ 45, 45, 55, 240 };
	p->border_color = (jcolor_t){ 80, 80, 100, 255 };
	p->title_bg_color = (jcolor_t){ 55, 55, 70, 240 };
	p->content_color = (jcolor_t){ 220, 220, 230, 255 };
	p->close_color = (jcolor_t){ 200, 60, 60, 255 };
	p->close_hover_color = (jcolor_t){ 255, 80, 80, 255 };
	p->title_text_color = (jcolor_t){ 255, 255, 255, 255 };
	p->title[0] = '\0';
	p->content[0] = '\0';
	return p;
}

void popup_destroy(popup_p p)
{
	free(p);
}

void popup_set_title(popup_p p, const char* text, int len)
{
	if (!p || !text) return;
	if (len < 0) len = (int)strlen(text);
	if (len >= POPUP_MAX_TITLE) len = POPUP_MAX_TITLE - 1;
	memcpy(p->title, text, len);
	p->title[len] = '\0';
}

void popup_set_content(popup_p p, const char* text, int len)
{
	if (!p || !text) return;
	if (len < 0) len = (int)strlen(text);
	if (len >= POPUP_MAX_CONTENT) len = POPUP_MAX_CONTENT - 1;
	memcpy(p->content, text, len);
	p->content[len] = '\0';
}

void popup_set_position(popup_p p, float x, float y)
{
	if (p) { p->x = x; p->y = y; }
}

void popup_set_visible(popup_p p, bool v)
{
	if (p) p->visible = v;
}

void popup_set_closeable(popup_p p, bool v)
{
	if (p) p->closeable = v;
}

static void popup_draw_text_lines(jr2d_p r2d, font_p font, const char* text, float x, float y, float max_w, jcolor_t color)
{
	if (!text || text[0] == '\0') return;
	/* 按行分割渲染 */
	const char* start = text;
	while (*start) {
		const char* end = strchr(start, '\n');
		if (!end) end = start + strlen(start);
		int line_len = (int)(end - start);
		if (line_len > 0) {
			jtext_printx(r2d, font, start, line_len, color, x, y);
			y += (float)font_get_size(font) + 4;
		}
		if (*end == '\n') end++;
		start = end;
	}
}

void popup_draw(popup_p p, font_p title_font, font_p content_font)
{
	if (!p || !p->visible) return;
	jr2d_p r = p->r2d;
	float x = p->x, y = p->y, w = p->w, h = p->h;

	/* 半透明背景遮罩（使用 blend 模式） */
	r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
	r2d_set_color(r, 0, 0, 0, 120);
	jrect_t full = { 0, 0, 1920, 1080 };
	shape_draw_rectangle(r, "fill", full);
	r2d_set_blend_mode(r, JR2D_BLEND_NONE);

	/* 边框 */
	r2d_set_color(r, p->border_color.r, p->border_color.g, p->border_color.b, p->border_color.a);
	shape_draw_rectangle(r, "line", (jrect_t){ x - 1, y - 1, w + 2, h + 2 });

	/* 背景 */
	r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
	r2d_set_color(r, p->bg_color.r, p->bg_color.g, p->bg_color.b, p->bg_color.a);
	shape_draw_rectangle(r, "fill", (jrect_t){ x, y, w, h });

	/* 标题栏背景 */
	r2d_set_color(r, p->title_bg_color.r, p->title_bg_color.g, p->title_bg_color.b, p->title_bg_color.a);
	shape_draw_rectangle(r, "fill", (jrect_t){ x, y, w, p->title_height });

	/* 标题栏分隔线 */
	r2d_set_color(r, p->border_color.r, p->border_color.g, p->border_color.b, p->border_color.a);
	shape_draw_line(r, x, y + p->title_height, x + w, y + p->title_height);

	/* 标题文字 */
	if (p->title[0] && title_font) {
		int title_len = (int)strlen(p->title);
		jtext_printx(r, title_font, p->title, title_len, p->title_text_color,
			x + p->padding, y + (p->title_height - (float)font_get_size(title_font)) / 2);
	}

	/* 关闭按钮 */
	if (p->closeable) {
		float cx = x + w - p->close_size - 4;
		float cy = y + (p->title_height - p->close_size) / 2;
		if (p->close_hovered)
			r2d_set_color(r, p->close_hover_color.r, p->close_hover_color.g, p->close_hover_color.b, p->close_hover_color.a);
		else
			r2d_set_color(r, p->close_color.r, p->close_color.g, p->close_color.b, p->close_color.a);
		shape_draw_rectangle(r, "fill", (jrect_t){ cx, cy, p->close_size, p->close_size });
		/* X 标记 */
		r2d_set_color(r, 255, 255, 255, 255);
		shape_draw_line(r, cx + 6, cy + 6, cx + p->close_size - 6, cy + p->close_size - 6);
		shape_draw_line(r, cx + p->close_size - 6, cy + 6, cx + 6, cy + p->close_size - 6);
	}

	/* 内容文字 */
	if (p->content[0] && content_font) {
		r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
		popup_draw_text_lines(r, content_font, p->content,
			x + p->padding, y + p->title_height + p->padding,
			w - p->padding * 2, p->content_color);
	}
	r2d_set_blend_mode(r, JR2D_BLEND_NONE);
}

bool popup_is_close_clicked(popup_p p, float mx, float my)
{
	if (!p || !p->visible || !p->closeable) return false;
	float cx = p->x + p->w - p->close_size - 4;
	float cy = p->y + (p->title_height - p->close_size) / 2;
	return mx >= cx && mx <= cx + p->close_size &&
		my >= cy && my <= cy + p->close_size;
}

bool popup_is_title_clicked(popup_p p, float mx, float my)
{
	if (!p || !p->visible) return false;
	return mx >= p->x && mx <= p->x + p->w &&
		my >= p->y && my <= p->y + p->title_height;
		/* 关闭按钮区域单独处理，但 title 区域也包含它 */
}

void popup_drag_move(popup_p p, float mx, float my)
{
	if (!p || !p->is_dragging) return;
	p->x = mx - p->drag_off_x;
	p->y = my - p->drag_off_y;
}

bool popup_is_inside(popup_p p, float mx, float my)
{
	if (!p || !p->visible) return false;
	return mx >= p->x && mx <= p->x + p->w &&
		my >= p->y && my <= p->y + p->h;
}

/* ===== Tooltip ===== */
tooltip_p tooltip_create(jr2d_p r2d, const char* text, int len)
{
	tooltip_p t = (tooltip_p)calloc(1, sizeof(tooltip_t));
	if (!t) return NULL;
	t->r2d = r2d;
	t->x = 0; t->y = 0;
	t->padding = 8;
	t->visible = true;
	t->bg_color = (jcolor_t){ 40, 40, 55, 220 };
	t->border_color = (jcolor_t){ 100, 100, 130, 255 };
	t->text_color = (jcolor_t){ 255, 255, 255, 255 };
	tooltip_set_text(t, text, len);
	return t;
}

void tooltip_destroy(tooltip_p t)
{
	free(t);
}

void tooltip_set_text(tooltip_p t, const char* text, int len)
{
	if (!t || !text) return;
	if (len < 0) len = (int)strlen(text);
	if (len >= TOOLTIP_MAX_TEXT) len = TOOLTIP_MAX_TEXT - 1;
	memcpy(t->text, text, len);
	t->text[len] = '\0';
	t->text_len = len;
}

void tooltip_set_position(tooltip_p t, float x, float y)
{
	if (t) { t->x = x; t->y = y; }
}

void tooltip_set_visible(tooltip_p t, bool v)
{
	if (t) t->visible = v;
}

void tooltip_draw(tooltip_p t, font_p font)
{
	if (!t || !t->visible || !t->text_len) return;
	jr2d_p r = t->r2d;

	/* 估算宽高：按字符数 * fontsize/2 粗略估算 */
	float fh = (float)font_get_size(font);
	float tw = t->text_len * fh * 0.55f + t->padding * 2;
	float th = fh + t->padding;

	/* 背景 */
	r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
	r2d_set_color(r, t->bg_color.r, t->bg_color.g, t->bg_color.b, t->bg_color.a);
	shape_draw_rectangle(r, "fill", (jrect_t){ t->x, t->y, tw, th });

	/* 边框 */
	r2d_set_blend_mode(r, JR2D_BLEND_NONE);
	r2d_set_color(r, t->border_color.r, t->border_color.g, t->border_color.b, t->border_color.a);
	shape_draw_rectangle(r, "line", (jrect_t){ t->x, t->y, tw, th });

	/* 文字 */
	jtext_printx(r, font, t->text, t->text_len, t->text_color,
		t->x + t->padding, t->y + t->padding / 2);
}

/* =================================================================
 * 通用事件处理函数
 * 将 label_handle_event 重命名为 label_process_event，风格统一
 * ================================================================= */

void label_process_event(label_p label, const void* ev)
{
	(void)label;
	(void)ev;
}

void button_process_event(button_p btn, const void* ev)
{
	if (!btn || !ev) return;
	const SDL_Event* e = (const SDL_Event*)ev;
	if (e->type == SDL_EVENT_MOUSE_MOTION) {
		button_is_hovered(btn, e->motion.x, e->motion.y);
	} else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		button_is_pressed(btn, e->button.x, e->button.y);
	} else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		btn->is_pressed = false;
	}
}

void inputbox_process_event(inputbox_p ti, const void* ev)
{
	if (!ti || !ev) return;
	const SDL_Event* e = (const SDL_Event*)ev;
	if (e->type == SDL_EVENT_MOUSE_MOTION) {
		inputbox_is_hovered(ti, e->motion.x, e->motion.y);
	} else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		inputbox_is_pressed(ti, e->button.x, e->button.y);
	} else if (e->type == SDL_EVENT_TEXT_INPUT) {
		if (ti->focused) {
			inputbox_insert(ti, e->text.text, (int)strlen(e->text.text));
		}
	} else if (e->type == SDL_EVENT_KEY_DOWN) {
		if (!ti->focused) return;
		switch (e->key.scancode) {
		case SDL_SCANCODE_BACKSPACE: inputbox_backspace(ti); break;
		case SDL_SCANCODE_DELETE:    inputbox_delete(ti); break;
		case SDL_SCANCODE_LEFT:
			if (e->key.mod & SDL_KMOD_SHIFT) inputbox_select_left(ti);
			else inputbox_cursor_left(ti);
			break;
		case SDL_SCANCODE_RIGHT:
			if (e->key.mod & SDL_KMOD_SHIFT) inputbox_select_right(ti);
			else inputbox_cursor_right(ti);
			break;
		case SDL_SCANCODE_HOME:
			if (e->key.mod & SDL_KMOD_SHIFT) inputbox_select_home(ti);
			else inputbox_cursor_home(ti);
			break;
		case SDL_SCANCODE_END:
			if (e->key.mod & SDL_KMOD_SHIFT) inputbox_select_end(ti);
			else inputbox_cursor_end(ti);
			break;
		case SDL_SCANCODE_A:
			if (e->key.mod & SDL_KMOD_CTRL) inputbox_select_all(ti);
			break;
		case SDL_SCANCODE_V:
			if (e->key.mod & SDL_KMOD_CTRL) inputbox_paste(ti);
			break;
		case SDL_SCANCODE_C:
			if (e->key.mod & SDL_KMOD_CTRL) inputbox_copy(ti);
			break;
		case SDL_SCANCODE_X:
			if (e->key.mod & SDL_KMOD_CTRL) inputbox_cut(ti);
			break;
		default: break;
		}
	}
}

void slider_process_event(slider_p s, const void* ev)
{
	if (!s || !ev) return;
	const SDL_Event* e = (const SDL_Event*)ev;
	if (e->type == SDL_EVENT_MOUSE_MOTION) {
		slider_is_hovered(s, e->motion.x, e->motion.y);
		if (s->is_dragging) slider_drag_move(s, e->motion.x, e->motion.y);
	} else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		slider_is_pressed(s, e->button.x, e->button.y);
	} else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		s->is_dragging = false;
	}
}

void tooltip_process_event(tooltip_p t, const void* ev)
{
	if (!t || !ev) return;
	const SDL_Event* e = (const SDL_Event*)ev;
	if (e->type == SDL_EVENT_MOUSE_MOTION) {
		tooltip_set_position(t, e->motion.x + 12, e->motion.y - 10);
	}
}

void popup_process_event(popup_p p, const void* ev)
{
	(void)p;
	(void)ev;
	/* Popup 需要配合外部状态管理（visible、dragging），不在 C 层自动处理 */
}

void combobox_process_event(combobox_p cb, const void* ev)
{
	(void)cb;
	(void)ev;
	/* Combobox 需要配合外部状态（open/close），不在 C 层自动处理 */
}

void checkbox_process_event(checkbox_p cb, const void* ev)
{
	if (!cb || !ev) return;
	const SDL_Event* e = (const SDL_Event*)ev;
	if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		checkbox_is_pressed(cb, e->button.x, e->button.y);
	}
}

void radio_process_event(radio_p rb, const void* ev)
{
	if (!rb || !ev) return;
	const SDL_Event* e = (const SDL_Event*)ev;
	if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		radio_is_pressed(rb, e->button.x, e->button.y);
	}
}

void datagrid_process_event(datagrid_p dg, const void* ev)
{
	(void)dg;
	(void)ev;
	/* datagrid 需要配合外部状态管理 */
}

void joystick_process_event(joystick_p js, const void* ev)
{
	if (!js || !ev) return;
	const SDL_Event* e = (const SDL_Event*)ev;
	if (e->type == SDL_EVENT_FINGER_DOWN || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		float x = (e->type == SDL_EVENT_FINGER_DOWN) ? e->tfinger.x * 800 : e->button.x;
		float y = (e->type == SDL_EVENT_FINGER_DOWN) ? e->tfinger.y * 600 : e->button.y;
		joystick_touch_down(js, x, y);
	} else if (e->type == SDL_EVENT_FINGER_MOTION || e->type == SDL_EVENT_MOUSE_MOTION) {
		if (joystick_is_dragging(js)) {
			float x = (e->type == SDL_EVENT_FINGER_MOTION) ? e->tfinger.x * 800 : e->motion.x;
			float y = (e->type == SDL_EVENT_FINGER_MOTION) ? e->tfinger.y * 600 : e->motion.y;
			joystick_touch_move(js, x, y);
		}
	} else if (e->type == SDL_EVENT_FINGER_UP || e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		joystick_touch_up(js);
	}
}

/* ===== 通用函数：将 SDL 事件路由到控件数组 ===== */
void jui_process_event(void** widgets, int count, const void* event)
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
