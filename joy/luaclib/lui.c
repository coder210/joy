#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../joy.h"

/* ===== UTF-8 字符串 → 码点数组（使用 jutils.h 的 utf8_decode_all） ===== */
static int* str_to_codepoints(lua_State* L, int idx, int* out_len) {
    size_t len;
    const char* s = lua_tolstring(L, idx, &len);
    size_t count = utf8_decode_all(s, len, NULL, 0);
    if (count == 0) { *out_len = 0; return NULL; }
    int* cps = (int*)malloc(sizeof(int) * count);
    utf8_decode_all(s, len, (int32_t*)cps, count);
    *out_len = (int)count;
    return cps;
}


static int l_label_create(lua_State* L)
{
	font_p fontinfo;
	jr2d_p r2d;
	label_p label;
	r2d = (jr2d_p)lua_touserdata(L, 1);
	fontinfo = (font_p)lua_touserdata(L, 2);
	if (lua_type(L, 3) == LUA_TSTRING) {
		int num_codepoints;
		int* codepoints = str_to_codepoints(L, 3, &num_codepoints);
		label = label_create(r2d, fontinfo, codepoints, num_codepoints);
		free(codepoints);
	} else {
		int num_codepoints = (int)luaL_len(L, 3);
		int* codepoints = (int*)malloc(sizeof(int) * num_codepoints);
		for (int i = 1; i <= num_codepoints; i++) {
			lua_geti(L, 3, i);
			codepoints[i - 1] = (int)luaL_checkinteger(L, -1);
			lua_pop(L, 1);
		}
		label = label_create(r2d, fontinfo, codepoints, num_codepoints);
		free(codepoints);
	}
	lua_pushlightuserdata(L, label);
	return 1;
}

static int l_label_destroy(lua_State* L)
{
	label_p label;
	label = (label_p)lua_touserdata(L, 1);
	label_destroy(label);
	return 0;
}

static int l_label_set_text(lua_State* L)
{
	label_p label;
	font_p fontinfo;
	jcolor_t color;
	color.r = 255; color.g = 255; color.b = 255; color.a = 255;

	label = (label_p)lua_touserdata(L, 1);
	fontinfo = (font_p)lua_touserdata(L, 2);

	if (lua_type(L, 3) == LUA_TSTRING) {
		int len;
		int* cps = str_to_codepoints(L, 3, &len);
		label_set_text(label, fontinfo, cps, len, color);
		free(cps);
	} else {
		int codepoints_len = (int)luaL_len(L, 3);
		int* codepoints = (int*)malloc(sizeof(int) * codepoints_len);
		for (int i = 1; i <= codepoints_len; i++) {
			lua_geti(L, 3, i);
			codepoints[i - 1] = (int)luaL_checkinteger(L, -1);
			lua_pop(L, 1);
		}
		label_set_text(label, fontinfo, codepoints, codepoints_len, color);
		free(codepoints);
	}
	return 0;
}

static int l_label_set_position(lua_State* L)
{
	label_p label;
	vec2_t position;
	label = (label_p)lua_touserdata(L, 1);
	position.x = (float)luaL_checknumber(L, 2);
	position.y = (float)luaL_checknumber(L, 3);
	//label->position = position;
	return 0;
}

static int l_label_draw(lua_State* L)
{
	label_p label;
	label = (label_p)lua_touserdata(L, 1);
	label_draw(label);
	return 0;
}


static int l_button_create(lua_State* L)
{
	jr2d_p r2d;
	button_p btn;
	r2d = (jr2d_p)lua_touserdata(L, 1);
	float w = (float)luaL_checknumber(L, 2);
	float h = (float)luaL_checknumber(L, 3);
	btn = button_create(r2d, w, h);
	lua_pushlightuserdata(L, btn);
	return 1;
}

static int l_button_destroy(lua_State* L)
{
	button_p btn;
	btn = (button_p)lua_touserdata(L, 1);
	button_destroy(btn);
	return 0;
}

static int l_button_set_text(lua_State* L)
{
	button_p btn;
	font_p fontinfo;
	jcolor_t color;

	btn = (button_p)lua_touserdata(L, 1);
	fontinfo = (font_p)lua_touserdata(L, 2);

	if (lua_type(L, 3) == LUA_TSTRING) {
		/* 字符串版 → button_set_textx */
		size_t slen;
		const char* s = lua_tolstring(L, 3, &slen);
		if (lua_type(L, 4) == LUA_TTABLE) {
			color.r = (int)luaL_checkinteger(L, lua_getfield(L, 4, "r") ? 4 : 0 ? 0 : 0); /* 简化：若颜色是 table */
			/* 更好的方式： */
		}
		/* 兼容：arg4=table(颜色对象) 或 arg4=r,arg5=g,arg6=b,arg7=a */
		if (lua_type(L, 4) == LUA_TTABLE) {
			/* {r,g,b,a} 对象 */
			lua_getfield(L, 4, "r"); color.r = (int)luaL_optinteger(L, -1, 255); lua_pop(L,1);
			lua_getfield(L, 4, "g"); color.g = (int)luaL_optinteger(L, -1, 255); lua_pop(L,1);
			lua_getfield(L, 4, "b"); color.b = (int)luaL_optinteger(L, -1, 255); lua_pop(L,1);
			lua_getfield(L, 4, "a"); color.a = (int)luaL_optinteger(L, -1, 255); lua_pop(L,1);
		} else {
			/* r,g,b,a 四个独立参数 */
			color.r = (int)luaL_optinteger(L, 4, 255);
			color.g = (int)luaL_optinteger(L, 5, 255);
			color.b = (int)luaL_optinteger(L, 6, 255);
			color.a = (int)luaL_optinteger(L, 7, 255);
		}
		button_set_textx(btn, fontinfo, s, (int)slen, color);
	} else {
		/* 码点数组版 */
		int num_codepoints = (int)luaL_len(L, 3);
		int* codepoints = (int*)malloc(sizeof(int) * num_codepoints);
		for (int i = 1; i <= num_codepoints; i++) {
			lua_geti(L, 3, i);
			codepoints[i - 1] = (int)luaL_checkinteger(L, -1);
			lua_pop(L, 1);
		}
		color.r = luaL_checkinteger(L, 4);
		color.g = luaL_checkinteger(L, 5);
		color.b = luaL_checkinteger(L, 6);
		color.a = luaL_checkinteger(L, 7);
		button_set_text(btn, fontinfo, codepoints, num_codepoints, color);
		free(codepoints);
	}
	return 0;
}

static int l_button_set_normal_color(lua_State* L)
{
	button_p btn;
	jcolor_t color;
	btn = (button_p)lua_touserdata(L, 1);
	color.r = luaL_checkinteger(L, 2);
	color.g = luaL_checkinteger(L, 3);
	color.b = luaL_checkinteger(L, 4);
	color.a = luaL_checkinteger(L, 5);
	button_set_normal_color(btn, color);
	return 0;
}

static int l_button_set_hover_color(lua_State* L)
{
	button_p btn;
	jcolor_t color;
	btn = (button_p)lua_touserdata(L, 1);
	color.r = luaL_checkinteger(L, 2);
	color.g = luaL_checkinteger(L, 3);
	color.b = luaL_checkinteger(L, 4);
	color.a = luaL_checkinteger(L, 5);
	button_set_hover_color(btn, color);
	return 0;
}

static int l_button_set_pressed_color(lua_State* L)
{
	button_p btn;
	jcolor_t color;
	btn = (button_p)lua_touserdata(L, 1);
	color.r = luaL_checkinteger(L, 2);
	color.g = luaL_checkinteger(L, 3);
	color.b = luaL_checkinteger(L, 4);
	color.a = luaL_checkinteger(L, 5);
	button_set_pressed_color(btn, color);
	return 0;
}

static int l_button_is_hovered(lua_State* L)
{
	button_p btn = (button_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, button_is_hovered(btn, x, y));
	return 1;
}

static int l_button_is_pressed(lua_State* L)
{
	button_p btn = (button_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, button_is_pressed(btn, x, y));
	return 1;
}

static int l_button_set_hovered(lua_State* L)
{
	button_p btn = (button_p)lua_touserdata(L, 1);
	button_set_hovered(btn, lua_toboolean(L, 2));
	return 0;
}

static int l_button_set_pressed(lua_State* L)
{
	button_p btn = (button_p)lua_touserdata(L, 1);
	button_set_pressed(btn, lua_toboolean(L, 2));
	return 0;
}

static int l_button_get_rect(lua_State* L)
{
	button_p btn = (button_p)lua_touserdata(L, 1);
	lua_newtable(L);
	//lua_pushnumber(L, btn->position.x); lua_setfield(L, -2, "x");
	//lua_pushnumber(L, btn->position.y); lua_setfield(L, -2, "y");
	//lua_pushnumber(L, btn->width);      lua_setfield(L, -2, "w");
	//lua_pushnumber(L, btn->height);     lua_setfield(L, -2, "h");
	return 1;
}

static int l_button_draw(lua_State* L)
{
	button_p btn;
	btn = (button_p)lua_touserdata(L, 1);
	button_draw(btn);
	return 0;
}


static int lcombobox_create(lua_State* L)
{
	jr2d_p r2d;
	combobox_p combobox;
	jrect_t rect;
	r2d = (jr2d_p)lua_touserdata(L, 1);
	rect.x = (float)luaL_checknumber(L, 2);
	rect.y = (float)luaL_checknumber(L, 3);
	rect.w = (float)luaL_checknumber(L, 4);
	rect.h = (float)luaL_checknumber(L, 5);
	combobox = combobox_create(r2d, rect);
	lua_pushlightuserdata(L, combobox);
	return 1;
}

static int lcombobox_draw(lua_State* L)
{
	combobox_p combobox;
	combobox = (combobox_p)lua_touserdata(L, 1);
	combobox_draw(combobox);
	return 0;
}

static int lcombobox_destroy(lua_State* L)
{
	combobox_p combobox;
	combobox = (combobox_p)lua_touserdata(L, 1);
	combobox_destroy(combobox);
	return 0;
}

static int lcombobox_additem(lua_State* L)
{
	combobox_p combobox = (combobox_p)lua_touserdata(L, 1);
	const char* text = luaL_checkstring(L, 2);
	if (!text) return 0;
	//if (combobox->item_count >= 20) return 0;
	//font_p font = (font_p)lua_touserdata(L, 3);
	//jcolor_t c = {0, 0, 0, 255};
	//combobox_item_p item = &combobox->items[combobox->item_count];
	//item->text = text_createx(combobox->r2d, font, text, (int)strlen(text), c);
	//item->is_selected = false;
	//combobox->item_count++;
	//combobox->dropdown_rect.h = combobox->item_count * combobox->item_height;
	return 0;
}

static int l_combobox_is_pressed(lua_State* L) {
	combobox_p cb = (combobox_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, combobox_is_pressed(cb, x, y));
	return 1;
}
static int l_combobox_is_open(lua_State* L) {
	lua_pushboolean(L, combobox_is_open((combobox_p)lua_touserdata(L, 1)));
	return 1;
}
static int l_combobox_set_open(lua_State* L) {
	combobox_set_open((combobox_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}
static int l_combobox_hit_item(lua_State* L) {
	combobox_p cb = (combobox_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushinteger(L, combobox_hit_item(cb, x, y));
	return 1;
}
static int l_combobox_set_selected(lua_State* L) {
	combobox_set_selected((combobox_p)lua_touserdata(L, 1), (int)luaL_checkinteger(L, 2));
	return 0;
}
static int l_combobox_get_selected(lua_State* L) {
	lua_pushinteger(L, combobox_get_selected((combobox_p)lua_touserdata(L, 1)));
	return 1;
}

static int l_checkbox_create(lua_State* L)
{
	checkbox_p checkbox;
	float x, y, size;
	const char* label;
	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	size = luaL_checkinteger(L, 3);
	label = luaL_checkstring(L, 4);
	checkbox = checkbox_create(x, y, size, label);
	lua_pushlightuserdata(L, checkbox);
	return 1;
}

static int l_checkbox_destroy(lua_State* L)
{
	checkbox_p checkbox;
	checkbox = (checkbox_p)lua_touserdata(L, 1);
	checkbox_destroy(checkbox);
	return 0;
}

static int l_checkbox_is_pressed(lua_State* L) {
	checkbox_p cb = (checkbox_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, checkbox_is_pressed(cb, x, y));
	return 1;
}
static int l_checkbox_set_hovered(lua_State* L) {
	checkbox_set_hovered((checkbox_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}

static int l_checkbox_draw(lua_State* L)
{
	checkbox_p checkbox;
	jr2d_p r2d;
	checkbox = (checkbox_p)lua_touserdata(L, 1);
	r2d = (jr2d_p)lua_touserdata(L, 2);
	checkbox_draw(checkbox, r2d, NULL);
	return 0;
}

static int l_radio_create(lua_State* L)
{
	radio_p btn;
	int x, y, font_size, group_id;
	const char* text;
	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	font_size = luaL_checkinteger(L, 3);
	text = luaL_checkstring(L, 4);
	group_id = luaL_checkinteger(L, 5);
	btn = radio_create(x, y, font_size, text, group_id);
	lua_pushlightuserdata(L, btn);
	return 1;
}

static int l_radio_destroy(lua_State* L)
{
	radio_p btn;
	btn = lua_touserdata(L, 1);
	radio_destroy(btn);
	return 0;
}

static int l_radio_is_pressed(lua_State* L) {
	radio_p r = (radio_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, radio_is_pressed(r, x, y));
	return 1;
}
static int l_radio_set_hovered(lua_State* L) {
	radio_set_hovered((radio_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}

static int ldatagrid_create(lua_State* L)
{
	datagrid_p datagrid;
	jr2d_p r2d;
	jrect_t rect;
	int col_count;
	bool has_header;
	void* font;

	luaL_checktype(L, 2, LUA_TTABLE);

	r2d = (jr2d_p)lua_touserdata(L, 1);

	lua_getfield(L, 2, "x");
	rect.x = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 2, "y");
	rect.y = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 2, "w");
	rect.w = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 2, "h");
	rect.h = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	col_count = luaL_checkinteger(L, 3);
	has_header = lua_toboolean(L, 4);
	font_p dgfont = (font_p)lua_touserdata(L, 5);

	datagrid = datagrid_create(r2d, rect, col_count, has_header, dgfont);
	lua_pushlightuserdata(L, datagrid);
	return 1;
}

static int ldatagrid_setheaders(lua_State* L)
{
	datagrid_p dg = (datagrid_p)lua_touserdata(L, 1);
	font_p fontinfo = (font_p)lua_touserdata(L, 2);
	jcolor_t color = { 0, 0, 0, 255 };
	luaL_checktype(L, 3, LUA_TTABLE);
	//int n = (int)luaL_len(L, 3);
	//for (int i = 1; i <= n && (i-1) < dg->col_count; i++) {
	//	lua_geti(L, 3, i);
	//	if (lua_isstring(L, -1)) {
	//		const char* text = lua_tostring(L, -1);
	//		if (dg->headers[i-1]) text_destroy(dg->headers[i-1]);
	//		dg->headers[i-1] = text_createx(dg->r2d, fontinfo, text, (int)strlen(text), color);
	//	}
	//	lua_pop(L, 1);
	//}
	return 0;
}

static int ldatagrid_draw(lua_State* L)
{
	datagrid_p datagrid;
	jr2d_p r2d;
	datagrid = (datagrid_p)lua_touserdata(L, 1);
	r2d = (jr2d_p)lua_touserdata(L, 2);
	datagrid_draw(datagrid, r2d);
	return 0;
}

static int ldatagrid_destroy(lua_State* L)
{
	datagrid_p datagrid;
	datagrid = (datagrid_p)lua_touserdata(L, 1);
	datagrid_destroy(datagrid);
	return 0;
}

static int ldatagrid_addrow(lua_State* L)
{
	datagrid_p dg = (datagrid_p)lua_touserdata(L, 1);
	//luaL_checktype(L, 2, LUA_TTABLE);
	//int cols = (int)luaL_len(L, 2);
	//if (cols > dg->col_count) cols = dg->col_count;
	//const char** row = (const char**)malloc(cols * sizeof(char*));
	//for (int j = 0; j < cols; j++) {
	//	lua_geti(L, 2, j + 1);
	//	row[j] = lua_tostring(L, -1);
	//	lua_pop(L, 1);
	//}
	//datagrid_addrow(dg, row);
	//free(row);
	return 0;
}

static int ldatagrid_get_row_count(lua_State* L) {
	datagrid_p dg = (datagrid_p)lua_touserdata(L, 1);
	//lua_pushinteger(L, dg->row_count);
	return 1;
}

static int ldatagrid_get_cell(lua_State* L) {
	datagrid_p dg = (datagrid_p)lua_touserdata(L, 1);
	//int r = (int)luaL_checkinteger(L, 2), c = (int)luaL_checkinteger(L, 3);
	//if (r >= 0 && r < (int)kv_size(dg->data) && c >= 0 && c < dg->col_count) {
	//	char* val = kv_A(kv_A(dg->data, r), c);
	//	if (val) lua_pushstring(L, val); else lua_pushnil(L);
	//} else lua_pushnil(L);
	return 1;
}

static int l_datagrid_hit_row(lua_State* L) {
	datagrid_p dg = (datagrid_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushinteger(L, datagrid_hit_row(dg, x, y));
	return 1;
}
static int l_datagrid_scroll(lua_State* L) {
	datagrid_scroll((datagrid_p)lua_touserdata(L, 1), (int)luaL_checkinteger(L, 2));
	return 0;
}
static int l_datagrid_set_selected_row(lua_State* L) {
	datagrid_set_selected_row((datagrid_p)lua_touserdata(L, 1), (int)luaL_checkinteger(L, 2));
	return 0;
}





/* ---- InputBox Lua bindings ---- */
static int l_inputbox_create(lua_State* L) {
	jr2d_p r = (jr2d_p)lua_touserdata(L, 1);
	font_p font = (font_p)lua_touserdata(L, 2);
	jrect_t rect;
	rect.x = (float)luaL_checknumber(L, 3);
	rect.y = (float)luaL_checknumber(L, 4);
	rect.w = (float)luaL_checknumber(L, 5);
	rect.h = (float)luaL_checknumber(L, 6);
	inputbox_p ti = inputbox_create(r, rect, font);
	lua_pushlightuserdata(L, ti);
	return 1;
}

static int l_inputbox_destroy(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	inputbox_destroy(ti);
	return 0;
}

static int l_inputbox_set_text(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	const char* text = luaL_checkstring(L, 2);
	inputbox_set_text(ti, text);
	return 0;
}

static int l_inputbox_get_text(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	lua_pushstring(L, inputbox_get_text(ti));
	return 1;
}

static int l_inputbox_draw(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	inputbox_draw(ti);
	return 0;
}

static int l_inputbox_is_pressed(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, inputbox_is_pressed(ti, x, y));
	return 1;
}

static int l_inputbox_is_hovered(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, inputbox_is_hovered(ti, x, y));
	return 1;
}

static int l_inputbox_focus(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	inputbox_focus(ti);
	return 0;
}

static int l_inputbox_defocus(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	inputbox_defocus(ti);
	return 0;
}

static int l_inputbox_insert(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	const char* text = luaL_checkstring(L, 2);
	inputbox_insert(ti, text, (int)strlen(text));
	return 0;
}

static int l_inputbox_backspace(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	inputbox_backspace(ti);
	return 0;
}

static int l_inputbox_delete(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	inputbox_delete(ti);
	return 0;
}

static int l_inputbox_cursor_left(lua_State* L) {
	inputbox_cursor_left((inputbox_p)lua_touserdata(L, 1));
	return 0;
}
static int l_inputbox_cursor_right(lua_State* L) {
	inputbox_cursor_right((inputbox_p)lua_touserdata(L, 1));
	return 0;
}
static int l_inputbox_cursor_home(lua_State* L) {
	inputbox_cursor_home((inputbox_p)lua_touserdata(L, 1));
	return 0;
}
static int l_inputbox_cursor_end(lua_State* L) {
	inputbox_cursor_end((inputbox_p)lua_touserdata(L, 1));
	return 0;
}

static int l_inputbox_select_left(lua_State* L) {
	inputbox_select_left((inputbox_p)lua_touserdata(L, 1)); return 0;
}
static int l_inputbox_select_right(lua_State* L) {
	inputbox_select_right((inputbox_p)lua_touserdata(L, 1)); return 0;
}
static int l_inputbox_select_home(lua_State* L) {
	inputbox_select_home((inputbox_p)lua_touserdata(L, 1)); return 0;
}
static int l_inputbox_select_end(lua_State* L) {
	inputbox_select_end((inputbox_p)lua_touserdata(L, 1)); return 0;
}
static int l_inputbox_select_all(lua_State* L) {
	inputbox_select_all((inputbox_p)lua_touserdata(L, 1)); return 0;
}

static int l_inputbox_paste(lua_State* L) {
	inputbox_paste((inputbox_p)lua_touserdata(L, 1));
	return 0;
}
static int l_inputbox_copy(lua_State* L) {
	inputbox_copy((inputbox_p)lua_touserdata(L, 1));
	return 0;
}
static int l_inputbox_cut(lua_State* L) {
	inputbox_cut((inputbox_p)lua_touserdata(L, 1));
	return 0;
}

static int l_inputbox_is_focused(lua_State* L) {
	inputbox_p ti = (inputbox_p)lua_touserdata(L, 1);
	lua_pushboolean(L, inputbox_is_focused(ti));
	return 1;
}

/* ---- Button helper (事件路由在 Lua 侧完成) ---- */
static int l_button_set_position(lua_State* L) {
	button_p btn = (button_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	button_set_position(btn, x, y);
	return 0;
}

/* ---- Slider Lua bindings ---- */
static int l_slider_create(lua_State* L) {
	jr2d_p r = (jr2d_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	float w = (float)luaL_checknumber(L, 4);
	float h = (float)luaL_checknumber(L, 5);
	float min = (float)luaL_checknumber(L, 6);
	float max = (float)luaL_checknumber(L, 7);
	float init = (float)luaL_checknumber(L, 8);
	slider_p s = slider_create(r, x, y, w, h, min, max, init);
	lua_pushlightuserdata(L, s);
	return 1;
}
static int l_slider_destroy(lua_State* L) {
	slider_destroy((slider_p)lua_touserdata(L, 1));
	return 0;
}
static int l_slider_set_value(lua_State* L) {
	slider_set_value((slider_p)lua_touserdata(L, 1), (float)luaL_checknumber(L, 2));
	return 0;
}
static int l_slider_get_value(lua_State* L) {
	lua_pushnumber(L, slider_get_value((slider_p)lua_touserdata(L, 1)));
	return 1;
}
static int l_slider_set_range(lua_State* L) {
	slider_p s = (slider_p)lua_touserdata(L, 1);
	float min = (float)luaL_checknumber(L, 2);
	float max = (float)luaL_checknumber(L, 3);
	slider_set_range(s, min, max);
	return 0;
}
static int l_slider_draw(lua_State* L) {
	slider_draw((slider_p)lua_touserdata(L, 1));
	return 0;
}
static int l_slider_is_pressed(lua_State* L) {
	slider_p s = (slider_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, slider_is_pressed(s, x, y));
	return 1;
}
static int l_slider_set_pressed(lua_State* L) {
	slider_set_pressed((slider_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}
static int l_slider_drag_move(lua_State* L) {
	slider_p s = (slider_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	slider_drag_move(s, x, y);
	return 0;
}
static int l_slider_is_hovered(lua_State* L) {
	slider_p s = (slider_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, slider_is_hovered(s, x, y));
	return 1;
}
static int l_slider_set_hovered(lua_State* L) {
	slider_set_hovered((slider_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}

/* ===== Popup ===== */
static int l_popup_create(lua_State* L) {
	jr2d_p r2d = (jr2d_p)lua_touserdata(L, 1);
	float w = (float)luaL_checknumber(L, 2), h = (float)luaL_checknumber(L, 3);
	lua_pushlightuserdata(L, popup_create(r2d, w, h));
	return 1;
}
static int l_popup_destroy(lua_State* L) { popup_destroy((popup_p)lua_touserdata(L, 1)); return 0; }
static int l_popup_set_title(lua_State* L) {
	popup_p p = (popup_p)lua_touserdata(L, 1);
	size_t len; const char* s = lua_tolstring(L, 2, &len);
	popup_set_title(p, s, (int)len);
	return 0;
}
static int l_popup_set_content(lua_State* L) {
	popup_p p = (popup_p)lua_touserdata(L, 1);
	size_t len; const char* s = lua_tolstring(L, 2, &len);
	popup_set_content(p, s, (int)len);
	return 0;
}
static int l_popup_set_position(lua_State* L) {
	popup_set_position((popup_p)lua_touserdata(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
	return 0;
}
static int l_popup_set_visible(lua_State* L) {
	popup_set_visible((popup_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}
static int l_popup_set_closeable(lua_State* L) {
	popup_set_closeable((popup_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}
static int l_popup_draw(lua_State* L) {
	popup_p p = (popup_p)lua_touserdata(L, 1);
	font_p tf = (font_p)lua_touserdata(L, 2);
	font_p cf = (font_p)lua_touserdata(L, 3);
	popup_draw(p, tf, cf);
	return 0;
}
static int l_popup_is_close_clicked(lua_State* L) {
	lua_pushboolean(L, popup_is_close_clicked((popup_p)lua_touserdata(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3)));
	return 1;
}
static int l_popup_is_title_clicked(lua_State* L) {
	lua_pushboolean(L, popup_is_title_clicked((popup_p)lua_touserdata(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3)));
	return 1;
}
static int l_popup_drag_move(lua_State* L) {
	popup_drag_move((popup_p)lua_touserdata(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
	return 0;
}
static int l_popup_is_inside(lua_State* L) {
	lua_pushboolean(L, popup_is_inside((popup_p)lua_touserdata(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3)));
	return 1;
}

/* ===== Tooltip ===== */
static int l_tooltip_create(lua_State* L) {
	jr2d_p r2d = (jr2d_p)lua_touserdata(L, 1);
	size_t len; const char* s = lua_tolstring(L, 2, &len);
	lua_pushlightuserdata(L, tooltip_create(r2d, s, (int)len));
	return 1;
}
static int l_tooltip_destroy(lua_State* L) { tooltip_destroy((tooltip_p)lua_touserdata(L, 1)); return 0; }
static int l_tooltip_set_text(lua_State* L) {
	tooltip_p t = (tooltip_p)lua_touserdata(L, 1);
	size_t len; const char* s = lua_tolstring(L, 2, &len);
	tooltip_set_text(t, s, (int)len); return 0;
}
static int l_tooltip_set_position(lua_State* L) {
	tooltip_set_position((tooltip_p)lua_touserdata(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
	return 0;
}
static int l_tooltip_set_visible(lua_State* L) {
	tooltip_set_visible((tooltip_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}
static int l_tooltip_process_event(lua_State* L) { tooltip_process_event((tooltip_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_tooltip_draw(lua_State* L) {
	tooltip_p t = (tooltip_p)lua_touserdata(L, 1);
	font_p font = (font_p)lua_touserdata(L, 2);
	tooltip_draw(t, font);
	return 0;
}

static int l_label_process_event(lua_State* L) { label_process_event((label_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_button_process_event(lua_State* L) { button_process_event((button_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_inputbox_process_event(lua_State* L) { inputbox_process_event((inputbox_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_slider_process_event(lua_State* L) { slider_process_event((slider_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_popup_process_event(lua_State* L) { popup_process_event((popup_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_combobox_process_event(lua_State* L) { combobox_process_event((combobox_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_checkbox_process_event(lua_State* L) { checkbox_process_event((checkbox_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_radio_process_event(lua_State* L) { radio_process_event((radio_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_datagrid_process_event(lua_State* L) { datagrid_process_event((datagrid_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }
static int l_joystick_process_event(lua_State* L) { joystick_process_event((joystick_p)lua_touserdata(L, 1), lua_touserdata(L, 2)); return 0; }

int luaopen_ui(lua_State* L)
{
	lua_newtable(L);

	luaL_Reg label_lib[] = {
	    {"create", l_label_create},
	    {"destroy", l_label_destroy},
	    {"set_text", l_label_set_text},
	    {"set_position", l_label_set_position},
	    {"draw", l_label_draw},
	    {"process_event", l_label_process_event},
	    {NULL, NULL},
	};
	luaL_newlib(L, label_lib);
	lua_setfield(L, -2, "label");

	luaL_Reg button_lib[] = {
	    {"create", l_button_create},
	    {"destroy", l_button_destroy},
	    {"set_text", l_button_set_text},
	    {"set_position", l_button_set_position},
	    {"set_normal_color", l_button_set_normal_color},
	    {"set_hover_color", l_button_set_hover_color},
	    {"set_pressed_color", l_button_set_pressed_color},
	    {"is_hovered", l_button_is_hovered},
	    {"is_pressed", l_button_is_pressed},
	    {"set_hovered", l_button_set_hovered},
	    {"set_pressed", l_button_set_pressed},
	    {"get_rect", l_button_get_rect},
	    {"draw", l_button_draw},
	    {"process_event", l_button_process_event},
	    {NULL, NULL},
	};
	luaL_newlib(L, button_lib);
	lua_setfield(L, -2, "button");

	luaL_Reg combobox_lib[] = {
	    {"create", lcombobox_create},
	    {"draw", lcombobox_draw},
	    {"additem", lcombobox_additem},
	    {"destroy", lcombobox_destroy},
	    {"is_pressed", l_combobox_is_pressed},
	    {"is_open", l_combobox_is_open},
	    {"set_open", l_combobox_set_open},
	    {"hit_item", l_combobox_hit_item},
	    {"set_selected", l_combobox_set_selected},
	    {"get_selected", l_combobox_get_selected},
	    {"process_event", l_combobox_process_event},
	    {NULL, NULL},
	};
	luaL_newlib(L, combobox_lib);
	lua_setfield(L, -2, "combobox");

	luaL_Reg checkbox_lib[] = {
		{"create", l_checkbox_create},
		{"destroy", l_checkbox_destroy},
		{"draw", l_checkbox_draw},
		{"is_pressed", l_checkbox_is_pressed},
		{"set_hovered", l_checkbox_set_hovered},
		{"process_event", l_checkbox_process_event},
		{NULL, NULL},
	};
	luaL_newlib(L, checkbox_lib);
	lua_setfield(L, -2, "checkbox");

	luaL_Reg radiobutton_lib[] = {
		{"create", l_radio_create},
		{"destroy", l_radio_destroy},
		{"is_pressed", l_radio_is_pressed},
		{"set_hovered", l_radio_set_hovered},
		{"process_event", l_radio_process_event},
		{NULL, NULL},
	};
	luaL_newlib(L, radiobutton_lib);
	lua_setfield(L, -2, "radiobutton");

	luaL_Reg datagrid_lib[] = {
	    {"create", ldatagrid_create},
	    {"draw", ldatagrid_draw},
	    {"setheaders", ldatagrid_setheaders},
	    {"addrow", ldatagrid_addrow},
	    {"destroy", ldatagrid_destroy},
	    {"hit_row", l_datagrid_hit_row},
	    {"scroll", l_datagrid_scroll},
	    {"set_selected_row", l_datagrid_set_selected_row},
	    {"get_row_count", ldatagrid_get_row_count},
	    {"get_cell", ldatagrid_get_cell},
	    {"process_event", l_datagrid_process_event},
	{NULL, NULL},
	};
	luaL_newlib(L, datagrid_lib);
	lua_setfield(L, -2, "datagrid");

	luaL_Reg inputbox_lib[] = {
		{"create", l_inputbox_create},
		{"destroy", l_inputbox_destroy},
		{"set_text", l_inputbox_set_text},
		{"get_text", l_inputbox_get_text},
		{"draw", l_inputbox_draw},
		{"is_pressed", l_inputbox_is_pressed},
		{"is_hovered", l_inputbox_is_hovered},
		{"focus", l_inputbox_focus},
		{"defocus", l_inputbox_defocus},
		{"insert", l_inputbox_insert},
		{"backspace", l_inputbox_backspace},
		{"delete", l_inputbox_delete},
		{"cursor_left", l_inputbox_cursor_left},
		{"cursor_right", l_inputbox_cursor_right},
		{"cursor_home", l_inputbox_cursor_home},
		{"cursor_end", l_inputbox_cursor_end},
		{"select_left", l_inputbox_select_left},
		{"select_right", l_inputbox_select_right},
		{"select_home", l_inputbox_select_home},
		{"select_end", l_inputbox_select_end},
		{"select_all", l_inputbox_select_all},
		{"paste", l_inputbox_paste},
		{"copy", l_inputbox_copy},
		{"cut", l_inputbox_cut},
		{"is_focused", l_inputbox_is_focused},
		{"process_event", l_inputbox_process_event},
		{NULL, NULL},
	};
	luaL_newlib(L, inputbox_lib);
	lua_setfield(L, -2, "inputbox");

	luaL_Reg slider_lib[] = {
		{"create", l_slider_create},
		{"destroy", l_slider_destroy},
		{"set_value", l_slider_set_value},
		{"get_value", l_slider_get_value},
		{"set_range", l_slider_set_range},
		{"draw", l_slider_draw},
		{"is_pressed", l_slider_is_pressed},
		{"set_pressed", l_slider_set_pressed},
		{"drag_move", l_slider_drag_move},
		{"is_hovered", l_slider_is_hovered},
		{"set_hovered", l_slider_set_hovered},
		{"process_event", l_slider_process_event},
		{NULL, NULL},
	};
	luaL_newlib(L, slider_lib);
	lua_setfield(L, -2, "slider");

	/* Popup */
	luaL_Reg popup_lib[] = {
		{"create", l_popup_create},
		{"destroy", l_popup_destroy},
		{"set_title", l_popup_set_title},
		{"set_content", l_popup_set_content},
		{"set_position", l_popup_set_position},
		{"set_visible", l_popup_set_visible},
		{"set_closeable", l_popup_set_closeable},
		{"draw", l_popup_draw},
		{"is_close_clicked", l_popup_is_close_clicked},
		{"is_title_clicked", l_popup_is_title_clicked},
		{"drag_move", l_popup_drag_move},
		{"is_inside", l_popup_is_inside},
		{"process_event", l_popup_process_event},
		{NULL, NULL},
	};
	luaL_newlib(L, popup_lib);
	lua_setfield(L, -2, "popup");

	/* Tooltip */
	luaL_Reg tooltip_lib[] = {
		{"create", l_tooltip_create},
		{"destroy", l_tooltip_destroy},
		{"set_text", l_tooltip_set_text},
		{"set_position", l_tooltip_set_position},
		{"set_visible", l_tooltip_set_visible},
		{"draw", l_tooltip_draw},
		{"process_event", l_tooltip_process_event},
		{NULL, NULL},
	};
	luaL_newlib(L, tooltip_lib);
	lua_setfield(L, -2, "tooltip");

	return 1;
}

