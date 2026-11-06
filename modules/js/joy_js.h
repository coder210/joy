#ifndef JOY_JS_H
#define JOY_JS_H

/*
 * joy_js —— joy 的可选 JavaScript(duktape) 脚本后端（独立仓库）。
 *
 * 从 joy 核心拆出：duktape 引擎 + jsclib 绑定 + jsx 运行时宿主。
 * 只有需要 JS 脚本驱动引擎的工程才链接本模块；joy 核心不依赖它、也不带 duktape。
 *
 * 依赖：joy 公共 API（<src/joy.h>）、SDL3、msgpack（jsclib 部分用到）。
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jsx jsx_t, * jsx_p;

/* 创建 / 销毁 JS 运行时 */
jsx_p jsx_create(void);
void  jsx_release(jsx_p jsx);

/* 加载 / 热重载脚本 */
bool  jsx_dofile(jsx_p jsx, const char* filename);
bool  jsx_reload(jsx_p jsx, const char* filename);

/* 事件分发与 joy.* 回调 */
void  jsx_dispatch_event(jsx_p jsx, const void* event);
bool  jsx_call_joy(jsx_p jsx, const char* func, int argc, float dt);

/* 文件监听 */
void        jsx_set_watch(jsx_p jsx, const char* filename);
bool        jsx_poll_reload(jsx_p jsx);
const char* jsx_get_watch(jsx_p jsx);

/* 透传命令行参数给脚本 */
void  jsx_set_args(jsx_p jsx, int argc, char* argv[]);

/* IDL 驱动的新绑定层：在全局注册 Joy.Api.Bindings.*（方法名与 cs / lua 一致）。
 * 参数是 duk_context*，用 void* 以免头文件依赖 duktape。 */
void        jsopen_api(void* duk_context);
/* 自检：建独立 duktape heap 跑内联脚本；成功返回 NULL，失败返回原因 */
const char* jsopen_api_selftest(void);

#ifdef __cplusplus
}
#endif
#endif /* JOY_JS_H */
