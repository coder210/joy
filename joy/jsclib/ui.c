#include <assert.h>
#include <stdlib.h>
#include "jsclib.h"
#include "../jui.h"

#define LABEL_TAG    "ui_label"
#define BUTTON_TAG   "ui_button"
#define COMBO_TAG    "ui_combobox"
#define CHECK_TAG    "ui_checkbox"
#define RADIO_TAG    "ui_radiobutton"
#define GRID_TAG     "ui_datagrid"
#define TEXTI_TAG    "ui_textinput"
#define SLIDER_TAG   "ui_slider"
#define RADIOG_TAG   "ui_radiogroup"

static SDL_Color read_color(duk_context* ctx, int idx)
{
	SDL_Color c = { 255, 255, 255, 255 };
	if (duk_is_object(ctx, idx)) {
		duk_get_prop_string(ctx, idx, "r"); c.r = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "g"); c.g = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "b"); c.b = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "a"); c.a = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
	}
	return c;
}

/* ===== Label ===== */
static duk_ret_t jui_label_create(duk_context* ctx)
{
	SDL_Renderer* r = (SDL_Renderer*)duk_get_pointer(ctx, 1);
	font_p* fp = (font_p*)duk_get_pointer(ctx, 2);
	duk_push_pointer(ctx, label_create(r, fp ? *fp : NULL, NULL, 0));
	return 1;
}
static duk_ret_t jui_label_destroy(duk_context* ctx) { label_destroy((label_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_label_set_position(duk_context* ctx) {
	label_p lb = (label_p)duk_get_pointer(ctx,1);
	lb->position.x = (float)duk_to_number(ctx,2); lb->position.y = (float)duk_to_number(ctx,3);
	return 0;
}
static duk_ret_t jui_label_draw(duk_context* ctx) { label_draw((label_p)duk_get_pointer(ctx,1)); return 0; }

/* ===== Button ===== */
static duk_ret_t jui_button_create(duk_context* ctx) {
	SDL_Renderer* r = (SDL_Renderer*)duk_get_pointer(ctx,1);
	duk_push_pointer(ctx, button_create(r, (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3)));
	return 1;
}
static duk_ret_t jui_button_destroy(duk_context* ctx) { button_destroy((button_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_button_set_position(duk_context* ctx) {
	button_set_position((button_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3));
	return 0;
}
static duk_ret_t jui_button_set_text(duk_context* ctx) {
	button_p b = (button_p)duk_get_pointer(ctx,1);
	font_p* fp = (font_p*)duk_get_pointer(ctx,2);
	const char* s = duk_to_string(ctx,3);
	SDL_Color c = {255,255,255,255};
	if (duk_is_object(ctx,4)) c = read_color(ctx,4);
	button_set_textx(b, fp ? *fp : NULL, s, strlen(s), c);
	return 0;
}
static duk_ret_t jui_button_is_hovered(duk_context* ctx) {
	duk_push_boolean(ctx, button_is_hovered((button_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3)));
	return 1;
}
static duk_ret_t jui_button_is_pressed(duk_context* ctx) {
	duk_push_boolean(ctx, button_is_pressed((button_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3)));
	return 1;
}
static duk_ret_t jui_button_set_hovered(duk_context* ctx) { button_set_hovered((button_p)duk_get_pointer(ctx,1), duk_get_boolean(ctx,2)); return 0; }
static duk_ret_t jui_button_set_pressed(duk_context* ctx) { button_set_pressed((button_p)duk_get_pointer(ctx,1), duk_get_boolean(ctx,2)); return 0; }
static duk_ret_t jui_button_draw(duk_context* ctx) { button_draw((button_p)duk_get_pointer(ctx,1)); return 0; }

/* ===== Combobox ===== */
static duk_ret_t jui_combobox_create(duk_context* ctx) {
	SDL_Renderer* r = (SDL_Renderer*)duk_get_pointer(ctx,1);
	SDL_FRect rect = {(float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3), (float)duk_to_number(ctx,4), (float)duk_to_number(ctx,5)};
	duk_push_pointer(ctx, combobox_create(r, rect));
	return 1;
}
static duk_ret_t jui_combobox_destroy(duk_context* ctx) { combobox_destroy((combobox_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_combobox_additem(duk_context* ctx) {
	combobox_p c = (combobox_p)duk_get_pointer(ctx,1);
	if (c->item_count >= 20) return 0;
	const char* text = duk_to_string(ctx,2);
	font_p* fp = (font_p*)duk_get_pointer(ctx,3);
	SDL_Color col = {0,0,0,255};
	combobox_item_p item = &c->items[c->item_count];
	item->text = text_createx(fp ? *fp : NULL, text, strlen(text), col);
	item->is_selected = false;
	c->item_count++;
	c->dropdown_rect.h = c->item_count * c->item_height;
	return 0;
}
static duk_ret_t jui_combobox_draw(duk_context* ctx) { combobox_draw((combobox_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_combobox_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, combobox_is_pressed((combobox_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_combobox_is_open(duk_context* ctx) { duk_push_boolean(ctx, combobox_is_open((combobox_p)duk_get_pointer(ctx,1))); return 1; }
static duk_ret_t jui_combobox_set_open(duk_context* ctx) { combobox_set_open((combobox_p)duk_get_pointer(ctx,1), duk_get_boolean(ctx,2)); return 0; }
static duk_ret_t jui_combobox_hit_item(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)combobox_hit_item((combobox_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_combobox_set_selected(duk_context* ctx) { combobox_set_selected((combobox_p)duk_get_pointer(ctx,1), (int)duk_to_number(ctx,2)); return 0; }
static duk_ret_t jui_combobox_get_selected(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)combobox_get_selected((combobox_p)duk_get_pointer(ctx,1))); return 1; }

/* ===== Checkbox ===== */
static duk_ret_t jui_checkbox_create(duk_context* ctx) {
	duk_push_pointer(ctx, checkbox_create((float)duk_to_number(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3), duk_to_string(ctx,4)));
	return 1;
}
static duk_ret_t jui_checkbox_destroy(duk_context* ctx) { checkbox_destroy((checkbox_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_checkbox_draw(duk_context* ctx) {
	checkbox_draw((checkbox_p)duk_get_pointer(ctx,1), (SDL_Renderer*)duk_get_pointer(ctx,2), NULL);
	return 0;
}
static duk_ret_t jui_checkbox_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, checkbox_is_pressed((checkbox_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_checkbox_set_hovered(duk_context* ctx) { checkbox_set_hovered((checkbox_p)duk_get_pointer(ctx,1), duk_get_boolean(ctx,2)); return 0; }

/* ===== Radio Button ===== */
static duk_ret_t jui_radiobutton_create(duk_context* ctx) {
	duk_push_pointer(ctx, radio_button_create((float)duk_to_number(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3), duk_to_string(ctx,4), (int)duk_to_number(ctx,5)));
	return 1;
}
static duk_ret_t jui_radiobutton_destroy(duk_context* ctx) { radio_button_destroy((radio_button_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_radiobutton_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, radio_button_is_pressed((radio_button_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_radiobutton_set_hovered(duk_context* ctx) { radio_button_set_hovered((radio_button_p)duk_get_pointer(ctx,1), duk_get_boolean(ctx,2)); return 0; }

/* ===== Radio Group ===== */
static duk_ret_t jui_radiogroup_create(duk_context* ctx) { duk_push_pointer(ctx, radio_button_group_create()); return 1; }
static duk_ret_t jui_radiogroup_destroy(duk_context* ctx) { radio_button_group_destroy((radio_button_group_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_radiogroup_add_button(duk_context* ctx) { radio_button_group_add_button((radio_button_group_p)duk_get_pointer(ctx,1), (radio_button_p)duk_get_pointer(ctx,2)); return 0; }
static duk_ret_t jui_radiogroup_set_selected(duk_context* ctx) { radio_button_group_set_selected((radio_button_group_p)duk_get_pointer(ctx,1), (int)duk_to_number(ctx,2)); return 0; }
static duk_ret_t jui_radiogroup_draw(duk_context* ctx) { radio_button_group_draw((radio_button_group_p)duk_get_pointer(ctx,1), (SDL_Renderer*)duk_get_pointer(ctx,2)); return 0; }

/* ===== Datagrid ===== */
static duk_ret_t jui_datagrid_create(duk_context* ctx) {
	SDL_Renderer* r = (SDL_Renderer*)duk_get_pointer(ctx,1);
	SDL_Rect rect = {(int)duk_to_number(ctx,2), (int)duk_to_number(ctx,3), (int)duk_to_number(ctx,4), (int)duk_to_number(ctx,5)};
	int cols = (int)duk_to_number(ctx,6);
	int has_hdr = duk_get_boolean(ctx,7);
	font_p* fp = (font_p*)duk_get_pointer(ctx,8);
	duk_push_pointer(ctx, datagrid_create(r, rect, cols, has_hdr, fp ? *fp : NULL));
	return 1;
}
static duk_ret_t jui_datagrid_destroy(duk_context* ctx) { datagrid_destroy((datagrid_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_datagrid_draw(duk_context* ctx) { datagrid_draw((datagrid_p)duk_get_pointer(ctx,1), (SDL_Renderer*)duk_get_pointer(ctx,2)); return 0; }
static duk_ret_t jui_datagrid_setheaders(duk_context* ctx) {
	datagrid_p g = (datagrid_p)duk_get_pointer(ctx,1);
	font_p* fp = (font_p*)duk_get_pointer(ctx,2);
	SDL_Color col = {0,0,0,255};
	int n = (int)duk_get_length(ctx,3);
	for (int i = 0; i < n && i < g->col_count; i++) {
		duk_get_prop_index(ctx, 3, (duk_uarridx_t)i);
		const char* text = duk_to_string(ctx, -1);
		if (g->headers[i]) text_destroy(g->headers[i]);
		g->headers[i] = text_createx(fp ? *fp : NULL, text, strlen(text), col);
		duk_pop(ctx);
	}
	return 0;
}
static duk_ret_t jui_datagrid_addrow(duk_context* ctx) {
	datagrid_p g = (datagrid_p)duk_get_pointer(ctx,1);
	int n = (int)duk_get_length(ctx,2);
	if (n > g->col_count) n = g->col_count;
	const char** row = (const char**)malloc(n * sizeof(char*));
	for (int i = 0; i < n; i++) {
		duk_get_prop_index(ctx, 2, (duk_uarridx_t)i);
		row[i] = duk_to_string(ctx, -1);
		duk_pop(ctx);
	}
	datagrid_addrow(g, row);
	free(row);
	return 0;
}
static duk_ret_t jui_datagrid_hit_row(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)datagrid_hit_row((datagrid_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_datagrid_scroll(duk_context* ctx) { datagrid_scroll((datagrid_p)duk_get_pointer(ctx,1), (int)duk_to_number(ctx,2)); return 0; }
static duk_ret_t jui_datagrid_set_selected_row(duk_context* ctx) { datagrid_set_selected_row((datagrid_p)duk_get_pointer(ctx,1), (int)duk_to_number(ctx,2)); return 0; }
static duk_ret_t jui_datagrid_get_row_count(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)((datagrid_p)duk_get_pointer(ctx,1))->row_count); return 1; }
static duk_ret_t jui_datagrid_get_cell(duk_context* ctx) {
	datagrid_p g = (datagrid_p)duk_get_pointer(ctx,1);
	int r = (int)duk_to_number(ctx,2), c = (int)duk_to_number(ctx,3);
	if (r >= 0 && (size_t)r < kv_size(g->data) && c >= 0 && c < g->col_count) {
		char* v = kv_A(kv_A(g->data, r), c);
		if (v) { duk_push_string(ctx, v); return 1; }
	}
	duk_push_undefined(ctx);
	return 1;
}

/* ===== TextInput ===== */
static duk_ret_t jui_textinput_create(duk_context* ctx) {
	SDL_Renderer* r = (SDL_Renderer*)duk_get_pointer(ctx,1);
	font_p* fp = (font_p*)duk_get_pointer(ctx,2);
	SDL_FRect rect = {(float)duk_to_number(ctx,3), (float)duk_to_number(ctx,4), (float)duk_to_number(ctx,5), (float)duk_to_number(ctx,6)};
	duk_push_pointer(ctx, textinput_create(r, rect, fp ? *fp : NULL));
	return 1;
}
static duk_ret_t jui_textinput_destroy(duk_context* ctx) { textinput_destroy((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_set_text(duk_context* ctx) { textinput_set_text((textinput_p)duk_get_pointer(ctx,1), duk_to_string(ctx,2)); return 0; }
static duk_ret_t jui_textinput_get_text(duk_context* ctx) { duk_push_string(ctx, textinput_get_text((textinput_p)duk_get_pointer(ctx,1))); return 1; }
static duk_ret_t jui_textinput_draw(duk_context* ctx) { textinput_draw((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, textinput_is_pressed((textinput_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_textinput_is_hovered(duk_context* ctx) { duk_push_boolean(ctx, textinput_is_hovered((textinput_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_textinput_focus(duk_context* ctx) { textinput_focus((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_defocus(duk_context* ctx) { textinput_defocus((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_insert(duk_context* ctx) { const char* t = duk_to_string(ctx,2); textinput_insert((textinput_p)duk_get_pointer(ctx,1), t, strlen(t)); return 0; }
static duk_ret_t jui_textinput_backspace(duk_context* ctx) { textinput_backspace((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_delete(duk_context* ctx) { textinput_delete((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_cursor_left(duk_context* ctx) { textinput_cursor_left((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_cursor_right(duk_context* ctx) { textinput_cursor_right((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_cursor_home(duk_context* ctx) { textinput_cursor_home((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_cursor_end(duk_context* ctx) { textinput_cursor_end((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_select_left(duk_context* ctx) { textinput_select_left((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_select_right(duk_context* ctx) { textinput_select_right((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_select_all(duk_context* ctx) { textinput_select_all((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_paste(duk_context* ctx) { textinput_paste((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_copy(duk_context* ctx) { textinput_copy((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_cut(duk_context* ctx) { textinput_cut((textinput_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_textinput_is_focused(duk_context* ctx) { duk_push_boolean(ctx, textinput_is_focused((textinput_p)duk_get_pointer(ctx,1))); return 1; }

/* ===== Slider ===== */
static duk_ret_t jui_slider_create(duk_context* ctx) {
	SDL_Renderer* r = (SDL_Renderer*)duk_get_pointer(ctx,1);
	duk_push_pointer(ctx, slider_create(r, (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3), (float)duk_to_number(ctx,4), (float)duk_to_number(ctx,5), (float)duk_to_number(ctx,6), (float)duk_to_number(ctx,7), (float)duk_to_number(ctx,8)));
	return 1;
}
static duk_ret_t jui_slider_destroy(duk_context* ctx) { slider_destroy((slider_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_slider_set_value(duk_context* ctx) { slider_set_value((slider_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2)); return 0; }
static duk_ret_t jui_slider_get_value(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)slider_get_value((slider_p)duk_get_pointer(ctx,1))); return 1; }
static duk_ret_t jui_slider_set_range(duk_context* ctx) { slider_set_range((slider_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3)); return 0; }
static duk_ret_t jui_slider_draw(duk_context* ctx) { slider_draw((slider_p)duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jui_slider_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, slider_is_pressed((slider_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_slider_set_pressed(duk_context* ctx) { slider_set_pressed((slider_p)duk_get_pointer(ctx,1), duk_get_boolean(ctx,2)); return 0; }
static duk_ret_t jui_slider_drag_move(duk_context* ctx) { slider_drag_move((slider_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3)); return 0; }
static duk_ret_t jui_slider_is_hovered(duk_context* ctx) { duk_push_boolean(ctx, slider_is_hovered((slider_p)duk_get_pointer(ctx,1), (float)duk_to_number(ctx,2), (float)duk_to_number(ctx,3))); return 1; }
static duk_ret_t jui_slider_set_hovered(duk_context* ctx) { slider_set_hovered((slider_p)duk_get_pointer(ctx,1), duk_get_boolean(ctx,2)); return 0; }

/* ===== Registration ===== */
typedef struct { duk_c_function func; const char* name; } reg_pair_t;

static void reg_sub(duk_context* ctx, const char* name, const reg_pair_t* pairs)
{
	duk_push_object(ctx);
	for (; pairs->func; pairs++) {
		duk_push_c_function(ctx, pairs->func, 0);
		duk_put_prop_string(ctx, -2, pairs->name);
	}
	duk_put_prop_string(ctx, -2, name);
}

void jsopen_ui(duk_context* ctx)
{
	duk_push_object(ctx);

	static const reg_pair_t label_fns[] = {
		{jui_label_create, "create"},
		{jui_label_destroy, "destroy"},
		{jui_label_set_position, "set_position"},
		{jui_label_draw, "draw"},
		{NULL, NULL}
	}; reg_sub(ctx, "label", label_fns);

	static const reg_pair_t button_fns[] = {
		{jui_button_create, "create"},
		{jui_button_destroy, "destroy"},
		{jui_button_set_position, "set_position"},
		{jui_button_set_text, "set_text"},
		{jui_button_is_hovered, "is_hovered"},
		{jui_button_is_pressed, "is_pressed"},
		{jui_button_set_hovered, "set_hovered"},
		{jui_button_set_pressed, "set_pressed"},
		{jui_button_draw, "draw"},
		{NULL, NULL}
	}; reg_sub(ctx, "button", button_fns);

	static const reg_pair_t combobox_fns[] = {
		{jui_combobox_create, "create"},
		{jui_combobox_destroy, "destroy"},
		{jui_combobox_additem, "additem"},
		{jui_combobox_draw, "draw"},
		{jui_combobox_is_pressed, "is_pressed"},
		{jui_combobox_is_open, "is_open"},
		{jui_combobox_set_open, "set_open"},
		{jui_combobox_hit_item, "hit_item"},
		{jui_combobox_set_selected, "set_selected"},
		{jui_combobox_get_selected, "get_selected"},
		{NULL, NULL}
	}; reg_sub(ctx, "combobox", combobox_fns);

	static const reg_pair_t checkbox_fns[] = {
		{jui_checkbox_create, "create"},
		{jui_checkbox_destroy, "destroy"},
		{jui_checkbox_draw, "draw"},
		{jui_checkbox_is_pressed, "is_pressed"},
		{jui_checkbox_set_hovered, "set_hovered"},
		{NULL, NULL}
	}; reg_sub(ctx, "checkbox", checkbox_fns);

	static const reg_pair_t radio_fns[] = {
		{jui_radiobutton_create, "create"},
		{jui_radiobutton_destroy, "destroy"},
		{jui_radiobutton_is_pressed, "is_pressed"},
		{jui_radiobutton_set_hovered, "set_hovered"},
		{NULL, NULL}
	}; reg_sub(ctx, "radiobutton", radio_fns);

	static const reg_pair_t radiog_fns[] = {
		{jui_radiogroup_create, "create"},
		{jui_radiogroup_destroy, "destroy"},
		{jui_radiogroup_add_button, "add_button"},
		{jui_radiogroup_set_selected, "set_selected"},
		{jui_radiogroup_draw, "draw"},
		{NULL, NULL}
	}; reg_sub(ctx, "button_group", radiog_fns);

	static const reg_pair_t grid_fns[] = {
		{jui_datagrid_create, "create"},
		{jui_datagrid_destroy, "destroy"},
		{jui_datagrid_draw, "draw"},
		{jui_datagrid_setheaders, "setheaders"},
		{jui_datagrid_addrow, "addrow"},
		{jui_datagrid_hit_row, "hit_row"},
		{jui_datagrid_scroll, "scroll"},
		{jui_datagrid_set_selected_row, "set_selected_row"},
		{jui_datagrid_get_row_count, "get_row_count"},
		{jui_datagrid_get_cell, "get_cell"},
		{NULL, NULL}
	}; reg_sub(ctx, "datagrid", grid_fns);

	static const reg_pair_t texti_fns[] = {
		{jui_textinput_create, "create"},
		{jui_textinput_destroy, "destroy"},
		{jui_textinput_set_text, "set_text"},
		{jui_textinput_get_text, "get_text"},
		{jui_textinput_draw, "draw"},
		{jui_textinput_is_pressed, "is_pressed"},
		{jui_textinput_is_hovered, "is_hovered"},
		{jui_textinput_focus, "focus"},
		{jui_textinput_defocus, "defocus"},
		{jui_textinput_insert, "insert"},
		{jui_textinput_backspace, "backspace"},
		{jui_textinput_delete, "delete"},
		{jui_textinput_cursor_left, "cursor_left"},
		{jui_textinput_cursor_right, "cursor_right"},
		{jui_textinput_cursor_home, "cursor_home"},
		{jui_textinput_cursor_end, "cursor_end"},
		{jui_textinput_select_left, "select_left"},
		{jui_textinput_select_right, "select_right"},
		{jui_textinput_select_all, "select_all"},
		{jui_textinput_paste, "paste"},
		{jui_textinput_copy, "copy"},
		{jui_textinput_cut, "cut"},
		{jui_textinput_is_focused, "is_focused"},
		{NULL, NULL}
	}; reg_sub(ctx, "textinput", texti_fns);

	static const reg_pair_t slider_fns[] = {
		{jui_slider_create, "create"},
		{jui_slider_destroy, "destroy"},
		{jui_slider_set_value, "set_value"},
		{jui_slider_get_value, "get_value"},
		{jui_slider_set_range, "set_range"},
		{jui_slider_draw, "draw"},
		{jui_slider_is_pressed, "is_pressed"},
		{jui_slider_set_pressed, "set_pressed"},
		{jui_slider_drag_move, "drag_move"},
		{jui_slider_is_hovered, "is_hovered"},
		{jui_slider_set_hovered, "set_hovered"},
		{NULL, NULL}
	}; reg_sub(ctx, "slider", slider_fns);

	duk_put_global_string(ctx, "ui");
}