/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-net.h
Description: ÍøÂçÏà¹Ø
Author: ydlc
Version: 1.0
Date: 2021.4.23
History:
*************************************************/
#ifndef LIVNET_NET_H
#define LIVNET_NET_H
#include <stdbool.h>
#include <stdint.h>
#include "liv-config.h"

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


kcpserver_p liv_ks_create(const char* ip, int port);
int liv_ks_destroy(kcpserver_p ks);
void liv_ks_send(kcpserver_p ks, int conv, const char *data, int len);
void liv_ks_broadcast(kcpserver_p ks, const char *data, int len);
void liv_ks_offline(kcpserver_p ks, int conv);
void liv_ks_update(kcpserver_p ks);
bool liv_ks_poll_message(kcpserver_p ks, net_message_p msg);


kcpclient_p liv_kc_create(const char* ip, int port);
void liv_kc_destroy(kcpclient_p kc);
bool liv_kc_getconv(kcpclient_p kc, int* conv);
int liv_kc_send(kcpclient_p kc, const char* data, int len);
void liv_kc_update(kcpclient_p kc);
bool liv_kc_poll_message(kcpclient_p kc, net_message_p msg);


#endif // !LIVNET_NET_H
