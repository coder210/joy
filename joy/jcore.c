#include "jcore.h"
#include <SDL3/SDL.h>

struct env {
	SDL_Environment* sdl_env;
};


typedef enum log_color {
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
} log_color_k;

struct simple_fps_counter {
	int frame_count;
	int fps;
	Uint64 last_time;
	Uint64 current_time;
};

int log_info(const char* format, ...)
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

int log_debug(const char* format, ...)
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

int log_error(const char* format, ...)
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

void log_title(int type)
{
	if (type == 1) {
		log_info("=======================================================");
		log_info("     ##   #####   ##   ##   #######   ####   ");
		log_info("     ##  ##   ##  ##   ##  ##   ##   ##  ##  ");
		log_info("     ##  ##   ##   ## ##       ##    ##   ## ");
		log_info(" ##  ##  ##   ##    ###       ##     ##  ##  ");
		log_info("  ####    #####     ###    ######     ####   ");
		log_info("=======================================================");
	}
	else if (type == 2) {
		log_info("---------------------------------------------------");
		log_info("                    J O Y 2 D                      ");
		log_info("                Game Engine v2.0                   ");
		log_info("---------------------------------------------------");
	}
	else if (type == 3) {
		log_info("-------------------------------------------------------");
		log_info("  JJJJ    OOOOO    Y   Y   22222    DDDDD             ");
		log_info("    J    O     O    Y Y        2    D    D            ");
		log_info("    J    O     O     Y      222     D     D           ");
		log_info("J   J    O     O     Y     2        D    D            ");
		log_info(" JJJ      OOOOO      Y    2222222   DDDDD             ");
		log_info("-------------------------------------------------------");
	}
	else {
		log_info("joy");
	}
}

env_p env_create(void)
{
	env_p env;
	env = SDL_malloc(sizeof(env_t));
	SDL_assert(env);
	env->sdl_env = SDL_CreateEnvironment(true);
	SDL_assert(env->sdl_env);
	return env;
}

const char* env_get(env_p env, const char* key)
{
	return SDL_GetEnvironmentVariable(env->sdl_env, key);
}

void env_set(env_p env, const char* key, const char* value)
{
	SDL_SetEnvironmentVariable(env->sdl_env, key, value, true);
}

void env_destroy(env_p env)
{
	SDL_assert(env);
	SDL_DestroyEnvironment(env->sdl_env);
	SDL_free(env);
}

simple_fps_counter_p simple_fps_create()
{
	simple_fps_counter_p fps;
	fps = (simple_fps_counter_p)SDL_malloc(sizeof(simple_fps_counter_t));
	fps->fps = fps->frame_count = 0;
	fps->last_time = SDL_GetTicks();
	return fps;
}

void simple_fps_destory(simple_fps_counter_p fps)
{
	SDL_free(fps);
}

void simple_fps_update(simple_fps_counter_p fps)
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

int simple_fps_value(simple_fps_counter_p fps)
{
	return fps->fps;
}

static void limit_frame_rate(game_timer_p timer, float current_delta_time)
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

void game_timer_init(game_timer_p timer)
{
	if (!timer) return;
	Uint64 now = SDL_GetTicksNS();
	timer->last_time = now;
	timer->frame_start_time = now;
	timer->delta_time = 0.0;
	timer->time_scale = 1.0;
	timer->target_fps = 0;
	timer->target_frame_time = 0.0;
	log_info("[Gametimer] Initialized. Last time: %llu ns", (unsigned long long)now);
}

void game_timer_update(game_timer_p timer)
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



float game_timer_get_delta_time(const game_timer_p timer)
{
	if (!timer) return 0.0f;
	return (float)(timer->delta_time * timer->time_scale);
}

float game_timer_get_unscaled_delta_time(const game_timer_p timer)
{
	if (!timer) return 0.0f;
	return (float)timer->delta_time;
}

void game_timer_set_time_scale(game_timer_p timer, float scale)
{
	if (!timer) return;
	if (scale < 0.0f) {
		log_info("[Gametimer] Warning: Time scale cannot be negative. Clamping to 0.");
		scale = 0.0f;
	}
	timer->time_scale = (double)scale;
}

float game_timer_get_time_scale(const game_timer_p timer)
{
	if (!timer) return 1.0f;
	return (float)timer->time_scale;
}

void game_timer_set_target_fps(game_timer_p timer, int fps)
{
	if (!timer) return;

	if (fps < 0) {
		log_info("[Gametimer] Warning: Target FPS cannot be negative. Setting to 0 (unlimited).");
		timer->target_fps = 0;
	}
	else {
		timer->target_fps = fps;
	}

	if (timer->target_fps > 0) {
		timer->target_frame_time = 1.0 / (double)timer->target_fps;
		log_info("[Gametimer] Target FPS set to: %d (Frame time: %.6f s)",
			timer->target_fps, timer->target_frame_time);
	}
	else {
		timer->target_frame_time = 0.0;
		log_info("[Gametimer] Target FPS set to: Unlimited");
	}
}

int game_timer_get_target_fps(const game_timer_p timer)
{
	if (!timer) return 0;
	return timer->target_fps;
}

