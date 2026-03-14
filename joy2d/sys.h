/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: sys.h
Description:
Author: ydlc
Version: 1.0
Date: 2021.3.20
History:
*************************************************/
#ifndef SYS_H
#define SYS_H
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

        JOY_API bool sys_init_netenv(void);
        JOY_API bool sys_release_netenv(void);
        JOY_API int sys_anyaddr(void);
        JOY_API bool sys_closesocket(int64_t sockfd);
        JOY_API int sys_set_sock_sndtimeo(int64_t sockfd, int ms);
        JOY_API int sys_set_sock_rcvtimeo(int64_t sockfd, int ms);
        JOY_API int sys_set_sock_accpettimeo(int64_t sockfd, int ms);
        JOY_API int64_t sys_tcp();
        JOY_API int64_t sys_udp();
        JOY_API bool sys_bind(int64_t sockfd, const char* ip, int port);
        JOY_API bool sys_connect(int64_t sockfd, const char* ip, int port);
        JOY_API bool sys_listen(int64_t sockfd);
        JOY_API int64_t sys_accept(int64_t sockfd, char* ip, int* port);
        JOY_API int sys_send(int64_t sockfd, const char* buf, int len);
        JOY_API int sys_recv(int64_t sockfd, char* buf);
        JOY_API int sys_recvfrom(int64_t sockfd, char* buf, char* ip, int* port);
        JOY_API int sys_sendto(int64_t sockfd, const char* buf,
                int len, const char* ip, int port);
        JOY_API int64_t sys_current_time();
        JOY_API void sys_sleep(int ms);
        JOY_API void sys_create_iocp(int ms);

#ifdef __cplusplus
}
#endif

#endif // !SYS_H
