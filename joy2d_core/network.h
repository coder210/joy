/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: network.h
Description: ÍøÂçÏà¹Ø
Author: ydlc
Version: 1.0
Date: 2021.4.23
History:
*************************************************/
#ifndef NETWORK_H
#define NETWORK_H
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

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

typedef struct kcpserver kcpserver_t, *kcpserver_p;
typedef struct kcpclient kcpclient_t, *kcpclient_p;

typedef struct tcpserver tcpserver_t, * tcpserver_p;
typedef struct tcpclient tcpclient_t, * tcpclient_p;

kcpserver_p kcpserver_create(const char* ip, int port);
int kcpserver_destroy(kcpserver_p ks);
void kcpserver_send(kcpserver_p ks, int conv, const char *data, int len);
void kcpserver_broadcast(kcpserver_p ks, const char *data, int len);
void kcpserver_offline(kcpserver_p ks, int conv);
void kcpserver_update(kcpserver_p ks);
bool kcpserver_poll_message(kcpserver_p ks, net_message_p msg);

kcpclient_p kcpclient_create(const char* ip, int port);
void kcpclient_destroy(kcpclient_p kc);
bool kcpclient_getconv(kcpclient_p kc, int* conv);
int kcpclient_send(kcpclient_p kc, const char* data, int len);
void kcpclient_update(kcpclient_p kc);
bool kcpclient_poll_message(kcpclient_p kc, net_message_p msg);


tcpserver_p tcpserver_create(const char* ip, int port);
int tcpserver_destroy(tcpserver_p tcpserver);
void tcpserver_send(tcpserver_p tcpserver, int conv, const char* data, int len);
void tcpserver_broadcast(tcpserver_p tcpserver, const char* data, int len);
void tcpserver_offline(tcpserver_p tcpserver, int conv);
void tcpserver_update(tcpserver_p tcpserver);
bool tcpserver_poll_message(tcpserver_p tcpserver, net_message_p msg);

tcpclient_p tcpclient_create(const char* ip, int port);
void tcpclient_destroy(tcpclient_p tcpclient);
bool tcpclient_getconv(tcpclient_p tcpclient, int* conv);
int tcpclient_send(tcpclient_p tcpclient, const char* data, int len);
void tcpclient_update(tcpclient_p tcpclient);
bool tcpclient_poll_message(tcpclient_p tcpclient, net_message_p msg);

#endif // !CORE_NETWORK_H
