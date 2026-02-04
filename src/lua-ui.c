#include "luaclib.h"
#include "liv-graphics.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"


static int l_label_create(lua_State* L)
{
	int* codepoints;
	int codepoint, num_codepoints;
	font_p fontinfo;
	SDL_Renderer* renderer;
	label_p label;
	renderer = (SDL_Renderer*)lua_touserdata(L, 1);
	fontinfo = (font_p)lua_touserdata(L, 2);
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
	fontinfo = (font_p)lua_touserdata(L, 2);
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

static int l_label_handle_event(lua_State* L)
{
	label_p label;
	SDL_Event* event;
	label = (label_p)lua_touserdata(L, 1);
	event = (SDL_Event*)lua_touserdata(L, 2);
	label_handle_event(label, event);
	return 0;
}


static int l_button_create(lua_State* L)
{
	SDL_Renderer* renderer;
	SDL_FRect rect;
	button_p btn;
	renderer = (SDL_Renderer*)lua_touserdata(L, 1);
	rect.x = luaL_checknumber(L, 2);
	rect.y = luaL_checknumber(L, 3);
	rect.w = luaL_checknumber(L, 4);
	rect.h = luaL_checknumber(L, 5);
	btn = button_create(renderer, rect);
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
	fontinfo = (font_p)lua_touserdata(L, 2);
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

static int l_button_is_clicked(lua_State* L)
{
	bool result;
	button_p btn;
	btn = (button_p)lua_touserdata(L, 1);
	result = btn->is_pressed;
	lua_pushboolean(L, result);
	return 1;
}


static int l_button_draw(lua_State* L)
{
	button_p btn;
	btn = (button_p)lua_touserdata(L, 1);
	button_draw(btn);
	return 0;
}

static int l_button_listen_event(lua_State* L)
{
	button_p btn;
	SDL_Event* event;
	btn = (button_p)lua_touserdata(L, 1);
	event = (SDL_Event*)lua_touserdata(L, 2);
	button_handle_event(btn, event);
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

static int l_button_group_handle_event(lua_State* L)
{
	radio_button_group_p btn_group;
	SDL_Event* event;
	SDL_Renderer* renderer;
	btn_group = (radio_button_group_p)lua_touserdata(L, 1);
	event = (SDL_Event*)lua_touserdata(L, 2);
	renderer = (SDL_Renderer*)lua_touserdata(L, 3);
	radio_button_group_handle_event(btn_group, event, renderer);
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
	rect.x = luaL_checknumber(L, 1);
	rect.y = luaL_checknumber(L, 2);
	rect.w = luaL_checknumber(L, 3);
	rect.h = luaL_checknumber(L, 4);
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
	combobox_p combobox;
	SDL_Color f;
	const char* text;
	combobox = (combobox_p)lua_touserdata(L, 1);
	text = luaL_checkstring(L, 2);
	//combobox_add_item(combobox, "resources/fonts/simhei.ttf", 20, NULL, 0, f);
	return 0;
}

static int lcombobox_handle_event(lua_State* L)
{
	combobox_p combobox;
	SDL_Event* event;
	combobox = (combobox_p)lua_touserdata(L, 1);
	event = (SDL_Event*)lua_touserdata(L, 2);
	combobox_handle_event(combobox, event);
	return 0;
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

static int l_checkbox_handle_event(lua_State* L)
{
	checkbox_p checkbox;
	SDL_Event* event;
	SDL_Renderer* renderer;
	checkbox = (checkbox_p)lua_touserdata(L, 1);
	event = (SDL_Event*)lua_touserdata(L, 2);
	renderer = (SDL_Renderer*)lua_touserdata(L, 3);
	checkbox_handle_event(checkbox, event, renderer);
	return 0;
}

static int l_checkbox_draw(lua_State* L)
{
	checkbox_p checkbox;
	SDL_Event* event;
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

	datagrid = datagrid_create(renderer, rect, col_count, has_header, NULL);
	lua_pushlightuserdata(L, datagrid);
	return 1;
}

static int ldatagrid_setheaders(lua_State* L)
{
	datagrid_p datagrid;
	char** headers;
	codepoint_array_p codepoints[128];
	int i, j, rows, cols, codepoint;
	datagrid = (datagrid_p)lua_touserdata(L, 1);
	rows = (int)luaL_len(L, 2);
	for (i = 1; i <= rows; i++) {
		// 获取第 i 行
		lua_geti(L, 2, i);

		// 获取内层数组的长度（列数）
		cols = (int)luaL_len(L, -1);

		// 分配内存存储该行的整数
		codepoints[i - 1] = (codepoint_array_p)SDL_malloc(sizeof(codepoint_array_t));
		codepoints[i - 1]->length = cols;
		codepoints[i - 1]->array = (int*)SDL_malloc(sizeof(int) * cols);

		for (j = 1; j <= cols; j++) {
			// 获取第 j 列的值
			lua_geti(L, -1, j);
			codepoints[i - 1]->array[j - 1] = luaL_checkinteger(L, -1);
			lua_pop(L, 1);
		}

		// 弹出内层表
		lua_pop(L, 1);
	}

	datagrid_setheaders(datagrid, codepoints);
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
	/* datagrid_p datagrid;
	 char** row_data;
	 int i, len;
	 datagrid = (datagrid_p)lua_touserdata(L, 1);
	 len = (int)luaL_len(L, 2);
	 row_data = (char**)SDL_malloc(sizeof(char *) * len);

	 for (i = 1; i <= len; i++) {
		 lua_geti(L, 2, i);
		 if (lua_isstring(L, -1)) {
			 const char* str = lua_tostring(L, -1);
			 row_data[i - 1] = SDL_strdup(str);
		 }
		 lua_pop(L, 1);
	 }

	 datagrid_addrow(datagrid, row_data);*/
	return 0;
}

static int ldatagrid_handle_event(lua_State* L)
{
	datagrid_p datagrid;
	SDL_Event* event;
	SDL_Renderer* renderer;
	datagrid = (datagrid_p)lua_touserdata(L, 1);
	event = (SDL_Event*)lua_touserdata(L, 2);
	renderer = (SDL_Renderer*)lua_touserdata(L, 3);
	datagrid_handle_event(datagrid, event, renderer);
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
	    {"handle_event", l_label_handle_event},
	    {NULL, NULL},
	};
	luaL_newlib(L, label_lib);
	lua_setfield(L, -2, "label");

	luaL_Reg button_lib[] = {
	    {"create", l_button_create},
	    {"destroy", l_button_destroy},
	    {"set_text", l_button_set_text},
	    {"set_normal_color", l_button_set_normal_color},
	    {"set_hover_color", l_button_set_hover_color},
	    {"set_pressed_color", l_button_set_pressed_color},
	    {"is_clicked", l_button_is_clicked},
	    {"draw", l_button_draw},
	    {"listen", l_button_listen_event},
	    {NULL, NULL},
	};
	luaL_newlib(L, button_lib);
	lua_setfield(L, -2, "button");

	luaL_Reg button_group_lib[] = {
	    {"create", l_button_group_create},
	    {"destroy", l_button_group_destroy},
	    {"add_button", l_button_group_add_button},
	    {"set_selected", l_button_group_set_selected},
	    {"handle_event", l_button_group_handle_event},
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
	    {"handle_event", lcombobox_handle_event},
	    {NULL, NULL},
	};
	luaL_newlib(L, combobox_lib);
	lua_setfield(L, -2, "combobox");

	luaL_Reg checkbox_lib[] = {
		{"create", l_checkbox_create},
		{"destroy", l_checkbox_destroy},
		{"handle_event", l_checkbox_handle_event},
		{"draw", l_checkbox_draw},
		{NULL, NULL},
	};
	luaL_newlib(L, checkbox_lib);
	lua_setfield(L, -2, "checkbox");

	luaL_Reg radiobutton_lib[] = {
		{"create", lradiobutton_create},
		{"destroy", lradiobutton_destroy},
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
	    {"handle_event", ldatagrid_handle_event},
	{NULL, NULL},
	};
	luaL_newlib(L, datagrid_lib);
	lua_setfield(L, -2, "datagrid");

	return 1;
}
