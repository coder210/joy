#include <string.h>
#include "external/klib/khash.h"
#include "external/klib/klist.h"
#include "external/kcp.h"
#include "liv-config.h"
#include "liv-utils.h"
#include "liv-sys.h"
#include "liv-net.h"
#include "liv-log.h"

typedef struct kcp_connection {
	ikcpcb* kcp;
	char ip[LIVNET_MAX_IP];
	int port;
	int timeout;
	delay_t updating_delay;
}kcp_connection_t, * kcp_connection_p;


KHASH_INIT(kconn, int, kcp_connection_p, 1, kh_int_hash_func, kh_int_hash_equal)
KLIST_INIT(kmq, net_message_t, free)

typedef struct kcpserver {
	uint64_t sockfd;
	int conv;
	khash_t(kconn)* conns;
	klist_t(kmq)* mq;
}kcpserver_t, * kcpserver_p;


typedef struct kcpclient {
	ikcpcb* kcp;
	uint64_t sockfd;
	char server_ip[LIVNET_MAX_IP];
	int server_port;
	short timeout;
	delay_t updating_delay;
	delay_t connection_delay;
	klist_t(kmq)* mq;
}kcpclient_t, * kcpclient_p;


LIVNET_INLINE uint32_t iclock(uint64_t clock64)
{
	return (uint32_t)(clock64 & 0xfffffffful);
}

kcpserver_p
liv_ks_create(const char* ip, int port)
{
	kcpserver_p ks;
	ks = (kcpserver_p)SDL_malloc(sizeof(kcpserver_t));
	SDL_assert(ks);
	ks->sockfd = sys_udp();
	ks->conv = 1000;
	ks->conns = kh_init(kconn);
	ks->mq = kl_init(kmq);
	if (!sys_bind(ks->sockfd, ip, port)){
		liv_log_info("bind error");
	}
	else{
		liv_log_info("bind successful");
	}
	sys_set_sock_rcvtimeo(ks->sockfd, 1);
	liv_log_info("ip:%s,port=%d", ip, port);
	liv_log_info("sockfd:%d", ks->sockfd);
	return ks;
}

int liv_ks_destroy(kcpserver_p ks)
{
	sys_closesocket(ks->sockfd);
	kh_destroy(kconn, ks->conns);
	SDL_free(ks);
	return 1;
}

static int
liv_ks_output(const char* data, int len, ikcpcb* kcp, void* user)
{
	IUINT32 conv;
	kcpserver_p ks;
	kcp_connection_p conn;
	khint_t k;
	ks = (kcpserver_p)user;
	conv = ikcp_getconv(kcp);
	k = kh_get(kconn, ks->conns, conv);
	if (k != kh_end(ks->conns)) {
		conn = kh_val(ks->conns, k);
		return sys_sendto(ks->sockfd, data, len, conn->ip, conn->port);
	}
	return -1;
}

static void
ks_input(kcpserver_p ks, const char* data, int len,
	const char* ip, int port)
{
	char buf[4];
	int conv, ret;
	khint_t k;
	kcp_connection_p conn;
	net_message_t msg;

	conv = utils_bit2int((uint8_t*)data);
	k = kh_get(kconn, ks->conns, conv);
	if (k == kh_end(ks->conns)) {
		for (k = kh_begin(ks->conns); k != kh_end(ks->conns); k++) {
			if (kh_exist(ks->conns, k)) {
				conn = kh_val(ks->conns, k);
				if (conn && strcmp(conn->ip, ip) == 0 && conn->port == port) {
					return;
				}
			}
		}

		conn = (kcp_connection_p)SDL_malloc(sizeof(kcp_connection_t));
		assert(conn);
		conn->kcp = ikcp_create(ks->conv++, ks);
		ikcp_wndsize(conn->kcp, 512, 512);
		ikcp_nodelay(conn->kcp, 1, 20, 2, 1);
		ikcp_setoutput(conn->kcp, liv_ks_output);
		strcpy(conn->ip, ip);
		conn->port = port;
		conn->timeout = 120;
		conn->updating_delay.time = sys_current_time();
		conn->updating_delay.timeout = 1000;

		conv = ikcp_getconv(conn->kcp);
		k = kh_put(kconn, ks->conns, conv, &ret);
		kh_val(ks->conns, k) = conn;

		utils_int2bit((uint8_t*)buf, conv);
		sys_sendto(ks->sockfd, buf, 4, conn->ip, conn->port);
		msg.type = NET_TYPE_CONNECTED;
		msg.data = SDL_strdup("connected");
		msg.len = SDL_strlen(msg.data);
		msg.conv = conv;
		*kl_pushp(kmq, ks->mq) = msg;
		//liv_log_debug("connected conv=%d\n", conv);
	}
	else {
		conn = kh_val(ks->conns, k);
		if (conn && strcmp(conn->ip, ip) == 0 && conn->port == port) {
			conn->timeout = 120;
			ikcp_input(conn->kcp, data, len);
		}
	}
}

void liv_ks_send(kcpserver_p ks, int conv, const char* data, int len)
{
	khint_t k;
	kcp_connection_p conn;
	k = kh_get(kconn, ks->conns, conv);
	if (k != kh_end(ks->conns)) {
		conn = kh_val(ks->conns, k);
		ikcp_send(conn->kcp, data, len);
	}
}

void liv_ks_broadcast(kcpserver_p ks, const char* data, int len)
{
	khint_t k;
	kcp_connection_p conn;
	k = kh_begin(ks->conns);
	while (k != kh_end(ks->conns)) {
		if (kh_exist(ks->conns, k)) {
			conn = kh_val(ks->conns, k);
			ikcp_send(conn->kcp, data, len);
		}
		k++;
	}
}

void liv_ks_offline(kcpserver_p ks, int conv)
{
	khint_t k;
	kcp_connection_p conn;
	k = kh_get(kconn, ks->conns, conv);
	if (k != kh_end(ks->conns)) {
		conn = kh_val(ks->conns, k);
		conn->timeout = -1;
	}
}

void liv_ks_update(kcpserver_p ks)
{
	uint64_t current_time;
	int len, port, conv;
	char buf[LIVNET_MAX_BUFFER], ip[LIVNET_MAX_IP];
	khint_t p;
	kcp_connection_p conn;
	net_message_t msg;

	current_time = sys_current_time();
	len = sys_recvfrom(ks->sockfd, buf, ip, &port);
	if (len > 0) {
		ks_input(ks, buf, len, ip, port);
	}

	for (p = kh_begin(ks->conns); p != kh_end(ks->conns); p++) {
		if (kh_exist(ks->conns, p)) {
			conv = kh_key(ks->conns, p);
			conn = kh_val(ks->conns, p);
			ikcp_update(conn->kcp, iclock(current_time));
			len = ikcp_recv(conn->kcp, buf, LIVNET_MAX_BUFFER);
			if (len > 0) {
				msg.type = NET_TYPE_MESSAGE;
				msg.len = len;
				msg.data = (char*)SDL_malloc(msg.len);
				SDL_memcpy(msg.data, buf, msg.len);
				msg.conv = ikcp_getconv(conn->kcp);
				*kl_pushp(kmq, ks->mq) = msg;
			}
		}
	}

	for (p = kh_begin(ks->conns); p != kh_end(ks->conns); p++) {
		if (kh_exist(ks->conns, p)) {
			conv = kh_key(ks->conns, p);
			conn = kh_val(ks->conns, p);
			if (conn->timeout < 0) {
				msg.type = NET_TYPE_DISCONNECTED;
				msg.data = SDL_strdup("disconnected");
				msg.len = SDL_strlen(msg.data);
				msg.conv = ikcp_getconv(conn->kcp);
				*kl_pushp(kmq, ks->mq) = msg;
				ikcp_release(conn->kcp);
				kh_del(kconn, ks->conns, p);
			}
			else if (utils_wait_delay(&conn->updating_delay, current_time)) {
				conn->timeout--;
			}
		}
	}
}

bool liv_ks_poll_message(kcpserver_p ks, net_message_p msg)
{
	kliter_t(kmq)* p;
	p = kl_begin(ks->mq);
	if (p != kl_end(ks->mq)) {
		*msg = kl_val(p);
		kl_shift(kmq, ks->mq, 0);
		return true;
	}
	else {
		return false;
	}
}



kcpclient_p liv_kc_create(const char* ip, int port)
{
	kcpclient_p kc;
	kc = (kcpclient_p)SDL_malloc(sizeof(kcpclient_t));
	SDL_assert(kc);
	kc->kcp = NULL;
	kc->sockfd = sys_udp();
	strcpy(kc->server_ip, ip);
	kc->server_port = port;
	kc->updating_delay.time = sys_current_time();
	kc->updating_delay.timeout = 1000;
	kc->connection_delay.time = sys_current_time();
	kc->connection_delay.timeout = 3000;
	kc->timeout = 1200;
	kc->mq = kl_init(kmq);
	sys_set_sock_rcvtimeo(kc->sockfd, 1);
	return kc;
}

void liv_kc_destroy(kcpclient_p kc)
{
	sys_closesocket(kc->sockfd);
	if (kc->kcp)
		ikcp_release(kc->kcp);
	SDL_free(kc);
}

bool liv_kc_getconv(kcpclient_p kc, int* conv)
{
	if (kc->kcp) {
		*conv = ikcp_getconv(kc->kcp);
		return true;
	}
	else {
		*conv = -1;
		return false;
	}
}

static int
liv_kc_output(const char* data, int size, ikcpcb *kcp, void* user)
{
	int n;
	kcpclient_p kc;
	kc = (kcpclient_p)user;
	n = sys_sendto(kc->sockfd, data, size, kc->server_ip, kc->server_port);
	return n;
}

static void
liv_kc_input(kcpclient_p kc, const char* data, int sz)
{
	net_message_t msg;
	int conv;
	if (kc->kcp) {
		if (sz > 4) {
			int len = ikcp_input(kc->kcp, data, sz);
			kc->timeout = 1200;
		}
	}
	else {
		liv_log_debug("liv-kc-input create\n");
		conv = utils_bit2int((uint8_t*)data);
		kc->kcp = ikcp_create(conv, kc);
		ikcp_wndsize(kc->kcp, 512, 512);
		ikcp_nodelay(kc->kcp, 1, 20, 2, 1);
		ikcp_setoutput(kc->kcp, liv_kc_output);
		msg.conv = conv;
		msg.data = SDL_strdup("connected");
		msg.len = SDL_strlen(msg.data);
		msg.type = NET_TYPE_CONNECTED;
		*kl_pushp(kmq, kc->mq) = msg;
	}
}

int liv_kc_send(kcpclient_p kc, const char* data, int len)
{
	if (kc->kcp) {
		return ikcp_send(kc->kcp, data, len);
	}
	return -1;
}

void liv_kc_update(kcpclient_p kc)
{
	int64_t current_time;
	net_message_t msg;
	char data[4] = { 0 };
	char buf[LIVNET_MAX_BUFFER];
	char ip[LIVNET_MAX_IP];
	int port, len;

	current_time = sys_current_time();
	len = sys_recvfrom(kc->sockfd, buf, ip, &port);
	if (len > 0) {
		liv_kc_input(kc, buf, len);
	}

	if (kc->kcp) {
		if (kc->timeout < 0) {
			return;
		}
		if (utils_wait_delay(&kc->updating_delay, current_time)) {
			kc->timeout--;
		}
		if (kc->timeout == 0) {
			msg.type = NET_TYPE_DISCONNECTED;
			msg.conv = 0;
			msg.len = 9;
			msg.data = SDL_strdup("connected");
			*kl_pushp(kmq, kc->mq) = msg;
			return;
		}
		ikcp_update(kc->kcp, ikcp_check(kc->kcp, iclock(current_time)));
		len = ikcp_recv(kc->kcp, buf, LIVNET_MAX_BUFFER);
		if (len > 0) {
			msg.type = NET_TYPE_MESSAGE;
			msg.conv = ikcp_getconv(kc->kcp);
			msg.len = len;
			msg.data = (char*)SDL_malloc(msg.len);
			SDL_memcpy(msg.data, buf, msg.len);
			*kl_pushp(kmq, kc->mq) = msg;
		}
	}
	else if (utils_wait_delay(&kc->connection_delay, current_time)) {
		data[0] = data[1] = data[2] = data[3] = 0;
		sys_sendto(kc->sockfd, data, 4, kc->server_ip, kc->server_port);
	}
}

bool liv_kc_poll_message(kcpclient_p kc, net_message_p msg)
{
	kliter_t(kmq)* p;
	p = kl_begin(kc->mq);
	if (p != kl_end(kc->mq)) {
		*msg = kl_val(p);
		kl_shift(kmq, kc->mq, 0);
		return true;
	}
	else {
		return false;
	}
}
