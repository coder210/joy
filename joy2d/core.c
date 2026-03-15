#include "core.h"
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

env_p env_create(void)
{
	env_p env;
	env = SDL_malloc(sizeof(env_t));
	SDL_assert(env);
	env->sdl_env = SDL_CreateEnvironment(true);
	SDL_assert(env->sdl_env);
	return env;
}

const char *env_get(env_p env, const char *key)
{
	return SDL_GetEnvironmentVariable(env->sdl_env, key);
}

void env_set(env_p env, const char *key, const char *value)
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

int simple_fps_update(simple_fps_counter_p fps)
{
        fps->frame_count++;
        fps->current_time = SDL_GetTicks();

        Uint64 elapsed = fps->current_time - fps->last_time;

        if (elapsed >= 1000) {
                fps->fps = fps->frame_count;
                fps->frame_count = 0;
                fps->last_time = fps->current_time;
        }

        return fps->fps;
}
