/* Android 启动阶段弹窗探针（诊断用）。
 *
 * 默认全部关闭：JPROBE 在任何平台都编译为空，Android 不再弹 "JOY BOOT PROBE"。
 * 需要临时排查启动闪退时，在编译期定义 JOY_ENABLE_BOOT_PROBE 即可重新打开；
 * 此时用 SDL_ShowSimpleMessageBox 把启动序列每一步显示出来，最后弹出的编号即崩溃点。
 */
#ifndef JPROBE_H
#define JPROBE_H

#if defined(__ANDROID__) && defined(JOY_ENABLE_BOOT_PROBE)

#include <SDL3/SDL_messagebox.h>

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
