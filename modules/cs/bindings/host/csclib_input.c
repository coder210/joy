/* C# → joy 胶水层：输入状态桥。
 *
 * 宿主(main.c)持有 SDL 事件循环，把键鼠状态写进引擎层输入快照；C# 侧
 * 通过 Joy.Input 的 internal call 轮询同一份状态。这样 C# 不需要理解
 * SDL_Event，也不用再自己维护第二份 s_keys/s_mouse_*（B 方案 P1）。
 *
 * 注册名格式：Joy.<类>::<方法>，与 C# 侧 [MethodImpl(InternalCall)] 对应。
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <joy/joy.h>
#include "joy_csharp.h"

/* ---------------- 宿主调用：转发到引擎层统一状态 ---------------- */

void csx_input_key(int scancode, bool down)
{
        joy_core_input_feed_key(scancode, down);
}

void csx_input_mouse_move(float x, float y)
{
        joy_core_input_feed_mouse_move(x, y);
}

void csx_input_mouse_button(int button, bool down)
{
        joy_core_input_feed_mouse_button(button, down);
}

void csx_input_wheel(float dy) { joy_core_input_feed_wheel(dy); }

void csx_input_end_frame(void) { joy_core_input_end_frame(); }

/* ---------------- C# 调用 ---------------- */

static bool w_key_down(int scancode)
{
        return joy_core_input_is_key_down(scancode);
}

static bool w_mouse_button(int button)
{
        return joy_core_input_is_mouse_button_down(button);
}

static float w_mouse_x(void) { return joy_core_input_get_mouse_x(); }
static float w_mouse_y(void) { return joy_core_input_get_mouse_y(); }

void csopen_input(void)
{
        csx_register_internal_call("Joy.Input::key_down", (const void*)w_key_down);
        csx_register_internal_call("Joy.Input::mouse_button", (const void*)w_mouse_button);
        csx_register_internal_call("Joy.Input::mouse_x", (const void*)w_mouse_x);
        csx_register_internal_call("Joy.Input::mouse_y", (const void*)w_mouse_y);
}
