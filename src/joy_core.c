/******************************************************************************
 * src/joy_core.c —— core 域唯一实现文件（一个域一个 .c，配 joy_core.h）
 * 职责：核心 / 系统工具 / 输入状态快照 / 第三方实现体
 * 合并自：jvendor_impl.c / jcore.c / jutils.c / joy_input.c（原 input 域）
 * 说明：本域公开 API 声明在配套头 src/joy_core.h，由 include/joy/japi.h 聚合包含；
 *       本文件是该域唯一的编译单元。
 *       内部符号（static / 私有宏 / typedef）保持各分片独立，未跨分片改名。
 ******************************************************************************/

#include "joy_core.h"

/* ==================== 合并自 src/jvendor_impl.c ==================== */
#define STB_TRUETYPE_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_truetype.h"
#include "external/dr_mp3.h"
#include "external/dr_flac.h"
#include "external/dr_wav.h"
#include "external/stb_image.h"
#include "external/stb_image_write.h"

/* OGG 支持（stb_vorbis.h 已在 external/ 中） */
#define STB_VORBIS_IMPLEMENTATION
#include "external/stb_vorbis.h"

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef STB_TRUETYPE_IMPLEMENTATION
#undef DR_MP3_IMPLEMENTATION
#undef DR_FLAC_IMPLEMENTATION
#undef DR_WAV_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#undef STB_VORBIS_IMPLEMENTATION

/* ==================== 结束 src/jvendor_impl.c ==================== */

/* ==================== 合并自 src/jcore.c ==================== */
#include "joy.h"
#include "external/stb_image.h"
#include <SDL3/SDL.h>

union joy_window {
        SDL_Window* sdl_window;
};


struct joy_env {
	SDL_Environment* sdl_env;
};

typedef enum joy_log_color {
	LOG_COLOR_BLACK = 30,
	LOG_COLOR_RED = 31,
	LOG_COLOR_GREEN = 32,
	LOG_COLOR_YELLOW = 33,
	LOG_COLOR_BLUE = 34,
	LOG_COLOR_MAGENTA = 35,
	LOG_COLOR_CYAN = 36,
	LOG_COLOR_WHITE = 37,
	LOG_COLOR_BRIGHT_BLACK = 90,
	LOG_COLOR_BRIGHT_RED = 91,
	LOG_COLOR_BRIGHT_GREEN = 92,
	LOG_COLOR_BRIGHT_YELLOW = 93,
	LOG_COLOR_BRIGHT_BLUE = 94,
	LOG_COLOR_BRIGHT_MAGENTA = 95,
	LOG_COLOR_BRIGHT_CYAN = 96,
	LOG_COLOR_BRIGHT_WHITE = 97
} joy_log_color_k;

struct joy_simple_fps_counter {
	int frame_count;
	int fps;
	Uint64 last_time;
	Uint64 current_time;
};

bool joy_core_init_env()
{
	return SDL_Init(SDL_INIT_AUDIO
		| SDL_INIT_VIDEO
		| SDL_INIT_EVENTS
		| SDL_INIT_JOYSTICK);
}

void joy_core_quit_env(void)
{
	SDL_Quit();
}

const char* joy_core_get_error(void)
{
	return SDL_GetError();
}

uint64_t joy_core_get_ticks(void)
{
	return SDL_GetTicks();
}

joy_window_p joy_core_create_window(const char* title, int w, int h)
{
        joy_window_p window = (joy_window_p)SDL_malloc(sizeof(joy_window_t));
        window->sdl_window = SDL_CreateWindow(title, w, h, SDL_WINDOW_RESIZABLE);
        if (!window->sdl_window) {
                SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "joy_core_create_window failed: %s", SDL_GetError());
                SDL_free(window);
                return NULL;
        }
        return window;
}

void joy_core_destroy_window(joy_window_p win)
{
	if (win) {
                SDL_DestroyWindow(win->sdl_window);
		SDL_free(win);
	}
}

void* joy_core_get_raw_window(joy_window_p win)
{
	return win->sdl_window;
}

void joy_core_window_set_fullscreen(joy_window_p win, bool fullscreen)
{
	if (win) SDL_SetWindowFullscreen(win->sdl_window, fullscreen);
}

bool joy_core_window_get_fullscreen(joy_window_p win)
{
        if (!win) return false;
        return (SDL_GetWindowFlags(win->sdl_window) & SDL_WINDOW_FULLSCREEN) != 0;
}

void joy_core_window_set_icon(joy_window_p win, const char* png_path)
{
        if (!win || !png_path) return;

        stbi_uc* filedata;
        size_t sz;
        int w, h, channels;
        unsigned char* pixels;
        SDL_Surface* surface;

        filedata = (stbi_uc*)SDL_LoadFile(png_path, &sz);
        if (!filedata) {
                SDL_Log("Failed to load icon file: %s", png_path);
                return;
        }

        pixels = stbi_load_from_memory(filedata, (int)sz, &w, &h, &channels, STBI_rgb_alpha);
        if (!pixels) {
                SDL_free(filedata);
                SDL_Log("Failed to decode icon PNG: %s", png_path);
                return;
        }

        surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, pixels, w * 4);
        if (!surface) {
                stbi_image_free(pixels);
                SDL_free(filedata);
                SDL_Log("Failed to create surface for icon: %s", png_path);
                return;
        }

        SDL_SetWindowIcon(win->sdl_window, surface);

        SDL_DestroySurface(surface);
        stbi_image_free(pixels);
        SDL_free(filedata);
}

static const char* key_name(SDL_Keycode k)
{
	static char buf[2];
	switch (k) {
	case SDLK_RETURN:    return "return";
	case SDLK_ESCAPE:    return "escape";
	case SDLK_BACKSPACE: return "backspace";
	case SDLK_TAB:       return "tab";
	case SDLK_SPACE:     return "space";
	case SDLK_DELETE:    return "delete";
	case SDLK_UP:        return "up";
	case SDLK_DOWN:      return "down";
	case SDLK_LEFT:      return "left";
	case SDLK_RIGHT:     return "right";
	case SDLK_HOME:      return "home";
	case SDLK_END:       return "end";
	case SDLK_PAGEUP:    return "pageup";
	case SDLK_PAGEDOWN:  return "pagedown";
	case SDLK_INSERT:    return "insert";
	case SDLK_LSHIFT:    return "lshift";
	case SDLK_RSHIFT:    return "rshift";
	case SDLK_LCTRL:     return "lctrl";
	case SDLK_RCTRL:     return "rctrl";
	case SDLK_LALT:      return "lalt";
	case SDLK_RALT:      return "ralt";
	default:
		if (k >= SDLK_A && k <= SDLK_Z) { buf[0] = (char)('a' + (k - SDLK_A)); buf[1] = 0; return buf; }
		if (k >= SDLK_0 && k <= SDLK_9) { buf[0] = (char)('0' + (k - SDLK_0)); buf[1] = 0; return buf; }
		return NULL;
	}
}


void joy_core_dispatch_event(joy_event_callbacks_t cbs,
	const void* ev, void* arg)
{
	const SDL_Event* event = (SDL_Event*)ev;
	switch (event->type) {
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		float x = (float)event->button.x;
		float y = (float)event->button.y;
		int b = event->button.button;
		joy_core_input_feed_mouse_move(x, y);
		joy_core_input_feed_mouse_button(b, true);
		if (cbs.on_mousepressed)
			cbs.on_mousepressed(x, y, b, arg);
		else
			;//joy_core_log_error("unhandled SDL_EVENT_MOUSE_BUTTON_DOWN");
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_UP: {
		float x = (float)event->button.x;
		float y = (float)event->button.y;
		int b = event->button.button;
		joy_core_input_feed_mouse_move(x, y);
		joy_core_input_feed_mouse_button(b, false);
		if (cbs.on_mousereleased)
			cbs.on_mousereleased(x, y, b, arg);
		else
			;//joy_core_log_error("unhandled SDL_EVENT_MOUSE_BUTTON_UP");
		break;
	}
	case SDL_EVENT_MOUSE_MOTION: {
		float x = (float)event->motion.x;
		float y = (float)event->motion.y;
		float dx = event->motion.xrel;
		float dy = event->motion.yrel;
		joy_core_input_feed_mouse_move(x, y);
		if (cbs.on_mousemoved)
			cbs.on_mousemoved(x, y, dx, dy, arg);
		else
			;//joy_core_log_error("unhandled SDL_EVENT_MOUSE_MOTION");
		break;
	}
	case SDL_EVENT_KEY_DOWN: {
		/* 状态按 scancode 记录, 不受 key_name() 的小键名表限制；
		   必须放在 name 判空之前, 否则未收录的键永远查不到。 */
		joy_core_input_feed_key((int)event->key.scancode, true);
		const char* name = key_name(event->key.key);
		if (!name) break;
		int sc = event->key.scancode;
		bool rep = event->key.repeat;
		bool ctrl = event->key.mod & SDL_KMOD_CTRL;
		bool shift = event->key.mod & SDL_KMOD_SHIFT;
		bool alt = event->key.mod & SDL_KMOD_ALT;
		if (cbs.on_keypressed)
			cbs.on_keypressed(name, sc, rep, ctrl, shift, alt, arg);
		else
			;//joy_core_log_error("unhandled SDL_EVENT_KEY_DOWN");
		break;
	}
	case SDL_EVENT_KEY_UP: {
		joy_core_input_feed_key((int)event->key.scancode, false);
		const char* name = key_name(event->key.key);
		if (!name) break;
		int sc = event->key.scancode;
		if (cbs.on_keyreleased)
			cbs.on_keyreleased(name, sc, arg);
		// else
		// 	joy_core_log_error("unhandled SDL_EVENT_KEY_UP");
		break;
	}
	case SDL_EVENT_TEXT_INPUT: {
		const char* text = event->text.text;
		if (cbs.on_textinput)
			cbs.on_textinput(text, arg);
		else
			;//joy_core_log_error("unhandled SDL_EVENT_TEXT_INPUT");
		break;
	}
	case SDL_EVENT_WINDOW_RESIZED: {
		int w = event->window.data1;
		int h = event->window.data2;
		if (cbs.on_resize)
			cbs.on_resize(w, h, arg);
		else
			;//joy_core_log_error("unhandled SDL_EVENT_WINDOW_RESIZED");
		break;
	}
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
		SDL_Event quit;
		SDL_zero(quit);
		quit.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&quit);
		break;
	}
	default:
		break;
	}
	if (cbs.on_process_event)
		cbs.on_process_event(ev, arg);
}

bool joy_core_push_user_event(int type, int code)
{
	SDL_Event event;
	SDL_zero(event);
	event.type = SDL_EVENT_USER + type;
	event.user.code = code;
	return SDL_PushEvent(&event);
}

int joy_core_get_event_type(const void* event)
{
	const SDL_Event* e = (const SDL_Event*)event;
	return e->type;
}

int joy_core_get_event_code(const void* event)
{
	const SDL_Event* e = (const SDL_Event*)event;
	return e->user.code;
}

bool joy_core_is_user_event(const void* event, int type)
{
	const SDL_Event* e = (const SDL_Event*)event;
	return e->type == SDL_EVENT_USER + type;
}

void joy_core_get_mouse_pos(const void* event, float* out_x, float* out_y)
{
	const SDL_Event* e = (const SDL_Event*)event;
	if (e->type == SDL_EVENT_MOUSE_MOTION) {
		*out_x = e->motion.x; *out_y = e->motion.y;
	} else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
	           e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		*out_x = e->button.x; *out_y = e->button.y;
	}
}

int joy_core_get_mouse_button(const void* event)
{
	const SDL_Event* e = (const SDL_Event*)event;
	if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
	    e->type == SDL_EVENT_MOUSE_BUTTON_UP)
		return e->button.button;
	return 0;
}

bool joy_core_push_quit_event(void)
{
	SDL_Event e; SDL_zero(e);
	e.type = SDL_EVENT_QUIT;
	return SDL_PushEvent(&e);
}

bool joy_core_should_close(void* ev)
{
	SDL_Event* e = (SDL_Event*)ev;
	return (e->type == SDL_EVENT_QUIT);
}


int joy_core_log_info(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char tmp[JOY_MAX_BUFFER];
	SDL_memset(tmp, 0, JOY_MAX_BUFFER);
	int max_size, len;
	char* data;

	data = NULL;
	len = SDL_vsnprintf(tmp, JOY_MAX_BUFFER, format, ap);
	va_end(ap);
	if (len >= 0 && len < JOY_MAX_BUFFER) {
		data = (char*)SDL_malloc(JOY_MAX_BUFFER);
		SDL_memcpy(data, tmp, len);
	}
	else {
		max_size = JOY_MAX_BUFFER;
		for (;;) {
			max_size *= 2;
			data = (char*)SDL_malloc(max_size);
			va_start(ap, format);
			len = SDL_vsnprintf(data, max_size, format, ap);
			va_end(ap);
			if (len < max_size) {
				break;
			}
			SDL_free(data);
		}
	}
	data[len] = 0;
	SDL_Log("\x1b[%dm%s\x1b[0m", LOG_COLOR_GREEN, data);
	SDL_free(data);
	return len;
}

int joy_core_log_debug(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char tmp[JOY_MAX_BUFFER];
	SDL_memset(tmp, 0, JOY_MAX_BUFFER);
	int max_size, len;
	char* data;

	data = NULL;
	len = SDL_vsnprintf(tmp, JOY_MAX_BUFFER, format, ap);
	va_end(ap);
	if (len >= 0 && len < JOY_MAX_BUFFER) {
		data = (char*)SDL_malloc(JOY_MAX_BUFFER);
		SDL_memcpy(data, tmp, len);
	}
	else {
		max_size = JOY_MAX_BUFFER;
		for (;;) {
			max_size *= 2;
			data = (char*)SDL_malloc(max_size);
			va_start(ap, format);
			len = SDL_vsnprintf(data, max_size, format, ap);
			va_end(ap);
			if (len < max_size) {
				break;
			}
			SDL_free(data);
		}
	}
	data[len] = 0;
	SDL_Log("\x1b[%dm%s\x1b[0m", LOG_COLOR_BLUE, data);
	SDL_free(data);
	return len;
}

int joy_core_log_error(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char tmp[JOY_MAX_BUFFER];
	SDL_memset(tmp, 0, JOY_MAX_BUFFER);
	int max_size, len;
	char* data;

	data = NULL;
	len = SDL_vsnprintf(tmp, JOY_MAX_BUFFER, format, ap);
	va_end(ap);
	if (len >= 0 && len < JOY_MAX_BUFFER) {
		data = (char*)SDL_malloc(JOY_MAX_BUFFER);
		SDL_memcpy(data, tmp, len);
	}
	else {
		max_size = JOY_MAX_BUFFER;
		for (;;) {
			max_size *= 2;
			data = (char*)SDL_malloc(max_size);
			va_start(ap, format);
			len = SDL_vsnprintf(data, max_size, format, ap);
			va_end(ap);
			if (len < max_size) {
				break;
			}
			SDL_free(data);
		}
	}
	data[len] = 0;
	SDL_Log("\x1b[%dm%s\x1b[0m", LOG_COLOR_RED, data);
	SDL_free(data);
	return len;
}

void joy_core_log_title(int type)
{
	if (type == 1) {
		joy_core_log_info("=======================================================");
		joy_core_log_info("     ##   #####   ##   ##   #######   ####   ");
		joy_core_log_info("     ##  ##   ##  ##   ##  ##   ##   ##  ##  ");
		joy_core_log_info("     ##  ##   ##   ## ##       ##    ##   ## ");
		joy_core_log_info(" ##  ##  ##   ##    ###       ##     ##  ##  ");
		joy_core_log_info("  ####    #####     ###    ######     ####   ");
		joy_core_log_info("=======================================================");
	}
	else if (type == 2) {
		joy_core_log_info("---------------------------------------------------");
		joy_core_log_info("                    J O Y 2 D                      ");
		joy_core_log_info("                Game Engine v2.0                   ");
		joy_core_log_info("---------------------------------------------------");
	}
	else if (type == 3) {
		joy_core_log_info("-------------------------------------------------------");
		joy_core_log_info("  JJJJ    OOOOO    Y   Y   22222    DDDDD             ");
		joy_core_log_info("    J    O     O    Y Y        2    D    D            ");
		joy_core_log_info("    J    O     O     Y      222     D     D           ");
		joy_core_log_info("J   J    O     O     Y     2        D    D            ");
		joy_core_log_info(" JJJ      OOOOO      Y    2222222   DDDDD             ");
		joy_core_log_info("-------------------------------------------------------");
	}
	else {
		joy_core_log_info("joy");
	}
}

joy_env_p joy_core_env_create(void)
{
	joy_env_p env;
	env = SDL_malloc(sizeof(joy_env_t));
	SDL_assert(env);
	env->sdl_env = SDL_CreateEnvironment(true);
	SDL_assert(env->sdl_env);
	return env;
}

const char* joy_core_env_get(joy_env_p env, const char* key)
{
	return SDL_GetEnvironmentVariable(env->sdl_env, key);
}

void joy_core_env_set(joy_env_p env, const char* key, const char* value)
{
	SDL_SetEnvironmentVariable(env->sdl_env, key, value, true);
}

void joy_core_env_destroy(joy_env_p env)
{
	SDL_assert(env);
	SDL_DestroyEnvironment(env->sdl_env);
	SDL_free(env);
}

joy_simple_fps_counter_p joy_core_simple_fps_create()
{
	joy_simple_fps_counter_p fps;
	fps = (joy_simple_fps_counter_p)SDL_malloc(sizeof(joy_simple_fps_counter_t));
	fps->fps = fps->frame_count = 0;
	fps->last_time = SDL_GetTicks();
	return fps;
}

void joy_core_simple_fps_destory(joy_simple_fps_counter_p fps)
{
	SDL_free(fps);
}

void joy_core_simple_fps_update(joy_simple_fps_counter_p fps)
{
	fps->frame_count++;
	fps->current_time = SDL_GetTicks();

	Uint64 elapsed = fps->current_time - fps->last_time;

	if (elapsed >= 1000) {
		fps->fps = fps->frame_count;
		fps->frame_count = 0;
		fps->last_time = fps->current_time;
	}
}

int joy_core_simple_fps_value(joy_simple_fps_counter_p fps)
{
	return fps->fps;
}

static void limit_frame_rate(joy_game_timer_p timer, float current_delta_time)
{
	if (!timer) return;

	if (current_delta_time < timer->target_frame_time) {
		double time_to_wait = timer->target_frame_time - (double)current_delta_time;
		Uint64 ns_to_wait = (Uint64)(time_to_wait * 1000000000.0);
		SDL_DelayNS(ns_to_wait);
		// 重新计算实际经过的时间（从 last_time 到此刻）
		timer->delta_time = (double)(SDL_GetTicksNS() - timer->last_time) / 1000000000.0;
	}
	// 如果 current_delta_time >= target_frame_time，不做任何事，delta_time 保持原值
}

void joy_core_game_timer_init(joy_game_timer_p timer)
{
	if (!timer) return;
	Uint64 now = SDL_GetTicksNS();
	timer->last_time = now;
	timer->frame_start_time = now;
	timer->delta_time = 0.0;
	timer->time_scale = 1.0;
	timer->target_fps = 0;
	timer->target_frame_time = 0.0;
	joy_core_log_info("[Gametimer] Initialized. Last time: %llu ns", (unsigned long long)now);
}

void joy_core_game_timer_update(joy_game_timer_p timer)
{
	if (!timer) return;

	// 记录进入 update 时的时间戳
	timer->frame_start_time = SDL_GetTicksNS();

	double current_delta_time = (double)(timer->frame_start_time - timer->last_time) / 1000000000.0;

	if (timer->target_frame_time > 0.0) {
		// 有限制帧率
		limit_frame_rate(timer, (float)current_delta_time);
	}
	else {
		timer->delta_time = current_delta_time;
	}

	// 记录离开 update 时的时间戳，供下一帧使用
	timer->last_time = SDL_GetTicksNS();
}



float joy_core_game_timer_get_delta_time(const joy_game_timer_p timer)
{
	if (!timer) return 0.0f;
	return (float)(timer->delta_time * timer->time_scale);
}

float joy_core_game_timer_get_unscaled_delta_time(const joy_game_timer_p timer)
{
	if (!timer) return 0.0f;
	return (float)timer->delta_time;
}

void joy_core_game_timer_set_time_scale(joy_game_timer_p timer, float scale)
{
	if (!timer) return;
	if (scale < 0.0f) {
		joy_core_log_info("[Gametimer] Warning: Time scale cannot be negative. Clamping to 0.");
		scale = 0.0f;
	}
	timer->time_scale = (double)scale;
}

float joy_core_game_timer_get_time_scale(const joy_game_timer_p timer)
{
	if (!timer) return 1.0f;
	return (float)timer->time_scale;
}

void joy_core_game_timer_set_target_fps(joy_game_timer_p timer, int fps)
{
	if (!timer) return;

	if (fps < 0) {
		joy_core_log_info("[Gametimer] Warning: Target FPS cannot be negative. Setting to 0 (unlimited).");
		timer->target_fps = 0;
	}
	else {
		timer->target_fps = fps;
	}

	if (timer->target_fps > 0) {
		timer->target_frame_time = 1.0 / (double)timer->target_fps;
		joy_core_log_info("[Gametimer] Target FPS set to: %d (Frame time: %.6f s)",
			timer->target_fps, timer->target_frame_time);
	}
	else {
		timer->target_frame_time = 0.0;
		joy_core_log_info("[Gametimer] Target FPS set to: Unlimited");
	}
}

int joy_core_game_timer_get_target_fps(const joy_game_timer_p timer)
{
	if (!timer) return 0;
	return timer->target_fps;
}

/* ---- 剪贴板 ---- */
char* joy_core_get_clipboard_text(void)
{
	const char* sdl_text = SDL_GetClipboardText();
	if (!sdl_text) return NULL;
	size_t len = strlen(sdl_text);
	char* copy = (char*)malloc(len + 1);
	if (copy) { memcpy(copy, sdl_text, len); copy[len] = '\0'; }
	return copy;
}

void joy_core_set_clipboard_text(const char* text)
{
	SDL_SetClipboardText(text);
}


/* ==================== 结束 src/jcore.c ==================== */

/* ==================== 合并自 src/jutils.c ==================== */
#include <time.h>
#include "external/md5.h"
#include "external/base64.h"
#include "external/aes_util.h"
#include "external/des.h"
#include <SDL3/SDL.h>
#include "joy.h"


#define MT_N 624
#define MT_M 397

struct joy_random_num {
	int index;
	uint32_t mt[MT_N];
};


char*
joy_core_utils_strdup(const char* str, size_t* out_sz)
{
	size_t sz = strlen(str);
	char* ret = (char*)malloc(sz + 1);
	if (ret)
		memcpy(ret, str, sz + 1);
	if (out_sz)
		*out_sz = sz;
	return ret;
}

bool
joy_core_utils_get_current_time(uint64_t *ms_ticks)
{
	bool resval;
	SDL_Time ticks;
	resval = SDL_GetCurrentTime(&ticks);
	*ms_ticks = ticks / 1000000;
	return resval;
}

void
joy_core_utils_current_datetime(const char* fmt, char* buf, size_t len)
{
	time_t t;
	time(&t);
	struct tm* now = (struct tm*)localtime(&t);
	strftime(buf, len, fmt, now);
}

/* 读整个文件。走 SDL_LoadFile（不是 fopen）——
 * 安卓上资源打包在 APK 内部，不是文件系统路径，fopen 读不到，
 * 只有 SDL 的 IO 层会接到 AAssetManager 从 APK 里取。
 * 返回 SDL_malloc 出来的缓冲（末尾带 '\0'），调用方用 SDL_free 释放；
 * 失败返回 NULL 且 *sz 置 0。契约与改动前一致。 */
char*
joy_core_utils_read_file(const char* filename, size_t* sz)
{
	size_t size = 0;
	char* buf;

	if (!sz) return NULL;
	*sz = 0;
	if (!filename || !*filename) return NULL;

	buf = (char*)SDL_LoadFile(filename, &size);
	if (!buf) return NULL;
	*sz = size;
	return buf;
}

/* 文件是否存在。同样走 SDL IO，理由同 joy_core_utils_read_file。 */
bool
joy_core_utils_check_file(const char* filename)
{
	SDL_IOStream* io;

	if (!filename || !*filename) return false;
	io = SDL_IOFromFile(filename, "rb");
	if (io) {
		SDL_CloseIO(io);
		return true;
	}
	return false;
}

void
joy_core_utils_append_file(const char* filename, const char* data)
{
	SDL_IOStream* io;

	if (!filename || !*filename || !data) return;
	io = SDL_IOFromFile(filename, "a+");
	if (io) {
		SDL_WriteIO(io, data, SDL_strlen(data));
		SDL_WriteIO(io, "\n", 1);
		SDL_CloseIO(io);
	}
}

short
joy_core_utils_bit2short(uint8_t* buf)
{
	short r = (short)buf[0] << 8 | (short)buf[1];
	return r;
}

void
joy_core_utils_short2bit(uint8_t* buf, short value)
{
	buf[0] = (value >> 8) & 0xff;
	buf[1] = value & 0xff;
}

/*
* c# = BitConverter.FromUInt32
*/
int
joy_core_utils_bit2int(uint8_t* buf)
{
	int r = buf[0];
	r |= (int)buf[1] << 8;
	r |= (int)buf[2] << 16;
	r |= (int)buf[3] << 24;
	return r;
}

/*
* c# = BitConverter.ToUInt32
*/
void
joy_core_utils_int2bit(uint8_t* buf, int value)
{
	buf[3] = ((0xff000000 & value) >> 24);
	buf[2] = ((0xff0000 & value) >> 16);
	buf[1] = ((0xff00 & value) >> 8);
	buf[0] = (0xff & value);
}

bool
joy_core_utils_wait_delay(joy_delay_t* delay, uint64_t current_time)
{
	if (current_time - delay->time >= delay->timeout) {
		delay->time = current_time;
		return true;
	}
	return false;
}

short
joy_core_utils_md5x(const char* input, size_t len, char* output)
{
	char value[16] = { 0 };
	md5(input, len, value);
	for (int i = 0; i < 16; i++) {
		char buf[4] = { 0 };
		sprintf(buf, "%02x", (unsigned char)value[i]);
		strcat(output, buf);
	}
	return 32;
}

short
utils_md5(const char* input, size_t len, char* output)
{
	md5(input, len, output);
	return 16;
}

char*
utils_b64_encode(const char* input, size_t len)
{
	char* result = b64_encode((const unsigned char*)input, len);
	return result;
}

unsigned char*
utils_b64_decode(const char* input, size_t len, size_t* out_len)
{
	unsigned char* result = b64_decode_ex(input, len, out_len);
	return result;
}

unsigned char*
joy_core_utils_aes_encrypt256(const char* input, size_t* len,
	const char* key, const char* iv)
{
	size_t input_len;
	unsigned char* output = NULL;

	//input_len = (int)SDL_strlen(input);
	//output = aes_encrypt256x(input, input_len, len, key, iv);

	return output;
}

/*
* key len is equal 32
*/
unsigned char*
joy_core_utils_aes_decrypt256(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output = NULL;
	//size_t input_len;

	//input_len = (int)SDL_strlen(input);
	//output = aes_decrypt256x(input, input_len, out_len, key, iv);

	return output;
}

/*
* key len is equal 16
*/
unsigned char*
joy_core_utils_aes_encrypt128(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output = NULL;
	int input_len;

	//input_len = (int)SDL_strlen(input);
	//output = aes_encrypt128x(input, input_len, out_len, key, iv);
	return output;
}

unsigned char*
joy_core_utils_aes_decrypt128(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output = NULL;
	int input_len;

	input_len = (int)SDL_strlen(input);
	//output = aes_decrypt128x(input, input_len, out_len, key, iv);

	return output;
}


char*
joy_core_utils_des_crypt(const char* key, const char* text,
	size_t textsz, size_t* out_len)
{
	uint32_t SK[32];
	uint8_t* buf;
	const uint8_t* utext;
	size_t keysz;
	int chunksz, bytes, i, j;
	uint8_t tail[8];

	keysz = (int)SDL_strlen(key);
	des_main_ks(SK, (const uint8_t*)key);
	utext = (const uint8_t*)text;
	chunksz = (textsz + 8) & ~7;
	buf = SDL_malloc(sizeof(uint8_t) * chunksz);
	for (i = 0; i < (int)textsz - 7; i += 8) {
		des_crypt(SK, utext + i, buf + i);
	}
	bytes = textsz - i;
	for (j = 0; j < 8; j++) {
		if (j < bytes) {
			tail[j] = utext[i + j];
		}
		else if (j == bytes) {
			tail[j] = 0x80;
		}
		else {
			tail[j] = 0;
		}
	}
	des_crypt(SK, tail, buf + i);
	*out_len = chunksz;
	return (char*)buf;
}

char*
joy_core_utils_des_decrypt(const char* key, const char* text, size_t textsz, size_t* out_len)
{
	uint32_t ESK[32];
	uint32_t SK[32];
	int i, padding;
	size_t keysz;
	const uint8_t* utext;
	uint8_t* buf;

	padding = 1;
	keysz = (int)SDL_strlen(key);
	des_main_ks(ESK, (const uint8_t*)key);
	utext = (const uint8_t*)text;

	for (i = 0; i < 32; i += 2) {
		SK[i] = ESK[30 - i];
		SK[i + 1] = ESK[31 - i];
	}

	if ((textsz & 7) || textsz == 0) {
		return NULL;
	}

	buf = SDL_malloc(sizeof(uint8_t) * textsz);
	for (i = 0; i < textsz; i += 8) {
		des_crypt(SK, utext + i, buf + i);
	}

	for (i = textsz - 1; i >= textsz - 8; i--) {
		if (buf[i] == 0) {
			padding++;
		}
		else if (buf[i] == 0x80) {
			break;
		}
		else {
			return NULL;
		}
	}
	if (padding > 8) {
		return NULL;
	}
	*out_len = textsz - padding;
	return (char*)buf;
}

char*
joy_core_utils_read_script(const char* file, size_t* data_size)
{
	size_t raw_data_size;
	char* raw_data, * data;
	const char* key;

	key = "com.livnet.liwei";
	raw_data = SDL_LoadFile(file, &raw_data_size);
	if (raw_data) {
		data = joy_core_utils_des_decrypt(key, raw_data, raw_data_size, data_size);
		if (data) {
			data[*data_size] = 0;
			SDL_free(raw_data);
		}
		else {
			data = raw_data;
			*data_size = raw_data_size;
		}
	}
	else {
		data = NULL;
		*data_size = 0;
	}

	return data;
}

joy_random_num_p
joy_core_utils_random_create(uint32_t seed)
{
	joy_random_num_p rnum;
	rnum = (joy_random_num_p)SDL_malloc(sizeof(joy_random_num_t));
	SDL_assert(rnum);
	rnum->mt[0] = seed;
	rnum->index = 0;
	for (int i = 1; i < MT_N; i++) {
		rnum->mt[i] = (1812433253 * (rnum->mt[i - 1]
			^ (rnum->mt[i - 1] >> 30)) + i);
	}
	return rnum;
}

void
joy_core_utils_random_destroy(joy_random_num_p rnum)
{
	SDL_free(rnum);
}

uint32_t
joy_core_utils_random_next(joy_random_num_p rnum)
{
	const uint32_t matrix_a = 0x9908b0df;
	const uint32_t upper_mask = 0x80000000;
	const uint32_t lower_mask = 0x7fffffff;
	if (rnum->index == 0) {
		for (int i = 0; i < MT_N; i++) {
			uint32_t y = (rnum->mt[i] & upper_mask)
				| (rnum->mt[(i + 1) % MT_N] & lower_mask);
			rnum->mt[i] = rnum->mt[(i + MT_M) % MT_N] ^ (y >> 1);
			if (y % 2 != 0)
				rnum->mt[i] ^= matrix_a;
		}
	}
	uint32_t y = rnum->mt[rnum->index];
	y ^= y >> 11;
	y ^= (y << 7) & 0x9d2c5680;
	y ^= (y << 15) & 0xefc60000;
	y ^= y >> 18;
	rnum->index = (rnum->index + 1) % MT_N;
	return y;
}

void
joy_core_utils_random_get_state(joy_random_num_p rnum, uint32_t* mt_out, int* index_out)
{
	if (rnum) {
		if (mt_out) SDL_memcpy(mt_out, rnum->mt, sizeof(uint32_t) * MT_N);
		if (index_out) *index_out = rnum->index;
	}
}

void
joy_core_utils_random_set_state(joy_random_num_p rnum, const uint32_t* mt_in, int index_in)
{
	if (rnum) {
		if (mt_in) SDL_memcpy(rnum->mt, mt_in, sizeof(uint32_t) * MT_N);
		rnum->index = index_in % MT_N;
	}
}

/* ============================================================
 * 单状态 LCG（64 位线性同余）。状态仅 1 个 uint64，
 * 帧同步时同步量最小（8 字节），替换 Mersenne Twister 用。
 * 算法：state = A*state + C（mod 2^64），返回值取高 32 位。
 * ============================================================ */
#define LCG_A 6364136223846793005ULL
#define LCG_C 1442695040888963407ULL

struct joy_lcg_num {
	uint64_t state;
};

joy_lcg_num_p
joy_core_lcg_random_create(uint64_t seed)
{
	joy_lcg_num_p lcg = (joy_lcg_num_p)SDL_malloc(sizeof(joy_lcg_num_t));
	SDL_assert(lcg);
	lcg->state = seed ? seed : 0x9e3779b97f4a7c15ULL; // 0 种子退化为默认非零初值
	return lcg;
}

void
joy_core_lcg_random_destroy(joy_lcg_num_p lcg)
{
	SDL_free(lcg);
}

uint32_t
joy_core_lcg_random_next(joy_lcg_num_p lcg)
{
	lcg->state = LCG_A * lcg->state + LCG_C;
	return (uint32_t)(lcg->state >> 32); // 取高 32 位，避免低位的短周期相关
}

void
joy_core_lcg_random_get_state(joy_lcg_num_p lcg, uint64_t* state_out)
{
	if (lcg && state_out) *state_out = lcg->state;
}

void
joy_core_lcg_random_set_state(joy_lcg_num_p lcg, uint64_t state)
{
	if (lcg) lcg->state = state;
}

unsigned long joy_core_sdbm_hash(const char* str, int size)
{
	unsigned long hash = 0;
	int c;
	while ((c = *str++)) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
	return hash % size;
}

unsigned long joy_core_fnv1a_hash(const char* str, int size)
{
	unsigned long hash = 2166136261u;  // FNV_offset_basis
	int c;
	while ((c = *str++)) {
		hash ^= c;
		hash *= 16777619u;  // FNV_prime
	}
	return hash % size;
}


unsigned long joy_core_jenkins_hash(const char* str, int size)
{
	unsigned long hash = 0;
	int c;
	while ((c = *str++)) {
		hash += c;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash % size;
}

unsigned long joy_core_murmur3_hash(const char* str)
{
	unsigned long hash = 0;
	const unsigned int seed = 0x9747b28c;
	const unsigned int c1 = 0xcc9e2d51;
	const unsigned int c2 = 0x1b873593;

	const unsigned char* data = (const unsigned char*)str;
	int len = strlen(str);

	while (len >= 4) {
		unsigned int k = *(unsigned int*)data;

		k *= c1;
		k = (k << 15) | (k >> 17);
		k *= c2;

		hash ^= k;
		hash = (hash << 13) | (hash >> 19);
		hash = hash * 5 + 0xe6546b64;

		data += 4;
		len -= 4;
	}

	unsigned int k = 0;
	switch (len) {
	case 3: k ^= data[2] << 16;
	case 2: k ^= data[1] << 8;
	case 1: k ^= data[0];
		k *= c1;
		k = (k << 15) | (k >> 17);
		k *= c2;
		hash ^= k;
	}

	hash ^= strlen(str);
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;

	return hash;
}

unsigned long joy_core_kr_hash(const char* str, int size)
{
	unsigned long hash = 0;
	int c;
	while ((c = *str++)) {
		hash = c + hash * 31;
	}
	return hash % size;
}


#define MAXUNICODE 0x10FFFFu
#define iscont(c) (((c) & 0xC0) == 0x80)

/* 各长度字符的最小合法码点（防止过短表示） */
static const uint32_t limits[] = {
    ~(uint32_t)0, 0x80, 0x800, 0x10000u, 0x200000u, 0x4000000u
};

size_t joy_core_utf8_decode(const char* s, size_t len, int32_t* code)
{
	if (len == 0) return 0;

	unsigned char c = (unsigned char)s[0];

	/* ASCII */
	if (c < 0x80) {
		*code = c;
		return 1;
	}

	int nbytes;
	uint32_t min;
	uint32_t res;

	/* 根据首字节判断字节数 */
	if (c < 0xC2) {
		return 0;
	}
	else if (c < 0xE0) {        /* 2字节: 110xxxxx */
		nbytes = 2;
		min = 0x80;
		res = c & 0x1F;
	}
	else if (c < 0xF0) {        /* 3字节: 1110xxxx */
		nbytes = 3;
		min = 0x800;
		res = c & 0x0F;
	}
	else if (c < 0xF5) {        /* 4字节: 11110xxx (F4 是能表示 0x10FFFF 的最大值) */
		nbytes = 4;
		min = 0x10000;
		res = c & 0x07;
	}
	else {
		return 0;                  /* 0xF5~0xFF 非法 */
	}

	if (len < (size_t)nbytes)
		return 0;

	/* 处理后续字节 */
	for (int i = 1; i < nbytes; i++) {
		unsigned char cc = (unsigned char)s[i];
		if (!iscont(cc))
			return 0;
		res = (res << 6) | (cc & 0x3F);
	}

	/* 过短检查 */
	if (res < min)
		return 0;

	/* 最大值及代理项检查（严格模式） */
	if (res > MAXUNICODE)
		return 0;
	if (0xD800 <= res && res <= 0xDFFF)
		return 0;

	*code = (int32_t)res;
	return nbytes;
}

int joy_core_utf8_next(const char* s, size_t len, size_t* pos, int32_t* code)
{
	if (*pos >= len) return 0;
	size_t consumed = joy_core_utf8_decode(s + *pos, len - *pos, code);
	if (consumed == 0) return 0;
	*pos += consumed;
	return 1;
}

size_t joy_core_utf8_decode_all(const char* s, size_t len, int32_t* out_codes, size_t out_capacity)
{
	size_t count = 0;
	size_t pos = 0;

	/* 第一次遍历：计算所需码点个数，同时验证字符串有效性 */
	while (pos < len) {
		int32_t cp;  /* 临时存储，无需实际值 */
		size_t consumed = joy_core_utf8_decode(s + pos, len - pos, &cp);
		if (consumed == 0) {
			return 0;  /* 无效 UTF-8 序列 */
		}
		count++;
		pos += consumed;
	}

	/* 如果 out_codes 为 NULL，则只返回所需长度（用于缓冲区预分配） */
	if (out_codes == NULL) {
		return count;
	}

	/* 检查缓冲区容量是否足够 */
	if (out_capacity < count) {
		return 0;  /* 缓冲区太小，失败 */
	}

	/* 第二次遍历：填充数组 */
	pos = 0;
	size_t idx = 0;
	while (pos < len) {
		int32_t cp;
		size_t consumed = joy_core_utf8_decode(s + pos, len - pos, &cp);
		/* 此时不会失败，因为第一次已验证过 */
		out_codes[idx++] = cp;
		pos += consumed;
	}

	return count;
}


int joy_core_pack_int8(char* buf, int8_t value, int offset)
{
	int type_size = sizeof(int8_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
int joy_core_pack_int16(char* buf, int16_t value, int offset)
{
	int type_size = sizeof(int16_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
int joy_core_pack_int32(char* buf, int32_t value, int offset)
{
	int type_size = sizeof(int32_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
int joy_core_pack_uint32(char* buf, uint32_t value, int offset)
{
	int type_size = sizeof(uint32_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
int joy_core_pack_int64(char* buf, int64_t value, int offset)
{
	int type_size = sizeof(int64_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}

int joy_core_unpack_int8(const char* buf, int8_t* value, int offset)
{
	int type_size = sizeof(int8_t);
	*value = *((int8_t*)(buf + offset));
	return offset + type_size;
}
int joy_core_unpack_int16(const char* buf, int16_t* value, int offset)
{
	int type_size = sizeof(int16_t);
	*value = *((int16_t*)(buf + offset));
	return offset + type_size;
}
int joy_core_unpack_int32(const char* buf, int32_t* value, int offset)
{
	int type_size = sizeof(int32_t);
	*value = *((int32_t*)(buf + offset));
	return offset + type_size;
}
int joy_core_unpack_uint32(const char* buf, uint32_t* value, int offset)
{
	int type_size = sizeof(uint32_t);
	*value = *((uint32_t*)(buf + offset));
	return offset + type_size;
}
int joy_core_unpack_int64(const char* buf, int64_t* value, int offset)
{
	int type_size = sizeof(int64_t);
	*value = *((int64_t*)(buf + offset));
	return offset + type_size;
}
int joy_core_pack_string(char* buf, const char* value, int type_size, int offset)
{
	memcpy(buf + offset, value, type_size);
	return offset + type_size;
}
int joy_core_unpack_string(const char* buf, char* value, int type_size, int offset)
{
	memcpy(value, buf + offset, type_size);
	return offset + type_size;
}



struct joy_timer {
	uint64_t last_time;        // 上次触发的时间点（SDL_GetTicks）
	int interval;              // 触发间隔（毫秒）
	bool active;               // 是否激活
	joy_timer_callback callback;   // 回调函数
	void* userdata;            // 用户数据
};

joy_timer_p joy_core_timer_create(int interval, joy_timer_callback cb, void* userdata)
{
	joy_timer_p jtimer = (joy_timer_p)malloc(sizeof(struct joy_timer));
	if (!jtimer) return NULL;

	jtimer->last_time = SDL_GetTicks();
	jtimer->interval = interval;
	jtimer->active = true;
	jtimer->callback = cb;
	jtimer->userdata = userdata;

	return jtimer;
}

void joy_core_timer_destroy(joy_timer_p jtimer)
{
	if (jtimer) {
		free(jtimer);
	}
}

void joy_core_timer_trigger(joy_timer_p jtimer)
{
	if (!jtimer || !jtimer->active || !jtimer->callback) {
		return;
	}

	uint64_t current_time = SDL_GetTicks();
	uint64_t elapsed = current_time - jtimer->last_time;

	if (elapsed >= (uint64_t)jtimer->interval) {
		int actual_interval = (int)elapsed;

		// 调用回调
		jtimer->callback(actual_interval, jtimer->interval, jtimer->userdata);

		// 更新最后触发时间（避免累积漂移）
		jtimer->last_time += jtimer->interval;

		// 如果落后太多，快速追赶（防止回调执行时间过长导致连续触发）
		while (current_time - jtimer->last_time >= (uint64_t)jtimer->interval) {
			jtimer->last_time += jtimer->interval;
		}
	}
}

void joy_core_timer_set_interval(joy_timer_p jtimer, int interval)
{
	if (jtimer) {
		jtimer->interval = interval;
	}
}

void joy_core_timer_set_active(joy_timer_p jtimer, bool active)
{
	if (jtimer) {
		jtimer->active = active;
		if (active) {
			jtimer->last_time = SDL_GetTicks(); // 重新激活时重置计时
		}
	}
}

void joy_core_timer_reset(joy_timer_p jtimer)
{
	if (jtimer) {
		jtimer->last_time = SDL_GetTicks();
	}
}

uint64_t joy_core_timer_get_elapsed(joy_timer_p jtimer)
{
	if (!jtimer) return 0;
	return SDL_GetTicks() - jtimer->last_time;
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef MT_N
#undef MT_M
#undef LCG_A
#undef LCG_C
#undef MAXUNICODE
#undef iscont

/* ==================== 结束 src/jutils.c ==================== */

/* ==================== 以下为原 input 域（已并回 core） ==================== */
/* ==================== 合并自 src/joy_input.c ==================== */
/* ============================================================
 * 引擎层输入状态快照（B 方案 P1/P2）
 *
 * 目标：输入消费方不必接触 SDL_Event。宿主把 SDL 事件翻译成标量喂进来，
 * 四端（C++ / Lua / JS / C#）用同一组纯标量 getter 查询当前状态。
 *
 * 语义：进程级单例输入流，与 joy 的单窗口模型一致。这里不提供句柄，
 * 也不假装支持多实例；若将来支持多窗口，再把状态挂到窗口对象上。
 *
 * 状态分两类：持续状态（is_key_down / mouse_x 等）与帧级边沿
 * （was_key_pressed / wheel），后者由 joy_core_input_end_frame 每帧清零。
 * 这些 API 全是标量（L0/L1），生成链可直接产出四端绑定；相比之下旧的
 * *_process_event(const void*) 因 void* 参数被判 status:off。
 * ============================================================ */
#include "joy.h"
#include <string.h>

#define JOY_INPUT_KEY_COUNT           512
#define JOY_INPUT_MOUSE_BUTTON_COUNT 8

static bool  s_keys[JOY_INPUT_KEY_COUNT];
static bool  s_key_pressed[JOY_INPUT_KEY_COUNT];
static bool  s_key_released[JOY_INPUT_KEY_COUNT];
static bool  s_mouse_buttons[JOY_INPUT_MOUSE_BUTTON_COUNT];
static float s_mouse_x;
static float s_mouse_y;
static float s_wheel;

void joy_core_input_feed_key(int scancode, bool down)
{
	if (scancode < 0 || scancode >= JOY_INPUT_KEY_COUNT) return;
	s_keys[scancode] = down;
	/* 边沿与持续状态一次喂入, 宿主不必再调第二个 API */
	if (down) s_key_pressed[scancode] = true;
	else      s_key_released[scancode] = true;
}

void joy_core_input_feed_mouse_move(float x, float y)
{
	s_mouse_x = x;
	s_mouse_y = y;
}

void joy_core_input_feed_mouse_button(int button, bool down)
{
	if (button >= 0 && button < JOY_INPUT_MOUSE_BUTTON_COUNT)
		s_mouse_buttons[button] = down;
}

void joy_core_input_feed_wheel(float dy)
{
	s_wheel += dy;
}

/* 每帧末清掉滚轮增量与按键边沿：它们是"本帧事件量"，不是持续状态。
 * 四端宿主都必须在 SDL_AppIterate 末尾调用一次。 */
void joy_core_input_end_frame(void)
{
	s_wheel = 0.0f;
	memset(s_key_pressed, 0, sizeof(s_key_pressed));
	memset(s_key_released, 0, sizeof(s_key_released));
}

void joy_core_input_reset(void)
{
	memset(s_keys, 0, sizeof(s_keys));
	memset(s_key_pressed, 0, sizeof(s_key_pressed));
	memset(s_key_released, 0, sizeof(s_key_released));
	memset(s_mouse_buttons, 0, sizeof(s_mouse_buttons));
	s_mouse_x = 0.0f;
	s_mouse_y = 0.0f;
	s_wheel = 0.0f;
}

bool joy_core_input_is_key_down(int scancode)
{
	return (scancode >= 0 && scancode < JOY_INPUT_KEY_COUNT)
		? s_keys[scancode] : false;
}

bool joy_core_input_was_key_pressed(int scancode)
{
	return (scancode >= 0 && scancode < JOY_INPUT_KEY_COUNT)
		? s_key_pressed[scancode] : false;
}

bool joy_core_input_was_key_released(int scancode)
{
	return (scancode >= 0 && scancode < JOY_INPUT_KEY_COUNT)
		? s_key_released[scancode] : false;
}

bool joy_core_input_is_mouse_button_down(int button)
{
	return (button >= 0 && button < JOY_INPUT_MOUSE_BUTTON_COUNT)
		? s_mouse_buttons[button] : false;
}

float joy_core_input_get_mouse_x(void) { return s_mouse_x; }
float joy_core_input_get_mouse_y(void) { return s_mouse_y; }
float joy_core_input_get_wheel(void)   { return s_wheel; }

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef JOY_INPUT_KEY_COUNT
#undef JOY_INPUT_MOUSE_BUTTON_COUNT

/* ==================== 结束 src/joy_input.c ==================== */
