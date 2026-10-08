#include "../joy.h"
#include "jsclib.h"

static jcolor_t read_color(duk_context* ctx, int idx)
{
	jcolor_t c = { 255, 255, 255, 255 };
	if (duk_is_object(ctx, idx)) {
		duk_get_prop_string(ctx, idx, "r"); c.r = (uint8_t)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "g"); c.g = (uint8_t)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "b"); c.b = (uint8_t)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "a"); c.a = (uint8_t)duk_to_number(ctx, -1); duk_pop(ctx);
	}
	return c;
}

#define REN(n) ((jr2d_p)duk_get_pointer(ctx, n))
#define PTR(n) duk_get_pointer(ctx, n)
#define NUM(n) ((float)duk_to_number(ctx, n))
#define INT(n) ((int)duk_to_number(ctx, n))
#define BOOL(n) duk_get_boolean(ctx, n)
#define STR(n) duk_to_string(ctx, n)

static duk_ret_t jui_label_create(duk_context* ctx)
{
	duk_push_pointer(ctx, label_create(REN(0), (font_p)PTR(1), NULL, 0));
	return 1;
}

static duk_ret_t jui_label_destroy(duk_context* ctx)
{
	label_destroy((label_p)PTR(0));
	return 0;
}

static duk_ret_t jui_label_set_position(duk_context* ctx)
{
	label_p lb = (label_p)PTR(0);
	//lb->position.x = NUM(1);
	//lb->position.y = NUM(2);
	return 0;
}

static duk_ret_t jui_label_draw(duk_context* ctx)
{
	label_draw((label_p)PTR(0));
	return 0;
}

static duk_ret_t jui_button_create(duk_context* ctx)
{
	duk_push_pointer(ctx, button_create(REN(0), 0.0f, 0.0f, NUM(1) / 480.0f, NUM(2) / 320.0f));
	return 1;
}

static duk_ret_t jui_button_destroy(duk_context* ctx)
{
	button_destroy((button_p)PTR(0));
	return 0;
}

static duk_ret_t jui_button_set_position(duk_context* ctx)
{
	button_set_position((button_p)PTR(0), NUM(1), NUM(2));
	return 0;
}

static duk_ret_t jui_button_set_text(duk_context* ctx)
{
	button_p b = (button_p)PTR(0);
	font_p fp = (font_p)PTR(1);
	const char* s = STR(2);
	jcolor_t c = { 255,255,255,255 };
	if (duk_is_object(ctx, 3))
		c = read_color(ctx, 3);
	button_set_textx(b, fp, s, strlen(s), c);
	return 0;
}

static duk_ret_t jui_button_is_hovered(duk_context* ctx)
{
	duk_push_boolean(ctx, button_is_hovered((button_p)PTR(0), NUM(1), NUM(2)));
	return 1;
}

static duk_ret_t jui_button_is_pressed(duk_context* ctx)
{
	duk_push_boolean(ctx, button_is_pressed((button_p)PTR(0), NUM(1), NUM(2)));
	return 1;
}

static duk_ret_t jui_button_set_hovered(duk_context* ctx)
{
	button_set_hovered((button_p)PTR(0), BOOL(1));
	return 0;
}

static duk_ret_t jui_button_set_pressed(duk_context* ctx)
{
	button_set_pressed((button_p)PTR(0), BOOL(1));
	return 0;
}

static duk_ret_t jui_button_draw(duk_context* ctx)
{
	button_draw((button_p)PTR(0));
	return 0;
}

static duk_ret_t jui_combobox_create(duk_context* ctx)
{
	jr2d_p r = REN(0);
		duk_push_pointer(ctx, combobox_create(r, NUM(1) / 480.0f, NUM(2) / 320.0f, NUM(3) / 480.0f, NUM(4) / 320.0f));
	return 1;
}

static duk_ret_t jui_combobox_destroy(duk_context* ctx)
{
	combobox_destroy((combobox_p)PTR(0));
	return 0;
}

static duk_ret_t jui_combobox_additem(duk_context* ctx)
{
	combobox_p c = (combobox_p)PTR(0);
	//if (c->item_count >= 20)
	//	return 0;
	//const char* text = STR(1);
	//font_p fp = (font_p)PTR(2);
	//jcolor_t col = { 0,0,0,255 };
	//combobox_item_p item = &c->items[c->item_count];
	//item->text = text_createx(c->r2d, fp, text, strlen(text), col);
	//item->is_selected = false;
	//c->item_count++;
	//c->dropdown_rect.h = c->item_count * c->item_height;
	return 0;
}

static duk_ret_t jui_combobox_draw(duk_context* ctx)
{
	combobox_draw((combobox_p)PTR(0));
	return 0;
}

static duk_ret_t jui_combobox_is_pressed(duk_context* ctx)
{
	duk_push_boolean(ctx, combobox_is_pressed((combobox_p)PTR(0), NUM(1), NUM(2)));
	return 1;
}

static duk_ret_t jui_combobox_is_open(duk_context* ctx)
{
	duk_push_boolean(ctx, combobox_is_open((combobox_p)PTR(0)));
	return 1;
}

static duk_ret_t jui_combobox_set_open(duk_context* ctx)
{
	combobox_set_open((combobox_p)PTR(0), BOOL(1));
	return 0;
}
static duk_ret_t jui_combobox_hit_item(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)combobox_hit_item((combobox_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_combobox_set_selected(duk_context* ctx) { combobox_set_selected((combobox_p)PTR(0), INT(1)); return 0; }
static duk_ret_t jui_combobox_get_selected(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)combobox_get_selected((combobox_p)PTR(0))); return 1; }

static duk_ret_t jui_checkbox_create(duk_context* ctx) { duk_push_pointer(ctx, checkbox_create(NUM(0) / 480.0f, NUM(1) / 320.0f, NUM(2) / 480.0f, STR(3))); return 1; }
static duk_ret_t jui_checkbox_destroy(duk_context* ctx) { checkbox_destroy((checkbox_p)PTR(0)); return 0; }
static duk_ret_t jui_checkbox_draw(duk_context* ctx) { checkbox_draw((checkbox_p)PTR(0), REN(1), NULL); return 0; }
static duk_ret_t jui_checkbox_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, checkbox_is_pressed((checkbox_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_checkbox_set_hovered(duk_context* ctx) { checkbox_set_hovered((checkbox_p)PTR(0), BOOL(1)); return 0; }

static duk_ret_t jui_radio_create(duk_context* ctx) { duk_push_pointer(ctx, radio_create(NUM(0) / 480.0f, NUM(1) / 320.0f, NUM(2) / 480.0f, STR(3), INT(4))); return 1; }
static duk_ret_t jui_radio_destroy(duk_context* ctx) { radio_destroy((radio_p)PTR(0)); return 0; }
static duk_ret_t jui_radio_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, radio_is_pressed((radio_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_radio_set_hovered(duk_context* ctx) { radio_set_hovered((radio_p)PTR(0), BOOL(1)); return 0; }

static duk_ret_t jui_datagrid_create(duk_context* ctx) { jr2d_p r = REN(0); duk_push_pointer(ctx, datagrid_create(r, INT(1) / 480.0f, INT(2) / 320.0f, INT(3) / 480.0f, INT(4) / 320.0f, INT(5), BOOL(6), (font_p)PTR(7))); return 1; }
static duk_ret_t jui_datagrid_destroy(duk_context* ctx) { datagrid_destroy((datagrid_p)PTR(0)); return 0; }
static duk_ret_t jui_datagrid_draw(duk_context* ctx) { datagrid_draw((datagrid_p)PTR(0), REN(1)); return 0; }
static duk_ret_t jui_datagrid_setheaders(duk_context* ctx) {
	//datagrid_p g = (datagrid_p)PTR(0); font_p fp = (font_p)PTR(1); jcolor_t col = { 0,0,0,255 }; int n = (int)duk_get_length(ctx, 2); for (int i = 0; i < n && i < g->col_count; i++) { duk_get_prop_index(ctx, 2, (duk_uarridx_t)i); const char* text = duk_to_string(ctx, -1); if (g->headers[i]) text_destroy(g->headers[i]); g->headers[i] = text_createx(g->r2d, fp, text, strlen(text), col); duk_pop(ctx); } 
	return 0; 
}

static duk_ret_t jui_datagrid_addrow(duk_context* ctx) { 
	/*datagrid_p g = (datagrid_p)PTR(0); int n = (int)duk_get_length(ctx, 1); if (n > g->col_count) n = g->col_count; const char** row = (const char**)malloc(n * sizeof(char*)); for (int i = 0; i < n; i++) { duk_get_prop_index(ctx, 1, (duk_uarridx_t)i); row[i] = duk_to_string(ctx, -1); duk_pop(ctx); } datagrid_addrow(g, row); free(row);*/
	return 0;
}

static duk_ret_t jui_datagrid_hit_row(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)datagrid_hit_row((datagrid_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_datagrid_scroll(duk_context* ctx) { datagrid_scroll((datagrid_p)PTR(0), INT(1)); return 0; }
static duk_ret_t jui_datagrid_set_selected_row(duk_context* ctx) { datagrid_set_selected_row((datagrid_p)PTR(0), INT(1)); return 0; }
static duk_ret_t jui_datagrid_get_row_count(duk_context* ctx) { 
	//duk_push_number(ctx, (duk_double_t)((datagrid_p)PTR(0))->row_count);
	return 1;
}
static duk_ret_t jui_datagrid_get_cell(duk_context* ctx) { 
	//datagrid_p g = (datagrid_p)PTR(0); int r = INT(1), c = INT(2); if (r >= 0 && (size_t)r < kv_size(g->data) && c >= 0 && c < g->col_count) { char* v = kv_A(kv_A(g->data, r), c); if (v) { duk_push_string(ctx, v); return 1; } } duk_push_undefined(ctx); 
	return 1;
}

static duk_ret_t jui_inputbox_create(duk_context* ctx) { jr2d_p r = REN(0); font_p fp = (font_p)PTR(1); duk_push_pointer(ctx, inputbox_create(r, NUM(2) / 480.0f, NUM(3) / 320.0f, NUM(4) / 480.0f, NUM(5) / 320.0f, fp)); return 1; }
static duk_ret_t jui_inputbox_destroy(duk_context* ctx) { inputbox_destroy((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_set_text(duk_context* ctx) { inputbox_set_text((inputbox_p)PTR(0), STR(1)); return 0; }
static duk_ret_t jui_inputbox_get_text(duk_context* ctx) { duk_push_string(ctx, inputbox_get_text((inputbox_p)PTR(0))); return 1; }
static duk_ret_t jui_inputbox_draw(duk_context* ctx) { inputbox_draw((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, inputbox_is_pressed((inputbox_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_inputbox_is_hovered(duk_context* ctx) { duk_push_boolean(ctx, inputbox_is_hovered((inputbox_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_inputbox_focus(duk_context* ctx) { inputbox_focus((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_defocus(duk_context* ctx) { inputbox_defocus((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_insert(duk_context* ctx) { inputbox_insert((inputbox_p)PTR(0), STR(1), strlen(STR(1))); return 0; }
static duk_ret_t jui_inputbox_backspace(duk_context* ctx) { inputbox_backspace((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_delete(duk_context* ctx) { inputbox_delete((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_cursor_left(duk_context* ctx) { inputbox_cursor_left((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_cursor_right(duk_context* ctx) { inputbox_cursor_right((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_cursor_home(duk_context* ctx) { inputbox_cursor_home((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_cursor_end(duk_context* ctx) { inputbox_cursor_end((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_select_left(duk_context* ctx) { inputbox_select_left((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_select_right(duk_context* ctx) { inputbox_select_right((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_select_all(duk_context* ctx) { inputbox_select_all((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_paste(duk_context* ctx) { inputbox_paste((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_copy(duk_context* ctx) { inputbox_copy((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_cut(duk_context* ctx) { inputbox_cut((inputbox_p)PTR(0)); return 0; }
static duk_ret_t jui_inputbox_is_focused(duk_context* ctx) { duk_push_boolean(ctx, inputbox_is_focused((inputbox_p)PTR(0))); return 1; }

static duk_ret_t jui_slider_create(duk_context* ctx) { duk_push_pointer(ctx, slider_create(REN(0), NUM(1) / 480.0f, NUM(2) / 320.0f, NUM(3) / 480.0f, NUM(4) / 320.0f, NUM(5), NUM(6), NUM(7))); return 1; }
static duk_ret_t jui_slider_destroy(duk_context* ctx) { slider_destroy((slider_p)PTR(0)); return 0; }
static duk_ret_t jui_slider_set_value(duk_context* ctx) { slider_set_value((slider_p)PTR(0), NUM(1)); return 0; }
static duk_ret_t jui_slider_get_value(duk_context* ctx) { duk_push_number(ctx, (duk_double_t)slider_get_value((slider_p)PTR(0))); return 1; }
static duk_ret_t jui_slider_set_range(duk_context* ctx) { slider_set_range((slider_p)PTR(0), NUM(1), NUM(2)); return 0; }
static duk_ret_t jui_slider_draw(duk_context* ctx) { slider_draw((slider_p)PTR(0)); return 0; }
static duk_ret_t jui_slider_is_pressed(duk_context* ctx) { duk_push_boolean(ctx, slider_is_pressed((slider_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_slider_set_pressed(duk_context* ctx) { slider_set_pressed((slider_p)PTR(0), BOOL(1)); return 0; }
static duk_ret_t jui_slider_drag_move(duk_context* ctx) { slider_drag_move((slider_p)PTR(0), NUM(1), NUM(2)); return 0; }
static duk_ret_t jui_slider_is_hovered(duk_context* ctx) { duk_push_boolean(ctx, slider_is_hovered((slider_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_slider_set_hovered(duk_context* ctx) { slider_set_hovered((slider_p)PTR(0), BOOL(1)); return 0; }

/* ===== Popup ===== */
static duk_ret_t jui_popup_create(duk_context* ctx) {
	jr2d_p r2d = REN(0);
	duk_push_pointer(ctx, popup_create(r2d, (float)duk_to_number(ctx, 1) / 480.0f, (float)duk_to_number(ctx, 2) / 320.0f));
	return 1;
}
static duk_ret_t jui_popup_destroy(duk_context* ctx) { popup_destroy((popup_p)PTR(0)); return 0; }
static duk_ret_t jui_popup_set_title(duk_context* ctx) { popup_set_title((popup_p)PTR(0), STR(1), -1); return 0; }
static duk_ret_t jui_popup_set_content(duk_context* ctx) { popup_set_content((popup_p)PTR(0), STR(1), -1); return 0; }
static duk_ret_t jui_popup_set_position(duk_context* ctx) { popup_set_position((popup_p)PTR(0), NUM(1), NUM(2)); return 0; }
static duk_ret_t jui_popup_set_visible(duk_context* ctx) { popup_set_visible((popup_p)PTR(0), BOOL(1)); return 0; }
static duk_ret_t jui_popup_set_closeable(duk_context* ctx) { popup_set_closeable((popup_p)PTR(0), BOOL(1)); return 0; }
static duk_ret_t jui_popup_draw(duk_context* ctx) {
	popup_p p = (popup_p)PTR(0);
	font_p tf = (font_p)duk_get_pointer(ctx, 1);
	font_p cf = (font_p)duk_get_pointer(ctx, 2);
	popup_draw(p, tf, cf);
	return 0;
}
static duk_ret_t jui_popup_is_close_clicked(duk_context* ctx) { duk_push_boolean(ctx, popup_is_close_clicked((popup_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_popup_is_title_clicked(duk_context* ctx) { duk_push_boolean(ctx, popup_is_title_clicked((popup_p)PTR(0), NUM(1), NUM(2))); return 1; }
static duk_ret_t jui_popup_drag_move(duk_context* ctx) { popup_drag_move((popup_p)PTR(0), NUM(1), NUM(2)); return 0; }
static duk_ret_t jui_popup_is_inside(duk_context* ctx) { duk_push_boolean(ctx, popup_is_inside((popup_p)PTR(0), NUM(1), NUM(2))); return 1; }

/* ===== Tooltip ===== */
static duk_ret_t jui_tooltip_create(duk_context* ctx) {
	jr2d_p r2d = REN(0);
	duk_push_pointer(ctx, tooltip_create(r2d, STR(1), -1));
	return 1;
}
static duk_ret_t jui_tooltip_destroy(duk_context* ctx) { tooltip_destroy((tooltip_p)PTR(0)); return 0; }
static duk_ret_t jui_tooltip_set_text(duk_context* ctx) { tooltip_set_text((tooltip_p)PTR(0), STR(1), -1); return 0; }
static duk_ret_t jui_tooltip_set_position(duk_context* ctx) { tooltip_set_position((tooltip_p)PTR(0), NUM(1), NUM(2)); return 0; }
static duk_ret_t jui_tooltip_set_visible(duk_context* ctx) { tooltip_set_visible((tooltip_p)PTR(0), BOOL(1)); return 0; }
static duk_ret_t jui_tooltip_process_event(duk_context* ctx) { tooltip_process_event((tooltip_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_tooltip_draw(duk_context* ctx) {
	tooltip_p t = (tooltip_p)PTR(0);
	font_p f = (font_p)duk_get_pointer(ctx, 1);
	tooltip_draw(t, f);
	return 0;
}

typedef struct { duk_c_function func; const char* name; } reg_pair_t;
static void reg_sub(duk_context* ctx, const char* name, const reg_pair_t* pairs) { duk_push_object(ctx); for (; pairs->func; pairs++) { duk_push_c_function(ctx, pairs->func, DUK_VARARGS); duk_put_prop_string(ctx, -2, pairs->name); } duk_put_prop_string(ctx, -2, name); }

static duk_ret_t jui_label_process_event(duk_context* ctx) { label_process_event((label_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_button_process_event(duk_context* ctx) { button_process_event((button_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_inputbox_process_event(duk_context* ctx) { inputbox_process_event((inputbox_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_slider_process_event(duk_context* ctx) { slider_process_event((slider_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_popup_process_event(duk_context* ctx) { popup_process_event((popup_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_combobox_process_event(duk_context* ctx) { combobox_process_event((combobox_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_checkbox_process_event(duk_context* ctx) { checkbox_process_event((checkbox_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_radio_process_event(duk_context* ctx) { radio_process_event((radio_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_datagrid_process_event(duk_context* ctx) { datagrid_process_event((datagrid_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }
static duk_ret_t jui_joystick_process_event(duk_context* ctx) { joystick_process_event((joystick_p)PTR(0), (const void*)duk_get_pointer(ctx, 1)); return 0; }

void jsopen_ui(duk_context* ctx)
{
	duk_push_object(ctx);
#define P(name) static const reg_pair_t name##_fns[]=
#define E {NULL,NULL}
#define R(name) reg_sub(ctx,#name,name##_fns)

	P(label) { { jui_label_create, "create" }, { jui_label_destroy,"destroy" }, { jui_label_set_position,"set_position" }, { jui_label_draw,"draw" }, { jui_label_process_event,"process_event" }, E }; R(label);
	P(button) { { jui_button_create, "create" }, { jui_button_destroy,"destroy" }, { jui_button_set_position,"set_position" }, { jui_button_set_text,"set_text" }, { jui_button_is_hovered,"is_hovered" }, { jui_button_is_pressed,"is_pressed" }, { jui_button_set_hovered,"set_hovered" }, { jui_button_set_pressed,"set_pressed" }, { jui_button_draw,"draw" }, { jui_button_process_event,"process_event" }, E }; R(button);
	P(combobox) { { jui_combobox_create, "create" }, { jui_combobox_destroy,"destroy" }, { jui_combobox_additem,"additem" }, { jui_combobox_draw,"draw" }, { jui_combobox_is_pressed,"is_pressed" }, { jui_combobox_is_open,"is_open" }, { jui_combobox_set_open,"set_open" }, { jui_combobox_hit_item,"hit_item" }, { jui_combobox_set_selected,"set_selected" }, { jui_combobox_get_selected,"get_selected" }, E }; R(combobox);
	P(checkbox) { { jui_checkbox_create, "create" }, { jui_checkbox_destroy,"destroy" }, { jui_checkbox_draw,"draw" }, { jui_checkbox_is_pressed,"is_pressed" }, { jui_checkbox_set_hovered,"set_hovered" }, { jui_checkbox_process_event,"process_event" }, E }; R(checkbox);
	P(radiobutton) { { jui_radio_create, "create" }, { jui_radio_destroy,"destroy" }, { jui_radio_is_pressed,"is_pressed" }, { jui_radio_set_hovered,"set_hovered" }, { jui_radio_process_event,"process_event" }, E }; 	R(radiobutton);
	P(datagrid) { { jui_datagrid_create, "create" }, { jui_datagrid_destroy,"destroy" }, { jui_datagrid_draw,"draw" }, { jui_datagrid_setheaders,"setheaders" }, { jui_datagrid_addrow,"addrow" }, { jui_datagrid_hit_row,"hit_row" }, { jui_datagrid_scroll,"scroll" }, { jui_datagrid_set_selected_row,"set_selected_row" }, { jui_datagrid_get_row_count,"get_row_count" }, { jui_datagrid_get_cell,"get_cell" }, { jui_datagrid_process_event,"process_event" }, E }; R(datagrid);
	P(inputbox) { { jui_inputbox_create, "create" }, { jui_inputbox_destroy,"destroy" }, { jui_inputbox_set_text,"set_text" }, { jui_inputbox_get_text,"get_text" }, { jui_inputbox_draw,"draw" }, { jui_inputbox_is_pressed,"is_pressed" }, { jui_inputbox_is_hovered,"is_hovered" }, { jui_inputbox_focus,"focus" }, { jui_inputbox_defocus,"defocus" }, { jui_inputbox_insert,"insert" }, { jui_inputbox_backspace,"backspace" }, { jui_inputbox_delete,"delete" }, { jui_inputbox_cursor_left,"cursor_left" }, { jui_inputbox_cursor_right,"cursor_right" }, { jui_inputbox_cursor_home,"cursor_home" }, { jui_inputbox_cursor_end,"cursor_end" }, { jui_inputbox_select_left,"select_left" }, { jui_inputbox_select_right,"select_right" }, { jui_inputbox_select_all,"select_all" }, { jui_inputbox_paste,"paste" }, { jui_inputbox_copy,"copy" }, { jui_inputbox_cut,"cut" }, { jui_inputbox_is_focused,"is_focused" }, { jui_inputbox_process_event,"process_event" }, E }; R(inputbox);
	P(slider) { { jui_slider_create, "create" }, { jui_slider_destroy,"destroy" }, { jui_slider_set_value,"set_value" }, { jui_slider_get_value,"get_value" }, { jui_slider_set_range,"set_range" }, { jui_slider_draw,"draw" }, { jui_slider_is_pressed,"is_pressed" }, { jui_slider_set_pressed,"set_pressed" }, { jui_slider_drag_move,"drag_move" }, { jui_slider_is_hovered,"is_hovered" }, { jui_slider_set_hovered,"set_hovered" }, { jui_slider_process_event,"process_event" }, E }; R(slider);
	P(popup) { { jui_popup_create, "create" }, { jui_popup_destroy,"destroy" }, { jui_popup_set_title,"set_title" }, { jui_popup_set_content,"set_content" }, { jui_popup_set_position,"set_position" }, { jui_popup_set_visible,"set_visible" }, { jui_popup_set_closeable,"set_closeable" }, { jui_popup_draw,"draw" }, { jui_popup_is_close_clicked,"is_close_clicked" }, { jui_popup_is_title_clicked,"is_title_clicked" }, { jui_popup_drag_move,"drag_move" }, { jui_popup_is_inside,"is_inside" }, { jui_popup_process_event,"process_event" }, E }; R(popup);
	P(tooltip) { { jui_tooltip_create, "create" }, { jui_tooltip_destroy,"destroy" }, { jui_tooltip_set_text,"set_text" }, { jui_tooltip_set_position,"set_position" }, { jui_tooltip_set_visible,"set_visible" }, { jui_tooltip_draw,"draw" }, { jui_tooltip_process_event,"process_event" }, E }; R(tooltip);

	duk_put_global_string(ctx, "ui");
}
