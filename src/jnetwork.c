#include "external/klib/khash.h"
#include "external/klib/klist.h"
#include "external/mongoose.h"
#include "external/kcp.h"
#include "joy.h"

#ifdef JOY_WIN
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif


#ifdef JOY_EMSCRIPTEN
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#endif

typedef struct joy_kcp_connection
{
        ikcpcb *kcp;
        char ip[JOY_MAX_IP];
        int port;
        int timeout;
        joy_delay_t updating_delay;
} kcp_connection_t, *kcp_connection_p;

typedef struct joy_tcp_connection
{
        int64_t sockfd;
        char ip[JOY_MAX_IP];
        int port;
        int timeout;
        joy_delay_t updating_delay;
} tcp_connection_t, *tcp_connection_p;

KHASH_INIT(kconn, int, kcp_connection_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(ktcp_conn, int, tcp_connection_p, 1, kh_int_hash_func, kh_int_hash_equal)

// no-op: free 作为 KMEMPOOL_INIT 析构函数会导致宏内建 free 之后 double-free
// 消息内部 data 由 kl_begin/kl_end 循环中的 free(msg->data) 负责释放
static void kmq_free_nop(void* p) {}
KLIST_INIT(kmq, joy_net_message_t, kmq_free_nop)

struct joy_kcpserver
{
        int64_t sockfd;
        int conv;
        khash_t(kconn) * conns;
        net_callback cb;
        klist_t(kmq) * mq;
        void* userdata;
};

struct joy_kcpclient
{
        ikcpcb *kcp;
        int64_t sockfd;
        char server_ip[JOY_MAX_IP];
        int server_port;
        short timeout;
        joy_delay_t updating_delay;
        joy_delay_t connection_delay;
        net_callback cb;
        klist_t(kmq) * mq;
        void* userdata;
};

struct joy_tcpserver
{
        int64_t sockfd;
        int conv;
        khash_t(ktcp_conn) * conns;
        klist_t(kmq) * mq;
};

struct joy_tcpclient
{
        int64_t sockfd;
        bool connected;
        char server_ip[JOY_MAX_IP];
        int server_port;
        short timeout;
        joy_delay_t updating_delay;
        joy_delay_t connection_delay;
        klist_t(kmq) * mq;
};



// Mongoose 连接上下文（存储在 mg_connection::user_data 中）
typedef struct {
        int conv;                      // Connection ID
        struct joy_wsnetserver* server;    // 服务器实例指针（用于后续事件访问）
} joy_wsnetserver_ctx_t;

struct joy_wsnetserver {
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
joy_kcpserver_p
joy_net_kcpserver_create(const char* ip, int port)
{
        joy_kcpserver_p ks;
        ks = (joy_kcpserver_p)malloc(sizeof(joy_kcpserver_t));
        assert(ks);
        memset(ks, 0, sizeof(joy_kcpserver_t));
        ks->sockfd = joy_net_sys_udp();
        ks->conv = 1000;
        ks->conns = kh_init(kconn);
        ks->mq = kl_init(kmq);
        ks->cb = NULL;
        if (!joy_net_sys_bind(ks->sockfd, ip, port))
        {
                joy_core_log_info("bind error");
        }
        else
        {
                joy_core_log_info("bind successful");
        }
        joy_net_sys_set_sock_rcvtimeo(ks->sockfd, 10);
        joy_core_log_info("ip:%s,port=%d", ip, port);
        joy_core_log_info("sockfd:%d", ks->sockfd);
        return ks;
}

int joy_net_kcpserver_destroy(joy_kcpserver_p ks)
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
                                free(conn);
                        }
                }
        }

        joy_net_message_t msg;
        while (joy_net_kcpserver_poll_message(ks, &msg))
        {
                if (msg.data) free(msg.data);
        }

        joy_net_sys_closesocket(ks->sockfd);
        kh_destroy(kconn, ks->conns);
        //kl_destroy(kmq, ks->mq);
        free(ks);
        return 1;
}

static int
kcpserver_output(const char* data, int len, ikcpcb* kcp, void* user)
{
        IUINT32 conv;
        joy_kcpserver_p ks;
        kcp_connection_p conn;
        khint_t k;
        ks = (joy_kcpserver_p)user;
        conv = ikcp_getconv(kcp);
        k = kh_get(kconn, ks->conns, conv);
        if (k != kh_end(ks->conns))
        {
                conn = kh_val(ks->conns, k);
                return joy_net_sys_sendto(ks->sockfd, data, len, conn->ip, conn->port);
        }
        return -1;
}

static void
kcpserver_input(joy_kcpserver_p ks, const char* data, int len,
        const char* ip, int port)
{
        char buf[4];
        int conv, ret;
        khint_t k;
        kcp_connection_p conn;
        joy_net_message_t msg;

        conv = joy_core_utils_bit2int((uint8_t*)data);
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

        conn = (kcp_connection_p)malloc(sizeof(kcp_connection_t));
        assert(conn);
        memset(conn, 0, sizeof(kcp_connection_t));

        conn->kcp = ikcp_create(ks->conv++, ks);
        ikcp_wndsize(conn->kcp, 512, 512);
        ikcp_nodelay(conn->kcp, 0, 40, 0, 0);
        ikcp_setoutput(conn->kcp, kcpserver_output);

        strcpy(conn->ip, ip);
        conn->port = port;
        conn->timeout = 120;
        conn->updating_delay.time = joy_net_sys_current_time();
        conn->updating_delay.timeout = 1000;

        conv = ikcp_getconv(conn->kcp);
        k = kh_put(kconn, ks->conns, conv, &ret);
        if (ret == -1)
        {
                joy_core_log_error("kh_put failed");
                ikcp_release(conn->kcp);
                free(conn);
                return;
        }
        kh_val(ks->conns, k) = conn;

        joy_core_utils_int2bit((uint8_t*)buf, conv);
        joy_net_sys_sendto(ks->sockfd, buf, 4, conn->ip, conn->port);

        msg.type = NET_TYPE_CONNECTED;
        msg.data = joy_core_utils_strdup("connected", NULL);
        msg.len = strlen(msg.data);
        msg.conv = conv;

        if (ks->cb) {
                ks->cb(&msg, ks->userdata);
        }
        else {
                *kl_pushp(kmq, ks->mq) = msg;
        }
}

void joy_net_kcpserver_send(joy_kcpserver_p ks, int conv, const char* data, int len)
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

void joy_net_kcpserver_broadcast(joy_kcpserver_p ks, const char* data, int len)
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

void joy_net_kcpserver_offline(joy_kcpserver_p ks, int conv)
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

void joy_net_kcpserver_update(joy_kcpserver_p ks)
{
        uint64_t current_time;
        int len, port;
        char buf[JOY_MAX_BUFFER], ip[JOY_MAX_IP];
        joy_net_message_t msg;

        current_time = joy_net_sys_current_time();
        len = joy_net_sys_recvfrom(ks->sockfd, buf, ip, &port);
        if (len > 0)
        {
                kcpserver_input(ks, buf, len, ip, port);
        }

        khint_t* del_list = malloc(kh_size(ks->conns) * sizeof(khint_t));
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

                if (joy_core_utils_wait_delay(&conn->updating_delay, current_time))
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
                        msg.data = malloc(msg.len);
                        memcpy(msg.data, buf, msg.len);
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
                msg.data = joy_core_utils_strdup("disconnected", NULL);
                msg.len = strlen(msg.data);
                msg.conv = ikcp_getconv(conn->kcp);

                if (ks->cb) {
                        ks->cb(&msg, ks->userdata);
                }
                else {
                        *kl_pushp(kmq, ks->mq) = msg;
                }

                ikcp_release(conn->kcp);
                free(conn);
                kh_del(kconn, ks->conns, p);
        }
        free(del_list);
}

int joy_net_kcpserver_connection_count(joy_kcpserver_p ks)
{
        return kh_size(ks->conns);
}

bool joy_net_kcpserver_poll_message(joy_kcpserver_p ks, joy_net_message_p msg)
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

void joy_net_kcpserver_set_callback(joy_kcpserver_p ks, net_callback cb, void* userdata)
{
        ks->cb = cb;
        ks->userdata = userdata;
}

// ==============================
// 客户端
// ==============================
joy_kcpclient_p joy_net_kcpclient_create(const char* ip, int port)
{
        joy_kcpclient_p kc;
        kc = (joy_kcpclient_p)malloc(sizeof(joy_kcpclient_t));
        assert(kc);
        memset(kc, 0, sizeof(joy_kcpclient_t)); // 【修复】初始化全0
        kc->kcp = NULL;
        kc->sockfd = joy_net_sys_udp();
        strcpy(kc->server_ip, ip);
        kc->server_port = port;
        kc->updating_delay.time = joy_net_sys_current_time();
        kc->updating_delay.timeout = 1000;
        kc->connection_delay.time = 0;  // 立即触发首次连接
        kc->connection_delay.timeout = 100;
        kc->timeout = 1200;
        kc->mq = kl_init(kmq);
        kc->cb = NULL;
        joy_net_sys_set_sock_rcvtimeo(kc->sockfd, 10);
        return kc;
}

void joy_net_kcpclient_destroy(joy_kcpclient_p kc)
{
        if (!kc) return;
        joy_net_sys_closesocket(kc->sockfd);
        if (kc->kcp) {
                ikcp_release(kc->kcp);
                kc->kcp = NULL;
        }
        joy_net_message_t msg;
        while (joy_net_kcpclient_poll_message(kc, &msg)) {
                if (msg.data) free(msg.data);
        }
        //kl_destroy(kmq, kc->mq);
        free(kc);
}

static int
kcpclient_output(const char* data, int size, ikcpcb* kcp, void* user)
{
        int n;
        joy_kcpclient_p kc;
        kc = (joy_kcpclient_p)user;
        n = joy_net_sys_sendto(kc->sockfd, data, size, kc->server_ip, kc->server_port);
        return n;
}

static void
kcpclient_input(joy_kcpclient_p kc, const char* data, int sz)
{
        joy_net_message_t msg;
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

                joy_core_log_debug("kcpclient create");
                conv = joy_core_utils_bit2int((uint8_t*)data);
                kc->kcp = ikcp_create(conv, kc);
                ikcp_wndsize(kc->kcp, 512, 512);
                ikcp_nodelay(kc->kcp, 0, 40, 0, 0);
                ikcp_setoutput(kc->kcp, kcpclient_output);
                msg.conv = conv;
                msg.data = joy_core_utils_strdup("connected", NULL);
                msg.len = strlen(msg.data);
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

int joy_net_kcpclient_send(joy_kcpclient_p kc, const char* data, int len)
{
        if (kc->kcp)
        {
                return ikcp_send(kc->kcp, data, len);
        }
        return -1;
}

void joy_net_kcpclient_update(joy_kcpclient_p kc)
{
        int64_t current_time;
        joy_net_message_t msg;
        char data[4] = { 0 };
        char buf[JOY_MAX_BUFFER];
        char ip[JOY_MAX_IP];
        int port, len;

        current_time = joy_net_sys_current_time();
        len = joy_net_sys_recvfrom(kc->sockfd, buf, ip, &port);

        if (len > 0) {
                kcpclient_input(kc, buf, len);
        }

        if (kc->kcp)
        {
                if (kc->timeout <= 0)
                {
                        return;
                }
                if (joy_core_utils_wait_delay(&kc->updating_delay, current_time))
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
                        msg.data = joy_core_utils_strdup("disconnected", NULL);
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
                        msg.data = (char*)malloc(msg.len);
                        memcpy(msg.data, buf, msg.len);

                        if (kc->cb) {
                                kc->cb(&msg, kc->userdata);
                        }
                        else {
                                *kl_pushp(kmq, kc->mq) = msg;
                        }
                }
        }
        else if (joy_core_utils_wait_delay(&kc->connection_delay, current_time))
        {
                data[0] = data[1] = data[2] = data[3] = 0;
                joy_net_sys_sendto(kc->sockfd, data, 4, kc->server_ip, kc->server_port);
                kc->connection_delay.time = current_time;
        }
}

bool joy_net_kcpclient_poll_message(joy_kcpclient_p kc, joy_net_message_p msg)
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

void joy_net_kcpclient_set_callback(joy_kcpclient_p kc, net_callback cb, void* userdata)
{
        kc->cb = cb;
        kc->userdata = userdata;
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////tcp//////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

joy_tcpserver_p
joy_net_tcpserver_create(const char *ip, int port)
{
        joy_tcpserver_p tcpserver;
        tcpserver = (joy_tcpserver_p)malloc(sizeof(joy_tcpserver_t));
        assert(tcpserver);
        tcpserver->sockfd = joy_net_sys_tcp();
        tcpserver->conv = 1000;
        tcpserver->conns = kh_init(ktcp_conn);
        tcpserver->mq = kl_init(kmq);
        joy_net_sys_set_sock_accpettimeo(tcpserver->sockfd, 1);
        if (!joy_net_sys_bind(tcpserver->sockfd, ip, port))
        {
                joy_core_log_info("bind error");
        }
        else
        {
                joy_core_log_info("bind successful");
        }

        if (!joy_net_sys_listen(tcpserver->sockfd))
        {
                joy_core_log_info("listen error");
        }
        else
        {
                joy_core_log_info("listen successful");
        }
        joy_core_log_info("ip:%s,port=%d", ip, port);
        joy_core_log_info("sockfd:%d", tcpserver->sockfd);
        return tcpserver;
}

int joy_net_tcpserver_destroy(joy_tcpserver_p tcpserver)
{
        if (!tcpserver)
                return 0;

        // 释放所有连接
        khint_t k;
        for (k = kh_begin(tcpserver->conns); k != kh_end(tcpserver->conns); ++k)
        {
                if (kh_exist(tcpserver->conns, k))
                {
                        tcp_connection_p conn = kh_val(tcpserver->conns, k);
                        joy_net_sys_closesocket(conn->sockfd);
                        free(conn);
                }
        }

        // 释放消息队列中的所有消息
        kliter_t(kmq) * p;
        for (p = kl_begin(tcpserver->mq);
             p != kl_end(tcpserver->mq); p = kl_next(p))
        {
                joy_net_message_t *msg = &kl_val(p);
                if (msg->data)
                        free(msg->data);
        }
        kl_destroy(kmq, tcpserver->mq);

        // 释放服务器资源
        kh_destroy(ktcp_conn, tcpserver->conns);
        joy_net_sys_closesocket(tcpserver->sockfd);
        free(tcpserver);
        return 1;
}

static void
tcpserver_accpet(joy_tcpserver_p tcpserver, int32_t sockfd, const char *ip, int port)
{
        int ret, conv;
        khint_t k;
        tcp_connection_p conn;
        joy_net_message_t msg;

        conn = (tcp_connection_p)malloc(sizeof(tcp_connection_t));
        assert(conn);
        conn->sockfd = sockfd;
        conn->port = port;
        conn->timeout = 120;
        conn->updating_delay.time = joy_net_sys_current_time();
        conn->updating_delay.timeout = 1000;
        conv = tcpserver->conv++;
        k = kh_put(ktcp_conn, tcpserver->conns, conv, &ret);
        kh_val(tcpserver->conns, k) = conn;

        msg.type = NET_TYPE_CONNECTED;
        msg.data = joy_core_utils_strdup("connected", NULL);
        msg.len = strlen(msg.data);
        msg.conv = conv;
        *kl_pushp(kmq, tcpserver->mq) = msg;
        joy_core_log_debug("connected conv=%d\n", conv);
}

void joy_net_tcpserver_send(joy_tcpserver_p tcpserver, int conv, const char *data, int len)
{
        khint_t k;
        tcp_connection_p conn;
        k = kh_get(ktcp_conn, tcpserver->conns, conv);
        if (k != kh_end(tcpserver->conns))
        {
                conn = kh_val(tcpserver->conns, k);
                joy_net_sys_send(conn->sockfd, data, len);
        }
}

void joy_net_tcpserver_broadcast(joy_tcpserver_p tcpserver, const char *data, int len)
{
        khint_t k;
        tcp_connection_p conn;
        k = kh_begin(tcpserver->conns);
        while (k != kh_end(tcpserver->conns))
        {
                if (kh_exist(tcpserver->conns, k))
                {
                        conn = kh_val(tcpserver->conns, k);
                        joy_net_sys_send(conn->sockfd, data, len);
                }
                k++;
        }
}

void joy_net_tcpserver_offline(joy_tcpserver_p tcpserver, int conv)
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

void joy_net_tcpserver_update(joy_tcpserver_p tcpserver)
{
        uint64_t current_time;
        int64_t sockfd;
        int len, port, conv;
        char buf[JOY_MAX_BUFFER], ip[JOY_MAX_IP];
        khint_t p;
        tcp_connection_p conn;
        joy_net_message_t msg;

        current_time = joy_net_sys_current_time();
        sockfd = joy_net_sys_accept(tcpserver->sockfd, ip, &port);
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
                        len = joy_net_sys_recv(conn->sockfd, buf);
                        if (len > 0)
                        {
                                msg.type = NET_TYPE_MESSAGE;
                                msg.len = len;
                                msg.data = (char *)malloc(msg.len);
                                memcpy(msg.data, buf, msg.len);
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
                                msg.data = joy_core_utils_strdup("disconnected", NULL);
                                msg.len = strlen(msg.data);
                                msg.conv = conv;
                                *kl_pushp(kmq, tcpserver->mq) = msg;
                                kh_del(ktcp_conn, tcpserver->conns, p);
                        }
                        else if (joy_core_utils_wait_delay(&conn->updating_delay, current_time))
                        {
                                conn->timeout--;
                        }
                }
        }
}

bool joy_net_tcpserver_poll_message(joy_tcpserver_p tcpserver, joy_net_message_p msg)
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

int joy_net_tcpserver_connection_count(joy_tcpserver_p tcpserver)
{
        if (!tcpserver) return 0;
        return (int)kh_size(tcpserver->conns);
}

joy_tcpclient_p joy_net_tcpclient_create(const char *ip, int port)
{
        joy_tcpclient_p tcpclient;
        tcpclient = (joy_tcpclient_p)malloc(sizeof(joy_tcpclient_t));
        assert(tcpclient);
        tcpclient->sockfd = joy_net_sys_tcp();
        tcpclient->connected = false;
        strcpy(tcpclient->server_ip, ip);
        tcpclient->server_port = port;
        tcpclient->updating_delay.time = joy_net_sys_current_time();
        tcpclient->updating_delay.timeout = 1000;
        tcpclient->connection_delay.time = 0;
        tcpclient->connection_delay.timeout = 100;
        tcpclient->timeout = 1200;
        tcpclient->mq = kl_init(kmq);
        joy_core_log_debug("joy_net_tcpclient_create ip=%s,port=%d", ip, port);
        joy_net_sys_set_sock_rcvtimeo(tcpclient->sockfd, 10);
        return tcpclient;
}

void joy_net_tcpclient_destroy(joy_tcpclient_p tcpclient)
{
        joy_net_sys_closesocket(tcpclient->sockfd);
        free(tcpclient);
}

int joy_net_tcpclient_send(joy_tcpclient_p tcpclient, const char *data, int len)
{
        return joy_net_sys_send(tcpclient->sockfd, data, len);
}

void joy_net_tcpclient_update(joy_tcpclient_p tcpclient)
{
        int64_t current_time;
        joy_net_message_t msg;
        char buf[JOY_MAX_BUFFER];
        int len;
        joy_core_log_debug("joy_net_tcpclient_update");
        current_time = joy_net_sys_current_time();
        if (tcpclient->connected)
        {
                len = joy_net_sys_recv(tcpclient->sockfd, buf);
                if (len > 0)
                {
                        msg.type = NET_TYPE_MESSAGE;
                        msg.conv = (int)tcpclient->sockfd;
                        msg.len = len;
                        msg.data = (char *)malloc(msg.len);
                        memcpy(msg.data, buf, msg.len);
                        *kl_pushp(kmq, tcpclient->mq) = msg;
                }
        }
        else
        {
                /*if (joy_core_utils_wait_delay(&tcpclient->connection_delay, current_time)) {
                        joy_core_log_debug("tcpclient connecting to %s:%d",
                                tcpclient->server_ip, tcpclient->server_port);
                        if (joy_net_sys_connect(tcpclient->sockfd, tcpclient->server_ip, tcpclient->server_port)) {
                                tcpclient->connected = true;
                                msg.type = NET_TYPE_CONNECTED;
                                msg.conv = (int)tcpclient->sockfd;
                                msg.data = joy_core_utils_strdup("connected", NULL);
                                msg.len = strlen(msg.data);
                                *kl_pushp(kmq, tcpclient->mq) = msg;
                        }
                        else {
                                joy_core_log_debug("tcpclient connect failed");
                        }
                }*/
                joy_core_log_debug("tcpclient connecting to %s:%d",
                          tcpclient->server_ip, tcpclient->server_port);
                if (joy_net_sys_connect(tcpclient->sockfd, tcpclient->server_ip, tcpclient->server_port))
                {
                        tcpclient->connected = true;
                        msg.type = NET_TYPE_CONNECTED;
                        msg.conv = (int)tcpclient->sockfd;
                        msg.data = joy_core_utils_strdup("connected", NULL);
                        msg.len = strlen(msg.data);
                        *kl_pushp(kmq, tcpclient->mq) = msg;
                }
                else
                {
                        joy_core_log_debug("tcpclient connect failed");
                }
        }

        if (tcpclient->timeout < 0)
        {
                return;
        }
        if (joy_core_utils_wait_delay(&tcpclient->updating_delay, current_time))
        {
                tcpclient->timeout--;
        }
        if (tcpclient->timeout == 0)
        {
                msg.type = NET_TYPE_DISCONNECTED;
                msg.conv = (int)tcpclient->sockfd;
                msg.len = 9;
                msg.data = joy_core_utils_strdup("disconnected", NULL);
                *kl_pushp(kmq, tcpclient->mq) = msg;
                return;
        }
}

bool joy_net_tcpclient_poll_message(joy_tcpclient_p tcpclient, joy_net_message_p msg)
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
        // 首先尝试从连接的 user_data 获取（客户端连接会在握手完成后设置）
        // 如果为 NULL，则从 manager 的 user_data 获取（监听连接）
        joy_wsnetserver_p ws = (joy_wsnetserver_p)c->user_data;
        if (ws == NULL && c->mgr != NULL) {
                ws = (joy_wsnetserver_p)c->mgr->user_data;
        }
        if (ws == NULL) return;
        
        if (ev == MG_EV_WEBSOCKET_HANDSHAKE_DONE) {
                // WebSocket 连接已建立
                // 为连接分配上下文
                joy_wsnetserver_ctx_t* ctx = (joy_wsnetserver_ctx_t*)malloc(sizeof(joy_wsnetserver_ctx_t));
                if (ctx) {
                        ctx->conv = ws->conv++;
                        ctx->server = ws;  // 保存服务器指针
                        c->user_data = ctx;
                        joy_core_log_info("wsnetserver: new connection conv=%d", ctx->conv);
                        
                        // 发送连接消息
                        joy_net_message_t msg;
                        msg.type = NET_TYPE_CONNECTED;
                        msg.conv = ctx->conv;
                        msg.data = joy_core_utils_strdup("connected", NULL);
                        msg.len = (int)strlen(msg.data);
                        if (ws->cb) {
                                ws->cb(&msg, ws->userdata);
                        } else {
                                *kl_pushp(kmq, ws->mq) = msg;
                        }
                }
        }
        else if (ev == MG_EV_WEBSOCKET_FRAME) {
                // 收到 WebSocket 消息
                struct websocket_message* wm = (struct websocket_message*)ev_data;
                int payload_len = (int)wm->size;
                
                joy_wsnetserver_ctx_t* ctx = (joy_wsnetserver_ctx_t*)c->user_data;
                
                if (payload_len > 0 && ctx != NULL && ctx->server != NULL) {
                        joy_wsnetserver_p server = ctx->server;
                        joy_net_message_t msg;
                        msg.type = NET_TYPE_MESSAGE;
                        msg.conv = ctx->conv;
                        msg.data = (char*)malloc(payload_len);
                        if (msg.data) {
                                memcpy(msg.data, wm->data, payload_len);
                                msg.len = payload_len;
                                
                                if (server->cb) {
                                        server->cb(&msg, server->userdata);
                                } else {
                                        *kl_pushp(kmq, server->mq) = msg;
                                }
                        }
                }
        }
        else if (ev == MG_EV_HTTP_REQUEST) {
                //struct mg_serve_http_opts opts;
                //opts.document_root = ".";  // Serve current directory
                //opts.enable_directory_listing = "yes";
                //mg_serve_http(c, (struct http_message*)ev_data, opts);
        }
        else if (ev == MG_EV_CLOSE) {
                // 连接关闭
                joy_wsnetserver_ctx_t* ctx = (joy_wsnetserver_ctx_t*)c->user_data;
                if (ctx != NULL && ctx->server != NULL) {
                        joy_wsnetserver_p server = ctx->server;
                        joy_core_log_info("wsnetserver: connection closed conv=%d", ctx->conv);
                        
                        joy_net_message_t msg;
                        msg.type = NET_TYPE_DISCONNECTED;
                        msg.conv = ctx->conv;
                        msg.data = joy_core_utils_strdup("disconnected", NULL);
                        msg.len = (int)strlen(msg.data);
                        if (server->cb) {
                                server->cb(&msg, server->userdata);
                        } else {
                                *kl_pushp(kmq, server->mq) = msg;
                        }
                        free(ctx);
                        c->user_data = NULL;
                }
        }
}

joy_wsnetserver_p joy_net_wsnetserver_create(const char* ip, int port)
{
        joy_wsnetserver_p ws = (joy_wsnetserver_p)malloc(sizeof(joy_wsnetserver_t));
        if (!ws) return NULL;
        memset(ws, 0, sizeof(joy_wsnetserver_t));

        ws->conv = 1000;
        ws->mq = kl_init(kmq);
        ws->cb = NULL;
        ws->userdata = NULL;
        
        // 初始化 Mongoose manager，传递 ws 作为 user_data 供后续连接使用
        mg_mgr_init(&ws->mgr, (void*)ws);

        // 构造监听地址
        // Mongoose 6.x: TCP 监听地址格式为 [tcp://][IP_ADDRESS]:PORT
        // WebSocket 协议通过 mg_set_protocol_http_websocket() 自动启用
        char addr[256];
        if (ip && ip[0] && strcmp(ip, "0.0.0.0") != 0) {
                // 绑定到特定 IP（如果需要限制在特定接口）
                snprintf(addr, sizeof(addr), "tcp://%s:%d", ip, port);
        } else {
                // 绑定到所有接口（推荐）
                snprintf(addr, sizeof(addr), "tcp://:%d", port);
        }
        
        // 创建 TCP 监听
        struct mg_connection* c = mg_bind(&ws->mgr, addr, wsnetserver_ev_handler);
        if (c == NULL) {
                joy_core_log_error("wsnetserver: failed to listen on %s", addr);
                mg_mgr_free(&ws->mgr);
                if (ws->mq) {
                        kl_destroy(kmq, ws->mq);
                }
                free(ws);
                return NULL;
        }
        
        // 启用 HTTP 和 WebSocket 协议支持（在连接上启用，不是 manager）
        mg_set_protocol_http_websocket(c);
        
        // 注意：不需要在这里设置 c->user_data = ws
        // mg_mgr_init 中传递的 ws 会在 mg_connect 或 accept 时自动传递给新连接
        
        ws->initialized = true;
        joy_core_log_info("wsnetserver listening on %s", addr);
        return ws;
}

int joy_net_wsnetserver_destroy(joy_wsnetserver_p ws)
{
        if (!ws) return 0;
        
        // 关闭 Mongoose manager（会自动关闭所有连接）
        mg_mgr_free(&ws->mgr);
        
        // Free messages
        kliter_t(kmq)* p;
        for (p = kl_begin(ws->mq); p != kl_end(ws->mq); p = kl_next(p)) {
                joy_net_message_t* msg = &kl_val(p);
                if (msg->data) free(msg->data);
        }
        kl_destroy(kmq, ws->mq);

        free(ws);
        return 1;
}

void joy_net_wsnetserver_send(joy_wsnetserver_p ws, int conv, const char* data, int len)
{
        if (!ws || conv < 0 || !data) return;
        
        // 遍历所有连接找到对应的 conv
        struct mg_connection* c;
        for (c = ws->mgr.active_connections; c != NULL; c = c->next) {
                joy_wsnetserver_ctx_t* ctx = (joy_wsnetserver_ctx_t*)c->user_data;
                if (ctx && ctx->conv == conv && (c->flags & MG_F_IS_WEBSOCKET)) {
                        mg_send_websocket_frame(c, WEBSOCKET_OP_BINARY, data, len);
                        return;
                }
        }
}

void joy_net_wsnetserver_broadcast(joy_wsnetserver_p ws, const char* data, int len)
{
        if (!ws || !data) return;
        
        // 向所有 WebSocket 连接广播消息
        struct mg_connection* c;
        for (c = ws->mgr.active_connections; c != NULL; c = c->next) {
                if (c->flags & MG_F_IS_WEBSOCKET) {
                        mg_send_websocket_frame(c, WEBSOCKET_OP_BINARY, data, len);
                }
        }
}

void joy_net_wsnetserver_offline(joy_wsnetserver_p ws, int conv)
{
        if (!ws || conv < 0) return;
        
        // 遍历找到对应连接并关闭
        struct mg_connection* c;
        for (c = ws->mgr.active_connections; c != NULL; c = c->next) {
                joy_wsnetserver_ctx_t* ctx = (joy_wsnetserver_ctx_t*)c->user_data;
                if (ctx && ctx->conv == conv) {
                        // 发送 WebSocket 关闭帧
                        mg_send_websocket_frame(c, WEBSOCKET_OP_CLOSE, "", 0);
                        c->flags |= MG_F_CLOSE_IMMEDIATELY;
                        return;
                }
        }
}

void joy_net_wsnetserver_update(joy_wsnetserver_p ws)
{
        if (!ws || !ws->initialized) return;
        
        // Mongoose 事件循环（0 毫秒超时，只处理已就绪的事件）
        mg_mgr_poll(&ws->mgr, 0);
}

int joy_net_wsnetserver_connection_count(joy_wsnetserver_p ws)
{
        if (!ws) return 0;
        
        int count = 0;
        struct mg_connection* c;
        for (c = ws->mgr.active_connections; c != NULL; c = c->next) {
                if (c->flags & MG_F_IS_WEBSOCKET) {
                        count++;
                }
        }
        return count;
}

bool joy_net_wsnetserver_poll_message(joy_wsnetserver_p ws, joy_net_message_p msg)
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

void joy_net_wsnetserver_set_callback(joy_wsnetserver_p ws, net_callback cb, void* userdata)
{
        if (!ws) return;
        ws->cb = cb;
        ws->userdata = userdata;
}






#ifndef JOY_EMSCRIPTEN

struct joy_wsnetclient
{
        struct mg_mgr mgr;             // Mongoose manager
        struct mg_connection* conn;   // 连接
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

joy_wsnetclient_p joy_net_wsnetclient_create(const char* url)
{
        joy_wsnetclient_p wc = (joy_wsnetclient_p)malloc(sizeof(joy_wsnetclient_t));
        if (!wc) return NULL;
        memset(wc, 0, sizeof(joy_wsnetclient_t));

        wc->connected = false;
        wc->connecting = true;
        wc->mq = kl_init(kmq);
        wc->cb = NULL;
        wc->userdata = NULL;
        wc->initialized = false;

        if (url) {
                strncpy(wc->url, url, sizeof(wc->url));
        }

        // 初始化 Mongoose manager
        mg_mgr_init(&wc->mgr, (void*)wc);

        // 验证 URL 格式
        if (strncmp(url, "ws://", 5) != 0 && strncmp(url, "wss://", 6) != 0) {
                joy_core_log_error("wsnetclient: FATAL - Invalid WebSocket URL format: %s", url);
                mg_mgr_free(&wc->mgr);
                kl_destroy(kmq, wc->mq);
                free(wc);
                return NULL;
        }
        
        // 连接到 WebSocket 服务器
        struct mg_connection* c = mg_connect_ws(&wc->mgr, wsnetclient_ev_handler, url, NULL, NULL);
        if (c == NULL) {
                joy_core_log_error("wsnetclient: FATAL - mg_connect_ws returned NULL for %s", url);
                joy_core_log_error("wsnetclient: This usually means TCP connection failed!");
                mg_mgr_free(&wc->mgr);
                kl_destroy(kmq, wc->mq);
                free(wc);
                return NULL;
        }

        // 设置连接的用户数据
        c->user_data = wc;
        wc->conn = c;
        wc->initialized = true;
        return wc;
}

void joy_net_wsnetclient_destroy(joy_wsnetclient_p wc)
{
        if (!wc) return;

        // 关闭连接
        if (wc->conn) {
                wc->conn->flags |= MG_F_CLOSE_IMMEDIATELY;
        }

        // 关闭 Mongoose manager（会自动关闭所有连接）
        mg_mgr_free(&wc->mgr);

        // Free messages
        kliter_t(kmq)* p;
        for (p = kl_begin(wc->mq); p != kl_end(wc->mq); p = kl_next(p)) {
                joy_net_message_t* msg = &kl_val(p);
                if (msg->data) free(msg->data);
        }
        kl_destroy(kmq, wc->mq);

        free(wc);
}

int joy_net_wsnetclient_send(joy_wsnetclient_p wc, const char* data, int len)
{
        if (!wc || !wc->connected || !wc->conn || !data || len <= 0) {
                return -1;
        }

        // 通过 WebSocket 发送二进制数据
        mg_send_websocket_frame(wc->conn, WEBSOCKET_OP_BINARY, data, len);
        return len;
}

void joy_net_wsnetclient_update(joy_wsnetclient_p wc)
{
        if (!wc || !wc->initialized) return;

        // Mongoose 事件循环（0 毫秒超时，只处理已就绪的事件）
        mg_mgr_poll(&wc->mgr, 0);
}

bool joy_net_wsnetclient_poll_message(joy_wsnetclient_p wc, joy_net_message_p msg)
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

void joy_net_wsnetclient_set_callback(joy_wsnetclient_p wc, net_callback cb, void* userdata)
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
        joy_wsnetclient_p wc = (joy_wsnetclient_p)c->user_data;
        if (wc == NULL) {
                // 尝试从 manager 获取
                if (c->mgr != NULL) {
                        wc = (joy_wsnetclient_p)c->mgr->user_data;
                }
        }
        if (wc == NULL) return;

        if (ev == MG_EV_CONNECT) {
                // 连接结果
                int* err = (int*)ev_data;
                if (*err != 0) {
                        joy_core_log_error("wsnetclient: MG_EV_CONNECT failed, error=%d", *err);
                        wc->connecting = false;
                        return;
                }
        }
        else if (ev == MG_EV_WEBSOCKET_HANDSHAKE_DONE) {
                // WebSocket 连接已建立
                struct http_message* hm = (struct http_message*)ev_data;
                
                if (hm->resp_code != 101) {
                        joy_core_log_error("wsnetclient: handshake FAILED, resp_code=%d", hm->resp_code);
                        wc->connecting = false;
                        return;
                }
                
                wc->connected = true;
                wc->connecting = false;

                // 发送连接消息
                joy_net_message_t msg;
                msg.type = NET_TYPE_CONNECTED;
                msg.conv = 0;
                msg.data = joy_core_utils_strdup("connected", NULL);
                msg.len = (int)strlen(msg.data);
                if (wc->cb) {
                        wc->cb(&msg, wc->userdata);
                } else {
                        *kl_pushp(kmq, wc->mq) = msg;
                }
        }
        else if (ev == MG_EV_WEBSOCKET_FRAME) {
                // 收到 WebSocket 消息
                struct websocket_message* wm = (struct websocket_message*)ev_data;
                int payload_len = (int)wm->size;

                if (payload_len > 0) {
                        joy_net_message_t msg;
                        msg.type = NET_TYPE_MESSAGE;
                        msg.conv = 0;
                        msg.data = (char*)malloc(payload_len);
                        if (msg.data) {
                                memcpy(msg.data, wm->data, payload_len);
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
                        wc->connected = false;
                        joy_net_message_t msg;
                        msg.type = NET_TYPE_DISCONNECTED;
                        msg.conv = 0;
                        msg.data = joy_core_utils_strdup("disconnected", NULL);
                        msg.len = (int)strlen(msg.data);
                        if (wc->cb) {
                                wc->cb(&msg, wc->userdata);
                        } else {
                                *kl_pushp(kmq, wc->mq) = msg;
                        }
                }
                wc->conn = NULL;
        }
}

#else

struct joy_wsnetclient {
        int ws_socket;                 // emscripten WebSocket socket descriptor
        bool connected;                // 连接状态
        char url[512];                 // WebSocket URL
        klist_t(kmq)* mq;            // Message queue
        net_callback cb;               // Global callback
        void* userdata;                // Global user data
        bool initialized;              // Init flag
        bool connecting;               // 连接中标志
};


// ==============================
// Emscripten WebSocket Client 实现
// ==============================

// Emscripten WebSocket 事件回调 (返回 bool 以符合 emscripten 6.x API)
static bool em_ws_onopen(int eventType, const EmscriptenWebSocketOpenEvent* e, void* userData)
{
        (void)eventType;
        (void)e;
        joy_wsnetclient_p wc = (joy_wsnetclient_p)userData;
        if (!wc) return true;

        joy_core_log_info("emscripten_ws: connected!");

        wc->connected = true;
        wc->connecting = false;

        // 发送连接消息
        joy_net_message_t msg;
        msg.type = NET_TYPE_CONNECTED;
        msg.conv = 0;
        msg.data = joy_core_utils_strdup("connected", NULL);
        msg.len = (int)strlen(msg.data);
        if (wc->cb) {
                wc->cb(&msg, wc->userdata);
                free(msg.data);
        }
        else {
                *kl_pushp(kmq, wc->mq) = msg;
        }
        return true;
}

static bool em_ws_onmessage(int eventType, const EmscriptenWebSocketMessageEvent* e, void* userData)
{
        (void)eventType;
        joy_wsnetclient_p wc = (joy_wsnetclient_p)userData;
        if (!wc) return true;

        if (e->numBytes > 0) {
                joy_net_message_t msg;
                msg.type = NET_TYPE_MESSAGE;
                msg.conv = 0;
                msg.len = (int)e->numBytes;
                msg.data = (char*)malloc(msg.len + 1);
                if (msg.data) {
                        memcpy(msg.data, e->data, msg.len);
                        msg.data[msg.len] = 0;

                        if (wc->cb) {
                                wc->cb(&msg, wc->userdata);
                                free(msg.data);
                        }
                        else {
                                *kl_pushp(kmq, wc->mq) = msg;
                        }
                }
        }
        return true;
}

static bool em_ws_onerror(int eventType, const EmscriptenWebSocketErrorEvent* e, void* userData)
{
        (void)eventType;
        (void)e;
        joy_wsnetclient_p wc = (joy_wsnetclient_p)userData;
        if (!wc) return true;
        joy_core_log_error("emscripten_ws: error");
        return true;
}

static bool em_ws_onclose(int eventType, const EmscriptenWebSocketCloseEvent* e, void* userData)
{
        (void)eventType;
        (void)e;
        joy_wsnetclient_p wc = (joy_wsnetclient_p)userData;
        if (!wc) return true;

        wc->connected = false;
        wc->connecting = false;

        joy_net_message_t msg;
        msg.type = NET_TYPE_DISCONNECTED;
        msg.conv = 0;
        msg.data = joy_core_utils_strdup("disconnected", NULL);
        msg.len = (int)strlen(msg.data);
        if (wc->cb) {
                wc->cb(&msg, wc->userdata);
                free(msg.data);
        }
        else {
                *kl_pushp(kmq, wc->mq) = msg;
        }
        return true;
}

joy_wsnetclient_p joy_net_wsnetclient_create(const char* url)
{
        joy_wsnetclient_p wc = (joy_wsnetclient_p)malloc(sizeof(joy_wsnetclient_t));
        if (!wc) return NULL;
        memset(wc, 0, sizeof(joy_wsnetclient_t));

        wc->connected = false;
        wc->connecting = true;
        wc->mq = kl_init(kmq);
        wc->cb = NULL;
        wc->userdata = NULL;
        wc->initialized = false;

        if (url) {
                strncpy(wc->url, url, sizeof(wc->url));
        }

        joy_core_log_info("emscripten_ws: === Starting connection to %s ===", url);

        // 验证 URL 格式
        if (strncmp(url, "ws://", 5) != 0 && strncmp(url, "wss://", 6) != 0) {
                joy_core_log_error("emscripten_ws: FATAL - Invalid WebSocket URL format: %s", url);
                kl_destroy(kmq, wc->mq);
                free(wc);
                return NULL;
        }

        // 使用 Emscripten 原生 WebSocket API
        EmscriptenWebSocketCreateAttributes attr;
        emscripten_websocket_init_create_attributes(&attr);
        attr.url = url;
        wc->ws_socket = emscripten_websocket_new(&attr);
        if (wc->ws_socket < 0) {
                joy_core_log_error("emscripten_ws: FATAL - failed to create WebSocket for %s", url);
                kl_destroy(kmq, wc->mq);
                free(wc);
                return NULL;
        }

        // 设置回调
        emscripten_websocket_set_onopen_callback(wc->ws_socket, wc, &em_ws_onopen);
        emscripten_websocket_set_onmessage_callback(wc->ws_socket, wc, &em_ws_onmessage);
        emscripten_websocket_set_onerror_callback(wc->ws_socket, wc, &em_ws_onerror);
        emscripten_websocket_set_onclose_callback(wc->ws_socket, wc, &em_ws_onclose);

        wc->initialized = true;
        joy_core_log_info("emscripten_ws: connection initiated, waiting for open...");
        return wc;
}

void joy_net_wsnetclient_destroy(joy_wsnetclient_p wc)
{
        if (!wc) return;

        if (wc->ws_socket >= 0 && wc->initialized) {
                emscripten_websocket_close(wc->ws_socket, 1000, "bye");
                emscripten_websocket_delete(wc->ws_socket);
        }

        // Free messages
        kliter_t(kmq)* p;
        for (p = kl_begin(wc->mq); p != kl_end(wc->mq); p = kl_next(p)) {
                joy_net_message_t* msg = &kl_val(p);
                if (msg->data) free(msg->data);
        }
        kl_destroy(kmq, wc->mq);

        free(wc);
}

int joy_net_wsnetclient_send(joy_wsnetclient_p wc, const char* data, int len)
{
        if (!wc || !wc->connected || !data || len <= 0) {
                return -1;
        }

        int result = emscripten_websocket_send_binary(wc->ws_socket, (void*)data, (size_t)len);
        if (result == 0) {
                return len;
        }
        else {
                joy_core_log_error("emscripten_ws: send failed, result=%d", result);
                return -1;
        }
}

void joy_net_wsnetclient_update(joy_wsnetclient_p wc)
{
        // Emscripten 原生 WebSocket 是异步回调，无需轮询
        (void)wc;
}

bool joy_net_wsnetclient_poll_message(joy_wsnetclient_p wc, joy_net_message_p msg)
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

void joy_net_wsnetclient_set_callback(joy_wsnetclient_p wc, net_callback cb, void* userdata)
{
        if (!wc) return;
        wc->cb = cb;
        wc->userdata = userdata;
}


#endif // JOY_EMSCRIPTEN - Mongoose WebSocket 客户端


// ==============================
// Unified NetClient (封装 kcp/tcp/ws)
// ==============================
struct joy_netclient
{
        net_client_type type;
        union {
                joy_kcpclient_p kcp;
                joy_tcpclient_p tcp;
                joy_wsnetclient_p ws; // mongoose WebSocket 客户端
                void* ptr;
        } client;
        net_callback cb;
        void* userdata;
};

joy_netclient_p joy_net_netclient_create(net_client_type type, const char* host, int port)
{
        joy_netclient_p nc = (joy_netclient_p)malloc(sizeof(joy_netclient_t));
        if (!nc) return NULL;

        memset(nc, 0, sizeof(joy_netclient_t));
        nc->type = type;
        nc->cb = NULL;
        nc->userdata = NULL;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                nc->client.kcp = joy_net_kcpclient_create(host, port);
                if (!nc->client.kcp) {
                        free(nc);
                        return NULL;
                }
                break;
        case NET_CLIENT_TCP:
                nc->client.tcp = joy_net_tcpclient_create(host, port);
                if (!nc->client.tcp) {
                        free(nc);
                        return NULL;
                }
                break;
        case NET_CLIENT_WEBSOCKET:
                {
                        char url[512];
                        if (strncmp(host, "ws://", 5) == 0 || strncmp(host, "wss://", 6) == 0) {
                                strncpy(url, host, sizeof(url));
                        } else {
                                snprintf(url, sizeof(url), "ws://%s:%d/", host, port);
                        }
                        nc->client.ws = joy_net_wsnetclient_create(url);
                        if (!nc->client.ws) {
                                free(nc);
                                return NULL;
                        }
                }
                break;
        default:
                free(nc);
                return NULL;
        }

        return nc;
}

void joy_net_netclient_destroy(joy_netclient_p nc)
{
        if (!nc) return;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                if (nc->client.kcp) joy_net_kcpclient_destroy(nc->client.kcp);
                break;
        case NET_CLIENT_TCP:
                if (nc->client.tcp) joy_net_tcpclient_destroy(nc->client.tcp);
                break;
        case NET_CLIENT_WEBSOCKET:
                if (nc->client.ws) joy_net_wsnetclient_destroy(nc->client.ws);
                break;
        default:
                break;
        }

        free(nc);
}

int joy_net_netclient_send(joy_netclient_p nc, const char* data, int len)
{
        if (!nc) return -1;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                return joy_net_kcpclient_send(nc->client.kcp, data, len);
        case NET_CLIENT_TCP:
                return joy_net_tcpclient_send(nc->client.tcp, data, len);
        case NET_CLIENT_WEBSOCKET:
                return joy_net_wsnetclient_send(nc->client.ws, data, len);
        default:
                return -1;
        }
}

void joy_net_netclient_update(joy_netclient_p nc)
{
        if (!nc) return;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                joy_net_kcpclient_update(nc->client.kcp);
                break;
        case NET_CLIENT_TCP:
                joy_net_tcpclient_update(nc->client.tcp);
                break;
        case NET_CLIENT_WEBSOCKET:
                joy_net_wsnetclient_update(nc->client.ws);
                break;
        default:
                break;
        }
}

bool joy_net_netclient_poll_message(joy_netclient_p nc, joy_net_message_p msg)
{
        if (!nc || !msg) return false;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                return joy_net_kcpclient_poll_message(nc->client.kcp, msg);
        case NET_CLIENT_TCP:
                return joy_net_tcpclient_poll_message(nc->client.tcp, msg);
        case NET_CLIENT_WEBSOCKET:
                return joy_net_wsnetclient_poll_message(nc->client.ws, msg);
        default:
                return false;
        }
}

void joy_net_netclient_set_callback(joy_netclient_p nc, net_callback cb, void* userdata)
{
        if (!nc) return;

        nc->cb = cb;
        nc->userdata = userdata;

        switch (nc->type) {
        case NET_CLIENT_KCP:
                joy_net_kcpclient_set_callback(nc->client.kcp, cb, userdata);
                break;
        case NET_CLIENT_TCP:
                // tcpclient 没有回调接口
                break;
        case NET_CLIENT_WEBSOCKET:
                joy_net_wsnetclient_set_callback(nc->client.ws, cb, userdata);
                break;
        default:
                break;
        }
}

net_client_type joy_net_netclient_get_type(joy_netclient_p nc)
{
        return nc->type;
}

// ==============================
// Unified NetServer (封装 kcp/tcp/ws 服务器)
// ==============================

struct joy_netserver
{
        net_server_type type;
        union {
                joy_kcpserver_p kcp;
                joy_tcpserver_p tcp;
                joy_wsnetserver_p ws;
                void* ptr;
        } server;
        net_callback cb;
        void* userdata;
};

joy_netserver_p joy_net_netserver_create(net_server_type type, const char* ip, int port)
{
        joy_netserver_p ns = (joy_netserver_p)malloc(sizeof(joy_netserver_t));
        if (!ns) return NULL;

        memset(ns, 0, sizeof(joy_netserver_t));
        ns->type = type;
        ns->cb = NULL;
        ns->userdata = NULL;

        switch (type) {
        case NET_SERVER_KCP:
                ns->server.kcp = joy_net_kcpserver_create(ip, port);
                if (!ns->server.kcp) {
                        free(ns);
                        return NULL;
                }
                break;
        case NET_SERVER_TCP:
                ns->server.tcp = joy_net_tcpserver_create(ip, port);
                if (!ns->server.tcp) {
                        free(ns);
                        return NULL;
                }
                break;
        case NET_SERVER_WEBSOCKET:
                ns->server.ws = joy_net_wsnetserver_create(ip, port);
                if (!ns->server.ws) {
                        free(ns);
                        return NULL;
                }
                break;
        default:
                free(ns);
                return NULL;
        }

        return ns;
}

void joy_net_netserver_destroy(joy_netserver_p ns)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                if (ns->server.kcp) joy_net_kcpserver_destroy(ns->server.kcp);
                break;
        case NET_SERVER_TCP:
                if (ns->server.tcp) joy_net_tcpserver_destroy(ns->server.tcp);
                break;
        case NET_SERVER_WEBSOCKET:
                if (ns->server.ws) joy_net_wsnetserver_destroy(ns->server.ws);
                break;
        default:
                break;
        }

        free(ns);
}

void joy_net_netserver_send(joy_netserver_p ns, int conv, const char* data, int len)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                joy_net_kcpserver_send(ns->server.kcp, conv, data, len);
                break;
        case NET_SERVER_TCP:
                joy_net_tcpserver_send(ns->server.tcp, conv, data, len);
                break;
        case NET_SERVER_WEBSOCKET:
                joy_net_wsnetserver_send(ns->server.ws, conv, data, len);
                break;
        default:
                break;
        }
}

void joy_net_netserver_broadcast(joy_netserver_p ns, const char* data, int len)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                joy_net_kcpserver_broadcast(ns->server.kcp, data, len);
                break;
        case NET_SERVER_TCP:
                joy_net_tcpserver_broadcast(ns->server.tcp, data, len);
                break;
        case NET_SERVER_WEBSOCKET:
                joy_net_wsnetserver_broadcast(ns->server.ws, data, len);
                break;
        default:
                break;
        }
}

void joy_net_netserver_offline(joy_netserver_p ns, int conv)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                joy_net_kcpserver_offline(ns->server.kcp, conv);
                break;
        case NET_SERVER_TCP:
                joy_net_tcpserver_offline(ns->server.tcp, conv);
                break;
        case NET_SERVER_WEBSOCKET:
                joy_net_wsnetserver_offline(ns->server.ws, conv);
                break;
        default:
                break;
        }
}

void joy_net_netserver_update(joy_netserver_p ns)
{
        if (!ns) return;

        switch (ns->type) {
        case NET_SERVER_KCP:
                joy_net_kcpserver_update(ns->server.kcp);
                break;
        case NET_SERVER_TCP:
                joy_net_tcpserver_update(ns->server.tcp);
                break;
        case NET_SERVER_WEBSOCKET:
                joy_net_wsnetserver_update(ns->server.ws);
                break;
        default:
                break;
        }
}

int joy_net_netserver_connection_count(joy_netserver_p ns)
{
        if (!ns) return 0;

        switch (ns->type) {
        case NET_SERVER_KCP:
                return joy_net_kcpserver_connection_count(ns->server.kcp);
        case NET_SERVER_TCP:
                return joy_net_tcpserver_connection_count(ns->server.tcp);
        case NET_SERVER_WEBSOCKET:
                return joy_net_wsnetserver_connection_count(ns->server.ws);
        default:
                return 0;
        }
}

bool joy_net_netserver_poll_message(joy_netserver_p ns, joy_net_message_p msg)
{
        if (!ns || !msg) return false;

        switch (ns->type) {
        case NET_SERVER_KCP:
                return joy_net_kcpserver_poll_message(ns->server.kcp, msg);
        case NET_SERVER_TCP:
                return joy_net_tcpserver_poll_message(ns->server.tcp, msg);
        case NET_SERVER_WEBSOCKET:
                return joy_net_wsnetserver_poll_message(ns->server.ws, msg);
        default:
                return false;
        }
}

void joy_net_netserver_set_callback(joy_netserver_p ns, net_callback cb, void* userdata)
{
        if (!ns) return;

        ns->cb = cb;
        ns->userdata = userdata;

        switch (ns->type) {
        case NET_SERVER_KCP:
                joy_net_kcpserver_set_callback(ns->server.kcp, cb, userdata);
                break;
        case NET_SERVER_TCP:
                // tcpserver 没有回调接口
                break;
        case NET_SERVER_WEBSOCKET:
                joy_net_wsnetserver_set_callback(ns->server.ws, cb, userdata);
                break;
        default:
                break;
        }
}

net_server_type joy_net_netserver_get_type(joy_netserver_p ns)
{
        return ns->type;
}

void joy_net_netmessage_free_data(joy_net_message_p msg)
{
        if (msg && msg->data) {
                free(msg->data);
                msg->data = NULL;
                msg->len = 0;
        }
}
