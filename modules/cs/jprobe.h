/* Android 启动阶段弹窗探针（诊断用，仅 __ANDROID__ 生效）。
 *
 * 闪退时没有 logcat，用 SDL_ShowSimpleMessageBox 把启动序列每一步显示出来：
 * 最后弹出的那个编号就是崩溃点。每步弹一次，点 OK 继续。
 * 桌面构建（非 Android）下 JPROBE 完全编译为空，零开销零影响。
 */
#ifndef JPROBE_H
#define JPROBE_H

#include <SDL3/SDL_messagebox.h>

#if defined(__ANDROID__)

/* step: 阶段描述，如 "3 csx_init_android: before coreclr_initialize"。
 * SDL_ShowSimpleMessageBox 的 window 传 NULL 在 Android 上会弹一个系统对话框。 */
static inline void jprobe(const char* step)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                             "JOY BOOT PROBE", step, NULL);
}
#define JPROBE(step) jprobe(step)

#else
#define JPROBE(step) ((void)0)
#endif

#endif /* JPROBE_H */
