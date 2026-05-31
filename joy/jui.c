#include "jmath.h"
#include "jshapes.h"
#include "jui.h"


// ��ɫ���
#define JOYSTICK_BG_COLOR 0x555555FF
#define JOYSTICK_HANDLE_COLOR 0x888888FF
#define JOYSTICK_HANDLE_ACTIVE_COLOR 0x4CAF50FF

// ҡ�˽ṹ��
struct joystick {
	SDL_Renderer* renderer;      // ��Ⱦ��
	SDL_FRect background;        // ��������
	SDL_FRect handle;           // �ֱ�����
	SDL_FPoint center;          // ҡ�����ĵ�
	float radius;               // �����뾶
	float handle_radius;        // �ֱ��뾶
	SDL_FPoint direction;       // �������� (x: -1��1, y: -1��1)
	float magnitude;            // ǿ�� (0��1)
	float deadzone;             // ������ֵ
	bool is_dragging;           // �Ƿ������϶�
	bool is_touch_active;       // �Ƿ�������
	int touch_id;               // ����ID
};

// ������������
static float distance_squared(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx * dx + dy * dy;
}

label_p
label_create(SDL_Renderer* renderer, font_p fontinfo, const int* codepoints, int num_codepoints)
{
	label_p label = (label_p)SDL_malloc(sizeof(label_t));
	label->renderer = renderer;
	label->position = (SDL_FPoint){ 0, 0 };
	label->color = (SDL_Color){ 255, 255,255, 255 };
	label->text = text_create(fontinfo, codepoints, num_codepoints, label->color);
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
label_set_text(label_p label, font_p fontinfo,
	const int* codepoints, int num_codepoints,
	SDL_Color color)
{
	if (label->text) {
		text_update(label->text, fontinfo, codepoints, num_codepoints, color);
	}
	else {
		label->text = text_create(fontinfo, codepoints, num_codepoints, color);
	}
}

void label_draw(label_p label)
{
	//SDL_FRect dest_rect;
	if (!label) return;

	//renderer = label->renderer;
	//dest_rect.x = label->position.x;
	//dest_rect.y = label->position.y;
	//dest_rect.w = label->text->w;
	//dest_rect.h = label->text->h;
	//SDL_RenderTexture(renderer, label->text, NULL, &dest_rect);
	text_print(label->renderer, label->text, label->position.x, label->position.y);
}

void label_handle_event(label_p label, SDL_Event* event)
{
	(void)label;
	(void)event;
}

button_p
button_create(SDL_Renderer* renderer, float width, float height)
{
	button_p btn = (button_p)SDL_malloc(sizeof(button_t));
	if (!btn) return NULL;
	btn->renderer = renderer;
	btn->position = (SDL_FPoint){0, 0};
	btn->width = width;
	btn->height = height;
	btn->normal_color = (SDL_Color){ 100, 100, 200, 255 };
	btn->hover_color = (SDL_Color){ 150, 150, 250, 255 };
	btn->pressed_color = (SDL_Color){ 50, 50, 150, 255 };
	btn->text_pos = (SDL_FPoint){0, 0};
	btn->image = NULL;
	btn->pressed_image = NULL;
	btn->text = NULL;
	btn->is_hovered = btn->is_pressed = false;
	return btn;
}

void button_set_normal_color(button_p btn, SDL_Color color)
{
	if (!btn) return;
	btn->normal_color = color;
}

void button_set_hover_color(button_p btn, SDL_Color color)
{
	if (!btn) return;
	btn->hover_color = color;
}

void button_set_pressed_color(button_p btn, SDL_Color color)
{
	if (!btn) return;
	btn->pressed_color = color;
}

void button_set_textx(button_p btn, font_p fontinfo,
	const char* str, int len, SDL_Color color)
{
	if (!btn) return;
	if (btn->text) {
		text_updatex(btn->text, fontinfo, str, len, color);
	}
	else {
		btn->text = text_createx(fontinfo, str, len, color);
	}
}

void
button_set_text(button_p btn, font_p fontinfo,
	const int* codepoints, int num_codepoints, SDL_Color color)
{
	if (!btn) return;
	if (btn->text) {
		text_update(btn->text, fontinfo, codepoints, num_codepoints, color);
	}
	else {
		btn->text = text_create(fontinfo, codepoints, num_codepoints, color);
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
	SDL_FPoint logic;
	SDL_RenderCoordinatesFromWindow(btn->renderer, x, y, &logic.x, &logic.y);
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
	SDL_Renderer* renderer;
	SDL_FRect dest_rect, rect;
	SDL_Color color;
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

	renderer = btn->renderer;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderRect(renderer, &rect);

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
		text_print(renderer, btn->text, dest_rect.x, dest_rect.y);
	}
}

/* 事件处理已移至 Lua 层，见 joy.lua 中的 joy.mousepressed/mousereleased/mousemoved */

combobox_p combobox_create(SDL_Renderer* renderer, SDL_FRect rect)
{
	combobox_p combobox;
	combobox = (combobox_p)SDL_malloc(sizeof(combobox_t));
	combobox->renderer = renderer;
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
	combobox->bg_color = (SDL_Color){ 240, 240, 240, 255 };
	combobox->text_color = (SDL_Color){ 0, 0, 0, 255 };
	combobox->hover_color = (SDL_Color){ 150, 150, 250, 255 };
	return combobox;
}

void combobox_destroy(combobox_p combobox)
{
	if (combobox) {
		SDL_free(combobox);
	}
}

void
combobox_set_text(combobox_p combobox, const char* filename, int fontsize,
	const int* codepoints, int len, SDL_Color color)
{
	if (combobox->text) {
		SDL_DestroyTexture(combobox->text);
	}
	//combobox->text = create_text_texture(combobox->renderer, filename, fontsize, codepoints, len, color);
}

void combobox_add_item(combobox_p combobox, const char* filename, int fontsize, const int* codepoints, int len, SDL_Color color)
{
	combobox_item_p item;
	SDL_Renderer* renderer;
	if (combobox->item_count > 20) {
		return;
	}
	renderer = combobox->renderer;
	item = combobox->items + combobox->item_count;
	//item->text = create_text_texture(renderer, filename, fontsize, codepoints, len, color);
	item->is_selected = false;
	combobox->item_count++;
	combobox->dropdown_rect.h = combobox->item_count * combobox->item_height;
}

bool combobox_is_pressed(combobox_p combobox, float x, float y)
{
	if (!combobox) return false;
	SDL_FPoint logic;
	SDL_RenderCoordinatesFromWindow(combobox->renderer, x, y, &logic.x, &logic.y);
	return SDL_PointInRectFloat(&logic, &combobox->main_rect);
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
	SDL_FPoint logic;
	SDL_RenderCoordinatesFromWindow(combobox->renderer, x, y, &logic.x, &logic.y);
	for (int i = 0; i < combobox->item_count; i++) {
		SDL_FRect item_rect = {
		    combobox->dropdown_rect.x,
		    combobox->dropdown_rect.y + i * combobox->item_height,
		    combobox->dropdown_rect.w,
		    combobox->item_height
		};
		if (SDL_PointInRectFloat(&logic, &item_rect)) return i;
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
	SDL_Renderer* renderer;
	renderer = combobox->renderer;
	SDL_SetRenderDrawColor(renderer, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
	SDL_RenderFillRect(renderer, &combobox->main_rect);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderRect(renderer, &combobox->main_rect);

	// ��Ⱦ��ǰѡ�е��ı�
	if (combobox->selected_index >= 0) {
		SDL_Point point;
		//char* text;
		point.x = combobox->main_rect.x;
		point.y = combobox->main_rect.y;
		//text = combobox->items[combobox->selected_index].text;
		//render_text(combobox->font_info, 18, text, SDL_strlen(text), point, renderer);
	}


	// ����������Ǵ򿪵ģ���Ⱦѡ���б�
	if (combobox->is_open) {
		// ��Ⱦ������ͷ
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_Vertex arrowPoints[3] = {
		    {combobox->main_rect.x + combobox->main_rect.w - 25, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 5, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 15, combobox->main_rect.y + 20}
		};
		SDL_RenderGeometry(renderer, NULL, arrowPoints, 3, NULL, 0);


		SDL_SetRenderDrawColor(renderer, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
		SDL_RenderFillRect(renderer, &combobox->dropdown_rect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderRect(renderer, &combobox->dropdown_rect);

		combobox_item_p item;

		for (int i = 0; i < combobox->item_count; i++) {
			SDL_FRect item_rect = {
			    combobox->dropdown_rect.x,
			    combobox->dropdown_rect.y + i * combobox->item_height,
			    combobox->dropdown_rect.w,
			    combobox->item_height
			};

			// ����ǵ�ǰѡ�е��ʹ����ͣ��ɫ
			if (i == combobox->selected_index) {
				SDL_SetRenderDrawColor(renderer, combobox->hover_color.r, combobox->hover_color.g, combobox->hover_color.b, combobox->hover_color.a);
				SDL_RenderFillRect(renderer, &item_rect);
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderRect(renderer, &item_rect);

			item = combobox->items + i;
			SDL_Point point;
			point.x = item_rect.x + 5;
			point.y = item_rect.y;

			//render_text(combobox->font_info, 18, item->text, SDL_strlen(item->text), point, renderer);
		}
	}
	else {
		// ��Ⱦ������ͷ
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_Vertex arrowPoints[3] = {
		    {combobox->main_rect.x + combobox->main_rect.w - 15, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 25, combobox->main_rect.y + 20},
		    {combobox->main_rect.x + combobox->main_rect.w - 5, combobox->main_rect.y + 20}
		};
		SDL_RenderGeometry(renderer, NULL, arrowPoints, 3, NULL, 0);
	}
}


datagrid_p
datagrid_create(SDL_Renderer* renderer, SDL_Rect rect, int col_count, bool has_header, void* font)
{
	datagrid_p grid;
	grid = (datagrid_p)SDL_malloc(sizeof(datagrid_t));
	if (!grid) return NULL;
	SDL_memset(grid, 0, sizeof(datagrid_t));

	grid->renderer = renderer;
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
	grid->data = NULL;
	grid->col_widths = (int*)SDL_malloc(col_count * sizeof(int));
	grid->bg_color = (SDL_Color){ 0xFF, 0xFF, 0xFF, 0xFF };
	grid->grid_color = (SDL_Color){ 0xCC, 0xCC, 0xCC, 0xFF };
	grid->text_color = (SDL_Color){ 0x00, 0x00, 0x00, 0xFF };
	grid->header_bg_color = (SDL_Color){ 0xEE, 0xEE, 0xEE, 0xFF };
	grid->selected_color = (SDL_Color){ 0x33, 0x99, 0xFF, 0xFF };
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
	SDL_Color color;
	color = (SDL_Color){ 0x0, 0x0, 0x0, 0xFF };
	if (!grid->has_header) return;
	fontinfo = grid->font;
	for (int i = 0; i < grid->col_count; i++) {
		//grid->headers[i] = create_text_texture(grid->renderer, fontinfo, headers[i]->array, headers[i]->length, color);
	}
}

void datagrid_setheader(datagrid_p grid, int index, const char* header)
{
	font_p fontinfo;
	SDL_Color color;
	if (!grid->has_header) return;
	if (index < grid->col_count) return;
	color = (SDL_Color){ 0x0, 0x0, 0x0, 0xFF };
	fontinfo = grid->font;
	if (grid->headers[index]) {
		//SDL_DestroyTexture(grid->headers[index]);
		//SDL_UpdateTexture(grid->headers[index], NULL, NULL, 0);
		//update_text_texture(grid->headers[index], fontinfo, header->array, header->length, color);
	}
	else {
		//grid->headers[index] = create_text_texture(grid->renderer, fontinfo, header->array, header->length, color);
	}
}

void datagrid_addrow(datagrid_p grid, const char** row_data)
{
	/*
	grid->row_count++;
	grid->data = SDL_realloc(grid->data, grid->row_count * sizeof(char**));
	grid->data[grid->row_count - 1] = SDL_malloc(grid->col_count * sizeof(char*));
	for (int i = 0; i < grid->col_count; i++) {
		grid->data[grid->row_count - 1][i] = row_data[i] ? SDL_strdup(row_data[i]) : NULL;
	}*/
}

void datagrid_setcell(datagrid_p grid, int row_index, int col_index, const char* data)
{
	/*
	if (!grid->has_header) return;
	if (row_index < grid->row_count && col_index < grid->col_count) {
		if (grid->data[row_index][col_index]) {
			SDL_free(grid->data[row_index][col_index]);
		}
		grid->data[row_index][col_index] = SDL_strdup(data);
	}*/
}

static void datagrid_cleardata(datagrid_p grid)
{
	for (int i = (grid->has_header ? 1 : 0); i < grid->row_count; i++) {
		for (int j = 0; j < grid->col_count; j++) {
			if (grid->data[i][j]) SDL_free(grid->data[i][j]);
		}
		SDL_free(grid->data[i]);
	}

	if (grid->row_count > (grid->has_header ? 1 : 0)) {
		SDL_free(grid->data);
		grid->data = NULL;
	}

	grid->row_count = grid->has_header ? 1 : 0;
}


void datagrid_destroy(datagrid_p grid)
{
	if (!grid) return;

	if (grid->has_header) {
		for (int i = 0; i < grid->col_count; i++) {
			if (grid->headers[i]) text_destroy(grid->headers[i]);
		}
	}

	// �ͷ�����
	datagrid_cleardata(grid);

	// �ͷ�������Դ
	SDL_free(grid->col_widths);
	SDL_free(grid);
}

void datagrid_draw(datagrid_p grid, SDL_Renderer* renderer)
{
	// ����ɼ��з�Χ
	int start_row = grid->scroll_offset;
	int end_row = start_row + grid->visible_rows;
	if (end_row > grid->row_count) end_row = grid->row_count;

	// ���Ʊ���
	SDL_Color bg_color = grid->bg_color;
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_FRect bg_rect = { grid->x, grid->y, grid->width, grid->height };
	SDL_RenderFillRect(renderer, &bg_rect);

	// ���������ߺ�����
	SDL_Color grid_color = grid->grid_color;
	SDL_Color text_color = grid->text_color;
	SDL_Color header_bg_color = grid->header_bg_color;
	SDL_Color selected_color = grid->selected_color;

	// ���Ʊ���
	SDL_SetRenderDrawColor(renderer, header_bg_color.r, header_bg_color.g, header_bg_color.b, header_bg_color.a);
	SDL_FRect header_rect = { grid->x, grid->y, grid->width, grid->row_height };
	SDL_RenderFillRect(renderer, &header_rect);

	{
		int x = grid->x;
		int y = grid->y;
		for (int col = 0; col < grid->col_count; col++) {
			SDL_FRect rect;
			float tw, th;
			rect.x = x;
			rect.y = y;
			SDL_Texture* tex = grid->headers[col] ? text_get_texture(grid->headers[col]) : NULL;
			if (tex && SDL_GetTextureSize(tex, &tw, &th)) {
				rect.w = tw;
				rect.h = th;
				SDL_RenderTexture(grid->renderer, tex, NULL, &rect);
			}

			// ���ƴ�ֱ��
			SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			SDL_RenderLine(renderer, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

			x += grid->col_widths[col];
		}
	}

	// ������������
	for (int row = start_row; row < end_row; row++) {
		int y = grid->y + (row - start_row) * grid->row_height + grid->row_height;

		// �����б���
		if (row == grid->selected_row) {
			SDL_SetRenderDrawColor(renderer, selected_color.r, selected_color.g, selected_color.b, selected_color.a);
			SDL_FRect selected_rect = { grid->x, y, grid->width, grid->row_height };
			SDL_RenderFillRect(renderer, &selected_rect);
		}
		else {
			/* SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			 SDL_FRect header_rect = { grid->x, y, grid->width, grid->row_height };
			 SDL_RenderFillRect(renderer, &header_rect);*/
		}

		// ���Ƶ�Ԫ�����ݺ�������
		int x = grid->x;
		for (int col = 0; col < grid->col_count; col++) {
			//text = grid->data[row][col];
			//if (text) {
			//        SDL_Point point;
			//        point.x = x;
			//        point.y = y;
			//        // grid->col_widths[col], grid->row_height
			//        //render_text(grid->font, 18, text, SDL_strlen(text), point, renderer);
			//}

			// ���ƴ�ֱ��
			SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			SDL_RenderLine(renderer, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

			x += grid->col_widths[col];
		}

		// ����ˮƽ��
		SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
		SDL_RenderLine(renderer, grid->x, y + grid->row_height, grid->x + grid->width, y + grid->row_height);
	}

	// ������߿�
	SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
	SDL_RenderRect(renderer, &bg_rect);
}

int datagrid_hit_row(datagrid_p grid, float x, float y)
{
	if (!grid) return -1;
	SDL_FPoint logic;
	SDL_RenderCoordinatesFromWindow(grid->renderer, x, y, &logic.x, &logic.y);
	SDL_FRect rect = { (float)grid->x, (float)(grid->y + grid->row_height), (float)grid->width, (float)grid->height };
	if (!SDL_PointInRectFloat(&logic, &rect)) return -1;
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

void radio_button_render(radio_button_p radio, SDL_Renderer* renderer)
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


	SDL_Vertex triangle[3];
	for (int i = 0; i < segments; i++) {
		float angle1 = (float)i / (float)segments * 2.0f * 3.14159f;
		float angle2 = (float)(i + 1) / (float)segments * 2.0f * 3.14159f;
		triangle[0].position = (SDL_FPoint){ center_x, center_y };
		triangle[0].color = radio->r1_color;
		triangle[1].position = (SDL_FPoint){ center_x + cosf(angle1) * r1, center_y + sinf(angle1) * r1 };
		triangle[1].color = radio->r1_color;
		triangle[2].position = (SDL_FPoint){ center_x + cosf(angle2) * r1, center_y + sinf(angle2) * r1 };
		triangle[2].color = radio->r1_color;
		SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);

		triangle[0].position = (SDL_FPoint){ center_x, center_y };
		triangle[0].color = radio->r2_color;
		triangle[1].position = (SDL_FPoint){ center_x + cosf(angle1) * r2, center_y + sinf(angle1) * r2 };
		triangle[1].color = radio->r2_color;
		triangle[2].position = (SDL_FPoint){ center_x + cosf(angle2) * r2, center_y + sinf(angle2) * r2 };
		triangle[2].color = radio->r2_color;
		SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);

		if (radio->checked) {
			triangle[0].position = (SDL_FPoint){ center_x, center_y };
			triangle[0].color = radio->r3_color;
			triangle[1].position = (SDL_FPoint){ center_x + cosf(angle1) * r3, center_y + sinf(angle1) * r3 };
			triangle[1].color = radio->r3_color;
			triangle[2].position = (SDL_FPoint){ center_x + cosf(angle2) * r3, center_y + sinf(angle2) * r3 };
			triangle[2].color = radio->r3_color;
			SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);
		}
	}

	// ���Ʊ�ǩ�ı�
	if (radio->label) {
		/* SDL_Color text_color = { 0, 0, 0, 255 };
		 SDL_Surface* text_surface = TTF_RenderText_Blended(font, radio->label, text_color);

		 if (text_surface) {
			 SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

			 if (text_texture) {
				 SDL_FRect text_rect = {
				     radio->x + radio->size + 5,
				     radio->y + (radio->size - text_surface->h) / 2,
				     text_surface->w,
				     text_surface->h
				 };

				 SDL_RenderTexture(renderer, text_texture, NULL, &text_rect);
				 SDL_DestroyTexture(text_texture);
			 }

			 SDL_DestroySurface(text_surface);
		 }*/
	}
}

void radio_button_group_draw(radio_button_group_p group, SDL_Renderer* renderer)
{
	if (!group) return;

	for (int i = 0; i < group->count; i++) {
		radio_button_render(group->buttons[i], renderer);
	}
}

bool radio_button_is_pressed(radio_button_p radio, float x, float y)
{
	if (!radio) return false;
	SDL_FPoint pt = { x, y };
	if (SDL_PointInRectFloat(&pt, &radio->hit_area)) {
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
	cb->box_color = (SDL_Color){ 0, 0, 0, 255 };        // ��ɫ�߿�
	cb->check_color = (SDL_Color){ 0, 120, 0, 255 };    // ��ɫ��ѡ
	cb->text_color = (SDL_Color){ 0, 0, 0, 255 };       // ��ɫ�ı�
	cb->hover_color = (SDL_Color){ 200, 200, 200, 255 };// ǳ��ɫ��ͣ

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
	SDL_FPoint pt = { x, y };
	if (SDL_PointInRectFloat(&pt, &cb->hit_area)) {
		checkbox_toggle(cb);
		return true;
	}
	return false;
}

void checkbox_set_hovered(checkbox_p cb, bool v)
{
	if (cb) cb->hover = v;
}

void checkbox_draw(checkbox_p checkbox, SDL_Renderer* renderer, void* font)
{
	if (!checkbox) return;

	// ���Ƹ�ѡ�򱳾��������ͣ��
	if (checkbox->hover && checkbox->enabled) {
		SDL_SetRenderDrawColor(renderer,
			checkbox->hover_color.r,
			checkbox->hover_color.g,
			checkbox->hover_color.b,
			checkbox->hover_color.a);
		SDL_FRect bgRect = {
		    checkbox->x - 2,
		    checkbox->y - 2,
		    checkbox->size + 4,
		    checkbox->size + 4
		};
		SDL_RenderFillRect(renderer, &bgRect);
	}

	// ���Ƹ�ѡ�����
	SDL_SetRenderDrawColor(renderer,
		checkbox->box_color.r,
		checkbox->box_color.g,
		checkbox->box_color.b,
		checkbox->box_color.a);
	SDL_FRect boxRect = {
	    checkbox->x,
	    checkbox->y,
	    checkbox->size,
	    checkbox->size
	};
	SDL_RenderRect(renderer, &boxRect);

	// �����ѡ�У����ƹ�ѡ���
	if (checkbox->state != JOY_CHECKBOX_UNCHECKED && checkbox->enabled) {
		SDL_SetRenderDrawColor(renderer,
			checkbox->check_color.r,
			checkbox->check_color.g,
			checkbox->check_color.b,
			checkbox->check_color.a);

		if (checkbox->state == JOY_CHECKBOX_CHECKED) {
			// ���ƹ�ѡ��ǣ��Ժţ�
			const float padding = checkbox->size * 0.2f;
			SDL_FPoint points[3] = {
			    {checkbox->x + padding, checkbox->y + checkbox->size * 0.5f},
			    {checkbox->x + checkbox->size * 0.4f, checkbox->y + checkbox->size - padding},
			    {checkbox->x + checkbox->size - padding, checkbox->y + padding}
			};
			SDL_RenderLines(renderer, points, 3);
		}
		else {
			// ���Ʋ�ȷ��״̬�����ߣ�
			const float padding = checkbox->size * 0.3f;
			SDL_FPoint points[2] = {
			    {checkbox->x + padding, checkbox->y + checkbox->size * 0.5f},
			    {checkbox->x + checkbox->size - padding, checkbox->y + checkbox->size * 0.5f}
			};
			SDL_RenderLine(renderer, points[0].x, points[0].y, points[1].x, points[1].y);
		}
	}

	// ������ã����ư�͸��Ч��
	if (!checkbox->enabled) {
		SDL_SetRenderDrawColor(renderer, 200, 200, 200, 128);
		SDL_RenderFillRect(renderer, &boxRect);
	}

	// ���Ʊ�ǩ�ı�
	if (checkbox->label && font) {
		/*SDL_Color textColor = checkbox->enabled ? checkbox->textColor : (SDL_Color) { 128, 128, 128, 255 };
		SDL_Surface* textSurface = TTF_RenderText_Blended(font, checkbox->label, textColor);

		if (textSurface) {
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

			if (textTexture) {
				SDL_FRect textRect = {
				    checkbox->x + checkbox->size + 5,
				    checkbox->y + (checkbox->size - textSurface->h) / 2,
				    textSurface->w,
				    textSurface->h
				};

				SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
				SDL_DestroyTexture(textTexture);
			}

			SDL_DestroySurface(textSurface);
		}*/
	}
}




// ��ʼ��ҡ��
joystick_p joystick_create(SDL_Renderer* renderer, float x, float y, float radius)
{
	joystick_p joystick;
	joystick = (joystick_p)SDL_malloc(sizeof(joystick_t));

	joystick->renderer = renderer;
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
	SDL_FPoint logic;
	SDL_RenderCoordinatesFromWindow(j->renderer, x, y, &logic.x, &logic.y);
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
	SDL_FPoint logic;
	SDL_RenderCoordinatesFromWindow(j->renderer, x, y, &logic.x, &logic.y);
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
	SDL_Renderer* renderer = joystick->renderer;

	// ���Ʊ���Բ��
	SDL_SetRenderDrawColor(renderer,
		(JOYSTICK_BG_COLOR >> 24) & 0xFF,
		(JOYSTICK_BG_COLOR >> 16) & 0xFF,
		(JOYSTICK_BG_COLOR >> 8) & 0xFF,
		JOYSTICK_BG_COLOR & 0xFF);
	// ����Բ�α�����ʹ�ö����ģ��Բ�Σ�
	shape_draw_circle(renderer, "line", joystick->center, joystick->radius, 32);

	// ����ʮ����
	SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
	SDL_RenderLine(renderer,
		joystick->center.x - joystick->radius, joystick->center.y,
		joystick->center.x + joystick->radius, joystick->center.y);
	SDL_RenderLine(renderer,
		joystick->center.x, joystick->center.y - joystick->radius,
		joystick->center.x, joystick->center.y + joystick->radius);

	// �����ֱ�
	SDL_Color handle_color = joystick->is_dragging ?
		(SDL_Color) {
		0x4C, 0xAF, 0x50, 0xFF
	} :
		(SDL_Color) {
		0x88, 0x88, 0x88, 0xFF
	};

		SDL_SetRenderDrawColor(renderer, handle_color.r, handle_color.g, handle_color.b, handle_color.a);

		// ����Բ���ֱ�

		SDL_FPoint handle_center = {
		joystick->handle.x + joystick->handle_radius,
		joystick->handle.y + joystick->handle_radius
		};
		shape_draw_circle(renderer, "fill", handle_center, joystick->radius * 0.5f, 24);

		// ����ֱ�
		SDL_SetRenderDrawColor(renderer, handle_color.r, handle_color.g, handle_color.b, handle_color.a - 0x40);

		SDL_FRect handle_fill = {
		    joystick->handle.x + 2,
		    joystick->handle.y + 2,
		    joystick->handle.w - 4,
		    joystick->handle.h - 4
		};
		//shape_draw_rectangle(renderer, "fill", handle_fill);



		SDL_RenderFillRect(renderer, &handle_fill);


		/*	if (joystick_get_magnitude(joystick) > 0) {
				SDL_FPoint dir = joystick_get_direction(joystick);
				SDL_RenderLine(renderer,
					joystick->center.x,
					joystick->center.y,
					joystick->center.x + dir.x * joystick->radius,
					joystick->center.y + dir.y * joystick->radius);
			}*/
}

// ��ȡҡ�˷���
SDL_FPoint joystick_get_direction(joystick_p joystick)
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

textinput_p textinput_create(SDL_Renderer* renderer, SDL_FRect rect, font_p font)
{
	textinput_p ti = (textinput_p)SDL_calloc(1, sizeof(textinput_t));
	if (!ti) return NULL;
	ti->renderer   = renderer;
	ti->rect       = rect;
	ti->font       = font;
	ti->bg_color   = (SDL_Color){255, 255, 255, 255};
	ti->border_color = (SDL_Color){180, 180, 180, 255};
	ti->focus_border_color = (SDL_Color){66, 133, 244, 255};
	ti->text_color = (SDL_Color){0, 0, 0, 255};
	ti->cursor_color = (SDL_Color){0, 0, 0, 255};
	ti->buf_len    = TEXTINPUT_BUF_INIT;
	ti->buffer     = (char*)SDL_calloc(1, ti->buf_len);
	ti->text_len   = 0;
	ti->cursor_pos = 0;
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

/* 在 buffer 的 cursor_pos 处插入 len 字节 */
void textinput_insert(textinput_p ti, const char* s, int len)
{
	if (ti->text_len + len >= ti->buf_len) {
		while (ti->text_len + len >= ti->buf_len)
			ti->buf_len *= 2;
		ti->buffer = (char*)SDL_realloc(ti->buffer, ti->buf_len);
	}
	/* 移位 */
	SDL_memmove(ti->buffer + ti->cursor_pos + len,
	            ti->buffer + ti->cursor_pos,
	            ti->text_len - ti->cursor_pos);
	SDL_memcpy(ti->buffer + ti->cursor_pos, s, len);
	ti->text_len += len;
	ti->cursor_pos += len;
	ti->buffer[ti->text_len] = '\0';
	if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
}

/* 删除光标前一个 UTF-8 字符（退格） */
void textinput_backspace(textinput_p ti)
{
	if (ti->cursor_pos <= 0) return;
	/* 定位到上一个 UTF-8 字符起始 */
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
}

/* 删除光标后一个 UTF-8 字符（Delete） */
void textinput_delete(textinput_p ti)
{
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
}

/* 获取光标之前的文本宽度（像素） */
static int textinput_cursor_x(textinput_p ti)
{
	if (ti->cursor_pos <= 0 || !ti->font) return 0;
	/* 用 text_createx 生成光标前的子串，量宽度 */
	SDL_Color c = {0,0,0,255};
	text_texture_p tex = text_createx(ti->font, ti->buffer, ti->cursor_pos, c);
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
	/* 从 renderer 获取 window 并启用文本输入 */
	SDL_Window* win = SDL_GetRenderWindow(ti->renderer);
	if (win) SDL_StartTextInput(win);
}

void textinput_defocus(textinput_p ti)
{
	if (!ti || !ti->focused) return;
	ti->focused = false;
	SDL_Window* win = SDL_GetRenderWindow(ti->renderer);
	if (win) SDL_StopTextInput(win);
}

void textinput_draw(textinput_p ti)
{
	if (!ti) return;
	SDL_Renderer* r = ti->renderer;
	SDL_FRect rect = ti->rect;

	/* 背景 */
	SDL_SetRenderDrawColor(r, ti->bg_color.r, ti->bg_color.g, ti->bg_color.b, ti->bg_color.a);
	SDL_RenderFillRect(r, &rect);

	/* 边框（聚焦时高亮） */
	SDL_Color border = ti->focused ? ti->focus_border_color : ti->border_color;
	SDL_SetRenderDrawColor(r, border.r, border.g, border.b, border.a);
	SDL_RenderRect(r, &rect);

	/* 文字 */
	if (ti->text_len > 0 && ti->font) {
		float text_x = rect.x + 4;
		float text_y = rect.y + (rect.h - text_get_height(text_createx(ti->font, "A", 1, ti->text_color))) / 2.0f;
		text_printx(r, ti->font, ti->buffer, ti->text_len, ti->text_color, text_x - (float)ti->scroll_offset, text_y);
	}

	/* 光标（聚焦时绘制） */
	if (ti->focused) {
		Uint64 now = SDL_GetTicks();
		if (now - ti->cursor_timer >= 530) {
			ti->cursor_visible = !ti->cursor_visible;
			ti->cursor_timer = now;
		}
		if (ti->cursor_visible) {
			int cx = textinput_cursor_x(ti) - ti->scroll_offset + (int)rect.x + 4;
			float cy = rect.y + 3;
			float ch = rect.h - 6;
			SDL_SetRenderDrawColor(r, ti->cursor_color.r, ti->cursor_color.g, ti->cursor_color.b, ti->cursor_color.a);
			SDL_RenderLine(r, (float)cx + 0.5f, cy, (float)cx + 0.5f, cy + ch);
		}
	}
}

void textinput_handle_event(textinput_p ti, SDL_Event* event)
{
	if (!ti || !event) return;
	SDL_Renderer* r = ti->renderer;
	SDL_FPoint logic_pos;
	SDL_FRect rect = ti->rect;

	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.which == SDL_TOUCH_MOUSEID) return;
		if (event->button.button == SDL_BUTTON_LEFT) {
			SDL_RenderCoordinatesFromWindow(r, event->button.x, event->button.y, &logic_pos.x, &logic_pos.y);
			if (SDL_PointInRectFloat(&logic_pos, &rect)) {
				textinput_focus(ti);
			} else {
				textinput_defocus(ti);
			}
		}
	}
	else if (event->type == SDL_EVENT_FINGER_DOWN) {
		int ww, wh;
		SDL_GetRenderOutputSize(r, &ww, &wh);
		SDL_RenderCoordinatesFromWindow(r, event->tfinger.x * ww, event->tfinger.y * wh, &logic_pos.x, &logic_pos.y);
		if (SDL_PointInRectFloat(&logic_pos, &rect)) {
			textinput_focus(ti);
		} else {
			textinput_defocus(ti);
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_MOTION) {
		if (event->motion.which == SDL_TOUCH_MOUSEID) return;
		SDL_RenderCoordinatesFromWindow(r, event->motion.x, event->motion.y, &logic_pos.x, &logic_pos.y);
		ti->hovered = SDL_PointInRectFloat(&logic_pos, &rect);
	}
	else if (event->type == SDL_EVENT_FINGER_MOTION) {
		int ww, wh;
		SDL_GetRenderOutputSize(r, &ww, &wh);
		SDL_RenderCoordinatesFromWindow(r, event->tfinger.x * ww, event->tfinger.y * wh, &logic_pos.x, &logic_pos.y);
		ti->hovered = SDL_PointInRectFloat(&logic_pos, &rect);
	}

	if (!ti->focused) return;

	if (event->type == SDL_EVENT_TEXT_INPUT) {
		const char* text = event->text.text;
		int len = (int)SDL_strlen(text);
		if (len > 0 && text[0] >= ' ') {
			textinput_insert(ti, text, len);
			/* 自动滚动到光标处 */
			int cw = textinput_cursor_x(ti);
			int visible_w = (int)ti->rect.w - 8;
			if (cw - ti->scroll_offset > visible_w)
				ti->scroll_offset = cw - visible_w;
			else if (cw - ti->scroll_offset < 0)
				ti->scroll_offset = cw;
		}
	}
	else if (event->type == SDL_EVENT_KEY_DOWN) {
		switch (event->key.key) {
		case SDLK_BACKSPACE:
			textinput_backspace(ti);
			break;
		case SDLK_DELETE:
			textinput_delete(ti);
			break;
		case SDLK_LEFT:
			if (ti->cursor_pos > 0) {
				ti->cursor_pos--;
				while (ti->cursor_pos > 0 && ((unsigned char)ti->buffer[ti->cursor_pos] & 0xC0) == 0x80)
					ti->cursor_pos--;
			}
			break;
		case SDLK_RIGHT:
			if (ti->cursor_pos < ti->text_len) {
				ti->cursor_pos++;
				while (ti->cursor_pos < ti->text_len && ((unsigned char)ti->buffer[ti->cursor_pos] & 0xC0) == 0x80)
					ti->cursor_pos++;
			}
			break;
		case SDLK_HOME:
			ti->cursor_pos = 0;
			ti->scroll_offset = 0;
			break;
		case SDLK_END:
			ti->cursor_pos = ti->text_len;
			break;
		case SDLK_RETURN:
			if (ti->on_submit) ti->on_submit(ti, ti->cb_userdata);
			break;
		case SDLK_V:
			if (event->key.mod & SDL_KMOD_CTRL) {
				const char* clip = SDL_GetClipboardText();
				if (clip) {
					textinput_insert(ti, clip, (int)SDL_strlen(clip));
				}
			}
			break;
		case SDLK_C:
			if (event->key.mod & SDL_KMOD_CTRL) {
				SDL_SetClipboardText(ti->buffer);
			}
			break;
		case SDLK_X:
			if (event->key.mod & SDL_KMOD_CTRL) {
				SDL_SetClipboardText(ti->buffer);
				ti->text_len = 0;
				ti->cursor_pos = 0;
				ti->buffer[0] = '\0';
			}
			break;
		default:
			break;
		}
	}
}

bool textinput_is_focused(textinput_p ti)
{
	return ti ? ti->focused : false;
}
