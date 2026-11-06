/* ============================================================
 * joy 共享绑定 API：对外唯一入口（P1）
 *
 * 宿主在冒烟模式下调用 joy_api_selftest() 验证值容器 / 句柄 /
 * 类型校验 / 自动释放链路。返回 NULL 表示通过，否则是失败的段落名。
 * ============================================================ */
#ifndef JOY_API_H
#define JOY_API_H

#ifdef __cplusplus
extern "C" {
#endif

const char* joy_api_selftest(void);

#ifdef __cplusplus
}
#endif
#endif /* JOY_API_H */
