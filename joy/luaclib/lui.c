#include "luaclib.h"
#include "../jui.h"
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"


static int l_label_create(lua_State* L)
{
	int* codepoints;
	int codepoint, num_codepoints;
	font_p fontinfo;
	SDL_Renderer* renderer;
	label_p label;
	renderer = (SDL_Renderer*)lua_touserdata(L, 1);
	fontinfo = *(font_p*)lua_touserdata(L, 2);
	num_codepoints = (int)luaL_len(L, 3);
	codepoints = (int*)SDL_malloc(sizeof(int) * num_codepoints);
	for (int i = 1; i <= num_codepoints; i++) {
		lua_geti(L, 3, i);
		codepoint = luaL_checkinteger(L, -1);
		codepoints[i - 1] = codepoint;
		lua_pop(L, 1);
	}
	label = label_create(renderer, fontinfo, codepoints, num_codepoints);
	lua_pushlightuserdata(L, label);
	SDL_free(codepoints);
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
	button_p btn;
	int i, pixels, codepoint;
	int* codepoints;
	int codepoints_len;
	font_p fontinfo;
	SDL_Color color;

	luaL_checktype(L, 3, LUA_TTABLE);

	label = (label_p)lua_touserdata(L, 1);
	fontinfo = *(font_p*)lua_touserdata(L, 2);
	codepoints_len = (int)luaL_len(L, 3);
	codepoints = (int*)SDL_malloc(sizeof(int) * codepoints_len);
	for (i = 1; i <= codepoints_len; i++) {
		lua_geti(L, 3, i);
		codepoint = luaL_checkinteger(L, -1);
		codepoints[i - 1] = codepoint;
		lua_pop(L, 1);
	}
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	label_set_text(label, fontinfo, codepoints, codepoints_len, color);
	SDL_free(codepoints);
	return 0;
}

static int l_label_set_position(lua_State* L)
{
	label_p label;
	SDL_FPoint position;
	label = (label_p)lua_touserdata(L, 1);
	position.x = (float)luaL_checknumber(L, 2);
	position.y = (float)luaL_checknumber(L, 3);
	label->position = position;
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
	SDL_Renderer* renderer;
	button_p btn;
	renderer = (SDL_Renderer*)lua_touserdata(L, 1);
	float w = (float)luaL_checknumber(L, 2);
	float h = (float)luaL_checknumber(L, 3);
	btn = button_create(renderer, w, h);
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
	int i, pixels, codepoint;
	int* codepoints;
	int num_codepoints;
	font_p fontinfo;
	SDL_Color color;

	luaL_checktype(L, 3, LUA_TTABLE);

	btn = (button_p)lua_touserdata(L, 1);
	fontinfo = *(font_p*)lua_touserdata(L, 2);
	num_codepoints = (int)luaL_len(L, 3);
	codepoints = (int*)SDL_malloc(sizeof(int) * num_codepoints);
	for (i = 1; i <= num_codepoints; i++) {
		lua_geti(L, 3, i);
		codepoint = luaL_checkinteger(L, -1);
		codepoints[i - 1] = codepoint;
		lua_pop(L, 1);
	}
	color.r = luaL_checkinteger(L, 4);
	color.g = luaL_checkinteger(L, 5);
	color.b = luaL_checkinteger(L, 6);
	color.a = luaL_checkinteger(L, 7);
	button_set_text(btn, fontinfo, codepoints, num_codepoints, color);
	SDL_free(codepoints);
	return 0;
}

static int l_button_set_normal_color(lua_State* L)
{
	button_p btn;
	SDL_Color color;
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
	SDL_Color color;
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
	SDL_Color color;
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
	lua_pushnumber(L, btn->position.x); lua_setfield(L, -2, "x");
	lua_pushnumber(L, btn->position.y); lua_setfield(L, -2, "y");
	lua_pushnumber(L, btn->width);      lua_setfield(L, -2, "w");
	lua_pushnumber(L, btn->height);     lua_setfield(L, -2, "h");
	return 1;
}

static int l_button_draw(lua_State* L)
{
	button_p btn;
	btn = (button_p)lua_touserdata(L, 1);
	button_draw(btn);
	return 0;
}


static int l_button_group_create(lua_State* L)
{
	radio_button_group_p btn_group;
	btn_group = radio_button_group_create();
	lua_pushlightuserdata(L, btn_group);
	return 1;
}

static int l_button_group_destroy(lua_State* L)
{
	radio_button_group_p btn_group;
	btn_group = (radio_button_group_p)lua_touserdata(L, 1);
	radio_button_group_destroy(btn_group);
	return 0;
}

static int l_button_group_add_button(lua_State* L)
{
	radio_button_group_p btn_group;
	radio_button_p btn;
	btn_group = (radio_button_group_p)lua_touserdata(L, 1);
	btn = (radio_button_p)lua_touserdata(L, 2);
	radio_button_group_add_button(btn_group, btn);
	return 0;
}

static int l_button_group_set_selected(lua_State* L)
{
	radio_button_group_p btn_group;
	int index;
	btn_group = (radio_button_group_p)lua_touserdata(L, 1);
	index = luaL_checkinteger(L, 2);
	radio_button_group_set_selected(btn_group, index);
	return 0;
}

static int l_button_group_draw(lua_State* L)
{
	radio_button_group_p btn_group;
	SDL_Renderer* renderer;
	btn_group = (radio_button_group_p)lua_touserdata(L, 1);
	renderer = (SDL_Renderer*)lua_touserdata(L, 2);
	radio_button_group_draw(btn_group, renderer);
	return 0;
}

static int lcombobox_create(lua_State* L)
{
	SDL_Renderer* renderer;
	combobox_p combobox;
	SDL_FRect rect;
	renderer = (SDL_Renderer*)lua_touserdata(L, 1);
	rect.x = (float)luaL_checknumber(L, 2);
	rect.y = (float)luaL_checknumber(L, 3);
	rect.w = (float)luaL_checknumber(L, 4);
	rect.h = (float)luaL_checknumber(L, 5);
	combobox = combobox_create(renderer, rect);
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
	if (combobox->item_count >= 20) return 0;
	font_p font = *(font_p*)lua_touserdata(L, 3);
	SDL_Color c = {0, 0, 0, 255};
	combobox_item_p item = &combobox->items[combobox->item_count];
	item->text = text_createx(font, text, (int)SDL_strlen(text), c);
	item->is_selected = false;
	combobox->item_count++;
	combobox->dropdown_rect.h = combobox->item_count * combobox->item_height;
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
	SDL_Renderer* renderer;
	checkbox = (checkbox_p)lua_touserdata(L, 1);
	renderer = (SDL_Renderer*)lua_touserdata(L, 2);
	checkbox_draw(checkbox, renderer, NULL);
	return 0;
}

static int lradiobutton_create(lua_State* L)
{
	radio_button_p btn;
	int x, y, font_size, group_id;
	const char* text;
	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	font_size = luaL_checkinteger(L, 3);
	text = luaL_checkstring(L, 4);
	group_id = luaL_checkinteger(L, 5);
	btn = radio_button_create(x, y, font_size, text, group_id);
	lua_pushlightuserdata(L, btn);
	return 1;
}

static int lradiobutton_destroy(lua_State* L)
{
	radio_button_p btn;
	btn = lua_touserdata(L, 1);
	radio_button_destroy(btn);
	return 0;
}

static int l_radio_button_is_pressed(lua_State* L) {
	radio_button_p r = (radio_button_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, radio_button_is_pressed(r, x, y));
	return 1;
}
static int l_radio_button_set_hovered(lua_State* L) {
	radio_button_set_hovered((radio_button_p)lua_touserdata(L, 1), lua_toboolean(L, 2));
	return 0;
}

static int ldatagrid_create(lua_State* L)
{
	datagrid_p datagrid;
	SDL_Renderer* renderer;
	SDL_Rect rect;
	int col_count;
	bool has_header;
	void* font;

	luaL_checktype(L, 2, LUA_TTABLE);

	renderer = (SDL_Renderer*)lua_touserdata(L, 1);

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
	font_p dgfont = *(font_p*)lua_touserdata(L, 5);

	datagrid = datagrid_create(renderer, rect, col_count, has_header, dgfont);
	lua_pushlightuserdata(L, datagrid);
	return 1;
}

static int ldatagrid_setheaders(lua_State* L)
{
	datagrid_p dg = (datagrid_p)lua_touserdata(L, 1);
	//char** headers;
	//codepoint_array_p codepoints[128];
	//int i, j, rows, cols, codepoint;
	//datagrid = (datagrid_p)lua_touserdata(L, 1);
	//rows = (int)luaL_len(L, 2);
	//for (i = 1; i <= rows; i++) {
	//	// 获取第 i 行
	//	lua_geti(L, 2, i);

	//	// 获取内层数组的长度（列数）
	//	cols = (int)luaL_len(L, -1);

	//	// 分配内存存储该行的整数
	//	codepoints[i - 1] = (codepoint_array_p)SDL_malloc(sizeof(codepoint_array_t));
	//	codepoints[i - 1]->length = cols;
	//	codepoints[i - 1]->array = (int*)SDL_malloc(sizeof(int) * cols);

	//	for (j = 1; j <= cols; j++) {
	//		// 获取第 j 列的值
	//		lua_geti(L, -1, j);
	//		codepoints[i - 1]->array[j - 1] = luaL_checkinteger(L, -1);
	//		lua_pop(L, 1);
	//	}

	//	// 弹出内层表
	//	lua_pop(L, 1);
	//}

	font_p fontinfo = *(font_p*)lua_touserdata(L, 2);
	SDL_Color color = { 0, 0, 0, 255 };
	luaL_checktype(L, 3, LUA_TTABLE);
	int n = (int)luaL_len(L, 3);
	for (int i = 1; i <= n && (i-1) < dg->col_count; i++) {
		lua_geti(L, 3, i);
		if (lua_isstring(L, -1)) {
			const char* text = lua_tostring(L, -1);
			if (dg->headers[i-1]) text_destroy(dg->headers[i-1]);
			dg->headers[i-1] = text_createx(fontinfo, text, (int)SDL_strlen(text), color);
		}
		lua_pop(L, 1);
	}
	return 0;
}

static int ldatagrid_draw(lua_State* L)
{
	datagrid_p datagrid;
	SDL_Renderer* renderer;
	datagrid = (datagrid_p)lua_touserdata(L, 1);
	renderer = (SDL_Renderer*)lua_touserdata(L, 2);
	datagrid_draw(datagrid, renderer);
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
	luaL_checktype(L, 2, LUA_TTABLE);
	int cols = (int)luaL_len(L, 2);
	if (cols > dg->col_count) cols = dg->col_count;
	dg->row_count++;
	dg->data = (int***)SDL_realloc(dg->data, dg->row_count * sizeof(int**));
	dg->data[dg->row_count-1] = (int**)SDL_malloc(dg->col_count * sizeof(int*));
	for (int i = 0; i < dg->col_count; i++)
		dg->data[dg->row_count-1][i] = NULL;
	for (int j = 1; j <= cols; j++) {
		lua_geti(L, 2, j);
		if (lua_isstring(L, -1))
			dg->data[dg->row_count-1][j-1] = (int*)SDL_strdup(lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	return 0;
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






/* ---- TextInput Lua bindings ---- */
static int l_textinput_create(lua_State* L) {
	SDL_Renderer* r = (SDL_Renderer*)lua_touserdata(L, 1);
	font_p font = *(font_p*)lua_touserdata(L, 2);
	SDL_FRect rect;
	rect.x = (float)luaL_checknumber(L, 3);
	rect.y = (float)luaL_checknumber(L, 4);
	rect.w = (float)luaL_checknumber(L, 5);
	rect.h = (float)luaL_checknumber(L, 6);
	textinput_p ti = textinput_create(r, rect, font);
	lua_pushlightuserdata(L, ti);
	return 1;
}

static int l_textinput_destroy(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	textinput_destroy(ti);
	return 0;
}

static int l_textinput_set_text(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	const char* text = luaL_checkstring(L, 2);
	textinput_set_text(ti, text);
	return 0;
}

static int l_textinput_get_text(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	lua_pushstring(L, textinput_get_text(ti));
	return 1;
}

static int l_textinput_draw(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	textinput_draw(ti);
	return 0;
}

static int l_textinput_is_pressed(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, textinput_is_pressed(ti, x, y));
	return 1;
}

static int l_textinput_is_hovered(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
	lua_pushboolean(L, textinput_is_hovered(ti, x, y));
	return 1;
}

static int l_textinput_focus(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	textinput_focus(ti);
	return 0;
}

static int l_textinput_defocus(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	textinput_defocus(ti);
	return 0;
}

static int l_textinput_insert(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	const char* text = luaL_checkstring(L, 2);
	textinput_insert(ti, text, (int)SDL_strlen(text));
	return 0;
}

static int l_textinput_backspace(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	textinput_backspace(ti);
	return 0;
}

static int l_textinput_delete(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	textinput_delete(ti);
	return 0;
}

static int l_textinput_is_focused(lua_State* L) {
	textinput_p ti = (textinput_p)lua_touserdata(L, 1);
	lua_pushboolean(L, textinput_is_focused(ti));
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

int luaopen_ui(lua_State* L)
{
	lua_newtable(L);

	luaL_Reg label_lib[] = {
	    {"create", l_label_create},
	    {"destroy", l_label_destroy},
	    {"set_text", l_label_set_text},
	    {"set_position", l_label_set_position},
	    {"draw", l_label_draw},
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
	    {NULL, NULL},
	};
	luaL_newlib(L, button_lib);
	lua_setfield(L, -2, "button");

	luaL_Reg button_group_lib[] = {
	    {"create", l_button_group_create},
	    {"destroy", l_button_group_destroy},
	    {"add_button", l_button_group_add_button},
	    {"set_selected", l_button_group_set_selected},
	    {"draw", l_button_group_draw},
	    {NULL, NULL},
	};
	luaL_newlib(L, button_group_lib);
	lua_setfield(L, -2, "button_group");

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
		{NULL, NULL},
	};
	luaL_newlib(L, checkbox_lib);
	lua_setfield(L, -2, "checkbox");

	luaL_Reg radiobutton_lib[] = {
		{"create", lradiobutton_create},
		{"destroy", lradiobutton_destroy},
		{"is_pressed", l_radio_button_is_pressed},
		{"set_hovered", l_radio_button_set_hovered},
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
	{NULL, NULL},
	};
	luaL_newlib(L, datagrid_lib);
	lua_setfield(L, -2, "datagrid");

	luaL_Reg textinput_lib[] = {
		{"create", l_textinput_create},
		{"destroy", l_textinput_destroy},
		{"set_text", l_textinput_set_text},
		{"get_text", l_textinput_get_text},
		{"draw", l_textinput_draw},
		{"is_pressed", l_textinput_is_pressed},
		{"is_hovered", l_textinput_is_hovered},
		{"focus", l_textinput_focus},
		{"defocus", l_textinput_defocus},
		{"insert", l_textinput_insert},
		{"backspace", l_textinput_backspace},
		{"delete", l_textinput_delete},
		{"is_focused", l_textinput_is_focused},
		{NULL, NULL},
	};
	luaL_newlib(L, textinput_lib);
	lua_setfield(L, -2, "textinput");

	return 1;
}
