/******************************************************************************
 * src/jsys.h —— sys 域声明头（jsys.c 的配套头）
 * 职责：跨平台 OS 原语的公开 API 声明 —— socket / 时间 / 崩溃处理 / 网卡枚举。
 * 边界：网络协议与服务器（含 IOCP）在 src/jnet.h；本头不放网络服务器 API。
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_SYS_IMPL_H
#define JOY_SYS_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        JOY_API bool joy_sys_init_netenv(void);
        JOY_API bool joy_sys_release_netenv(void);
        JOY_API int joy_sys_get_local_ips(char ips[][JOY_MAX_IP], int max_count);
        JOY_API int joy_sys_anyaddr(void);
        JOY_API bool joy_sys_closesocket(int64_t sockfd);
        JOY_API int joy_sys_set_sock_sndtimeo(int64_t sockfd, int ms);
        JOY_API int joy_sys_set_sock_rcvtimeo(int64_t sockfd, int ms);
        JOY_API int joy_sys_set_sock_accpettimeo(int64_t sockfd, int ms);
        /* UDP 广播 / 地址复用开关（局域网房间发现用）：成功返回 0 */
        JOY_API int joy_sys_set_sock_broadcast(int64_t sockfd);
        JOY_API int joy_sys_set_sock_reuseaddr(int64_t sockfd);
        JOY_API int64_t joy_sys_tcp();
        JOY_API int64_t joy_sys_udp();
        JOY_API bool joy_sys_bind(int64_t sockfd, const char* ip, int port);
        JOY_API bool joy_sys_connect(int64_t sockfd, const char* ip, int port);
        JOY_API bool joy_sys_listen(int64_t sockfd);
        JOY_API int64_t joy_sys_accept(int64_t sockfd, char* ip, int* port);
        JOY_API int joy_sys_send(int64_t sockfd, const char* buf, int len);
        JOY_API int joy_sys_recv(int64_t sockfd, char* buf);
        JOY_API int joy_sys_recvfrom(int64_t sockfd, char* buf, char* ip, int* port);
        JOY_API int joy_sys_sendto(int64_t sockfd, const char* buf,
                int len, const char* ip, int port);
        JOY_API int64_t joy_sys_current_time();
        JOY_API void joy_sys_sleep(int ms);
        JOY_API void joy_sys_install_crash_handlers();

#ifdef __cplusplus
}
#endif

#endif /* JOY_SYS_IMPL_H */
