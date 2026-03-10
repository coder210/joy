#include "timer.h"
#include <stdlib.h>
#include <SDL3/SDL.h>

struct timer {
        uint64_t last_time;        // 上次触发的时间点（SDL_GetTicks）
        int interval;              // 触发间隔（毫秒）
        bool active;               // 是否激活
        timer_callback callback;   // 回调函数
        void* userdata;            // 用户数据
};

timer_p timer_create(int interval, timer_callback cb, void* userdata)
{
        timer_p timer = (timer_p)malloc(sizeof(struct timer));
        if (!timer) return NULL;

        timer->last_time = SDL_GetTicks();
        timer->interval = interval;
        timer->active = true;
        timer->callback = cb;
        timer->userdata = userdata;

        return timer;
}

void timer_destroy(timer_p timer)
{
        if (timer) {
                free(timer);
        }
}

void timer_trigger(timer_p timer)
{
        if (!timer || !timer->active || !timer->callback) {
                return;
        }

        uint64_t current_time = SDL_GetTicks();
        uint64_t elapsed = current_time - timer->last_time;

        if (elapsed >= (uint64_t)timer->interval) {
                int actual_interval = (int)elapsed;

                // 调用回调
                timer->callback(actual_interval, timer->interval, timer->userdata);

                // 更新最后触发时间（避免累积漂移）
                timer->last_time += timer->interval;

                // 如果落后太多，快速追赶（防止回调执行时间过长导致连续触发）
                while (current_time - timer->last_time >= (uint64_t)timer->interval) {
                        timer->last_time += timer->interval;
                }
        }
}

void timer_set_interval(timer_p timer, int interval)
{
        if (timer) {
                timer->interval = interval;
        }
}

void timer_set_active(timer_p timer, bool active)
{
        if (timer) {
                timer->active = active;
                if (active) {
                        timer->last_time = SDL_GetTicks(); // 重新激活时重置计时
                }
        }
}

void timer_reset(timer_p timer)
{
        if (timer) {
                timer->last_time = SDL_GetTicks();
        }
}

uint64_t timer_get_elapsed(timer_p timer)
{
        if (!timer) return 0;
        return SDL_GetTicks() - timer->last_time;
}