#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <SDL3/SDL.h>
#include "external/duktape/duktape.h"
#include "jsx.h"
#include "jutils.h"
#include "jcore.h"
#include "jsclib/jsclib.h"

/* setEnv / getEnv 前向声明 */
static duk_ret_t js_set_env(duk_context* ctx);
static duk_ret_t js_get_env(duk_context* ctx);

struct jsx {
	duk_context* ctx;
	void* userdata;
	char* watch_file;
	SDL_Time last_mtime;
	duk_context* env;
	jevent_callbacks_t cbs;   /* C 层事件回调集合 */
};

/* 查找 joy.func 并调用 args… */
static bool call_joy_js(duk_context* ctx, const char* func, int nargs)
{
	/* 栈上已推入 nargs 个参数: [arg1, arg2, ..., argN] */
	duk_get_global_string(ctx, "joy");
	if (duk_is_undefined(ctx, -1)) {
		duk_pop_n(ctx, nargs + 1);  /* pop joy + 所有 arg */
		return false;
	}
	duk_get_prop_string(ctx, -1, func);
	if (duk_is_undefined(ctx, -1)) {
		duk_pop_n(ctx, nargs + 2);  /* pop func + joy + 所有 arg */
		return false;
	}
	/* 栈: [args..., joy, func] */
	duk_remove(ctx, -2);           /* 去掉 joy → [args..., func] */

	/* 移动 func 到 args 之下: [func, args...] */
	duk_insert(ctx, -(nargs + 1));

	if (duk_pcall(ctx, nargs)) {
		log_error("joy.%s: %s", func, duk_to_string(ctx, -1));
		duk_pop(ctx);
		return false;
	}
	duk_pop(ctx);  /* 弹出返回值 */
	return true;
}

/* ===== 默认回调，自动转发到 joy.* ===== */

static void default_mousepressed(float x, float y, int b, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	duk_context* ctx = jsx->ctx;
	duk_push_number(ctx, (duk_double_t)x);
	duk_push_number(ctx, (duk_double_t)y);
	duk_push_number(ctx, (duk_double_t)b);
	call_joy_js(ctx, "mousepressed", 3);
}

static void default_mousereleased(float x, float y, int b, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	duk_context* ctx = jsx->ctx;
	duk_push_number(ctx, (duk_double_t)x);
	duk_push_number(ctx, (duk_double_t)y);
	duk_push_number(ctx, (duk_double_t)b);
	call_joy_js(ctx, "mousereleased", 3);
}

static void default_mousemoved(float x, float y, float dx, float dy, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	duk_context* ctx = jsx->ctx;
	duk_push_number(ctx, (duk_double_t)x);
	duk_push_number(ctx, (duk_double_t)y);
	duk_push_number(ctx, (duk_double_t)dx);
	duk_push_number(ctx, (duk_double_t)dy);
	call_joy_js(ctx, "mousemoved", 4);
}

static void default_keypressed(const char* key, int sc, bool rep, bool ctrl, bool shift, bool alt, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	duk_context* ctx = jsx->ctx;
	duk_push_string(ctx, key);
	duk_push_number(ctx, (duk_double_t)sc);
	duk_push_boolean(ctx, rep ? 1 : 0);
	duk_push_boolean(ctx, ctrl ? 1 : 0);
	duk_push_boolean(ctx, shift ? 1 : 0);
	duk_push_boolean(ctx, alt ? 1 : 0);
	call_joy_js(ctx, "keypressed", 6);
}

static void default_keyreleased(const char* key, int sc, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	duk_context* ctx = jsx->ctx;
	duk_push_string(ctx, key);
	duk_push_number(ctx, (duk_double_t)sc);
	call_joy_js(ctx, "keyreleased", 2);
}

static void default_textinput(const char* text, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	duk_context* ctx = jsx->ctx;
	duk_push_string(ctx, text);
	call_joy_js(ctx, "textinput", 1);
}

static void default_resize(int w, int h, void* arg)
{
	jsx_p jsx = (jsx_p)arg;
	duk_context* ctx = jsx->ctx;
	duk_push_number(ctx, (duk_double_t)w);
	duk_push_number(ctx, (duk_double_t)h);
	call_joy_js(ctx, "resize", 2);
}


jsx_p jsx_create(void)
{
	jsx_p jsx;
	jsx = (jsx_p)malloc(sizeof(jsx_t));
	assert(jsx);
	memset(jsx, 0, sizeof(jsx_t));
	jsx->ctx = duk_create_heap(NULL, NULL, NULL, jsx, NULL);
	assert(jsx->ctx);
	/* 独立的 env heap（类似 Lua 的 lua_State* env，热更新不重置）*/
	jsx->env = duk_create_heap(NULL, NULL, NULL, NULL, NULL);
	assert(jsx->env);
	/* 把 jsx 指针存入主 ctx heap stash，供 set_env/get_env 查找 */
	duk_push_heap_stash(jsx->ctx);
	duk_push_pointer(jsx->ctx, jsx);
	duk_put_prop_string(jsx->ctx, -2, "\xff" "jsx_ptr");
	duk_pop(jsx->ctx);

	/* 设置默认回调，自动转发到 JS */
	jsx->cbs.on_mousepressed   = default_mousepressed;
	jsx->cbs.on_mousereleased  = default_mousereleased;
	jsx->cbs.on_mousemoved     = default_mousemoved;
	jsx->cbs.on_keypressed     = default_keypressed;
	jsx->cbs.on_keyreleased    = default_keyreleased;
	jsx->cbs.on_textinput      = default_textinput;
	jsx->cbs.on_resize         = default_resize;

	/* 注册 jsclib 模块 */
	jsopen_base(jsx->ctx);
	jsopen_fp(jsx->ctx);
	jsopen_ft(jsx->ctx);
	jsopen_vec2f(jsx->ctx);
	jsopen_vec2(jsx->ctx);
	jsopen_vec3f(jsx->ctx);
	jsopen_vec3(jsx->ctx);
	jsopen_mat22f(jsx->ctx);
	jsopen_mat22(jsx->ctx);
	jsopen_collision2d(jsx->ctx);
	jsopen_collision2df(jsx->ctx);
	jsopen_collision3d(jsx->ctx);
	jsopen_utils(jsx->ctx);
	jsopen_event(jsx->ctx);
	jsopen_sdl(jsx->ctx);
	jsopen_keyboard(jsx->ctx);
	jsopen_audio(jsx->ctx);
	jsopen_animation(jsx->ctx);
	jsopen_network(jsx->ctx);
	jsopen_timer(jsx->ctx);
	jsopen_joystick(jsx->ctx);
	jsopen_ui(jsx->ctx);
	jsopen_render2d(jsx->ctx);
	jsopen_window(jsx->ctx);
	jsopen_render3d(jsx->ctx);
	jsopen_gjk3d(jsx->ctx);
	jsopen_janimator(jsx->ctx);
	jsopen_behavior(jsx->ctx);
	jsopen_scene(jsx->ctx);
	jsopen_msgpack(jsx->ctx);

	/* 创建全局 joy 对象 */
	duk_push_object(jsx->ctx);
	duk_put_global_string(jsx->ctx, "joy");

	/* 注册全局 set_env / get_env（与 Lua API 一致）*/
	duk_push_c_function(jsx->ctx, js_set_env, 2);
	duk_put_global_string(jsx->ctx, "set_env");
	duk_push_c_function(jsx->ctx, js_get_env, 1);
	duk_put_global_string(jsx->ctx, "get_env");

	return jsx;
}


bool jsx_dofile(jsx_p jsx, const char* filename)
{
	size_t data_size;
	char* data;
	data = utils_read_file(filename, &data_size);
	if (data) {
		duk_push_lstring(jsx->ctx, data, data_size);
		if (duk_peval(jsx->ctx) != 0) {
			/* 执行错误，错误消息在栈顶 */
			SDL_free(data);
			log_info("%s", duk_to_string(jsx->ctx, -1));
			duk_pop(jsx->ctx);
			return false;
		}
		duk_pop(jsx->ctx);  /* 弹出执行结果 */
		SDL_free(data);
		return true;
	}
	return false;
}


void jsx_release(jsx_p jsx)
{
	if (jsx->watch_file) SDL_free(jsx->watch_file);
	duk_destroy_heap(jsx->ctx);
	free(jsx);
}


void jsx_dispatch_event(jsx_p jsx, const void* event)
{
	dispatch_event(jsx->cbs, event, jsx);
}


bool jsx_call_joy(jsx_p jsx, const char* func, int argc, float dt)
{
	duk_context* ctx = jsx->ctx;
	int top = duk_get_top(ctx);

	duk_get_global_string(ctx, "joy");
	if (duk_is_undefined(ctx, -1)) { duk_pop(ctx); return false; }
	duk_get_prop_string(ctx, -1, func);
	if (duk_is_undefined(ctx, -1)) { duk_pop_n(ctx, 2); return false; }

	duk_remove(ctx, -2);  /* 移除 joy，留下 func */

	if (argc > 0)
		duk_push_number(ctx, (duk_double_t)dt);

	if (duk_pcall(ctx, argc)) {
		log_error("joy.%s error: %s", func, duk_to_string(ctx, -1));
		duk_pop(ctx);
		duk_set_top(ctx, top);
		return false;
	}
	duk_set_top(ctx, top);
	return true;
}


void jsx_set_args(jsx_p jsx, int argc, char* argv[])
{
	duk_context* ctx = jsx->ctx;
	duk_push_array(ctx);
	for (int i = 0; i < argc; i++) {
		duk_push_string(ctx, argv[i]);
		duk_put_prop_index(ctx, -2, (duk_uarridx_t)i);
	}
	duk_put_global_string(ctx, "args");
}

/* ===== set_env / get_env (用 duk_xmove_top 跨 heap 传递，类似 Lua 的 lua_xmove) ===== */
static duk_ret_t js_set_env(duk_context* ctx)
{
	/* 从主 ctx heap stash 获取 jsx 指针 */
	duk_push_heap_stash(ctx);
	duk_get_prop_string(ctx, -1, "\xff" "jsx_ptr");
	jsx_p jsx = (jsx_p)duk_to_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (!jsx || !jsx->env) return 0;

	duk_context* E = jsx->env;
	/* 把 key, value 用 duk_xmove_top 转移到 env heap，类似 Lua 的 lua_xmove */
	duk_dup(ctx, 0);                       /* dup key */
	duk_dup(ctx, 1);                       /* dup value */
	duk_xmove_top(E, ctx, 2);              /* move key, value → env stack */
	{ const char* k = duk_to_string(E, -2); duk_put_global_string(E, k); duk_pop(E); }
	return 0;
}
static duk_ret_t js_get_env(duk_context* ctx)
{
	duk_push_heap_stash(ctx);
	duk_get_prop_string(ctx, -1, "\xff" "jsx_ptr");
	jsx_p jsx = (jsx_p)duk_to_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (!jsx || !jsx->env) return 0;

	duk_context* E = jsx->env;
	const char* key = duk_to_string(ctx, 0);
	duk_get_global_string(E, key);         /* get value from env heap */
	if (duk_is_undefined(E, -1)) { duk_pop(E); return 0; }
	duk_xmove_top(ctx, E, 1);              /* move value → main ctx */
	return 1;
}

bool jsx_reload(jsx_p jsx, const char* filename)
{
	return jsx_dofile(jsx, filename);
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
