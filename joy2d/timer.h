#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 定时器回调函数类型
	// actual_interval: 实际经过的时间（ms）
	// interval: 设定的间隔（ms）
	// userdata: 用户自定义数据
	typedef void (*timer_callback)(int actual_interval, int interval, void* userdata);

	// 定时器对象（不透明指针）
	typedef struct timer xtimer_t, *timer_p;

	/**
	 * 创建定时器
	 * @param interval 触发间隔（毫秒）
	 * @param cb       回调函数（为 NULL 时不触发）
	 * @param userdata 用户数据，透传给回调
	 * @return 定时器指针，失败返回 NULL
	 */
	JOY_API timer_p joy_timer_create(int interval, timer_callback cb, void* userdata);

	/**
	 * 销毁定时器
	 * @param timer 定时器指针
	 */
	JOY_API void joy_timer_destroy(timer_p timer);

	/**
	 * 触发检查（通常放在主循环中每帧调用）
	 * 内部使用 SDL_GetTicks() 获取当前时间，若达到间隔则调用回调
	 * @param timer 定时器指针
	 */
	JOY_API void joy_timer_trigger(timer_p timer);

	/**
	 * 设置定时器间隔
	 * @param timer    定时器指针
	 * @param interval 新间隔（毫秒）
	 */
	JOY_API void joy_timer_set_interval(timer_p timer, int interval);

	/**
	 * 设置激活状态
	 * @param timer  定时器指针
	 * @param active true 激活，false 暂停
	 */
	JOY_API void joy_timer_set_active(timer_p timer, bool active);

	/**
	 * 重置定时器（将上次触发时间设为当前时间）
	 * @param timer 定时器指针
	 */
	JOY_API void timer_reset(timer_p timer);

	/**
	 * 获取自上次触发以来经过的时间（毫秒）
	 * @param timer 定时器指针
	 * @return 经过的毫秒数
	 */
	JOY_API uint64_t timer_get_elapsed(timer_p timer);

#ifdef __cplusplus
}
#endif

#endif // TIMER_H