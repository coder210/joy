/* ============================================================
 * 引擎层输入状态快照（B 方案 P1）
 *
 * 目标：输入消费方不必接触 SDL_Event。宿主把 SDL 事件翻译成标量喂进来，
 * 四端（C++ / Lua / JS / C#）用同一组纯标量 getter 查询当前状态。
 *
 * 语义：进程级单例输入流，与 joy 的单窗口模型一致。这里不提供句柄，
 * 也不假装支持多实例；若将来支持多窗口，再把状态挂到窗口对象上。
 *
 * 这些 API 全是标量（L0/L1），生成链可直接产出四端绑定；相比之下旧的
 * *_process_event(const void*) 因 void* 参数被判 status:off，脚本端拿不到。
 * ============================================================ */
#include "joy.h"

#define JOY_INPUT_KEY_COUNT           512
#define JOY_INPUT_MOUSE_BUTTON_COUNT 8

static bool  s_keys[JOY_INPUT_KEY_COUNT];
static bool  s_mouse_buttons[JOY_INPUT_MOUSE_BUTTON_COUNT];
static float s_mouse_x;
static float s_mouse_y;
static float s_wheel;

void joy_input_feed_key(int scancode, bool down)
{
	if (scancode >= 0 && scancode < JOY_INPUT_KEY_COUNT)
		s_keys[scancode] = down;
}

void joy_input_feed_mouse_move(float x, float y)
{
	s_mouse_x = x;
	s_mouse_y = y;
}

void joy_input_feed_mouse_button(int button, bool down)
{
	if (button >= 0 && button < JOY_INPUT_MOUSE_BUTTON_COUNT)
		s_mouse_buttons[button] = down;
}

void joy_input_feed_wheel(float dy)
{
	s_wheel += dy;
}

/* 每帧末清掉滚轮增量：滚轮是"本帧事件量"，不是可持续查询的状态。 */
void joy_input_end_frame(void)
{
	s_wheel = 0.0f;
}

void joy_input_reset(void)
{
	SDL_memset(s_keys, 0, sizeof(s_keys));
	SDL_memset(s_mouse_buttons, 0, sizeof(s_mouse_buttons));
	s_mouse_x = 0.0f;
	s_mouse_y = 0.0f;
	s_wheel = 0.0f;
}

bool joy_input_is_key_down(int scancode)
{
	return (scancode >= 0 && scancode < JOY_INPUT_KEY_COUNT)
		? s_keys[scancode] : false;
}

bool joy_input_is_mouse_button_down(int button)
{
	return (button >= 0 && button < JOY_INPUT_MOUSE_BUTTON_COUNT)
		? s_mouse_buttons[button] : false;
}

float joy_input_get_mouse_x(void) { return s_mouse_x; }
float joy_input_get_mouse_y(void) { return s_mouse_y; }
float joy_input_get_wheel(void)   { return s_wheel; }
