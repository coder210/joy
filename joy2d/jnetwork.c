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

#include "external/mongoose.h"

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



// Mongoose 连接上下文（存储在 mg_connection::data 中）
// MG_DATA_SIZE = 32 字节，足够存储 conv ID
typedef struct {
        int conv;                      // Connection ID
} wsnetserver_ctx_t;

struct wsnetserver {
        struct mg_mgr mgr;             // Mongoose manager
        int conv;                      // Next conv ID
        klist_t(kmq) * mq;             // Message queue
        net_callback cb;               // Global callback
        void* userdata;                // Global user data
        bool initialized;              // Init flag
};


// Forward declaration for event handler
static void wsnetserver_ev_handler(struct mg_connection* c, int ev, void* ev_data);

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

int tcpserver_connection_count(tcpserver_p tcpserver)
{
        if (!tcpserver) return 0;
        return (int)kh_size(tcpserver->conns);
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
// Native WebSocket Server (非 Emscripten 平台) - 基于 Mongoose
// ==============================

// Mongoose 事件处理函数
static void wsnetserver_ev_handler(struct mg_connection* c, int ev, void* ev_data)
{
        if (c == NULL) return;
        
        // 获取服务器实例
        wsnetserver_p ws = (wsnetserver_p)c->fn_data;
        if (ws == NULL) return;
        
        if (ev == MG_EV_HTTP_MSG) {
                // HTTP 升级请求 - 升级为 WebSocket
                struct mg_http_message* hm = (struct mg_http_message*)ev_data;
                // 检查是否为 WebSocket 升级请求
                struct mg_str* upgrade = mg_http_get_header(hm, "Upgrade");
                if (upgrade && strstr(upgrade->buf, "websocket") != NULL) {
                        // 执行 WebSocket 升级，第三个参数为 WebSocket 路径（NULL 表示任意路径）
                        mg_ws_upgrade(c, hm, NULL);
                }
        }
        else if (ev == MG_EV_WS_OPEN) {
                // WebSocket 连接已建立
                wsnetserver_ctx_t* ctx = (wsnetserver_ctx_t*)c->data;
                ctx->conv = ws->conv++;
                
                log_info("wsnetserver: new connection conv=%d", ctx->conv);
                
                // 发送连接消息
                net_message_t msg;
                msg.type = NET_TYPE_CONNECTED;
                msg.conv = ctx->conv;
                msg.data = SDL_strdup("connected");
                msg.len = (int)strlen(msg.data);
                if (ws->cb) {
                        ws->cb(&msg, ws->userdata);
                } else {
                        *kl_pushp(kmq, ws->mq) = msg;
                }
        }
        else if (ev == MG_EV_WS_MSG) {
                // 收到 WebSocket 消息
                wsnetserver_ctx_t* ctx = (wsnetserver_ctx_t*)c->data;
                struct mg_ws_message* wm = (struct mg_ws_message*)ev_data;
                int payload_len = (int)wm->data.len;
                
                if (payload_len > 0 && ctx != NULL) {
                        net_message_t msg;
                        msg.type = NET_TYPE_MESSAGE;
                        msg.conv = ctx->conv;
                        msg.data = (char*)SDL_malloc(payload_len);
                        if (msg.data) {
                                memcpy(msg.data, wm->data.buf, payload_len);
                                msg.len = payload_len;
                                
                                if (ws->cb) {
                                        ws->cb(&msg, ws->userdata);
                                } else {
                                        *kl_pushp(kmq, ws->mq) = msg;
                                }
                        }
                }
        }
        else if (ev == MG_EV_CLOSE) {
                // 连接关闭
                wsnetserver_ctx_t* ctx = (wsnetserver_ctx_t*)c->data;
                if (ctx != NULL) {
                        log_info("wsnetserver: connection closed conv=%d", ctx->conv);
                        
                        net_message_t msg;
                        msg.type = NET_TYPE_DISCONNECTED;
                        msg.conv = ctx->conv;
                        msg.data = SDL_strdup("disconnected");
                        msg.len = (int)strlen(msg.data);
                        if (ws->cb) {
                                ws->cb(&msg, ws->userdata);
                        } else {
                                *kl_pushp(kmq, ws->mq) = msg;
                        }
                }
        }
}

wsnetserver_p wsnetserver_create(const char* ip, int port)
{
        wsnetserver_p ws = (wsnetserver_p)SDL_malloc(sizeof(wsnetserver_t));
        if (!ws) return NULL;
        SDL_memset(ws, 0, sizeof(wsnetserver_t));

        ws->conv = 1000;
        ws->mq = kl_init(kmq);
        ws->cb = NULL;
        ws->userdata = NULL;
        
        // 关闭 Mongoose 日志输出
        mg_log_set(MG_LL_NONE);
        
        // 初始化 Mongoose manager
        mg_mgr_init(&ws->mgr);
        
        // 构造监听 URL
        char url[256];
        SDL_snprintf(url, sizeof(url), "wss://%s:%d", ip && ip[0] ? ip : "0.0.0.0", port);
        
        // 创建 HTTP 监听（用于接收 WebSocket 升级请求）
        struct mg_connection* c = mg_http_listen(&ws->mgr, url, wsnetserver_ev_handler, ws);
        if (c == NULL) {
                log_error("wsnetserver: failed to listen on %s", url);
                mg_mgr_free(&ws->mgr);
                kl_destroy(kmq, ws->mq);
                SDL_free(ws);
                return NULL;
        }
        
        ws->initialized = true;
        log_info("wsnetserver listening on %s", url);
        return ws;
}

int wsnetserver_destroy(wsnetserver_p ws)
{
        if (!ws) return 0;
        
        // 关闭 Mongoose manager（会自动关闭所有连接）
        mg_mgr_free(&ws->mgr);
        
        // Free messages
        kliter_t(kmq)* p;
        for (p = kl_begin(ws->mq); p != kl_end(ws->mq); p = kl_next(p)) {
                net_message_t* msg = &kl_val(p);
                if (msg->data) SDL_free(msg->data);
        }
        kl_destroy(kmq, ws->mq);

        SDL_free(ws);
        return 1;
}

void wsnetserver_send(wsnetserver_p ws, int conv, const char* data, int len)
{
        if (!ws || conv < 0 || !data) return;
        
        // 遍历所有连接找到对应的 conv
        struct mg_connection* c;
        for (c = ws->mgr.conns; c != NULL; c = c->next) {
                wsnetserver_ctx_t* ctx = (wsnetserver_ctx_t*)c->data;
                if (ctx && ctx->conv == conv) {
                        mg_ws_send(c, data, len, WEBSOCKET_OP_BINARY);
                        return;
                }
        }
}

void wsnetserver_broadcast(wsnetserver_p ws, const char* data, int len)
{
        if (!ws || !data) return;
        
        // 向所有 WebSocket 连接广播消息
        struct mg_connection* c;
        for (c = ws->mgr.conns; c != NULL; c = c->next) {
                if (c->is_websocket) {
                        mg_ws_send(c, data, len, WEBSOCKET_OP_BINARY);
                }
        }
}

void wsnetserver_offline(wsnetserver_p ws, int conv)
{
        if (!ws || conv < 0) return;
        
        // 遍历找到对应连接并关闭
        struct mg_connection* c;
        for (c = ws->mgr.conns; c != NULL; c = c->next) {
                wsnetserver_ctx_t* ctx = (wsnetserver_ctx_t*)c->data;
                if (ctx && ctx->conv == conv) {
                        // 发送 WebSocket 关闭帧
                        mg_ws_send(c, "", 0, WEBSOCKET_OP_CLOSE);
                        c->is_closing = 1;
                        return;
                }
        }
}

void wsnetserver_update(wsnetserver_p ws)
{
        if (!ws || !ws->initialized) return;
        
        // Mongoose 事件循环（0 毫秒超时，只处理已就绪的事件）
        mg_mgr_poll(&ws->mgr, 0);
}

int wsnetserver_connection_count(wsnetserver_p ws)
{
        if (!ws) return 0;
        
        int count = 0;
        struct mg_connection* c;
        for (c = ws->mgr.conns; c != NULL; c = c->next) {
                if (c->is_websocket) {
                        count++;
                }
        }
        return count;
}

bool wsnetserver_poll_message(wsnetserver_p ws, net_message_p msg)
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

void wsnetserver_set_callback(wsnetserver_p ws, net_callback cb, void* userdata)
{
        if (!ws) return;
        ws->cb = cb;
        ws->userdata = userdata;
}




struct wsnetclient
{
        struct mg_mgr mgr;             // Mongoose manager
        struct mg_connection* conn;   // 连接
        int conv;                      // Connection ID (使用 sockfd)
        bool connected;                // 连接状态
        char url[512];                 // WebSocket URL
        klist_t(kmq) * mq;            // Message queue
        net_callback cb;               // Global callback
        void* userdata;                // Global user data
        bool initialized;              // Init flag
        bool connecting;               // 连接中标志
};

// Forward declaration for event handler
static void wsnetclient_ev_handler(struct mg_connection* c, int ev, void* ev_data);

wsnetclient_p wsnetclient_create(const char* url)
{
        wsnetclient_p wc = (wsnetclient_p)SDL_malloc(sizeof(wsnetclient_t));
        if (!wc) return NULL;
        SDL_memset(wc, 0, sizeof(wsnetclient_t));

        wc->conv = -1;
        wc->connected = false;
        wc->connecting = true;
        wc->mq = kl_init(kmq);
        wc->cb = NULL;
        wc->userdata = NULL;
        wc->initialized = false;

        if (url) {
                SDL_strlcpy(wc->url, url, sizeof(wc->url));
        }

        // 关闭 Mongoose 日志输出
        mg_log_set(MG_LL_NONE);

        // 初始化 Mongoose manager
        mg_mgr_init(&wc->mgr);

        // 连接到 WebSocket 服务器
        struct mg_connection* c = mg_ws_connect(&wc->mgr, url, wsnetclient_ev_handler, wc, NULL);
        if (c == NULL) {
                log_error("wsnetclient: failed to connect to %s", url);
                mg_mgr_free(&wc->mgr);
                kl_destroy(kmq, wc->mq);
                SDL_free(wc);
                return NULL;
        }

        wc->conn = c;
        wc->initialized = true;
        log_info("wsnetclient connecting to %s", url);
        return wc;
}

void wsnetclient_destroy(wsnetclient_p wc)
{
        if (!wc) return;

        // 关闭连接
        if (wc->conn) {
                wc->conn->is_closing = 1;
        }

        // 关闭 Mongoose manager（会自动关闭所有连接）
        mg_mgr_free(&wc->mgr);

        // Free messages
        kliter_t(kmq)* p;
        for (p = kl_begin(wc->mq); p != kl_end(wc->mq); p = kl_next(p)) {
                net_message_t* msg = &kl_val(p);
                if (msg->data) SDL_free(msg->data);
        }
        kl_destroy(kmq, wc->mq);

        SDL_free(wc);
}

bool wsnetclient_getconv(wsnetclient_p wc, int* conv)
{
        if (!wc || !conv) return false;
        *conv = wc->conv;
        return wc->connected;
}

int wsnetclient_send(wsnetclient_p wc, const char* data, int len)
{
        if (!wc || !wc->connected || !wc->conn || !data || len <= 0) {
                return -1;
        }

        // 通过 WebSocket 发送二进制数据
        mg_ws_send(wc->conn, data, len, WEBSOCKET_OP_BINARY);
        return len;
}

void wsnetclient_update(wsnetclient_p wc)
{
        if (!wc || !wc->initialized) return;

        // Mongoose 事件循环（0 毫秒超时，只处理已就绪的事件）
        mg_mgr_poll(&wc->mgr, 0);
}

bool wsnetclient_poll_message(wsnetclient_p wc, net_message_p msg)
{
        if (!wc || !msg) return false;

        kliter_t(kmq)* p = kl_begin(wc->mq);
        if (p != kl_end(wc->mq)) {
                *msg = kl_val(p);
                kl_shift(kmq, wc->mq, 0);
                return true;
        }
        return false;
}

void wsnetclient_set_callback(wsnetclient_p wc, net_callback cb, void* userdata)
{
        if (!wc) return;
        wc->cb = cb;
        wc->userdata = userdata;
}

// Mongoose 事件处理函数
static void wsnetclient_ev_handler(struct mg_connection* c, int ev, void* ev_data)
{
        if (c == NULL) return;

        // 获取客户端实例
        wsnetclient_p wc = (wsnetclient_p)c->fn_data;
        if (wc == NULL) return;

        if (ev == MG_EV_WS_OPEN) {
                // WebSocket 连接已建立
                wc->connected = true;
                wc->connecting = false;
                wc->conv = (int)(intptr_t)c->fd;

                log_info("wsnetclient: connected, conv=%d", wc->conv);

                // 发送连接消息
                net_message_t msg;
                msg.type = NET_TYPE_CONNECTED;
                msg.conv = wc->conv;
                msg.data = SDL_strdup("connected");
                msg.len = (int)strlen(msg.data);
                if (wc->cb) {
                        wc->cb(&msg, wc->userdata);
                } else {
                        *kl_pushp(kmq, wc->mq) = msg;
                }
        }
        else if (ev == MG_EV_WS_MSG) {
                // 收到 WebSocket 消息
                struct mg_ws_message* wm = (struct mg_ws_message*)ev_data;
                int payload_len = (int)wm->data.len;

                if (payload_len > 0) {
                        net_message_t msg;
                        msg.type = NET_TYPE_MESSAGE;
                        msg.conv = wc->conv;
                        msg.data = (char*)SDL_malloc(payload_len);
                        if (msg.data) {
                                memcpy(msg.data, wm->data.buf, payload_len);
                                msg.len = payload_len;

                                if (wc->cb) {
                                        wc->cb(&msg, wc->userdata);
                                } else {
                                        *kl_pushp(kmq, wc->mq) = msg;
                                }
                        }
                }
        }
        else if (ev == MG_EV_CLOSE) {
                // 连接关闭
                if (wc->connected) {
                        log_info("wsnetclient: connection closed, conv=%d", wc->conv);

                        wc->connected = false;

                        net_message_t msg;
                        msg.type = NET_TYPE_DISCONNECTED;
                        msg.conv = wc->conv;
                        msg.data = SDL_strdup("disconnected");
                        msg.len = (int)strlen(msg.data);
                        if (wc->cb) {
                                wc->cb(&msg, wc->userdata);
                        } else {
                                *kl_pushp(kmq, wc->mq) = msg;
                        }
                }
                wc->conn = NULL;
        }
        else if (ev == MG_EV_ERROR) {
                // 连接错误
                log_error("wsnetclient: connection error");
                wc->connected = false;
                wc->connecting = false;
        }
}


// ==============================
// Unified NetClient (封装 kcp/tcp/ws)
// ==============================
struct netclient
{
        net_client_type type;
        union {
                kcpclient_p kcp;
                tcpclient_p tcp;
                wsnetclient_p ws; // mongoose WebSocket 客户端
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

        switch (nc->type) {
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
        case NET_CLIENT_WEBSOCKET:
                {
                        char url[512];
                        if (strncmp(host, "ws://", 5) == 0 || strncmp(host, "wss://", 6) == 0) {
                                SDL_strlcpy(url, host, sizeof(url));
                        } else {
                                SDL_snprintf(url, sizeof(url), "wss://%s:%d", host, port);
                        }
                        nc->client.ws = wsnetclient_create(url);
                        if (!nc->client.ws) {
                                SDL_free(nc);
                                return NULL;
                        }
                }
                break;
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
        case NET_CLIENT_WEBSOCKET:
                if (nc->client.ws) wsnetclient_destroy(nc->client.ws);
                break;
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
        case NET_CLIENT_WEBSOCKET:
                return wsnetclient_getconv(nc->client.ws, conv);
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
        case NET_CLIENT_WEBSOCKET:
                return wsnetclient_send(nc->client.ws, data, len);
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
        case NET_CLIENT_WEBSOCKET:
                wsnetclient_update(nc->client.ws);
                break;
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
                return false;
        case NET_CLIENT_TCP:
                return tcpclient_poll_message(nc->client.tcp, msg);
        case NET_CLIENT_WEBSOCKET:
                return wsnetclient_poll_message(nc->client.ws, msg);
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
        case NET_CLIENT_WEBSOCKET:
                wsnetclient_set_callback(nc->client.ws, cb, userdata);
                break;
        default:
                break;
        }
}

net_client_type netclient_get_type(netclient_p nc)
{
        return nc->type;
}

// ==============================
// Unified NetServer (封装 kcp/tcp/ws 服务器)
// ==============================

struct netserver
{
        net_server_type type;
        union {
                kcpserver_p kcp;
                tcpserver_p tcp;
                wsnetserver_p ws;
                void* ptr;
        } server;
        net_callback cb;
        void* userdata;
};

netserver_p netserver_create(net_server_type type, const char* ip, int port)
{
        netserver_p ns = (netserver_p)SDL_malloc(sizeof(netserver_t));
        if (!ns) return NULL;

        SDL_memset(ns, 0, sizeof(netserver_t));
        ns->type = type;
        ns->cb = NULL;
        ns->userdata = NULL;

        switch (type) {
        case NET_SERVER_KCP:
                ns->server.kcp = kcpserver_create(ip, port);
                if (!ns->server.kcp) {
                        SDL_free(ns);
                        return NULL;
                }
                break;
        case NET_SERVER_TCP:
                ns->server.tcp = tcpserver_create(ip, port);
                if (!ns->server.tcp) {
                        SDL_free(ns);
                        return NULL;
                }
                break;
        case NET_SERVER_WEBSOCKET:
                ns->server.ws = wsnetserver_create(ip, port);
                if (!ns->server.ws) {
                        SDL_free(ns);
                        return NULL;
                }
                break;
        default:
                SDL_free(ns);
                return NULL;
        }

        return ns;
}

void netserver_destroy(netserver_p ns)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                if (ns->server.kcp) kcpserver_destroy(ns->server.kcp);
                break;
        case NET_SERVER_TCP:
                if (ns->server.tcp) tcpserver_destroy(ns->server.tcp);
                break;
        case NET_SERVER_WEBSOCKET:
                if (ns->server.ws) wsnetserver_destroy(ns->server.ws);
                break;
        default:
                break;
        }

        SDL_free(ns);
}

void netserver_send(netserver_p ns, int conv, const char* data, int len)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                kcpserver_send(ns->server.kcp, conv, data, len);
                break;
        case NET_SERVER_TCP:
                tcpserver_send(ns->server.tcp, conv, data, len);
                break;
        case NET_SERVER_WEBSOCKET:
                wsnetserver_send(ns->server.ws, conv, data, len);
                break;
        default:
                break;
        }
}

void netserver_broadcast(netserver_p ns, const char* data, int len)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                kcpserver_broadcast(ns->server.kcp, data, len);
                break;
        case NET_SERVER_TCP:
                tcpserver_broadcast(ns->server.tcp, data, len);
                break;
        case NET_SERVER_WEBSOCKET:
                wsnetserver_broadcast(ns->server.ws, data, len);
                break;
        default:
                break;
        }
}

void netserver_offline(netserver_p ns, int conv)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                kcpserver_offline(ns->server.kcp, conv);
                break;
        case NET_SERVER_TCP:
                tcpserver_offline(ns->server.tcp, conv);
                break;
        case NET_SERVER_WEBSOCKET:
                wsnetserver_offline(ns->server.ws, conv);
                break;
        default:
                break;
        }
}

void netserver_update(netserver_p ns)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                kcpserver_update(ns->server.kcp);
                break;
        case NET_SERVER_TCP:
                tcpserver_update(ns->server.tcp);
                break;
        case NET_SERVER_WEBSOCKET:
                wsnetserver_update(ns->server.ws);
                break;
        default:
                break;
        }
}

int netserver_connection_count(netserver_p ns)
{
        if (!ns) return 0;

        switch (ns->type) {
        case NET_SERVER_KCP:
                return kcpserver_connection_count(ns->server.kcp);
        case NET_SERVER_TCP:
                return tcpserver_connection_count(ns->server.tcp);
        case NET_SERVER_WEBSOCKET:
                return wsnetserver_connection_count(ns->server.ws);
        default:
                return 0;
        }
}

bool netserver_poll_message(netserver_p ns, net_message_p msg)
{
        if (!ns || !msg) return false;

        switch (ns->type) {
        case NET_SERVER_KCP:
                return kcpserver_poll_message(ns->server.kcp, msg);
        case NET_SERVER_TCP:
                return tcpserver_poll_message(ns->server.tcp, msg);
        case NET_SERVER_WEBSOCKET:
                return wsnetserver_poll_message(ns->server.ws, msg);
        default:
                return false;
        }
}

void netserver_set_callback(netserver_p ns, net_callback cb, void* userdata)
{
        if (!ns) return;

        ns->cb = cb;
        ns->userdata = userdata;

        switch (ns->type) {
        case NET_SERVER_KCP:
                kcpserver_set_callback(ns->server.kcp, cb, userdata);
                break;
        case NET_SERVER_TCP:
                // tcpserver 没有回调接口
                break;
        case NET_SERVER_WEBSOCKET:
                wsnetserver_set_callback(ns->server.ws, cb, userdata);
                break;
        default:
                break;
        }
}

net_server_type netserver_get_type(netserver_p ns)
{
        return ns->type;
}
