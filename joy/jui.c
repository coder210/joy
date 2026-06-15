#include <SDL3/SDL.h>
#include "jui.h"
#include "jcollision.h"

#define JOYSTICK_BG_COLOR 0x555555FF
#define JOYSTICK_HANDLE_COLOR 0x888888FF
#define JOYSTICK_HANDLE_ACTIVE_COLOR 0x4CAF50FF

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
	label_p label = (label_p)SDL_malloc(sizeof(label_t));
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
	SDL_free(label);
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

void label_handle_event(label_p label, SDL_Event* event)
{
	(void)label;
	(void)event;
}

button_p
button_create(jr2d_p r2d, float width, float height)
{
	button_p btn = (button_p)SDL_malloc(sizeof(button_t));
	if (!btn) return NULL;
	btn->r2d = r2d;
	btn->position = (vec2_t){0, 0};
	btn->width = width;
	btn->height = height;
	btn->normal_color = (jcolor_t){ 100, 100, 200, 255 };
	btn->hover_color = (jcolor_t){ 150, 150, 250, 255 };
	btn->pressed_color = (jcolor_t){ 50, 50, 150, 255 };
	btn->jtext_pos = (vec2_t){0, 0};
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
	SDL_free(btn);
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
		dest_rect.x = rect.x + (rect.w - widht) / 2.0f + btn->jtext_pos.x;
		dest_rect.y = rect.y + (rect.h - height) / 2.0f + btn->jtext_pos.y;
		jtext_print(r2d, btn->text, dest_rect.x, dest_rect.y);
	}
}

/* 事件处理已移至 Lua 层，见 joy.lua 中的 joy.mousepressed/mousereleased/mousemoved */

combobox_p combobox_create(jr2d_p r2d, jrect_t rect)
{
	combobox_p combobox;
	combobox = (combobox_p)SDL_malloc(sizeof(combobox_t));
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
		SDL_free(combobox);
	}
}

void
combobox_set_text(combobox_p combobox, const char* filename, int fontsize,
	const int* codepoints, int len, jcolor_t color)
{
	if (combobox->text) {
		SDL_DestroyTexture(combobox->text);
	}
	//combobox->text = create_jtext(combobox->r2d, filename, fontsize, codepoints, len, color);
}

void combobox_add_item(combobox_p combobox, const char* filename, int fontsize, const int* codepoints, int len, jcolor_t color)
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
	grid = (datagrid_p)SDL_malloc(sizeof(datagrid_t));
	if (!grid) return NULL;
	SDL_memset(grid, 0, sizeof(datagrid_t));

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
	grid->col_widths = (int*)SDL_malloc(col_count * sizeof(int));
	grid->bg_color = (jcolor_t){ 0xFF, 0xFF, 0xFF, 0xFF };
	grid->grid_color = (jcolor_t){ 0xCC, 0xCC, 0xCC, 0xFF };
	grid->text_color = (jcolor_t){ 0x00, 0x00, 0x00, 0xFF };
	grid->header_bg_color = (jcolor_t){ 0xEE, 0xEE, 0xEE, 0xFF };
	grid->selected_color = (jcolor_t){ 0x33, 0x99, 0xFF, 0xFF };
	SDL_memset(grid->headers, 0, 512 * sizeof(char));
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
		//SDL_DestroyTexture(grid->headers[index]);
		//SDL_UpdateTexture(grid->headers[index], NULL, NULL, 0);
		//update_jtext(grid->headers[index], fontinfo, header->array, header->length, color);
	}
	else {
		//grid->headers[index] = create_jtext(grid->r2d, fontinfo, header->array, header->length, color);
	}
}

void datagrid_addrow(datagrid_p grid, const char** row_data)
{
	if (!grid) return;
	dg_row_t row;
	kv_init(row);
	for (int i = 0; i < grid->col_count; i++) {
		kv_push(char*, row, row_data && row_data[i] ? SDL_strdup(row_data[i]) : NULL);
	}
	kv_push(dg_row_t, grid->data, row);
	grid->row_count = (int)kv_size(grid->data);
}

void datagrid_setcell(datagrid_p grid, int row_index, int col_index, const char* data)
{
	if (!grid) return;
	if (row_index < (int)kv_size(grid->data) && col_index < grid->col_count) {
		dg_row_t *row = &kv_A(grid->data, row_index);
		if (kv_A(*row, col_index)) SDL_free(kv_A(*row, col_index));
		kv_A(*row, col_index) = data ? SDL_strdup(data) : NULL;
	}
}

static void datagrid_cleardata(datagrid_p grid)
{
	for (size_t i = 0; i < kv_size(grid->data); i++) {
		dg_row_t *row = &kv_A(grid->data, i);
		for (size_t j = 0; j < kv_size(*row); j++)
			if (kv_A(*row, j)) { SDL_free(kv_A(*row, j)); kv_A(*row, j) = NULL; }
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
	SDL_free(grid->col_widths);
	SDL_free(grid);
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
			/*jrect_t rect;
			float tw, th;*/
			/*rect.x = x;
			rect.y = y;*/
			jtext_p tex = grid->headers[col];
			/*if (tex && SDL_GetTextureSize(tex, &tw, &th)) {
				rect.w = tw;
				rect.h = th;
				SDL_RenderTexture(grid->r2d, tex, NULL, &rect);
			}*/
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
			/* r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			 jrect_t header_rect = { grid->x, y, grid->width, grid->row_height };
			 SDL_RenderFillRect(r2d, &header_rect);*/
		}

		// ���Ƶ�Ԫ�����ݺ�����
		int x = grid->x;
		for (int col = 0; col < grid->col_count; col++) {
			if ((size_t)row < kv_size(grid->data) && (size_t)col < kv_size(kv_A(grid->data, row))) {
				char* text = kv_A(kv_A(grid->data, row), col);
				if (text) {
					jcolor_t c = {0,0,0,255};
					jtext_printx(r2d, grid->font, text, (int)SDL_strlen(text), c, (float)x + 2, (float)y);
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

radio_button_p radio_button_create(int x, int y, int size, const char* label, int group_id)
{
	radio_button_p radio;
	radio = (radio_button_p)SDL_malloc(sizeof(radio_button_t));
	if (!radio) return NULL;

	radio->x = x;
	radio->y = y;
	radio->size = size;
	radio->checked = false;
	radio->group_id = group_id;
	radio->hover = false;

	// ���Ʊ�ǩ�ı�
	if (label) {
		radio->label = SDL_strdup(label);
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

void radio_button_destroy(radio_button_p radio)
{
	if (!radio) return;

	if (radio->label) {
		SDL_free(radio->label);
	}

	SDL_free(radio);
}


radio_button_group_p radio_button_group_create()
{
	radio_button_group_p group;
	group = (radio_button_group_p)SDL_malloc(sizeof(radio_button_group_t));
	if (!group) return NULL;

	group->capacity = 16;
	group->buttons = SDL_malloc(group->capacity * sizeof(radio_button_p));
	group->count = 0;
	group->selected_index = -1;

	return group;
}

void radio_button_group_add_button(radio_button_group_p group, radio_button_p radio)
{
	if (!group || !radio) return;

	group->count++;
	if (group->capacity < group->count) {
		group->capacity *= 2;
		group->buttons = SDL_realloc(group->buttons, group->capacity * sizeof(radio_button_p));
	}
	group->buttons[group->count - 1] = radio;
}

void radio_button_group_set_selected(radio_button_group_p group, int index)
{
	if (!group || index < 0 || index >= group->count) return;

	// ȡ��֮ǰѡ�еİ�ť
	if (group->selected_index >= 0 && group->selected_index < group->count) {
		group->buttons[group->selected_index]->checked = false;
	}

	// �����µ�ѡ�а�ť
	group->selected_index = index;
	group->buttons[index]->checked = true;
}

// ͨ����ťָ������ѡ��
void radio_button_group_set_selected_by_button(radio_button_group_p group, radio_button_p selected)
{
	if (!group || !selected) return;
	for (int i = 0; i < group->count; i++) {
		if (group->buttons[i] == selected) {
			radio_button_group_set_selected(group, i);
			return;
		}
	}
}

int radio_button_group_get_selected_index(radio_button_group_p group)
{
	return group ? group->selected_index : -1;
}

radio_button_p radio_button_group_get_selected_button(radio_button_group_p group)
{
	if (!group || group->selected_index < 0 || group->selected_index >= group->count) {
		return NULL;
	}
	return group->buttons[group->selected_index];
}

// �ͷŵ�ѡ��ť���ڴ�
void radio_button_group_destroy(radio_button_group_p group)
{
	if (!group) return;

	// ע�⣺���ﲻ�ͷŵ�����ť���ڴ棬�ɴ����߸���
	SDL_free(group->buttons);
	SDL_free(group);
}

void radio_button_render(radio_button_p radio, jr2d_p r2d)
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


	/*SDL_Vertex triangle[3];
	for (int i = 0; i < segments; i++) {
		float angle1 = (float)i / (float)segments * 2.0f * 3.14159f;
		float angle2 = (float)(i + 1) / (float)segments * 2.0f * 3.14159f;
		triangle[0].position = { center_x, center_y };
		triangle[0].color = radio->r1_color;
		triangle[1].position = (vec2_t){ center_x + cosf(angle1) * r1, center_y + sinf(angle1) * r1 };
		triangle[1].color = radio->r1_color;
		triangle[2].position = (vec2_t){ center_x + cosf(angle2) * r1, center_y + sinf(angle2) * r1 };
		triangle[2].color = radio->r1_color;
		SDL_RenderGeometry(r2d, NULL, triangle, 3, NULL, 0);

		triangle[0].position = (vec2_t){ center_x, center_y };
		triangle[0].color = radio->r2_color;
		triangle[1].position = (vec2_t){ center_x + cosf(angle1) * r2, center_y + sinf(angle1) * r2 };
		triangle[1].color = radio->r2_color;
		triangle[2].position = (vec2_t){ center_x + cosf(angle2) * r2, center_y + sinf(angle2) * r2 };
		triangle[2].color = radio->r2_color;
		SDL_RenderGeometry(r2d, NULL, triangle, 3, NULL, 0);

		if (radio->checked) {
			triangle[0].position = (vec2_t){ center_x, center_y };
			triangle[0].color = radio->r3_color;
			triangle[1].position = (vec2_t){ center_x + cosf(angle1) * r3, center_y + sinf(angle1) * r3 };
			triangle[1].color = radio->r3_color;
			triangle[2].position = (vec2_t){ center_x + cosf(angle2) * r3, center_y + sinf(angle2) * r3 };
			triangle[2].color = radio->r3_color;
			SDL_RenderGeometry(r2d, NULL, triangle, 3, NULL, 0);
		}
	}*/

	// ���Ʊ�ǩ�ı�
	if (radio->label) {
		/* jcolor_t text_color = { 0, 0, 0, 255 };
		 SDL_Surface* text_surface = TTF_RenderText_Blended(font, radio->label, text_color);

		 if (text_surface) {
			 SDL_Texture* jtext = SDL_CreateTextureFromSurface(r2d, text_surface);

			 if (jtext) {
				 jrect_t text_rect = {
				     radio->x + radio->size + 5,
				     radio->y + (radio->size - text_surface->h) / 2,
				     text_surface->w,
				     text_surface->h
				 };

				 SDL_RenderTexture(r2d, jtext, NULL, &text_rect);
				 SDL_DestroyTexture(jtext);
			 }

			 SDL_DestroySurface(text_surface);
		 }*/
	}
}

void radio_button_group_draw(radio_button_group_p group, jr2d_p r2d)
{
	if (!group) return;

	for (int i = 0; i < group->count; i++) {
		radio_button_render(group->buttons[i], r2d);
	}
}

bool radio_button_is_pressed(radio_button_p radio, float x, float y)
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

void radio_button_set_hovered(radio_button_p radio, bool v)
{
	if (radio) radio->hover = v;
}


checkbox_p checkbox_create(int x, int y, int size, const char* label)
{
	checkbox_p cb;
	cb = SDL_malloc(sizeof(checkbox_t));
	if (!cb) return NULL;

	cb->x = x;
	cb->y = y;
	cb->size = size;
	cb->state = JOY_CHECKBOX_UNCHECKED;
	cb->label = label ? SDL_strdup(label) : NULL;
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
		label_width = SDL_strlen(label) * 8;
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
		SDL_free(checkbox->label);
	}

	SDL_free(checkbox);
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
		/*jcolor_t textColor = checkbox->enabled ? checkbox->textColor : (jcolor_t) { 128, 128, 128, 255 };
		SDL_Surface* textSurface = TTF_RenderText_Blended(font, checkbox->label, textColor);

		if (textSurface) {
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(r2d, textSurface);

			if (textTexture) {
				jrect_t textRect = {
				    checkbox->x + checkbox->size + 5,
				    checkbox->y + (checkbox->size - textSurface->h) / 2,
				    textSurface->w,
				    textSurface->h
				};

				SDL_RenderTexture(r2d, textTexture, NULL, &textRect);
				SDL_DestroyTexture(textTexture);
			}

			SDL_DestroySurface(textSurface);
		}*/
	}
}




// ��ʼ��ҡ��
joystick_p joystick_create(jr2d_p r2d, float x, float y, float radius)
{
	joystick_p joystick;
	joystick = (joystick_p)SDL_malloc(sizeof(joystick_t));

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
	SDL_free(joystick);
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


		/*	if (joystick_get_magnitude(joystick) > 0) {
				vec2_t dir = joystick_get_direction(joystick);
				SDL_RenderLine(r2d,
					joystick->center.x,
					joystick->center.y,
					joystick->center.x + dir.x * joystick->radius,
					joystick->center.y + dir.y * joystick->radius);
			}*/
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
 * TextInput
 * ================================================================= */

#define TEXTINPUT_BUF_INIT 256

textinput_p textinput_create(jr2d_p r2d, jrect_t rect, font_p font)
{
	textinput_p ti = (textinput_p)SDL_calloc(1, sizeof(textinput_t));
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
	ti->buf_len    = TEXTINPUT_BUF_INIT;
	ti->buffer     = (char*)SDL_calloc(1, ti->buf_len);
	ti->text_len   = 0;
	ti->cursor_pos = 0;
	ti->sel_start  = -1;
	ti->sel_end    = -1;
	ti->focused    = false;
	ti->cursor_timer = SDL_GetTicks();
	ti->cursor_visible = true;
	return ti;
}

void textinput_destroy(textinput_p ti)
{
	if (!ti) return;
	SDL_free(ti->buffer);
	SDL_free(ti);
}


/* ---- 选中支持 ---- */

static void textinput_clear_sel(textinput_p ti)
{
	ti->sel_start = ti->sel_end = -1;
}

static bool textinput_has_sel(textinput_p ti)
{
	return ti->sel_start >= 0 && ti->sel_end >= 0 && ti->sel_start != ti->sel_end;
}

/* 删除选中文本，将光标置于选中起始 */
static void textinput_delete_sel(textinput_p ti)
{
	if (!textinput_has_sel(ti)) return;
	int a = ti->sel_start, b = ti->sel_end;
	if (a > b) { int t = a; a = b; b = t; }
	SDL_memmove(ti->buffer + a, ti->buffer + b, ti->text_len - b);
	ti->text_len -= (b - a);
	ti->cursor_pos = a;
	textinput_clear_sel(ti);
	ti->buffer[ti->text_len] = '\0';
}


void textinput_set_text(textinput_p ti, const char* text)
{
	if (!ti || !text) return;
	int len = (int)SDL_strlen(text);
	if (len >= ti->buf_len) {
		ti->buf_len = len + 64;
		ti->buffer = (char*)SDL_realloc(ti->buffer, ti->buf_len);
	}
	SDL_memcpy(ti->buffer, text, len);
	ti->buffer[len] = '\0';
	ti->text_len = len;
	ti->cursor_pos = len;
	textinput_clear_sel(ti);
	ti->scroll_offset = 0;
	if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
}

const char* textinput_get_text(textinput_p ti)
{
	return ti ? ti->buffer : "";
}

void textinput_set_callback(textinput_p ti,
	void (*submit)(textinput_p, void*),
	void (*change)(textinput_p, void*),
	void* userdata)
{
	if (!ti) return;
	ti->on_submit  = submit;
	ti->on_change  = change;
	ti->cb_userdata = userdata;
}

static int textinput_cursor_x(textinput_p ti);

/* 滚动到光标可见 */
static void textinput_scroll_to_cursor(textinput_p ti)
{
	int cw = textinput_cursor_x(ti);
	int visible_w = (int)ti->rect.w - 8;
	if (cw - ti->scroll_offset > visible_w)
		ti->scroll_offset = cw - visible_w;
	else if (cw - ti->scroll_offset < 0)
		ti->scroll_offset = cw;
}

/* 在 buffer 的 cursor_pos 处插入 len 字节 */
void textinput_insert(textinput_p ti, const char* s, int len)
{
	/* 有选中时先删除选中文本 */
	if (textinput_has_sel(ti)) textinput_delete_sel(ti);

	if (ti->text_len + len >= ti->buf_len) {
		while (ti->text_len + len >= ti->buf_len)
			ti->buf_len *= 2;
		ti->buffer = (char*)SDL_realloc(ti->buffer, ti->buf_len);
	}
	SDL_memmove(ti->buffer + ti->cursor_pos + len,
	            ti->buffer + ti->cursor_pos,
	            ti->text_len - ti->cursor_pos);
	SDL_memcpy(ti->buffer + ti->cursor_pos, s, len);
	ti->text_len += len;
	ti->cursor_pos += len;
	ti->buffer[ti->text_len] = '\0';
	if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
	textinput_scroll_to_cursor(ti);
}

/* 移动光标并清除选中 */
static void textinput_move_cursor(textinput_p ti, int pos)
{
	ti->cursor_pos = pos;
	textinput_clear_sel(ti);
	textinput_scroll_to_cursor(ti);
}

/* 扩展选中到光标位置 */
static void textinput_update_sel(textinput_p ti)
{
	if (ti->sel_start < 0) ti->sel_start = ti->cursor_pos;
	ti->sel_end = ti->cursor_pos;
}

/* ---- end 选中支持 ---- */

void textinput_cursor_left(textinput_p ti)
{
	if (!ti || ti->cursor_pos <= 0) return;
	textinput_move_cursor(ti, ti->cursor_pos - 1);
}

void textinput_cursor_right(textinput_p ti)
{
	if (!ti || ti->cursor_pos >= ti->text_len) return;
	textinput_move_cursor(ti, ti->cursor_pos + 1);
}

void textinput_cursor_home(textinput_p ti)
{
	if (!ti) return;
	textinput_move_cursor(ti, 0);
	ti->scroll_offset = 0;
}

void textinput_cursor_end(textinput_p ti)
{
	if (!ti) return;
	textinput_move_cursor(ti, ti->text_len);
	textinput_scroll_to_cursor(ti);
}

void textinput_select_left(textinput_p ti)
{
	if (!ti || ti->cursor_pos <= 0) return;
	ti->cursor_pos--;
	textinput_update_sel(ti);
	textinput_scroll_to_cursor(ti);
}

void textinput_select_right(textinput_p ti)
{
	if (!ti || ti->cursor_pos >= ti->text_len) return;
	ti->cursor_pos++;
	textinput_update_sel(ti);
	textinput_scroll_to_cursor(ti);
}

void textinput_select_home(textinput_p ti)
{
	if (!ti) return;
	textinput_update_sel(ti);
	ti->cursor_pos = 0;
	ti->scroll_offset = 0;
	ti->sel_end = 0;
}

void textinput_select_end(textinput_p ti)
{
	if (!ti) return;
	textinput_update_sel(ti);
	ti->cursor_pos = ti->text_len;
	ti->sel_end = ti->text_len;
	textinput_scroll_to_cursor(ti);
}

void textinput_select_all(textinput_p ti)
{
	if (!ti) return;
	ti->sel_start = 0;
	ti->sel_end = ti->text_len;
	ti->cursor_pos = ti->text_len;
	textinput_scroll_to_cursor(ti);
}

/* 删除光标前一个 UTF-8 字符（退格） */
void textinput_backspace(textinput_p ti)
{
	if (textinput_has_sel(ti)) {
		textinput_delete_sel(ti);
		textinput_scroll_to_cursor(ti);
		if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
		return;
	}
	if (ti->cursor_pos <= 0) return;
	int pos = ti->cursor_pos - 1;
	while (pos > 0 && ((unsigned char)ti->buffer[pos] & 0xC0) == 0x80)
		pos--;
	int ch_len = ti->cursor_pos - pos;
	SDL_memmove(ti->buffer + pos, ti->buffer + ti->cursor_pos,
	            ti->text_len - ti->cursor_pos);
	ti->text_len -= ch_len;
	ti->cursor_pos = pos;
	ti->buffer[ti->text_len] = '\0';
	if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
	textinput_scroll_to_cursor(ti);
}

/* 删除光标后一个 UTF-8 字符（Delete） */
void textinput_delete(textinput_p ti)
{
	if (textinput_has_sel(ti)) {
		textinput_delete_sel(ti);
		textinput_scroll_to_cursor(ti);
		if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
		return;
	}
	if (ti->cursor_pos >= ti->text_len) return;
	int pos = ti->cursor_pos;
	int ch_len = 1;
	while (pos + ch_len < ti->text_len && ((unsigned char)ti->buffer[pos + ch_len] & 0xC0) == 0x80)
		ch_len++;
	SDL_memmove(ti->buffer + pos, ti->buffer + pos + ch_len,
	            ti->text_len - pos - ch_len);
	ti->text_len -= ch_len;
	ti->buffer[ti->text_len] = '\0';
	if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
	textinput_scroll_to_cursor(ti);
}

void textinput_paste(textinput_p ti)
{
	if (!ti) return;
	const char* clip = SDL_GetClipboardText();
	if (clip) textinput_insert(ti, clip, (int)SDL_strlen(clip));
}

void textinput_copy(textinput_p ti)
{
	if (!ti || ti->text_len <= 0) return;
	if (textinput_has_sel(ti)) {
		int a = ti->sel_start, b = ti->sel_end;
		if (a > b) { int t = a; a = b; b = t; }
		int len = b - a;
		char* buf = (char*)SDL_malloc(len + 1);
		SDL_memcpy(buf, ti->buffer + a, len);
		buf[len] = '\0';
		SDL_SetClipboardText(buf);
		SDL_free(buf);
	} else {
		SDL_SetClipboardText(ti->buffer);
	}
}

void textinput_cut(textinput_p ti)
{
	if (!ti || ti->text_len <= 0) return;
	if (textinput_has_sel(ti)) {
		int a = ti->sel_start, b = ti->sel_end;
		if (a > b) { int t = a; a = b; b = t; }
		int len = b - a;
		char* buf = (char*)SDL_malloc(len + 1);
		SDL_memcpy(buf, ti->buffer + a, len);
		buf[len] = '\0';
		SDL_SetClipboardText(buf);
		SDL_free(buf);
		textinput_delete_sel(ti);
		ti->buffer[ti->text_len] = '\0';
		if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
	} else {
		SDL_SetClipboardText(ti->buffer);
		ti->text_len = 0;
		ti->cursor_pos = 0;
		ti->buffer[0] = '\0';
		if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
	}
}

/* 获取光标之前的文本宽度（像素） */
static int textinput_cursor_x(textinput_p ti)
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

void textinput_focus(textinput_p ti)
{
	if (!ti || ti->focused) return;
	ti->focused = true;
	ti->cursor_timer = SDL_GetTicks();
	ti->cursor_visible = true;
	/* 从 r2d 获取 window 并启用文本输入 */
	//SDL_Window* win = SDL_GetRenderWindow(ti->r2d);
	//if (win) SDL_StartTextInput(win);
}

void textinput_defocus(textinput_p ti)
{
	if (!ti || !ti->focused) return;
	ti->focused = false;
	textinput_clear_sel(ti);
	//SDL_Window* win = SDL_GetRenderWindow(ti->r2d);
	//if (win) SDL_StopTextInput(win);
}

void textinput_draw(textinput_p ti)
{
	if (!ti) return;
	jr2d_p r = ti->r2d;
	jrect_t rect = ti->rect;

	/* 背景 */
	//r2d_set_color(r, ti->bg_color.r, ti->bg_color.g, ti->bg_color.b, ti->bg_color.a);
	//SDL_RenderFillRect(r, &rect);

	///* 边框（聚焦时高亮） */
	//jcolor_t border = ti->focused ? ti->focus_border_color : ti->border_color;
	//r2d_set_color(r, border.r, border.g, border.b, border.a);
	//SDL_RenderRect(r, &rect);

	///* 选中高亮（绘制在文字下方） */
	//if (textinput_has_sel(ti) && ti->font) {
	//	SDL_Rect clip = { (int)rect.x + 2, (int)rect.y, (int)rect.w - 4, (int)rect.h };
	//	SDL_SetRenderClipRect(r, &clip);
	//	int a = ti->sel_start, b = ti->sel_end;
	//	if (a > b) { int t = a; a = b; b = t; }
	//	jcolor_t c = {0,0,0,255};
	//	jtext_p ta = text_createx(ti->font, ti->buffer, a, c);
	//	jtext_p tb = text_createx(ti->font, ti->buffer + a, b - a, c);
	//	if (ta && tb) {
	//		float sx = rect.x + 4 + (float)(text_get_width(ta) - ti->scroll_offset);
	//		float sw = (float)text_get_width(tb);
	//		jrect_t sel_rect = { sx, rect.y + 1, sw, rect.h - 2 };
	//		r2d_set_color(r, ti->sel_color.r, ti->sel_color.g, ti->sel_color.b, ti->sel_color.a);
	//		SDL_RenderFillRect(r, &sel_rect);
	//	}
	//	if (ta) text_destroy(ta);
	//	if (tb) text_destroy(tb);
	//	SDL_SetRenderClipRect(r, NULL);
	//}

	///* 文字（设裁剪区域防止溢出） */
	//if (ti->text_len > 0 && ti->font) {
	//	SDL_Rect clip = { (int)rect.x + 2, (int)rect.y, (int)rect.w - 4, (int)rect.h };
	//	SDL_SetRenderClipRect(r, &clip);
	//	float text_x = rect.x + 4;
	//	float text_y = rect.y + (rect.h - text_get_height(text_createx(ti->font, "A", 1, ti->text_color))) / 2.0f;
	//	float ox = text_x - (float)ti->scroll_offset;

	//	if (textinput_has_sel(ti)) {
	//		int a = ti->sel_start, b = ti->sel_end;
	//		if (a > b) { int t = a; a = b; b = t; }
	//		jcolor_t white = {255, 255, 255, 255};
	//		/* 选中前部分 */
	//		if (a > 0)
	//			jtext_printx(r, ti->font, ti->buffer, a, ti->text_color, ox, text_y);
	//		/* 选中部分（白色） */
	//		if (b > a) {
	//			float offset_before = (float)text_get_width(text_createx(ti->font, ti->buffer, a, ti->text_color));
	//			jtext_printx(r, ti->font, ti->buffer + a, b - a, white, ox + offset_before, text_y);
	//		}
	//		/* 选中后部分 */
	//		if (b < ti->text_len) {
	//			float offset_sel = (float)text_get_width(text_createx(ti->font, ti->buffer, b, ti->text_color));
	//			jtext_printx(r, ti->font, ti->buffer + b, ti->text_len - b, ti->text_color, ox + offset_sel, text_y);
	//		}
	//	} else {
	//		jtext_printx(r, ti->font, ti->buffer, ti->text_len, ti->text_color, ox, text_y);
	//	}
	//	SDL_SetRenderClipRect(r, NULL);
	//}

	///* 光标（聚焦时绘制） */
	//if (ti->focused) {
	//	Uint64 now = SDL_GetTicks();
	//	if (now - ti->cursor_timer >= 530) {
	//		ti->cursor_visible = !ti->cursor_visible;
	//		ti->cursor_timer = now;
	//	}
	//	if (ti->cursor_visible) {
	//		int cx = textinput_cursor_x(ti) - ti->scroll_offset + (int)rect.x + 4;
	//		/* 光标在可视区域内时才绘制 */
	//		if (cx >= rect.x + 2 && cx <= rect.x + rect.w - 2) {
	//			float cy = rect.y + 3;
	//			float ch = rect.h - 6;
	//			r2d_set_color(r, ti->cursor_color.r, ti->cursor_color.g, ti->cursor_color.b, ti->cursor_color.a);
	//			SDL_RenderLine(r, (float)cx + 0.5f, cy, (float)cx + 0.5f, cy + ch);
	//		}
	//	}
	//}
}

bool textinput_is_pressed(textinput_p ti, float x, float y)
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
		textinput_focus(ti);
		return true;
	}
	textinput_defocus(ti);
	return false;
}

bool textinput_is_hovered(textinput_p ti, float x, float y)
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

bool textinput_is_focused(textinput_p ti)
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
	slider_p s = (slider_p)SDL_calloc(1, sizeof(slider_t));
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
	SDL_free(s);
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
	//r2d_set_color(r, s->track_color.r, s->track_color.g, s->track_color.b, s->track_color.a);
	//SDL_RenderFillRect(r, &s->track);

	///* 已填充部分（从 track.x 到 thumb 中心） */
	//float tx = slider_thumb_x(s);
	//float fill_w = tx - s->track.x;
	//if (fill_w > 0) {
	//	r2d_set_color(r, s->fill_color.r, s->fill_color.g, s->fill_color.b, s->fill_color.a);
	//	jrect_t fill_rect = { s->track.x, s->track.y, fill_w, s->track.h };
	//	SDL_RenderFillRect(r, &fill_rect);
	//}

	///* 滑块 */
	//jrect_t tr = slider_thumb_rect(s);
	//jcolor_t tc = s->hovered ? s->thumb_hover_color : s->thumb_color;
	//r2d_set_color(r, tc.r, tc.g, tc.b, tc.a);
	//SDL_RenderFillRect(r, &tr);
	//r2d_set_color(r, 0, 0, 0, 255);
	//SDL_RenderRect(r, &tr);
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
	popup_p p = (popup_p)SDL_calloc(1, sizeof(popup_t));
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
	SDL_free(p);
}

void popup_set_title(popup_p p, const char* text, int len)
{
	if (!p || !text) return;
	if (len < 0) len = (int)SDL_strlen(text);
	if (len >= POPUP_MAX_TITLE) len = POPUP_MAX_TITLE - 1;
	SDL_memcpy(p->title, text, len);
	p->title[len] = '\0';
}

void popup_set_content(popup_p p, const char* text, int len)
{
	if (!p || !text) return;
	if (len < 0) len = (int)SDL_strlen(text);
	if (len >= POPUP_MAX_CONTENT) len = POPUP_MAX_CONTENT - 1;
	SDL_memcpy(p->content, text, len);
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
		const char* end = SDL_strchr(start, '\n');
		if (!end) end = start + SDL_strlen(start);
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
	jr2d_p r2d = p->r2d;
	float x = p->x, y = p->y, w = p->w, h = p->h;

	///* 半透明背景遮罩 */
	//SDL_SetRenderDrawBlendMode(r2d, SDL_BLENDMODE_BLEND);
	//r2d_set_color(r2d, 0, 0, 0, 120);
	//jrect_t full = { 0, 0, 1920, 1080 }; /* 覆盖全屏 */
	//shape_draw_rectangle(r2d, "fill", full);
	//SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);

	///* 边框 */
	//r2d_set_color(ren, p->border_color.r, p->border_color.g, p->border_color.b, p->border_color.a);
	//SDL_RenderRect(ren, &(jrect_t){ x - 1, y - 1, w + 2, h + 2 });

	///* 背景 */
	//SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	//r2d_set_color(ren, p->bg_color.r, p->bg_color.g, p->bg_color.b, p->bg_color.a);
	//SDL_RenderFillRect(ren, &(jrect_t){ x, y, w, h });

	///* 标题栏背景 */
	//r2d_set_color(ren, p->title_bg_color.r, p->title_bg_color.g, p->title_bg_color.b, p->title_bg_color.a);
	//SDL_RenderFillRect(ren, &(jrect_t){ x, y, w, p->title_height });

	///* 标题栏分隔线 */
	//r2d_set_color(ren, p->border_color.r, p->border_color.g, p->border_color.b, p->border_color.a);
	//SDL_RenderLine(ren, x, y + p->title_height, x + w, y + p->title_height);

	///* 标题文字 */
	//if (p->title[0] && title_font) {
	//	jtext_printx(ren, title_font, p->title, (int)SDL_strlen(p->title), p->title_text_color,
	//		x + p->padding, y + (p->title_height - (float)title_font->fontsize) / 2);
	//}

	///* 关闭按钮 */
	//if (p->closeable) {
	//	float cx = x + w - p->close_size - 4;
	//	float cy = y + (p->title_height - p->close_size) / 2;
	//	if (p->close_hovered)
	//		r2d_set_color(ren, p->close_hover_color.r, p->close_hover_color.g, p->close_hover_color.b, p->close_hover_color.a);
	//	else
	//		r2d_set_color(ren, p->close_color.r, p->close_color.g, p->close_color.b, p->close_color.a);
	//	SDL_RenderFillRect(ren, &(jrect_t){ cx, cy, p->close_size, p->close_size });
	//	/* X 标记 */
	//	r2d_set_color(ren, 255, 255, 255, 255);
	//	SDL_RenderLine(ren, cx + 6, cy + 6, cx + p->close_size - 6, cy + p->close_size - 6);
	//	SDL_RenderLine(ren, cx + p->close_size - 6, cy + 6, cx + 6, cy + p->close_size - 6);
	//}

	///* 内容文字 */
	//if (p->content[0] && content_font) {
	//	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	//	popup_draw_text_lines(ren, content_font, p->content,
	//		x + p->padding, y + p->title_height + p->padding,
	//		w - p->padding * 2, p->content_color);
	//}
	//SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
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
	tooltip_p t = (tooltip_p)SDL_calloc(1, sizeof(tooltip_t));
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
	SDL_free(t);
}

void tooltip_set_text(tooltip_p t, const char* text, int len)
{
	if (!t || !text) return;
	if (len < 0) len = (int)SDL_strlen(text);
	if (len >= TOOLTIP_MAX_TEXT) len = TOOLTIP_MAX_TEXT - 1;
	SDL_memcpy(t->text, text, len);
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
	jr2d_p r2d = t->r2d;

	///* 估算宽高：按字符数 * fontsize/2 粗略估算 */
	//float fh = (float)font->fontsize;
	//float tw = t->text_len * fh * 0.55f + t->padding * 2;
	//float th = fh + t->padding;

	///* 背景 */
	//SDL_SetRenderDrawBlendMode(r2d, SDL_BLENDMODE_BLEND);
	//r2d_set_color(r2d, t->bg_color.r, t->bg_color.g, t->bg_color.b, t->bg_color.a);
	//SDL_RenderFillRect(ren, &(jrect_t){ t->x, t->y, tw, th });

	///* 边框 */
	//SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
	//r2d_set_color(ren, t->border_color.r, t->border_color.g, t->border_color.b, t->border_color.a);
	//SDL_RenderRect(ren, &(jrect_t){ t->x, t->y, tw, th });

	///* 文字 */
	//jtext_printx(ren, font, t->text, t->text_len, t->text_color,
	//	t->x + t->padding, t->y + t->padding / 2);
}
