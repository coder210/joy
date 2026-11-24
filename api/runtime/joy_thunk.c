#include "joy_thunk.h"

#include <string.h>

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
                        return table[i].fn(args, argc, st);
                }
        }
        return joy_v_err(st, JOY_ERR_TYPE, name);
}
