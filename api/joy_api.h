/* ============================================================
 * joy 共享绑定 API：对外唯一入口（P1）
 *
 * 宿主在冒烟模式下调用 joy_api_selftest() 验证值容器 / 句柄 /
 * 类型校验 / 自动释放链路。返回 NULL 表示通过，否则是失败的段落名。
 * ============================================================ */
/* guard 不能叫 JOY_API_H：那是 include/joy/api.h（公共 API 主头）的 guard，
 * 先包含主头的 TU 会把本头整体跳过。C 里只表现为隐式声明警告，C++ 直接
 * 报「找不到标识符 joy_api_selftest」。 */
#ifndef JOY_API_BINDINGS_H
#define JOY_API_BINDINGS_H

#ifdef __cplusplus
extern "C" {
#endif

const char* joy_api_selftest(void);

#ifdef __cplusplus
}
#endif
#endif /* JOY_API_BINDINGS_H */
