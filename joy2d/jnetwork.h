/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: network.h
Description: �������
Author: ydlc
Version: 1.0
Date: 2021.4.23
History:
*************************************************/
#ifndef NETWORK_H
#define NETWORK_H
#include <stdbool.h>
#include <stdint.h>
#include "jconfig.h"

typedef enum {
	NET_TYPE_CONNECTED,
	NET_TYPE_DISCONNECTED,
	NET_TYPE_MESSAGE,
} net_event_type_k;

typedef struct net_message {
	net_event_type_k type;
	int conv;
	char* data;
	int len;
}net_message_t, *net_message_p;

typedef void (*net_callback)(net_message_p msg, void* userdata);
typedef struct kcpserver kcpserver_t, *kcpserver_p;
typedef struct kcpclient kcpclient_t, *kcpclient_p;
typedef struct tcpserver tcpserver_t, * tcpserver_p;
typedef struct tcpclient tcpclient_t, * tcpclient_p;

#ifdef __cplusplus
extern "C" {
#endif

	// ==============================
	// KCP Server & Client
	// ==============================
	JOY_API kcpserver_p kcpserver_create(const char* ip, int port);
	JOY_API int kcpserver_destroy(kcpserver_p ks);
	JOY_API void kcpserver_send(kcpserver_p ks, int conv, const char *data, int len);
	JOY_API void kcpserver_broadcast(kcpserver_p ks, const char *data, int len);
	JOY_API void kcpserver_offline(kcpserver_p ks, int conv);
	JOY_API void kcpserver_update(kcpserver_p ks);
	JOY_API int kcpserver_connection_count(kcpserver_p ks);
	JOY_API bool kcpserver_poll_message(kcpserver_p ks, net_message_p msg);
	JOY_API void kcpserver_set_callback(kcpserver_p ks, net_callback cb, void* userdata);

	JOY_API kcpclient_p kcpclient_create(const char* ip, int port);
	JOY_API void kcpclient_destroy(kcpclient_p kc);
	JOY_API bool kcpclient_getconv(kcpclient_p kc, int* conv);
	JOY_API int kcpclient_send(kcpclient_p kc, const char* data, int len);
	JOY_API void kcpclient_update(kcpclient_p kc);
	JOY_API bool kcpclient_poll_message(kcpclient_p kc, net_message_p msg);
	JOY_API void kcpclient_set_callback(kcpclient_p kc, net_callback cb, void* userdata);

	JOY_API tcpserver_p tcpserver_create(const char* ip, int port);
	JOY_API int tcpserver_destroy(tcpserver_p tcpserver);
	JOY_API void tcpserver_send(tcpserver_p tcpserver, int conv, const char* data, int len);
	JOY_API void tcpserver_broadcast(tcpserver_p tcpserver, const char* data, int len);
	JOY_API void tcpserver_offline(tcpserver_p tcpserver, int conv);
	JOY_API void tcpserver_update(tcpserver_p tcpserver);
	JOY_API int tcpserver_connection_count(tcpserver_p tcpserver);
	JOY_API bool tcpserver_poll_message(tcpserver_p tcpserver, net_message_p msg);

	JOY_API tcpclient_p tcpclient_create(const char* ip, int port);
	JOY_API void tcpclient_destroy(tcpclient_p tcpclient);
	JOY_API bool tcpclient_getconv(tcpclient_p tcpclient, int* conv);
	JOY_API int tcpclient_send(tcpclient_p tcpclient, const char* data, int len);
	JOY_API void tcpclient_update(tcpclient_p tcpclient);
	JOY_API bool tcpclient_poll_message(tcpclient_p tcpclient, net_message_p msg);


	typedef struct wsnetserver wsnetserver_t, *wsnetserver_p;
	typedef struct wsnetclient wsnetclient_t, *wsnetclient_p;

	JOY_API wsnetserver_p wsnetserver_create(const char* ip, int port);
	JOY_API int wsnetserver_destroy(wsnetserver_p ws);
	JOY_API void wsnetserver_send(wsnetserver_p ws, int conv, const char* data, int len);
	JOY_API void wsnetserver_broadcast(wsnetserver_p ws, const char* data, int len);
	JOY_API void wsnetserver_offline(wsnetserver_p ws, int conv);
	JOY_API void wsnetserver_update(wsnetserver_p ws);
	JOY_API int wsnetserver_connection_count(wsnetserver_p ws);
	JOY_API bool wsnetserver_poll_message(wsnetserver_p ws, net_message_p msg);
	JOY_API void wsnetserver_set_callback(wsnetserver_p ws, net_callback cb, void* userdata);

	JOY_API wsnetclient_p wsnetclient_create(const char* url);
	JOY_API void wsnetclient_destroy(wsnetclient_p wc);
	JOY_API bool wsnetclient_getconv(wsnetclient_p wc, int* conv);
	JOY_API int wsnetclient_send(wsnetclient_p wc, const char* data, int len);
	JOY_API void wsnetclient_update(wsnetclient_p wc);
	JOY_API bool wsnetclient_poll_message(wsnetclient_p wc, net_message_p msg);
	JOY_API void wsnetclient_set_callback(wsnetclient_p wc, net_callback cb, void* userdata);

	// ==============================
	// Unified NetClient (封装 kcp/tcp/ws)
	// ==============================
	typedef enum {
		NET_CLIENT_TCP,
		NET_CLIENT_KCP,
		NET_CLIENT_WEBSOCKET, // WebSocket: emscripten用原生, 非emscripten用mongoose
	} net_client_type;

	typedef struct netclient netclient_t, *netclient_p;

	JOY_API netclient_p netclient_create(net_client_type type, const char* host, int port);
	JOY_API void netclient_destroy(netclient_p nc);
	JOY_API bool netclient_getconv(netclient_p nc, int* conv);
	JOY_API int netclient_send(netclient_p nc, const char* data, int len);
	JOY_API void netclient_update(netclient_p nc);
	JOY_API bool netclient_poll_message(netclient_p nc, net_message_p msg);
	JOY_API void netclient_set_callback(netclient_p nc, net_callback cb, void* userdata);
	JOY_API net_client_type netclient_get_type(netclient_p nc);

	// ==============================
	// Unified NetServer (封装 kcp/tcp/ws 服务器)
	// ==============================
	typedef enum {
		NET_SERVER_KCP,
		NET_SERVER_TCP,
		NET_SERVER_WEBSOCKET,
	} net_server_type;

	typedef struct netserver netserver_t, *netserver_p;

	JOY_API netserver_p netserver_create(net_server_type type, const char* ip, int port);
	JOY_API void netserver_destroy(netserver_p ns);
	JOY_API void netserver_send(netserver_p ns, int conv, const char* data, int len);
	JOY_API void netserver_broadcast(netserver_p ns, const char* data, int len);
	JOY_API void netserver_offline(netserver_p ns, int conv);
	JOY_API void netserver_update(netserver_p ns);
	JOY_API int netserver_connection_count(netserver_p ns);
	JOY_API bool netserver_poll_message(netserver_p ns, net_message_p msg);
	JOY_API void netserver_set_callback(netserver_p ns, net_callback cb, void* userdata);
	JOY_API net_server_type netserver_get_type(netserver_p ns);

#ifdef __cplusplus
}
#endif

#endif // !NETWORK_H
