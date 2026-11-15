/******************************************************************************
 * src/joy_net.h —— net 域声明头（joy_net.c 的配套头）
 * 职责：网络协议与服务器（KCP / TCP / WebSocket / IOCP）
 * 边界：跨平台 OS 原语（socket / 时间 / 崩溃 / 网卡枚举）已拆到 src/joy_sys.h。
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_NET_IMPL_H
#define JOY_NET_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Windows IOCP 服务器：网络服务器实现，归 net 域（非跨平台 OS 原语）。 */

        JOY_API joy_iocp_server_p joy_net_iocp_server_create(const char* bind_ip, int bind_port);
        JOY_API void joy_net_iocp_server_destroy(joy_iocp_server_p server);
        JOY_API void joy_net_iocp_process_events(joy_iocp_server_p server, int timeout_ms);

        // ==============================
        // KCP Server & Client
        // ==============================
        JOY_API joy_kcpserver_p joy_net_kcpserver_create(const char* ip, int port);
        JOY_API int joy_net_kcpserver_destroy(joy_kcpserver_p ks);
        JOY_API void joy_net_kcpserver_send(joy_kcpserver_p ks, int conv, const char* data, int len);
        JOY_API void joy_net_kcpserver_broadcast(joy_kcpserver_p ks, const char* data, int len);
        JOY_API void joy_net_kcpserver_offline(joy_kcpserver_p ks, int conv);
        JOY_API void joy_net_kcpserver_update(joy_kcpserver_p ks);
        JOY_API int joy_net_kcpserver_connection_count(joy_kcpserver_p ks);
        JOY_API bool joy_net_kcpserver_poll_message(joy_kcpserver_p ks, joy_net_message_p msg);
        JOY_API void joy_net_kcpserver_set_callback(joy_kcpserver_p ks, joy_net_callback cb, void* userdata);
        JOY_API joy_kcpclient_p joy_net_kcpclient_create(const char* ip, int port);
        JOY_API void joy_net_kcpclient_destroy(joy_kcpclient_p kc);
        JOY_API int joy_net_kcpclient_send(joy_kcpclient_p kc, const char* data, int len);
        JOY_API void joy_net_kcpclient_update(joy_kcpclient_p kc);
        JOY_API bool joy_net_kcpclient_poll_message(joy_kcpclient_p kc, joy_net_message_p msg);
        JOY_API void joy_net_kcpclient_set_callback(joy_kcpclient_p kc, joy_net_callback cb, void* userdata);
        JOY_API joy_tcpserver_p joy_net_tcpserver_create(const char* ip, int port);
        JOY_API int joy_net_tcpserver_destroy(joy_tcpserver_p tcpserver);
        JOY_API void joy_net_tcpserver_send(joy_tcpserver_p tcpserver, int conv, const char* data, int len);
        JOY_API void joy_net_tcpserver_broadcast(joy_tcpserver_p tcpserver, const char* data, int len);
        JOY_API void joy_net_tcpserver_offline(joy_tcpserver_p tcpserver, int conv);
        JOY_API void joy_net_tcpserver_update(joy_tcpserver_p tcpserver);
        JOY_API int joy_net_tcpserver_connection_count(joy_tcpserver_p tcpserver);
        JOY_API bool joy_net_tcpserver_poll_message(joy_tcpserver_p tcpserver, joy_net_message_p msg);
        JOY_API joy_tcpclient_p joy_net_tcpclient_create(const char* ip, int port);
        JOY_API void joy_net_tcpclient_destroy(joy_tcpclient_p tcpclient);
        JOY_API int joy_net_tcpclient_send(joy_tcpclient_p tcpclient, const char* data, int len);
        JOY_API void joy_net_tcpclient_update(joy_tcpclient_p tcpclient);
        JOY_API bool joy_net_tcpclient_poll_message(joy_tcpclient_p tcpclient, joy_net_message_p msg);
        JOY_API joy_wsnetserver_p joy_net_wsnetserver_create(const char* ip, int port);
        JOY_API int joy_net_wsnetserver_destroy(joy_wsnetserver_p ws);
        JOY_API void joy_net_wsnetserver_send(joy_wsnetserver_p ws, int conv, const char* data, int len);
        JOY_API void joy_net_wsnetserver_broadcast(joy_wsnetserver_p ws, const char* data, int len);
        JOY_API void joy_net_wsnetserver_offline(joy_wsnetserver_p ws, int conv);
        JOY_API void joy_net_wsnetserver_update(joy_wsnetserver_p ws);
        JOY_API int joy_net_wsnetserver_connection_count(joy_wsnetserver_p ws);
        JOY_API bool joy_net_wsnetserver_poll_message(joy_wsnetserver_p ws, joy_net_message_p msg);
        JOY_API void joy_net_wsnetserver_set_callback(joy_wsnetserver_p ws, joy_net_callback cb, void* userdata);
        JOY_API joy_wsnetclient_p joy_net_wsnetclient_create(const char* url);
        JOY_API void joy_net_wsnetclient_destroy(joy_wsnetclient_p wc);
        JOY_API int joy_net_wsnetclient_send(joy_wsnetclient_p wc, const char* data, int len);
        JOY_API void joy_net_wsnetclient_update(joy_wsnetclient_p wc);
        JOY_API bool joy_net_wsnetclient_poll_message(joy_wsnetclient_p wc, joy_net_message_p msg);
        JOY_API void joy_net_wsnetclient_set_callback(joy_wsnetclient_p wc, joy_net_callback cb, void* userdata);
        JOY_API joy_netclient_p joy_net_netclient_create(joy_net_client_type type, const char* host, int port);
        JOY_API void joy_net_netclient_destroy(joy_netclient_p nc);
        JOY_API int joy_net_netclient_send(joy_netclient_p nc, const char* data, int len);
        JOY_API void joy_net_netclient_update(joy_netclient_p nc);
        JOY_API bool joy_net_netclient_poll_message(joy_netclient_p nc, joy_net_message_p msg);
        JOY_API void joy_net_netclient_set_callback(joy_netclient_p nc, joy_net_callback cb, void* userdata);
        JOY_API joy_net_client_type joy_net_netclient_get_type(joy_netclient_p nc);
        JOY_API joy_netserver_p joy_net_netserver_create(joy_net_server_type type, const char* ip, int port);
        JOY_API void joy_net_netserver_destroy(joy_netserver_p ns);
        JOY_API void joy_net_netserver_send(joy_netserver_p ns, int conv, const char* data, int len);
        JOY_API void joy_net_netserver_broadcast(joy_netserver_p ns, const char* data, int len);
        JOY_API void joy_net_netserver_offline(joy_netserver_p ns, int conv);
        JOY_API void joy_net_netserver_update(joy_netserver_p ns);
        JOY_API int joy_net_netserver_connection_count(joy_netserver_p ns);
        JOY_API bool joy_net_netserver_poll_message(joy_netserver_p ns, joy_net_message_p msg);
        JOY_API void joy_net_netserver_set_callback(joy_netserver_p ns, joy_net_callback cb, void* userdata);
        JOY_API joy_net_server_type joy_net_netserver_get_type(joy_netserver_p ns);
        JOY_API void joy_net_netmessage_free_data(joy_net_message_p msg);

#ifdef __cplusplus
}
#endif

#endif /* JOY_NET_IMPL_H */
