#include "joy_thunk.h"

#include <stdio.h>
#include <string.h>

/* [临时诊断] Linux 托管侧第一次调用 native 就 rip=0x0，标记只到 OnInit:begin。
 * 在 C 侧打点，区分三种情况：没进到这里 / 进了但 fn 为空 / fn 非空仍崩。
 * 定位完成后删除本文件的 fprintf 与 fflush。 */
#define JOY_THUNK_PROBE(...) do { \
        fprintf(stderr, "[thunk] " __VA_ARGS__); \
        fputc('\n', stderr); \
        fflush(stderr); \
} while (0)

/* s_thunks 定义在生成物 thunks_generated.c 里，这里通过注册表间接访问，
 * 这样共享运行层不需要包含任何生成头文件。 */
joy_value_t joy_api_call(const char* name, joy_value_t* args, int argc, joy_status_t* st)
{
        int count = 0;
        int i;
        const joy_thunk_entry* table = joy_thunks_table(&count);
        if (st) *st = JOY_OK;
        if (!name || !table) return joy_v_err(st, JOY_ERR_TYPE, name);
        for (i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                        JOY_THUNK_PROBE("call %s argc=%d fn=%p", name, argc, (void*)table[i].fn);
                        return table[i].fn(args, argc, st);
                }
        }
        JOY_THUNK_PROBE("MISS %s (table count=%d)", name, count);
        return joy_v_err(st, JOY_ERR_TYPE, name);
}
