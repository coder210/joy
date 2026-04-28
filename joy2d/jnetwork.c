#include <string.h>
#include <SDL3/SDL.h>
#include "external/klib/khash.h"
#include "external/klib/klist.h"
#include "external/kcp.h"
#include "jconfig.h"
#include "jutils.h"
#include "jsys.h"
#include "jcore.h"
#include "jnetwork.h"

#ifdef JOY_WIN
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/websocket.h>
#endif

typedef struct kcp_connection
{
        ikcpcb *kcp;
        char ip[JOY_MAX_IP];
        int port;
        int timeout;
        delay_t updating_delay;
} kcp_connection_t, *kcp_connection_p;

typedef struct tcp_connection
{
        int64_t sockfd;
        char ip[JOY_MAX_IP];
        int port;
        int timeout;
        delay_t updating_delay;
} tcp_connection_t, *tcp_connection_p;

KHASH_INIT(kconn, int, kcp_connection_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(ktcp_conn, int, tcp_connection_p, 1, kh_int_hash_func, kh_int_hash_equal)

KLIST_INIT(kmq, net_message_t, free)

struct kcpserver
{
        int64_t sockfd;
        int conv;
        khash_t(kconn) * conns;
        net_callback cb;
        klist_t(kmq) * mq;
        void* userdata;
};

struct kcpclient
{
        ikcpcb *kcp;
        int64_t sockfd;
        char server_ip[JOY_MAX_IP];
        int server_port;
        short timeout;
        delay_t updating_delay;
        delay_t connection_delay;
        net_callback cb;
        klist_t(kmq) * mq;
        void* userdata;
};

struct tcpserver
{
        int64_t sockfd;
        int conv;
        khash_t(ktcp_conn) * conns;
        klist_t(kmq) * mq;
};

struct tcpclient
{
        int64_t sockfd;
        bool connected;
        char server_ip[JOY_MAX_IP];
        int server_port;
        short timeout;
        delay_t updating_delay;
        delay_t connection_delay;
        klist_t(kmq) * mq;
};

JOY_INLINE uint32_t iclock(uint64_t clock64)
{
        return (uint32_t)(clock64 & 0xfffffffful);
}

// ==============================
// 服务端
// ==============================
kcpserver_p
kcpserver_create(const char* ip, int port)
{
        kcpserver_p ks;
        ks = (kcpserver_p)SDL_malloc(sizeof(kcpserver_t));
        SDL_assert(ks);
        SDL_memset(ks, 0, sizeof(kcpserver_t));
        ks->sockfd = sys_udp();
        ks->conv = 1000;
        ks->conns = kh_init(kconn);
        ks->mq = kl_init(kmq);
        ks->cb = NULL;
        if (!sys_bind(ks->sockfd, ip, port))
        {
                log_info("bind error");
        }
        else
        {
                log_info("bind successful");
        }
        sys_set_sock_rcvtimeo(ks->sockfd, 10);
        log_info("ip:%s,port=%d", ip, port);
        log_info("sockfd:%d", ks->sockfd);
        return ks;
}

int kcpserver_destroy(kcpserver_p ks)
{
        if (!ks) return 0;

        for (khint_t p = kh_begin(ks->conns); p != kh_end(ks->conns); p++)
        {
                if (kh_exist(ks->conns, p))
                {
                        kcp_connection_p conn = kh_val(ks->conns, p);
                        if (conn)
                        {
                                if (conn->kcp) ikcp_release(conn->kcp);
                                SDL_free(conn);
                        }
                }
        }

        net_message_t msg;
        while (kcpserver_poll_message(ks, &msg))
        {
                if (msg.data) SDL_free(msg.data);
        }

        sys_closesocket(ks->sockfd);
        kh_destroy(kconn, ks->conns);
        //kl_destroy(kmq, ks->mq);
        SDL_free(ks);
        return 1;
}

static int
kcpserver_output(const char* data, int len, ikcpcb* kcp, void* user)
{
        IUINT32 conv;
        kcpserver_p ks;
        kcp_connection_p conn;
        khint_t k;
        ks = (kcpserver_p)user;
        conv = ikcp_getconv(kcp);
        k = kh_get(kconn, ks->conns, conv);
        if (k != kh_end(ks->conns))
        {
                conn = kh_val(ks->conns, k);
                return sys_sendto(ks->sockfd, data, len, conn->ip, conn->port);
        }
        return -1;
}

static void
kcpserver_input(kcpserver_p ks, const char* data, int len,
        const char* ip, int port)
{
        char buf[4];
        int conv, ret;
        khint_t k;
        kcp_connection_p conn;
        net_message_t msg;

        conv = utils_bit2int((uint8_t*)data);
        k = kh_get(kconn, ks->conns, conv);

        if (k != kh_end(ks->conns))
        {
                conn = kh_val(ks->conns, k);
                if (conn && strcmp(conn->ip, ip) == 0 && conn->port == port)
                {
                        conn->timeout = 120;
                        ikcp_input(conn->kcp, data, len);
                }
                return;
        }

        for (k = kh_begin(ks->conns); k != kh_end(ks->conns); k++)
        {
                if (kh_exist(ks->conns, k))
                {
                        conn = kh_val(ks->conns, k);
                        if (conn && strcmp(conn->ip, ip) == 0 && conn->port == port)
                        {
                                conn->timeout = 120;
                                ikcp_input(conn->kcp, data, len);
                                return;
                        }
                }
        }

        conn = (kcp_connection_p)SDL_malloc(sizeof(kcp_connection_t));
        assert(conn);
        memset(conn, 0, sizeof(kcp_connection_t));

        conn->kcp = ikcp_create(ks->conv++, ks);
        ikcp_wndsize(conn->kcp, 512, 512);
        ikcp_nodelay(conn->kcp, 0, 40, 0, 0);
        ikcp_setoutput(conn->kcp, kcpserver_output);

        strcpy(conn->ip, ip);
        conn->port = port;
        conn->timeout = 120;
        conn->updating_delay.time = sys_current_time();
        conn->updating_delay.timeout = 1000;

        conv = ikcp_getconv(conn->kcp);
        k = kh_put(kconn, ks->conns, conv, &ret);
        if (ret == -1)
        {
                log_error("kh_put failed");
                ikcp_release(conn->kcp);
                SDL_free(conn);
                return;
        }
        kh_val(ks->conns, k) = conn;

        utils_int2bit((uint8_t*)buf, conv);
        sys_sendto(ks->sockfd, buf, 4, conn->ip, conn->port);

        msg.type = NET_TYPE_CONNECTED;
        msg.data = SDL_strdup("connected");
        msg.len = SDL_strlen(msg.data);
        msg.conv = conv;

        if (ks->cb) {
                ks->cb(&msg, ks->userdata);
        }
        else {
                *kl_pushp(kmq, ks->mq) = msg;
        }
}

void kcpserver_send(kcpserver_p ks, int conv, const char* data, int len)
{
        khint_t k;
        kcp_connection_p conn;
        k = kh_get(kconn, ks->conns, conv);
        if (k != kh_end(ks->conns))
        {
                conn = kh_val(ks->conns, k);
                ikcp_send(conn->kcp, data, len);
        }
}

void kcpserver_broadcast(kcpserver_p ks, const char* data, int len)
{
        khint_t k;
        kcp_connection_p conn;
        k = kh_begin(ks->conns);
        while (k != kh_end(ks->conns))
        {
                if (kh_exist(ks->conns, k))
                {
                        conn = kh_val(ks->conns, k);
                        ikcp_send(conn->kcp, data, len);
                }
                k++;
        }
}

void kcpserver_offline(kcpserver_p ks, int conv)
{
        khint_t k;
        kcp_connection_p conn;
        k = kh_get(kconn, ks->conns, conv);
        if (k != kh_end(ks->conns))
        {
                conn = kh_val(ks->conns, k);
                conn->timeout = -1;
        }
}

void kcpserver_update(kcpserver_p ks)
{
        uint64_t current_time;
        int len, port;
        char buf[JOY_MAX_BUFFER], ip[JOY_MAX_IP];
        net_message_t msg;

        current_time = sys_current_time();
        len = sys_recvfrom(ks->sockfd, buf, ip, &port);
        if (len > 0)
        {
                kcpserver_input(ks, buf, len, ip, port);
        }

        khint_t* del_list = SDL_malloc(kh_size(ks->conns) * sizeof(khint_t));
        int del_cnt = 0;

        for (khint_t p = kh_begin(ks->conns); p != kh_end(ks->conns); p++)
        {
                if (!kh_exist(ks->conns, p)) continue;
                kcp_connection_p conn = kh_val(ks->conns, p);

                if (conn->timeout < 0)
                {
                        del_list[del_cnt++] = p;
                        continue;
                }

                if (utils_wait_delay(&conn->updating_delay, current_time))
                {
                        conn->timeout -= 1;
                }

                ikcp_update(conn->kcp, ikcp_check(conn->kcp, current_time));
                len = ikcp_recv(conn->kcp, buf, JOY_MAX_BUFFER - 1);
                if (len > 0)
                {
                        buf[len] = 0;
                        msg.type = NET_TYPE_MESSAGE;
                        msg.len = len;
                        msg.data = SDL_malloc(msg.len);
                        SDL_memcpy(msg.data, buf, msg.len);
                        msg.conv = ikcp_getconv(conn->kcp);

                        if (ks->cb) {
                                ks->cb(&msg, ks->userdata);
                        }
                        else {
                                *kl_pushp(kmq, ks->mq) = msg;
                        }
                }
        }

        for (int i = 0; i < del_cnt; i++)
        {
                khint_t p = del_list[i];
                kcp_connection_p conn = kh_val(ks->conns, p);

                msg.type = NET_TYPE_DISCONNECTED;
                msg.data = SDL_strdup("disconnected");
                msg.len = SDL_strlen(msg.data);
                msg.conv = ikcp_getconv(conn->kcp);

                if (ks->cb) {
                        ks->cb(&msg, ks->userdata);
                }
                else {
                        *kl_pushp(kmq, ks->mq) = msg;
                }

                ikcp_release(conn->kcp);
                SDL_free(conn);
                kh_del(kconn, ks->conns, p);
        }
        SDL_free(del_list);
}

int kcpserver_connection_count(kcpserver_p ks)
{
        return kh_size(ks->conns);
}

bool kcpserver_poll_message(kcpserver_p ks, net_message_p msg)
{
        if (!ks || !msg) return false;
        kliter_t(kmq)* p;
        p = kl_begin(ks->mq);
        if (p != kl_end(ks->mq))
        {
                *msg = kl_val(p);
                kl_shift(kmq, ks->mq, 0);
                return true;
        }
        else
        {
                return false;
        }
}

void kcpserver_set_callback(kcpserver_p ks, net_callback cb, void* userdata)
{
        ks->cb = cb;
        ks->userdata = userdata;
}

// ==============================
// 客户端
// ==============================
kcpclient_p kcpclient_create(const char* ip, int port)
{
        kcpclient_p kc;
        kc = (kcpclient_p)SDL_malloc(sizeof(kcpclient_t));
        SDL_assert(kc);
        memset(kc, 0, sizeof(kcpclient_t)); // 【修复】初始化全0
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
        kc->cb = NULL;
        sys_set_sock_rcvtimeo(kc->sockfd, 10);
        return kc;
}

void kcpclient_destroy(kcpclient_p kc)
{
        if (!kc) return;
        sys_closesocket(kc->sockfd);
        if (kc->kcp) {
                ikcp_release(kc->kcp);
                kc->kcp = NULL;
        }
        net_message_t msg;
        while (kcpclient_poll_message(kc, &msg)) {
                if (msg.data) SDL_free(msg.data);
        }
        //kl_destroy(kmq, kc->mq);
        SDL_free(kc);
}

bool kcpclient_getconv(kcpclient_p kc, int* conv)
{
        if (kc->kcp)
        {
                *conv = ikcp_getconv(kc->kcp);
                return true;
        }
        else
        {
                *conv = -1;
                return false;
        }
}

static int
kcpclient_output(const char* data, int size, ikcpcb* kcp, void* user)
{
        int n;
        kcpclient_p kc;
        kc = (kcpclient_p)user;
        n = sys_sendto(kc->sockfd, data, size, kc->server_ip, kc->server_port);
        return n;
}

static void
kcpclient_input(kcpclient_p kc, const char* data, int sz)
{
        net_message_t msg;
        int conv;
        if (kc->kcp)
        {
                if (sz >= 4)
                {
                        ikcp_input(kc->kcp, data, sz);
                        kc->timeout = 1200;
                }
        }
        else
        {
                // 【修复】必须判断长度=4，防止野包创建连接
                if (sz != 4) return;

                log_debug("kcpclient create");
                conv = utils_bit2int((uint8_t*)data);
                kc->kcp = ikcp_create(conv, kc);
                ikcp_wndsize(kc->kcp, 512, 512);
                ikcp_nodelay(kc->kcp, 0, 40, 0, 0);
                ikcp_setoutput(kc->kcp, kcpclient_output);
                msg.conv = conv;
                msg.data = SDL_strdup("connected");
                msg.len = SDL_strlen(msg.data);
                msg.type = NET_TYPE_CONNECTED;
                if (kc->cb)
                {
                        kc->cb(&msg, kc->userdata);
                }
                else
                {
                        *kl_pushp(kmq, kc->mq) = msg;
                }
        }
}

int kcpclient_send(kcpclient_p kc, const char* data, int len)
{
        if (kc->kcp)
        {
                return ikcp_send(kc->kcp, data, len);
        }
        return -1;
}

void kcpclient_update(kcpclient_p kc)
{
        int64_t current_time;
        net_message_t msg;
        char data[4] = { 0 };
        char buf[JOY_MAX_BUFFER];
        char ip[JOY_MAX_IP];
        int port, len;

        current_time = sys_current_time();
        len = sys_recvfrom(kc->sockfd, buf, ip, &port);

        if (len > 0) {
                kcpclient_input(kc, buf, len);
        }

        if (kc->kcp)
        {
                if (kc->timeout <= 0)
                {
                        return;
                }
                if (utils_wait_delay(&kc->updating_delay, current_time))
                {
                        kc->timeout--;
                }
                if (kc->timeout == 0)
                {
                        // 【修复】断开后释放kcp
                        ikcp_release(kc->kcp);
                        kc->kcp = NULL;

                        msg.type = NET_TYPE_DISCONNECTED;
                        msg.conv = 0;
                        msg.len = 11;
                        msg.data = SDL_strdup("disconnected");
                        if (kc->cb)
                        {
                                kc->cb(&msg, kc->userdata);
                        }
                        else
                        {
                                *kl_pushp(kmq, kc->mq) = msg;
                        }
                        return;
                }
                ikcp_update(kc->kcp, ikcp_check(kc->kcp, iclock(current_time)));
                len = ikcp_recv(kc->kcp, buf, JOY_MAX_BUFFER);
                if (len > 0) {
                        msg.type = NET_TYPE_MESSAGE;
                        msg.conv = ikcp_getconv(kc->kcp);
                        msg.len = len;
                        msg.data = (char*)SDL_malloc(msg.len);
                        SDL_memcpy(msg.data, buf, msg.len);

                        if (kc->cb) {
                                kc->cb(&msg, kc->userdata);
                        }
                        else {
                                *kl_pushp(kmq, kc->mq) = msg;
                        }
                }
        }
        else if (utils_wait_delay(&kc->connection_delay, current_time))
        {
                data[0] = data[1] = data[2] = data[3] = 0;
                sys_sendto(kc->sockfd, data, 4, kc->server_ip, kc->server_port);
                kc->connection_delay.time = current_time;
        }
}

bool kcpclient_poll_message(kcpclient_p kc, net_message_p msg)
{
        if (!kc || !msg) return false;
        kliter_t(kmq)* p;
        p = kl_begin(kc->mq);
        if (p != kl_end(kc->mq))
        {
                *msg = kl_val(p);
                kl_shift(kmq, kc->mq, 0);
                return true;
        }
        else
        {
                return false;
        }
}

void kcpclient_set_callback(kcpclient_p kc, net_callback cb, void* userdata)
{
        kc->cb = cb;
        kc->userdata = userdata;
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////tcp//////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

tcpserver_p
tcpserver_create(const char *ip, int port)
{
        tcpserver_p tcpserver;
        tcpserver = (tcpserver_p)SDL_malloc(sizeof(tcpserver_t));
        SDL_assert(tcpserver);
        tcpserver->sockfd = sys_tcp();
        tcpserver->conv = 1000;
        tcpserver->conns = kh_init(ktcp_conn);
        tcpserver->mq = kl_init(kmq);
        sys_set_sock_accpettimeo(tcpserver->sockfd, 1);
        if (!sys_bind(tcpserver->sockfd, ip, port))
        {
                log_info("bind error");
        }
        else
        {
                log_info("bind successful");
        }

        if (!sys_listen(tcpserver->sockfd))
        {
                log_info("listen error");
        }
        else
        {
                log_info("listen successful");
        }
        log_info("ip:%s,port=%d", ip, port);
        log_info("sockfd:%d", tcpserver->sockfd);
        return tcpserver;
}

int tcpserver_destroy(tcpserver_p tcpserver)
{
        if (!tcpserver)
                return 0;

        // �ͷ���������
        khint_t k;
        for (k = kh_begin(tcpserver->conns); k != kh_end(tcpserver->conns); ++k)
        {
                if (kh_exist(tcpserver->conns, k))
                {
                        tcp_connection_p conn = kh_val(tcpserver->conns, k);
                        sys_closesocket(conn->sockfd);
                        SDL_free(conn);
                }
        }

        // �ͷ���Ϣ�����е���Ϣ
        kliter_t(kmq) * p;
        for (p = kl_begin(tcpserver->mq);
             p != kl_end(tcpserver->mq); p = kl_next(p))
        {
                net_message_t *msg = &kl_val(p);
                if (msg->data)
                        SDL_free(msg->data);
        }
        kl_destroy(kmq, tcpserver->mq);

        // �ͷ�������Դ
        kh_destroy(ktcp_conn, tcpserver->conns);
        sys_closesocket(tcpserver->sockfd);
        SDL_free(tcpserver);
        return 1;
}

static void
tcpserver_accpet(tcpserver_p tcpserver, int32_t sockfd, const char *ip, int port)
{
        int ret, conv;
        khint_t k;
        tcp_connection_p conn;
        net_message_t msg;

        conn = (tcp_connection_p)SDL_malloc(sizeof(tcp_connection_t));
        assert(conn);
        conn->sockfd = sockfd;
        conn->port = port;
        conn->timeout = 120;
        conn->updating_delay.time = sys_current_time();
        conn->updating_delay.timeout = 1000;
        conv = tcpserver->conv++;
        k = kh_put(ktcp_conn, tcpserver->conns, conv, &ret);
        kh_val(tcpserver->conns, k) = conn;

        msg.type = NET_TYPE_CONNECTED;
        msg.data = SDL_strdup("connected");
        msg.len = SDL_strlen(msg.data);
        msg.conv = conv;
        *kl_pushp(kmq, tcpserver->mq) = msg;
        log_debug("connected conv=%d\n", conv);
}

void tcpserver_send(tcpserver_p tcpserver, int conv, const char *data, int len)
{
        khint_t k;
        tcp_connection_p conn;
        k = kh_get(ktcp_conn, tcpserver->conns, conv);
        if (k != kh_end(tcpserver->conns))
        {
                conn = kh_val(tcpserver->conns, k);
                sys_send(conn->sockfd, data, len);
        }
}

void tcpserver_broadcast(tcpserver_p tcpserver, const char *data, int len)
{
        khint_t k;
        tcp_connection_p conn;
        k = kh_begin(tcpserver->conns);
        while (k != kh_end(tcpserver->conns))
        {
                if (kh_exist(tcpserver->conns, k))
                {
                        conn = kh_val(tcpserver->conns, k);
                        sys_send(conn->sockfd, data, len);
                }
                k++;
        }
}

void tcpserver_offline(tcpserver_p tcpserver, int conv)
{
        khint_t k;
        tcp_connection_p conn;
        k = kh_get(ktcp_conn, tcpserver->conns, conv);
        if (k != kh_end(tcpserver->conns))
        {
                conn = kh_val(tcpserver->conns, k);
                conn->timeout = -1;
        }
}

void tcpserver_update(tcpserver_p tcpserver)
{
        uint64_t current_time;
        int64_t sockfd;
        int len, port, conv;
        char buf[JOY_MAX_BUFFER], ip[JOY_MAX_IP];
        khint_t p;
        tcp_connection_p conn;
        net_message_t msg;

        current_time = sys_current_time();
        sockfd = sys_accept(tcpserver->sockfd, ip, &port);
        if (sockfd > 0)
        {
                tcpserver_accpet(tcpserver, sockfd, ip, port);
        }

        for (p = kh_begin(tcpserver->conns); p != kh_end(tcpserver->conns); p++)
        {
                if (kh_exist(tcpserver->conns, p))
                {
                        conv = kh_key(tcpserver->conns, p);
                        conn = kh_val(tcpserver->conns, p);
                        len = sys_recv(conn->sockfd, buf);
                        if (len > 0)
                        {
                                msg.type = NET_TYPE_MESSAGE;
                                msg.len = len;
                                msg.data = (char *)SDL_malloc(msg.len);
                                SDL_memcpy(msg.data, buf, msg.len);
                                msg.conv = conv;
                                *kl_pushp(kmq, tcpserver->mq) = msg;
                        }
                }
        }

        for (p = kh_begin(tcpserver->conns); p != kh_end(tcpserver->conns); p++)
        {
                if (kh_exist(tcpserver->conns, p))
                {
                        conv = kh_key(tcpserver->conns, p);
                        conn = kh_val(tcpserver->conns, p);
                        if (conn->timeout < 0)
                        {
                                msg.type = NET_TYPE_DISCONNECTED;
                                msg.data = SDL_strdup("disconnected");
                                msg.len = SDL_strlen(msg.data);
                                msg.conv = conv;
                                *kl_pushp(kmq, tcpserver->mq) = msg;
                                kh_del(ktcp_conn, tcpserver->conns, p);
                        }
                        else if (utils_wait_delay(&conn->updating_delay, current_time))
                        {
                                conn->timeout--;
                        }
                }
        }
}

bool tcpserver_poll_message(tcpserver_p tcpserver, net_message_p msg)
{
        kliter_t(kmq) * p;
        p = kl_begin(tcpserver->mq);
        if (p != kl_end(tcpserver->mq))
        {
                *msg = kl_val(p);
                kl_shift(kmq, tcpserver->mq, 0);
                return true;
        }
        else
        {
                return false;
        }
}

tcpclient_p tcpclient_create(const char *ip, int port)
{
        tcpclient_p tcpclient;
        tcpclient = (tcpclient_p)SDL_malloc(sizeof(tcpclient_t));
        SDL_assert(tcpclient);
        tcpclient->sockfd = sys_tcp();
        tcpclient->connected = false;
        strcpy(tcpclient->server_ip, ip);
        tcpclient->server_port = port;
        tcpclient->updating_delay.time = sys_current_time();
        tcpclient->updating_delay.timeout = 1000;
        tcpclient->connection_delay.time = sys_current_time();
        tcpclient->connection_delay.timeout = 3000;
        tcpclient->timeout = 1200;
        tcpclient->mq = kl_init(kmq);
        log_debug("tcpclient_create ip=%s,port=%d", ip, port);
        sys_set_sock_rcvtimeo(tcpclient->sockfd, 10);
        return tcpclient;
}

void tcpclient_destroy(tcpclient_p tcpclient)
{
        sys_closesocket(tcpclient->sockfd);
        SDL_free(tcpclient);
}

bool tcpclient_getconv(tcpclient_p tcpclient, int *conv)
{
        *conv = 0;
        return true;
}

int tcpclient_send(tcpclient_p tcpclient, const char *data, int len)
{
        return sys_send(tcpclient->sockfd, data, len);
}

void tcpclient_update(tcpclient_p tcpclient)
{
        int64_t current_time;
        net_message_t msg;
        char buf[JOY_MAX_BUFFER];
        int len;
        log_debug("tcpclient_update");
        current_time = sys_current_time();
        if (tcpclient->connected)
        {
                len = sys_recv(tcpclient->sockfd, buf);
                if (len > 0)
                {
                        msg.type = NET_TYPE_MESSAGE;
                        msg.conv = (int)tcpclient->sockfd;
                        msg.len = len;
                        msg.data = (char *)SDL_malloc(msg.len);
                        SDL_memcpy(msg.data, buf, msg.len);
                        *kl_pushp(kmq, tcpclient->mq) = msg;
                }
        }
        else
        {
                /*if (utils_wait_delay(&tcpclient->connection_delay, current_time)) {
                        log_debug("tcpclient connecting to %s:%d",
                                tcpclient->server_ip, tcpclient->server_port);
                        if (sys_connect(tcpclient->sockfd, tcpclient->server_ip, tcpclient->server_port)) {
                                tcpclient->connected = true;
                                msg.type = NET_TYPE_CONNECTED;
                                msg.conv = (int)tcpclient->sockfd;
                                msg.data = SDL_strdup("connected");
                                msg.len = SDL_strlen(msg.data);
                                *kl_pushp(kmq, tcpclient->mq) = msg;
                        }
                        else {
                                log_debug("tcpclient connect failed");
                        }
                }*/
                log_debug("tcpclient connecting to %s:%d",
                          tcpclient->server_ip, tcpclient->server_port);
                if (sys_connect(tcpclient->sockfd, tcpclient->server_ip, tcpclient->server_port))
                {
                        tcpclient->connected = true;
                        msg.type = NET_TYPE_CONNECTED;
                        msg.conv = (int)tcpclient->sockfd;
                        msg.data = SDL_strdup("connected");
                        msg.len = SDL_strlen(msg.data);
                        *kl_pushp(kmq, tcpclient->mq) = msg;
                }
                else
                {
                        log_debug("tcpclient connect failed");
                }
        }

        if (tcpclient->timeout < 0)
        {
                return;
        }
        if (utils_wait_delay(&tcpclient->updating_delay, current_time))
        {
                tcpclient->timeout--;
        }
        if (tcpclient->timeout == 0)
        {
                msg.type = NET_TYPE_DISCONNECTED;
                msg.conv = (int)tcpclient->sockfd;
                msg.len = 9;
                msg.data = SDL_strdup("disconnected");
                *kl_pushp(kmq, tcpclient->mq) = msg;
                return;
        }
}

bool tcpclient_poll_message(tcpclient_p tcpclient, net_message_p msg)
{
        kliter_t(kmq) * p;
        p = kl_begin(tcpclient->mq);
        if (p != kl_end(tcpclient->mq))
        {
                *msg = kl_val(p);
                kl_shift(kmq, tcpclient->mq, 0);
                return true;
        }
        else
        {
                return false;
        }
}

// ==============================
// WebSocket Client (仅 emscripten)
// ==============================
#ifdef __EMSCRIPTEN__

struct wsclient
{
        EMSCRIPTEN_WEBSOCKET_T socket;
        bool connected;
        klist_t(kmq) * mq;
        net_callback cb;
        void* userdata;
};

static void ws_on_open(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData)
{
        wsclient_p ws = (wsclient_p)userData;
        if (!ws) return;

        net_message_t msg;
        msg.type = NET_TYPE_CONNECTED;
        msg.conv = 0;
        msg.data = SDL_strdup("connected");
        msg.len = SDL_strlen(msg.data);
        ws->connected = true;

        if (ws->cb) {
                ws->cb(&msg, ws->userdata);
        } else {
                *kl_pushp(kmq, ws->mq) = msg;
        }
        log_info("WebSocket connected");
}

static void ws_on_message(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData)
{
        wsclient_p ws = (wsclient_p)userData;
        if (!ws) return;

        net_message_t msg;
        msg.type = NET_TYPE_MESSAGE;
        msg.conv = 0;
        msg.len = (int)e->numBytes;
        msg.data = (char*)SDL_malloc(msg.len);
        SDL_memcpy(msg.data, e->data, msg.len);

        if (ws->cb) {
                ws->cb(&msg, ws->userdata);
        } else {
                *kl_pushp(kmq, ws->mq) = msg;
        }
}

static void ws_on_close(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData)
{
        wsclient_p ws = (wsclient_p)userData;
        if (!ws) return;

        ws->connected = false;

        net_message_t msg;
        msg.type = NET_TYPE_DISCONNECTED;
        msg.conv = 0;
        msg.data = SDL_strdup("disconnected");
        msg.len = SDL_strlen(msg.data);

        if (ws->cb) {
                ws->cb(&msg, ws->userdata);
        } else {
                *kl_pushp(kmq, ws->mq) = msg;
        }
        log_info("WebSocket closed");
}

static void ws_on_error(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData)
{
        wsclient_p ws = (wsclient_p)userData;
        if (!ws) return;

        log_error("WebSocket error");
}

wsclient_p wsclient_create(const char* url)
{
        wsclient_p ws = (wsclient_p)SDL_malloc(sizeof(wsclient_t));
        if (!ws) return NULL;

        SDL_memset(ws, 0, sizeof(wsclient_t));
        ws->mq = kl_init(kmq);
        ws->connected = false;

        EmscriptenWebSocketCreateAttributes ws_attrs = {
                .url = url,
                .protocols = NULL,
                .created = false
        };

        EMSCRIPTEN_WEBSOCKET_T socket;
        EMSCRIPTEN_RESULT result = emscripten_websocket_create(&ws_attrs, &socket, ws);
        if (result != EMSCRIPTEN_RESULT_SUCCESS) {
                log_error("Failed to create WebSocket: %d", result);
                SDL_free(ws);
                return NULL;
        }
        ws->socket = socket;

        emscripten_websocket_set_onopen_callback(socket, ws, ws_on_open);
        emscripten_websocket_set_onmessage_callback(socket, ws, ws_on_message);
        emscripten_websocket_set_onclose_callback(socket, ws, ws_on_close);
        emscripten_websocket_set_onerror_callback(socket, ws, ws_on_error);

        log_info("WebSocket creating: %s", url);
        return ws;
}

void wsclient_destroy(wsclient_p ws)
{
        if (!ws) return;

        if (ws->connected && ws->socket != 0) {
                emscripten_websocket_close(ws->socket);
        }

        // 清理消息队列
        kliter_t(kmq)* p;
        for (p = kl_begin(ws->mq); p != kl_end(ws->mq); p = kl_next(p)) {
                net_message_t* msg = &kl_val(p);
                if (msg->data) SDL_free(msg->data);
        }
        kl_destroy(kmq, ws->mq);

        SDL_free(ws);
}

bool wsclient_getconv(wsclient_p ws, int* conv)
{
        if (!ws || !conv) return false;
        *conv = (int)ws->socket;
        return ws->connected;
}

int wsclient_send(wsclient_p ws, const char* data, int len)
{
        if (!ws || !ws->connected || ws->socket == 0 || !data || len <= 0) {
                return -1;
        }

        // WebSocket 发送二进制数据
        EMSCRIPTEN_RESULT result = emscripten_websocket_send_binary(ws->socket, (char*)data, len);
        if (result != EMSCRIPTEN_RESULT_SUCCESS) {
                log_error("WebSocket send failed: %d", result);
                return -1;
        }
        return len;
}

void wsclient_update(wsclient_p ws)
{
        // emscripten WebSocket 是事件驱动的，update 主要用于保持连接活跃
        // WebSocket 事件会在回调中处理，这里可以做心跳等操作
        (void)ws;
}

bool wsclient_poll_message(wsclient_p ws, net_message_p msg)
{
        if (!ws || !msg) return false;

        kliter_t(kmq)* p = kl_begin(ws->mq);
        if (p != kl_end(ws->mq)) {
                *msg = kl_val(p);
                kl_shift(kmq, ws->mq, 0);
                return true;
        }
        return false;
}

void wsclient_set_callback(wsclient_p ws, net_callback cb, void* userdata)
{
        if (!ws) return;
        ws->cb = cb;
        ws->userdata = userdata;
}

#endif // __EMSCRIPTEN__

// ==============================
// Unified NetClient (封装 kcp/tcp/ws)
// ==============================

struct netclient
{
        net_client_type type;
        union {
                kcpclient_p kcp;
                tcpclient_p tcp;
#ifdef __EMSCRIPTEN__
                wsclient_p ws;
#endif
                void* ptr;
        } client;
        net_callback cb;
        void* userdata;
};

netclient_p netclient_create(net_client_type type, const char* host, int port)
{
        netclient_p nc = (netclient_p)SDL_malloc(sizeof(netclient_t));
        if (!nc) return NULL;

        SDL_memset(nc, 0, sizeof(netclient_t));
        nc->type = type;
        nc->cb = NULL;
        nc->userdata = NULL;

        // 自动选择类型
        if (type == NET_CLIENT_AUTO) {
#ifdef __EMSCRIPTEN__
                type = NET_CLIENT_WEBSOCKET;
#else
                type = NET_CLIENT_KCP;
#endif
                nc->type = type;
        }

        switch (type) {
        case NET_CLIENT_KCP:
                nc->client.kcp = kcpclient_create(host, port);
                if (!nc->client.kcp) {
                        SDL_free(nc);
                        return NULL;
                }
                break;
        case NET_CLIENT_TCP:
                nc->client.tcp = tcpclient_create(host, port);
                if (!nc->client.tcp) {
                        SDL_free(nc);
                        return NULL;
                }
                break;
#ifdef __EMSCRIPTEN__
        case NET_CLIENT_WEBSOCKET:
                // 构建 ws:// URL
                {
                        char url[512];
                        // 检查是否已经是 ws:// 或 wss:// 开头
                        if (strncmp(host, "ws://", 5) == 0 || strncmp(host, "wss://", 6) == 0) {
                                SDL_strlcpy(url, host, sizeof(url));
                        } else {
                                SDL_snprintf(url, sizeof(url), "ws://%s:%d", host, port);
                        }
                        nc->client.ws = wsclient_create(url);
                        if (!nc->client.ws) {
                                SDL_free(nc);
                                return NULL;
                        }
                }
                break;
#endif
        default:
                SDL_free(nc);
                return NULL;
        }

        return nc;
}

void netclient_destroy(netclient_p nc)
{
        if (!nc) return;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                if (nc->client.kcp) kcpclient_destroy(nc->client.kcp);
                break;
        case NET_CLIENT_TCP:
                if (nc->client.tcp) tcpclient_destroy(nc->client.tcp);
                break;
#ifdef __EMSCRIPTEN__
        case NET_CLIENT_WEBSOCKET:
                if (nc->client.ws) wsclient_destroy(nc->client.ws);
                break;
#endif
        default:
                break;
        }

        SDL_free(nc);
}

bool netclient_getconv(netclient_p nc, int* conv)
{
        if (!nc || !conv) return false;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                return kcpclient_getconv(nc->client.kcp, conv);
        case NET_CLIENT_TCP:
                return tcpclient_getconv(nc->client.tcp, conv);
#ifdef __EMSCRIPTEN__
        case NET_CLIENT_WEBSOCKET:
                return wsclient_getconv(nc->client.ws, conv);
#endif
        default:
                return false;
        }
}

int netclient_send(netclient_p nc, const char* data, int len)
{
        if (!nc) return -1;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                return kcpclient_send(nc->client.kcp, data, len);
        case NET_CLIENT_TCP:
                return tcpclient_send(nc->client.tcp, data, len);
#ifdef __EMSCRIPTEN__
        case NET_CLIENT_WEBSOCKET:
                return wsclient_send(nc->client.ws, data, len);
#endif
        default:
                return -1;
        }
}

void netclient_update(netclient_p nc)
{
        if (!nc) return;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                kcpclient_update(nc->client.kcp);
                break;
        case NET_CLIENT_TCP:
                tcpclient_update(nc->client.tcp);
                break;
#ifdef __EMSCRIPTEN__
        case NET_CLIENT_WEBSOCKET:
                wsclient_update(nc->client.ws);
                break;
#endif
        default:
                break;
        }
}

bool netclient_poll_message(netclient_p nc, net_message_p msg)
{
        if (!nc || !msg) return false;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                return kcpclient_poll_message(nc->client.kcp, msg);
        case NET_CLIENT_TCP:
                return tcpclient_poll_message(nc->client.tcp, msg);
#ifdef __EMSCRIPTEN__
        case NET_CLIENT_WEBSOCKET:
                return wsclient_poll_message(nc->client.ws, msg);
#endif
        default:
                return false;
        }
}

void netclient_set_callback(netclient_p nc, net_callback cb, void* userdata)
{
        if (!nc) return;

        nc->cb = cb;
        nc->userdata = userdata;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                kcpclient_set_callback(nc->client.kcp, cb, userdata);
                break;
        case NET_CLIENT_TCP:
                // tcpclient 没有回调接口
                break;
#ifdef __EMSCRIPTEN__
        case NET_CLIENT_WEBSOCKET:
                wsclient_set_callback(nc->client.ws, cb, userdata);
                break;
#endif
        default:
                break;
        }
}

net_client_type netclient_get_type(netclient_p nc)
{
        if (!nc) return NET_CLIENT_TCP; // 默认值
        return nc->type;
}
