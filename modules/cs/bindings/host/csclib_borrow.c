/* C# → joy 桥：宿主自建对象的「借用句柄」。
 *
 * 生成链路（Joy.Api.Bindings）只认 joy_obj 句柄表里的条目，而窗口 / 渲染器
 * 由 C 宿主创建（examples/csharp_host/src/main.cpp 调
 * joy_core_create_window + joy_r2d_create），脚本层通过 Host.OnInit
 * 拿到的只是裸地址，不在表里。
 *
 * 这里按 C 类型名把宿主对象登记成借用句柄（dtor 为 NULL）：
 *   - 脚本层可以拿它调绘制类 API（joy_r2d_set_colorf /
 *     joy_r2d_shape_draw_rectangle …）；
 *   - 引用计数归零时不会释放底层对象，所有权仍在宿主手里。
 * 一个宿主对象只应登记一次（在 Host.OnInit 里），否则每帧都会占用一个槽位。
 */
#include <stdint.h>
#include <mono/metadata/object.h>
#include "joy_csharp.h"
#include <joy/joy.h>
#include <joy/api/runtime/joy_obj.h>

/* tag 必须与 thunk 里的类型名一致（见 api/generated/thunks_generated.c 的
 * joy_obj_deref(..., "<tag>", ...)）；joy_obj 只保存指针，故只能传字符串字面量。 */
static uint64_t borrow(intptr_t p, const char* tag)
{
        if (!p) return 0;
        return joy_obj_register((void*)p, tag, NULL, "host.borrow");
}

static uint64_t w_borrow_r2d(intptr_t p)    { return borrow(p, "jr2d_p"); }
static uint64_t w_borrow_window(intptr_t p) { return borrow(p, "jwindow_p"); }
static uint64_t w_borrow_font(intptr_t p)   { return borrow(p, "font_p"); }
static uint64_t w_borrow_image(intptr_t p)  { return borrow(p, "image_p"); }

void csopen_borrow(void)
{
        csx_register_internal_call("Joy.Api.HostHandles::borrow_r2d", (const void*)w_borrow_r2d);
        csx_register_internal_call("Joy.Api.HostHandles::borrow_window", (const void*)w_borrow_window);
        csx_register_internal_call("Joy.Api.HostHandles::borrow_font", (const void*)w_borrow_font);
        csx_register_internal_call("Joy.Api.HostHandles::borrow_image", (const void*)w_borrow_image);
}
