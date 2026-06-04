#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <SDL3/SDL.h>
#include "external/mujs/mujs.h"
#include "jsx.h"
#include "jutils.h"
#include "jcore.h"
#include "jevent.h"
#include "jsclib/jsclib.h"

struct jsx {
	js_State* j;
	void* userdata;
	char* watch_file;
	SDL_Time last_mtime;

	event_callbacks_t cbs;   /* C 层事件回调集合 */
};

/* 查找 joy.func 并调用 args… */
static bool call_joy_js(js_State* J, const char* func, int nargs)
{
	/* 栈上已推入 nargs 个参数:
	 *   [arg1, arg2, ..., argN]
	 */
	js_getglobal(J, "joy");
	if (js_isundefined(J, -1)) {
		js_pop(J, nargs + 1);  /* pop joy + 所有 arg */
		return false;
	}
	js_getproperty(J, -1, func);
	if (js_isundefined(J, -1)) {
		js_pop(J, nargs + 2);  /* pop func + joy + 所有 arg */
		return false;
	}
	/* 栈: [args..., joy, func] */
	js_remove(J, -2);           /* 去掉 joy → [args..., func] */

	/* 旋转使 func 放到 args 之下 */
	js_rot(J, nargs + 1);       /* [func, args...] */

	if (js_pcall(J, nargs)) {
		log_error("joy.%s: %s", func, js_tostring(J, -1));
		js_pop(J, 1);
		return false;
	}
	return true;
}

/* ===== 默认回调，自动转发到 joy.* ===== */

static void default_mousepressed(float x, float y, int b, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	js_State* J = jsx->j;
	js_pushnumber(J, x);
	js_pushnumber(J, y);
	js_pushnumber(J, (double)b);
	call_joy_js(J, "mousepressed", 3);
}

static void default_mousereleased(float x, float y, int b, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	js_State* J = jsx->j;
	js_pushnumber(J, x);
	js_pushnumber(J, y);
	js_pushnumber(J, (double)b);
	call_joy_js(J, "mousereleased", 3);
}

static void default_mousemoved(float x, float y, float dx, float dy, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	js_State* J = jsx->j;
	js_pushnumber(J, x);
	js_pushnumber(J, y);
	js_pushnumber(J, dx);
	js_pushnumber(J, dy);
	call_joy_js(J, "mousemoved", 4);
}

static void default_keypressed(const char* key, int sc, bool rep, bool ctrl, bool shift, bool alt, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	js_State* J = jsx->j;
	js_pushstring(J, key);
	js_pushnumber(J, (double)sc);
	js_pushboolean(J, rep ? 1 : 0);
	js_pushboolean(J, ctrl ? 1 : 0);
	js_pushboolean(J, shift ? 1 : 0);
	js_pushboolean(J, alt ? 1 : 0);
	call_joy_js(J, "keypressed", 6);
}

static void default_keyreleased(const char* key, int sc, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	js_State* J = jsx->j;
	js_pushstring(J, key);
	js_pushnumber(J, (double)sc);
	call_joy_js(J, "keyreleased", 2);
}

static void default_textinput(const char* text, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	js_State* J = jsx->j;
	js_pushstring(J, text);
	call_joy_js(J, "textinput", 1);
}

static void default_resize(int w, int h, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	js_State* J = jsx->j;
	js_pushnumber(J, (double)w);
	js_pushnumber(J, (double)h);
	call_joy_js(J, "resize", 2);
}


jsx_p jsx_create(void)
{
	jsx_p jsx;
	jsx = (jsx_p)malloc(sizeof(jsx_t));
	assert(jsx);
	memset(jsx, 0, sizeof(jsx_t));
	jsx->j = js_newstate(NULL, jsx, JS_STRICT);
	assert(jsx->j);

	/* 设置默认回调，自动转发到 JS */
	jsx->cbs.on_mousepressed   = default_mousepressed;
	jsx->cbs.on_mousereleased  = default_mousereleased;
	jsx->cbs.on_mousemoved     = default_mousemoved;
	jsx->cbs.on_keypressed     = default_keypressed;
	jsx->cbs.on_keyreleased    = default_keyreleased;
	jsx->cbs.on_textinput      = default_textinput;
	jsx->cbs.on_resize         = default_resize;

	/* 注册 jsclib 模块 */
	jsopen_base(jsx->j);
	jsopen_fp(jsx->j);
	jsopen_ft(jsx->j);
	jsopen_vec2f(jsx->j);
	jsopen_vec2(jsx->j);
	jsopen_vec3f(jsx->j);
	jsopen_vec3(jsx->j);
	jsopen_mat22f(jsx->j);
	jsopen_mat22(jsx->j);
	jsopen_collision2d(jsx->j);
	jsopen_collision2df(jsx->j);
	jsopen_collision3d(jsx->j);
	jsopen_utils(jsx->j);
	jsopen_event(jsx->j);
	jsopen_sdl(jsx->j);
	jsopen_keyboard(jsx->j);
	jsopen_audio(jsx->j);
	jsopen_animation(jsx->j);
	jsopen_network(jsx->j);
	jsopen_timer(jsx->j);
	jsopen_joystick(jsx->j);
	jsopen_ui(jsx->j);
	jsopen_graphics(jsx->j);
	jsopen_window(jsx->j);

	/* 创建全局 joy 对象 */
	js_newobject(jsx->j);
	js_setglobal(jsx->j, "joy");

	js_setcontext(jsx->j, jsx);
	return jsx;
}


bool jsx_dofile(jsx_p jsx, const char* filename)
{
	size_t data_size;
	char* data;
	data = utils_read_file(filename, &data_size);
	if (data) {
		if (js_dostring(jsx->j, data)) {
			free(data);
			log_info("%s", js_tostring(jsx->j, -1));
			return true;
		}
		else {
			free(data);
			return false;
		}
	}
	return false;
}


void jsx_release(jsx_p jsx)
{
	if (jsx->watch_file) SDL_free(jsx->watch_file);
	js_freestate(jsx->j);
	free(jsx);
}


void jsx_dispatch_event(jsx_p jsx, const SDL_Event* event)
{
	dispatch_event(jsx->cbs, event, jsx);
}


bool jsx_call_joy(jsx_p jsx, const char* func, int argc, float dt)
{
	js_State* J = jsx->j;

	js_getglobal(J, "joy");
	if (js_isundefined(J, -1)) { js_pop(J, 1); return false; }
	js_getproperty(J, -1, func);
	if (js_isundefined(J, -1)) { js_pop(J, 2); return false; }

	js_remove(J, -2);  /* 移除 joy，留下 func */

	if (argc > 0)
		js_pushnumber(J, (double)dt);

	if (js_pcall(J, argc)) {
		log_error("joy.%s error: %s", func, js_tostring(J, -1));
		js_pop(J, 1);
		return false;
	}
	return true;
}


/* ===== state 持久化 ===== */

void jsx_init_state(jsx_p jsx)
{
	js_State* J = jsx->j;
	js_getglobal(J, "state");
	if (js_isundefined(J, -1)) {
		js_pop(J, 1);
		js_newobject(J);
		js_setglobal(J, "state");
	}
	else {
		js_pop(J, 1);
	}
	/* 备份到 registry */
	js_getglobal(J, "state");
	js_setregistry(J, "jsx_state");
}

void jsx_set_args(jsx_p jsx, int argc, char* argv[])
{
	js_State* J = jsx->j;
	js_newarray(J);
	for (int i = 0; i < argc; i++) {
		js_pushstring(J, argv[i]);
		js_setindex(J, -2, i);
	}
	js_setglobal(J, "args");
}

static void jsx_restore_state(jsx_p jsx)
{
	js_State* J = jsx->j;
	js_getregistry(J, "jsx_state");
	if (!js_isundefined(J, -1)) {
		js_setglobal(J, "state");
	}
	else {
		js_pop(J, 1);
		jsx_init_state(jsx);
	}
}

bool jsx_reload(jsx_p jsx, const char* filename)
{
	/* 备份当前 state → 执行脚本 → 恢复 state */
	js_getglobal(jsx->j, "state");
	js_setregistry(jsx->j, "jsx_state");

	bool ok = jsx_dofile(jsx, filename);
	jsx_restore_state(jsx);
	return ok;
}


/* ===== 文件监控 ===== */

void jsx_set_watch(jsx_p jsx, const char* filename)
{
	if (jsx->watch_file) SDL_free(jsx->watch_file);
	jsx->watch_file = filename ? SDL_strdup(filename) : NULL;
	if (filename) {
		SDL_PathInfo info;
		if (SDL_GetPathInfo(filename, &info))
			jsx->last_mtime = info.modify_time;
	}
}

bool jsx_poll_reload(jsx_p jsx)
{
	if (!jsx->watch_file) return false;
	SDL_PathInfo info;
	if (!SDL_GetPathInfo(jsx->watch_file, &info))
		return false;
	if (info.modify_time == jsx->last_mtime)
		return false;
	jsx->last_mtime = info.modify_time;
	log_info("[CHANGED] %s", jsx->watch_file);
	if (!jsx_reload(jsx, jsx->watch_file))
		return false;
	jsx_call_joy(jsx, "hotfix", 0, 0);
	return true;
}

const char* jsx_get_watch(jsx_p jsx)
{
	return jsx->watch_file;
}
