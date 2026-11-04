#ifndef JOY_LUA_H
#define JOY_LUA_H

/*
 * joy_lua —— joy 的可选 Lua 脚本后端（独立仓库）。
 *
 * 从 joy 核心拆出：Lua VM + luaclib 绑定 + lua_cjson + luax 宿主。
 * 只有需要 Lua 脚本驱动引擎的工程才链接本模块；joy 核心不依赖它、也不带 Lua。
 *
 * 依赖：joy 公共 API（<src/joy.h>）、SDL3、msgpack（部分绑定用到）。
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct luax luax_t, * luax_p;

/* 创建 / 销毁 Lua 运行时 */
luax_p luax_create(void);
void   luax_release(luax_p luax);

/* 加载 / 热重载脚本 */
bool luax_dofile(luax_p luax, const char* filename);
bool luax_reload(luax_p luax, const char* filename);

/* 底层 lua_State（需要脚本层强交互时使用） */
void* luax_get_state(luax_p luax);

/* 事件分发与 joy.* 回调 */
bool luax_call_joy(luax_p luax, const char* func, int argc, float dt);
void luax_dispatch_event(luax_p luax, const void* event);

/* 文件监听 */
void        luax_set_watch(luax_p luax, const char* filename);
bool        luax_poll_reload(luax_p luax);
const char* luax_get_watch(luax_p luax);

/* 透传命令行参数给脚本 */
void luax_set_args(luax_p luax, int argc, char* argv[]);

#ifdef __cplusplus
}
#endif
#endif /* JOY_LUA_H */
