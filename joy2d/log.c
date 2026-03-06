#include <stdarg.h>
#include "config.h"
#include "log.h"
#include <SDL3/SDL.h>

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
