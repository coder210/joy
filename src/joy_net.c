/******************************************************************************
 * src/joy_net.c —— net 域唯一实现文件（一个域一个 .c，配 joy_net.h）
 * 职责：网络 / 平台系统适配
 * 合并自：jnetwork.c / jsys.c
 * 说明：本域公开 API 声明在配套头 src/joy_net.h，由 include/joy/japi.h 聚合包含；
 *       本文件是该域唯一的编译单元。
 *       内部符号（static / 私有宏 / typedef）保持各分片独立，未跨分片改名。
 ******************************************************************************/

#include "joy_net.h"

/* ==================== 合并自 src/jnetwork.c ==================== */
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
        joy_net_callback cb;
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
        joy_net_callback cb;
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
        joy_net_callback cb;               // Global callback
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

void joy_net_kcpserver_set_callback(joy_kcpserver_p ks, joy_net_callback cb, void* userdata)
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

void joy_net_kcpclient_set_callback(joy_kcpclient_p kc, joy_net_callback cb, void* userdata)
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

void joy_net_wsnetserver_set_callback(joy_wsnetserver_p ws, joy_net_callback cb, void* userdata)
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
        joy_net_callback cb;               // Global callback
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

void joy_net_wsnetclient_set_callback(joy_wsnetclient_p wc, joy_net_callback cb, void* userdata)
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
        joy_net_callback cb;               // Global callback
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

void joy_net_wsnetclient_set_callback(joy_wsnetclient_p wc, joy_net_callback cb, void* userdata)
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
        joy_net_client_type type;
        union {
                joy_kcpclient_p kcp;
                joy_tcpclient_p tcp;
                joy_wsnetclient_p ws; // mongoose WebSocket 客户端
                void* ptr;
        } client;
        joy_net_callback cb;
        void* userdata;
};

joy_netclient_p joy_net_netclient_create(joy_net_client_type type, const char* host, int port)
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

void joy_net_netclient_set_callback(joy_netclient_p nc, joy_net_callback cb, void* userdata)
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

joy_net_client_type joy_net_netclient_get_type(joy_netclient_p nc)
{
        return nc->type;
}

// ==============================
// Unified NetServer (封装 kcp/tcp/ws 服务器)
// ==============================

struct joy_netserver
{
        joy_net_server_type type;
        union {
                joy_kcpserver_p kcp;
                joy_tcpserver_p tcp;
                joy_wsnetserver_p ws;
                void* ptr;
        } server;
        joy_net_callback cb;
        void* userdata;
};

joy_netserver_p joy_net_netserver_create(joy_net_server_type type, const char* ip, int port)
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

void joy_net_netserver_set_callback(joy_netserver_p ns, joy_net_callback cb, void* userdata)
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

joy_net_server_type joy_net_netserver_get_type(joy_netserver_p ns)
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

/* ==================== 结束 src/jnetwork.c ==================== */

/* ==================== 合并自 src/jsys.c ==================== */
#include "joy.h"
#include <string.h>

#if defined(__ANDROID__)
#  include <unistd.h>
#  include <net/if.h>
#  include <sys/ioctl.h>
#  include <sys/socket.h>
#  include <arpa/inet.h>
#endif

/* backtrace() 可用性（crashHandler 用它采集调用栈）：
 *  - Linux / macOS / Unix(glic 系) 的 <execinfo.h> 提供 backtrace / backtrace_symbols_fd；
 *  - Android(bionic) 直到 API 33 才在 <execinfo.h> 公开这两个函数，本项目按 API 21 构建，
 *    因此在 Android 上跳过栈回溯（仍然打印信号信息），否则 C99 下会被判为隐式函数声明。 */
#if defined(__ANDROID__)
#  ifndef __ANDROID_API__
#    define JOY_HAVE_BACKTRACE 0
#  elif __ANDROID_API__ >= 33
#    define JOY_HAVE_BACKTRACE 1
#  else
#    define JOY_HAVE_BACKTRACE 0
#  endif
#else
#  define JOY_HAVE_BACKTRACE 1
#endif

#if defined(JOY_WIN)
#	include <time.h>
#	include <WinSock2.h>
#	include <mswsock.h>
#	include <Windows.h>
#	include <ws2tcpip.h>
#       include <DbgHelp.h>
#	include <Iphlpapi.h>
#	pragma comment(lib, "ws2_32.lib")
#       pragma comment(lib, "DbgHelp.lib")
#	pragma comment(lib, "Iphlpapi.lib")



static LONG WINAPI crashHandler(EXCEPTION_POINTERS* ep)
{
        char dumpPath[MAX_PATH];
        SYSTEMTIME st;
        GetLocalTime(&st);
        snprintf(dumpPath, sizeof(dumpPath), "crash_%04d%02d%02d_%02d%02d%02d.dmp",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        HANDLE hFile = CreateFileA(dumpPath, GENERIC_WRITE, 0, NULL,
                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
                MINIDUMP_EXCEPTION_INFORMATION ei = { GetCurrentThreadId(), ep, FALSE };
                MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                        hFile, MiniDumpWithDataSegs, &ei, NULL, NULL);
                CloseHandle(hFile);
        }
        return EXCEPTION_CONTINUE_SEARCH;
}


#elif defined(JOY_LINUX)
#	include<unistd.h>
#	include<sys/time.h>
#	include<sys/socket.h>
#	include<arpa/inet.h>
#	include<netdb.h>        // getaddrinfo / struct addrinfo / freeaddrinfo（安卓 bionic 不会自动包含）
#	include<errno.h>
#	include<sys/ioctl.h>
#	include<sys/fcntl.h>
#       include<signal.h>
#       include<execinfo.h>
#       include <fcntl.h>
#       include <time.h>


static void crashHandler(int sig)
{
        // 打印信号信息
        char msg[128];
        snprintf(msg, sizeof(msg), "\n======== CRASH ======== signal=%d (%s)\n",
                sig, sig == SIGSEGV ? "SIGSEGV" : sig == SIGABRT ? "SIGABRT" : "other");
        write(STDERR_FILENO, msg, strlen(msg));

        // 采集调用堆栈
#if JOY_HAVE_BACKTRACE
        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
#endif

        // 写入日志文件
        time_t t = time(NULL);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
#if JOY_HAVE_BACKTRACE
                backtrace_symbols_fd(buffer, frames, fd);
#endif
                close(fd);
        }

        // 恢复默认信号处理并重新触发，让系统生成 core dump
        signal(sig, SIG_DFL);
        raise(sig);
}

#elif defined(JOY_MACOS)
#   include <unistd.h>
#   include <sys/time.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <errno.h>
#   include <sys/ioctl.h>
#   include <sys/fcntl.h>
#   include <signal.h>
#   include <execinfo.h>
#   include <fcntl.h>
#   include <time.h>

static void crashHandler(int sig)
{
        char msg[128];
        snprintf(msg, sizeof(msg), "\n======== CRASH ======== signal=%d (%s)\n",
                sig, sig == SIGSEGV ? "SIGSEGV" : sig == SIGABRT ? "SIGABRT" : "other");
        write(STDERR_FILENO, msg, strlen(msg));

#if JOY_HAVE_BACKTRACE
        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
#endif

        time_t t = time(NULL);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
#if JOY_HAVE_BACKTRACE
                backtrace_symbols_fd(buffer, frames, fd);
#endif
                close(fd);
        }

        signal(sig, SIG_DFL);
        raise(sig);
}

#elif defined(JOY_UNIX)
// 适用于 FreeBSD, Solaris 等支持 execinfo.h 的 Unix 系统
#   include <unistd.h>
#   include <sys/time.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <errno.h>
#   include <sys/ioctl.h>
#   include <sys/fcntl.h>
#   include <signal.h>
#   include <execinfo.h>
#   include <fcntl.h>
#   include <time.h>

static void crashHandler(int sig)
{
        char msg[128];
        snprintf(msg, sizeof(msg), "\n======== CRASH ======== signal=%d (%s)\n",
                sig, sig == SIGSEGV ? "SIGSEGV" : sig == SIGABRT ? "SIGABRT" : "other");
        write(STDERR_FILENO, msg, strlen(msg));

#if JOY_HAVE_BACKTRACE
        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
#endif

        time_t t = time(NULL);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
#if JOY_HAVE_BACKTRACE
                backtrace_symbols_fd(buffer, frames, fd);
#endif
                close(fd);
        }

        signal(sig, SIG_DFL);
        raise(sig);
}
#else

static void crashHandler(int sig)
{
       // unkonwn
}

#endif

// 统一为非 Windows（POSIX-like，含 Emscripten / Web）补充网络相关头文件。
// 各平台分支只包含了其中一部分；Emscripten 走上面的 #else（不含任何头文件），
// 导致 getaddrinfo / inet_ntop / gethostname 等未声明，Web CI 编译失败。
// 这里集中补齐，重复包含（含 Linux/macOS/UNIX 已包含的）无害（头文件有 include guard）。
#if !defined(JOY_WIN)
#	include <unistd.h>        // gethostname
#	include <sys/socket.h>    // AF_INET / SOCK_STREAM
#	include <arpa/inet.h>     // inet_ntop
#	include <netdb.h>         // getaddrinfo / struct addrinfo / freeaddrinfo
#endif

bool joy_net_sys_init_netenv(void)
{
#if defined(JOY_WIN)
	WSADATA data;
	int r = WSAStartup(0x0201, &data);
	return r == 0;
#elif defined(JOY_LINUX)
	return true;
#elif defined(JOY_MACOS)
	return true;
#elif defined(JOY_UNIX)
	return true;
#else
	return false;
#endif
}

bool joy_net_sys_release_netenv(void)
{
#if defined(JOY_WIN)
	int r;
	r = WSACleanup();
	return r == 0;   /* WSACleanup 成功返回 0, 失败返回 SOCKET_ERROR */
#elif defined(JOY_LINUX)
	return true;
#elif defined(JOY_MACOS)
	return true;
#elif defined(JOY_UNIX)
	return true;
#else
	return false;
#endif
	return false;
}

int joy_net_sys_anyaddr(void)
{
	/* INADDR_ANY 恒为 0, 网络字节序下仍是 0。原来 macOS/UNIX 分支返回
	 * true(1), 会把绑定地址变成 1.0.0.0 导致 bind 失败。 */
	return 0;
}

bool joy_net_sys_closesocket(int64_t sockfd)
{
#if defined(JOY_WIN)
	return closesocket((SOCKET)sockfd) == 0;
#elif defined(JOY_LINUX) || defined(JOY_MACOS) || defined(JOY_UNIX)
	return close((int)sockfd) == 0;
#else
	return false;
#endif
}

int joy_net_sys_set_sock_sndtimeo(int64_t sockfd, int ms)
{
	int r, sz;

	r = 0;
#if defined(JOY_WIN)
	sz = (int)sizeof(int);
	r = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&ms, sz);
#elif defined(JOY_LINUX)
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;
	sz = (int)sizeof(tv);
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sz);
#elif defined(JOY_MACOS)

#elif defined(JOY_UNIX)

#else

#endif
	return r;
}

int joy_net_sys_set_sock_rcvtimeo(int64_t sockfd, int ms)
{
	int r, sz;

	r = 0;
#if defined(JOY_WIN)
	sz = (int)sizeof(int);
	r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sz);
#elif defined(JOY_LINUX)
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sizeof(int));
#elif defined(JOY_MACOS)
#elif defined(JOY_UNIX)
#else
#endif
	return r;
}

int joy_net_sys_set_sock_accpettimeo(int64_t sockfd, int ms)
{
	int r, sz;

	r = 0;
#if defined(JOY_WIN)
	sz = (int)sizeof(int);
        r = ioctlsocket(sockfd, FIONBIO, (u_long*)&ms);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sz);
#elif defined(JOY_LINUX)
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sizeof(int));
#elif defined(JOY_MACOS)
#elif defined(JOY_UNIX)
#else
#endif
	return r;
}

int joy_net_sys_set_sock_broadcast(int64_t sockfd)
{
	int r;
#if defined(JOY_WIN)
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char*)&on, (int)sizeof(on));
#elif defined(JOY_LINUX)
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, (socklen_t)sizeof(on));
#else
	r = -1;
#endif
	return r;
}

int joy_net_sys_set_sock_reuseaddr(int64_t sockfd)
{
	int r;
#if defined(JOY_WIN)
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, (int)sizeof(on));
#elif defined(JOY_LINUX)
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, (socklen_t)sizeof(on));
#else
	r = -1;
#endif
	return r;
}

int64_t joy_net_sys_tcp()
{
	int64_t sockfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
#else
	sockfd = 0;
#endif
	return sockfd;
}

int64_t joy_net_sys_udp()
{
	int64_t sockfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
#else
	sockfd = 0;
#endif
	return sockfd;
}

bool joy_net_sys_bind(int64_t sockfd, const char* ip, int port)
{
	bool resval;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = inet_addr(ip);
	peer.sin_port = htons(port);
	resval = bind(sockfd, (struct sockaddr*)&peer, sizeof(peer)) == 0;
#else
	resval = false;
#endif
	return resval;
}

bool joy_net_sys_connect(int64_t sockfd, const char* ip, int port)
{
	bool resval;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = inet_addr(ip);
	peer.sin_port = htons(port);
	resval = connect(sockfd, (struct sockaddr*)&peer, sizeof(peer)) == 0;
#else
	resval = false;
#endif
	return resval;
}

bool joy_net_sys_listen(int64_t sockfd)
{
	bool resval;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	resval = listen(sockfd, SOMAXCONN) == 0;
#else
	resval = false;
#endif
	return resval;
}

int64_t joy_net_sys_accept(int64_t sockfd, char* ip, int* port)
{
	int64_t acceptfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in client;
#if defined(JOY_WIN)
	int len = (int)sizeof(client);
#else
	socklen_t len = sizeof(client);
#endif
	acceptfd = accept(sockfd, (struct sockaddr*)&client, &len);
	strncpy(ip, inet_ntoa(client.sin_addr), JOY_MAX_IP);
	*port = ntohs(client.sin_port);
#else
	acceptfd = 0;
#endif
	return acceptfd;
}

int joy_net_sys_send(int64_t sockfd, const char* buf, int len)
{
	int n;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	n = send(sockfd, buf, len, 0);
#else
	n = -1;
#endif
	return n;
}

int joy_net_sys_recv(int64_t sockfd, char* buf)
{
	int n;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	/* 调用方的 buf 是 char[JOY_MAX_BUFFER], 留 1 字节给结尾的 '\0' */
	n = recv(sockfd, buf, JOY_MAX_BUFFER - 1, 0);
	if (n > 0) buf[n] = 0;
#else
	n = -1;
#endif
	return n;
}

int joy_net_sys_recvfrom(int64_t sockfd, char* buf, char* ip, int* port)
{
	int n;
#if defined(JOY_WIN)
	int len;
#else
	socklen_t len;
#endif
	if (ip) ip[0] = '\0';
	if (port) *port = 0;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	len = sizeof(peer);
	n = recvfrom(sockfd, buf, JOY_MAX_BUFFER, 0,
		(struct sockaddr*)&peer, &len);
	/* 只有成功收到数据时 peer 才被填充, 失败时不能读未初始化内容 */
	if (n >= 0 && ip && port) {
		strcpy(ip, inet_ntoa(peer.sin_addr));
		*port = ntohs(peer.sin_port);
	}
#else
	n = -1;
#endif
	return n;
}

int joy_net_sys_sendto(int64_t sockfd, const char* buf, int len,
	const char* ip, int port)
{
	int n;
	size_t sz;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = inet_addr(ip);
	peer.sin_port = htons(port);
	sz = sizeof(peer);
	n = sendto(sockfd, buf, len, 0, (struct sockaddr*)&peer, sz);
#else
	n = -1;
#endif
	return n;
}

int64_t joy_net_sys_current_time()
{
	int64_t current_time;
#if defined(JOY_WIN)
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	int64_t sec = mktime(&tm);
	current_time = sec * 1000 + wtm.wMilliseconds;
#elif defined(JOY_LINUX)
	struct timeval tv;
	gettimeofday(&tv, NULL);
	current_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#elif defined(JOY_MACOS)
	current_time = 0;
#elif defined(JOY_UNIX)
	current_time = 0;
#else
	current_time = 0;
#endif
	return current_time;
}

void joy_net_sys_sleep(int ms)
{
#if defined(JOY_WIN)
	Sleep(ms);
#elif defined(JOY_LINUX)
	usleep(ms * 1000);
#elif defined(JOY_MACOS)
#elif defined(JOY_UNIX)
#else
#endif
}


void joy_net_sys_install_crash_handlers()
{
#if defined(JOY_WIN)
        SetUnhandledExceptionFilter(crashHandler);
#elif defined(JOY_LINUX)
        signal(SIGSEGV, crashHandler);
        signal(SIGABRT, crashHandler);
        signal(SIGFPE, crashHandler);
        signal(SIGILL, crashHandler);
#elif defined(JOY_MACOS)
        signal(SIGSEGV, crashHandler);
        signal(SIGABRT, crashHandler);
        signal(SIGFPE, crashHandler);
        signal(SIGILL, crashHandler);
#elif defined(JOY_UNIX)
        signal(SIGSEGV, crashHandler);
        signal(SIGABRT, crashHandler);
        signal(SIGFPE, crashHandler);
        signal(SIGILL, crashHandler);
#else
	printf("unknown platform, crash handlers not installed");
#endif
}


#ifdef JOY_WIN

// 客户端会话结构
typedef struct joy_iocp_client_session
{
        char client_ip[JOY_MAX_IP];
        int client_port;
        SOCKET client_socket;
        int heartbeat_count;          // 心跳计数器
        bool is_handshaked;           // 是否已完成握手
        bool is_active;               // 会话是否活跃
        // unsigned char send_buffer[MAX_SEND_PKG];
} iocp_client_session_t, * iocp_client_session_ptr;

// IOCP操作包结构
typedef struct joy_iocp_operation_packet {
        WSAOVERLAPPED overlapped;
        WSABUF wsabuffer;
        int operation_type;           // 操作类型
        SOCKET target_socket;         // 目标套接字
        int bytes_received;           // 已接收字节数
        unsigned char* long_buffer;   // 长包缓冲区
        int max_buffer_length;        // 缓冲区最大长度
        unsigned char buffer[JOY_MAX_BUFFER + 1];
        int direction;                // 0:接收, 1:发送
        iocp_client_session_ptr associated_session; // 关联的客户端会话
} joy_iocp_operation_packet_t, * joy_iocp_operation_packet_ptr;

// IOCP服务器实例结构
typedef struct joy_iocp_server
{
        HANDLE completion_port;               // IOCP完成端口句柄
        SOCKET listen_socket;                 // 监听套接字
        enum joy_packet_format {
                PACKET_FORMAT_TCP_BINARY = 0, // TCP二进制封包
                PACKET_FORMAT_TCP_JSON,       // TCP JSON封包
                PACKET_FORMAT_WEBSOCKET_JSON  // WebSocket JSON封包
        } packet_format;
        int heartbeat_interval;               // 心跳检测间隔(秒)
        enum joy_iocp_operation {
                IOCP_OPERATION_ACCEPT = 0,
                IOCP_OPERATION_RECEIVE,
                IOCP_OPERATION_SEND,
        } iocp_operation_type;
        LPFN_ACCEPTEX acceptex_function;
        LPFN_GETACCEPTEXSOCKADDRS get_acceptex_sockaddrs_function;
        bool is_running;                      // 服务器运行状态
        CRITICAL_SECTION critical_section;    // 线程同步临界区

} joy_iocp_server_t, * joy_iocp_server_p;

// 函数声明
static void iocp_post_accept_operation(joy_iocp_server_p server, joy_iocp_operation_packet_ptr packet);
static int iocp_post_receive_operation(iocp_client_session_ptr session);
static void iocp_handle_accept_completion(joy_iocp_server_p server, joy_iocp_operation_packet_ptr packet);
static void iocp_handle_receive_completion(joy_iocp_operation_packet_ptr packet, DWORD bytes_transferred, iocp_client_session_ptr session);
static void iocp_cleanup_client_session(iocp_client_session_ptr session);

// 清理客户端会话资源
static void iocp_cleanup_client_session(iocp_client_session_ptr session)
{
        if (!session) return;

        if (session->client_socket != INVALID_SOCKET) {
                closesocket(session->client_socket);
                session->client_socket = INVALID_SOCKET;
        }

        free(session);
}

// 投递AcceptEx操作
static void iocp_post_accept_operation(joy_iocp_server_p server, joy_iocp_operation_packet_ptr packet)
{
        if (!server || !packet) return;

        packet->wsabuffer.buf = (CHAR*)packet->buffer;
        packet->wsabuffer.len = JOY_MAX_BUFFER;
        packet->operation_type = IOCP_OPERATION_ACCEPT;

        // 创建用于AcceptEx的客户端套接字
        packet->target_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                NULL, 0, WSA_FLAG_OVERLAPPED);
        if (packet->target_socket == INVALID_SOCKET) {
                //printf("WSASocket for client failed: %d", WSAGetLastError());
                return;
        }

        DWORD bytes_received = 0;
        int address_size = (sizeof(struct sockaddr_in) + 16);

        // 使用AcceptEx异步接受连接
        if (!server->acceptex_function(server->listen_socket,
                packet->target_socket,
                packet->wsabuffer.buf, 0,
                address_size, address_size,
                &bytes_received, &packet->overlapped)) {
                if (WSAGetLastError() != ERROR_IO_PENDING) {
                        //printf("AcceptEx operation failed: %d", WSAGetLastError());
                        closesocket(packet->target_socket);
                }
        }
}

// 投递接收数据操作
static int iocp_post_receive_operation(iocp_client_session_ptr session)
{
        if (!session || session->client_socket == INVALID_SOCKET) return -1;

        joy_iocp_operation_packet_ptr packet = (joy_iocp_operation_packet_ptr)calloc(1, sizeof(joy_iocp_operation_packet_t));
        if (!packet) return -1;

        packet->operation_type = IOCP_OPERATION_RECEIVE;
        packet->wsabuffer.buf = (CHAR*)packet->buffer;
        packet->wsabuffer.len = JOY_MAX_BUFFER;
        packet->max_buffer_length = JOY_MAX_BUFFER;
        packet->associated_session = session;
        packet->target_socket = session->client_socket;

        DWORD bytes_received = 0;
        DWORD flags = 0;

        int result = WSARecv(session->client_socket, &(packet->wsabuffer),
                1, &bytes_received, &flags, &(packet->overlapped), NULL);

        if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                //printf("WSARecv operation failed: %d", WSAGetLastError());
                free(packet);
                return -1;
        }

        return 0;
}

// 处理接收完成事件
static void iocp_handle_receive_completion(joy_iocp_operation_packet_ptr packet, DWORD bytes_transferred, iocp_client_session_ptr session)
{
        if (!packet || !session) return;

        packet->bytes_received += bytes_transferred;

        // 处理接收到的数据
        if (bytes_transferred > 0) {
                //printf("Received %lu bytes from client %s:%d",
                 //       bytes_transferred, session->client_ip, session->client_port);

                // 示例：回显数据。WSASend 是重叠 IO, 缓冲必须活到完成事件;
                // 本函数末尾会 free(packet), 不能直接引用 packet->buffer。
                joy_iocp_operation_packet_ptr send_packet = (joy_iocp_operation_packet_ptr)calloc(1, sizeof(joy_iocp_operation_packet_t));
                if (send_packet) {
                        ULONG to_copy = bytes_transferred;
                        if (to_copy > JOY_MAX_BUFFER) to_copy = JOY_MAX_BUFFER;
                        memcpy(send_packet->buffer, packet->buffer, to_copy);
                        send_packet->wsabuffer.buf = (char*)send_packet->buffer;
                        send_packet->wsabuffer.len = to_copy;
                        send_packet->operation_type = IOCP_OPERATION_SEND;
                        send_packet->associated_session = session;
                        send_packet->target_socket = session->client_socket;

                        DWORD bytes_sent = 0;
                        if (WSASend(session->client_socket,
                                &send_packet->wsabuffer,
                                1, &bytes_sent,
                                0, &send_packet->overlapped, NULL) == SOCKET_ERROR) {
                                if (WSAGetLastError() != WSA_IO_PENDING) {
                                        printf("WSASend operation failed: %d", WSAGetLastError());
                                        free(send_packet);
                                }
                        }
                }
        }

        // 继续接收数据
        if (session->is_active) {
                iocp_post_receive_operation(session);
        }

        // 释放操作包
        free(packet);
}

// 处理Accept完成事件
static void iocp_handle_accept_completion(joy_iocp_server_p server, joy_iocp_operation_packet_ptr packet)
{
        if (!server || !packet) return;

        SOCKET client_socket = packet->target_socket;
        int address_size = (sizeof(struct sockaddr_in) + 16);
        struct sockaddr_in* local_address = NULL;
        int local_address_length = sizeof(struct sockaddr_in);
        struct sockaddr_in* remote_address = NULL;
        int remote_address_length = sizeof(struct sockaddr_in);

        // 获取客户端地址信息
        server->get_acceptex_sockaddrs_function(packet->wsabuffer.buf,
                0,
                address_size, address_size,
                (struct sockaddr**)&local_address, &local_address_length,
                (struct sockaddr**)&remote_address, &remote_address_length);

        // 创建客户端会话
        iocp_client_session_ptr session = (iocp_client_session_ptr)calloc(1, sizeof(iocp_client_session_t));
        if (!session) {
                closesocket(client_socket);
                return;
        }

        session->client_socket = client_socket;
        session->is_active = true;
        session->heartbeat_count = 5; // 初始化心跳计数器

        // 获取客户端IP和端口
        char client_ip_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &remote_address->sin_addr, client_ip_address, sizeof(client_ip_address));
        snprintf(session->client_ip, sizeof(session->client_ip), "%s", client_ip_address);
        session->client_port = ntohs(remote_address->sin_port);

        printf("New client connected from %s:%d", session->client_ip, session->client_port);

        // 将客户端套接字关联到完成端口
        if (CreateIoCompletionPort((HANDLE)client_socket,
                server->completion_port,
                (ULONG_PTR)session, 0) == NULL) {
                printf("Failed to associate client socket with completion port: %d", GetLastError());
                iocp_cleanup_client_session(session);
                return;
        }

        // 设置套接字上下文
        setsockopt(client_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                (char*)&server->listen_socket, sizeof(server->listen_socket));

        // 开始接收客户端数据
        if (iocp_post_receive_operation(session) != 0) {
                iocp_cleanup_client_session(session);
                return;
        }

        // 重新投递Accept操作，准备接受下一个连接
        joy_iocp_operation_packet_ptr new_packet = (joy_iocp_operation_packet_ptr)calloc(1, sizeof(joy_iocp_operation_packet_t));
        if (new_packet) {
                iocp_post_accept_operation(server, new_packet);
        }

        // 释放当前Accept操作包
        free(packet);
}

// 创建IOCP服务器
joy_iocp_server_p joy_net_iocp_server_create(const char* ip, int port)
{
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
                printf("WSAStartup initialization failed: %d", WSAGetLastError());
                return NULL;
        }

        DWORD bytes_returned = 0;
        GUID guid_acceptex = WSAID_ACCEPTEX;
        GUID guid_get_acceptex_sockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

        joy_iocp_server_p server = (joy_iocp_server_p)calloc(1, sizeof(joy_iocp_server_t));
        if (!server) {
                WSACleanup();   /* 与上面的 WSAStartup 配对 */
                return NULL;
        }
        /* calloc 把 SOCKET 置 0, 但 0 不是 INVALID_SOCKET; 失败清理路径
           会误把 0 当成有效套接字交给 closesocket。 */
        server->listen_socket = INVALID_SOCKET;

        InitializeCriticalSection(&server->critical_section);
        server->is_running = true;

        // 创建IOCP完成端口
        server->completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                NULL, 0, 0);
        if (server->completion_port == NULL) {
                printf("Failed to create completion port: %d", GetLastError());
                goto error_cleanup;
        }

        // 创建监听套接字
        server->listen_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
                WSA_FLAG_OVERLAPPED);
        if (server->listen_socket == INVALID_SOCKET) {
                printf("Failed to create listen socket: %d", WSAGetLastError());
                goto error_cleanup;
        }

        // 设置端口复用选项
        int reuse_address = 1;
        setsockopt(server->listen_socket, SOL_SOCKET, SO_REUSEADDR,
                (char*)&reuse_address, sizeof(reuse_address));

        struct sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = inet_addr(ip);
        server_address.sin_port = htons(port);

        if (bind(server->listen_socket, (struct sockaddr*)&server_address,
                sizeof(server_address)) != 0) {
                printf("Failed to bind socket: %d", WSAGetLastError());
                goto error_cleanup;
        }

        if (listen(server->listen_socket, SOMAXCONN) != 0) {
                printf("Failed to listen on socket: %d", WSAGetLastError());
                goto error_cleanup;
        }

        // 获取AcceptEx扩展函数
        if (WSAIoctl(server->listen_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                &guid_acceptex, sizeof(guid_acceptex),
                &server->acceptex_function, sizeof(server->acceptex_function),
                &bytes_returned, NULL, NULL) != 0) {
                printf("Failed to get AcceptEx function: %d", WSAGetLastError());
                goto error_cleanup;
        }

        // 获取GetAcceptExSockaddrs扩展函数
        if (WSAIoctl(server->listen_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                &guid_get_acceptex_sockaddrs,
                sizeof(guid_get_acceptex_sockaddrs),
                &server->get_acceptex_sockaddrs_function,
                sizeof(server->get_acceptex_sockaddrs_function),
                &bytes_returned, NULL, NULL) != 0) {
                printf("Failed to get GetAcceptExSockaddrs function: %d", WSAGetLastError());
                goto error_cleanup;
        }

        // 将监听套接字关联到完成端口
        if (CreateIoCompletionPort((HANDLE)server->listen_socket,
                server->completion_port,
                (ULONG_PTR)server, 0) == NULL) {
                printf("Failed to associate listen socket with completion port: %d",
                        GetLastError());
                goto error_cleanup;
        }

        // 投递初始的Accept操作
        joy_iocp_operation_packet_ptr packet = (joy_iocp_operation_packet_ptr)calloc(1, sizeof(joy_iocp_operation_packet_t));
        if (packet) {
                iocp_post_accept_operation(server, packet);
        }

        printf("IOCP server created successfully on %s:%d", ip, port);
        return server;

error_cleanup:
        joy_net_iocp_server_destroy(server);
        return NULL;
}

// 销毁IOCP服务器
void joy_net_iocp_server_destroy(joy_iocp_server_p server)
{
        if (!server) return;

        EnterCriticalSection(&server->critical_section);
        server->is_running = false;
        LeaveCriticalSection(&server->critical_section);

        if (server->listen_socket != INVALID_SOCKET) {
                closesocket(server->listen_socket);
                server->listen_socket = INVALID_SOCKET;
        }

        if (server->completion_port != NULL) {
                CloseHandle(server->completion_port);
                server->completion_port = NULL;
        }

        DeleteCriticalSection(&server->critical_section);
        free(server);

        WSACleanup();
}

// 处理IOCP事件
void joy_net_iocp_process_events(joy_iocp_server_p server, int timeout_ms)
{
        if (!server || !server->is_running) return;

        DWORD bytes_transferred = 0;
        ULONG_PTR completion_key = 0;
        joy_iocp_operation_packet_ptr operation_packet = NULL;

        BOOL result = GetQueuedCompletionStatus(server->completion_port,
                &bytes_transferred,
                &completion_key,
                (LPOVERLAPPED*)&operation_packet,
                timeout_ms);

        if (!result) {
                DWORD error_code = GetLastError();
                if (error_code != WAIT_TIMEOUT) {
                        printf("GetQueuedCompletionStatus failed with error: %d", error_code);
                }
                return;
        }

        // 检查是否为取消操作
        if (bytes_transferred == 0 && completion_key == 0 && operation_packet == NULL) {
                return;
        }

        if (completion_key == (ULONG_PTR)server) {
                // 监听套接字事件
                if (operation_packet && operation_packet->operation_type == IOCP_OPERATION_ACCEPT) {
                        iocp_handle_accept_completion(server, operation_packet);
                }
        }
        else {
                // 客户端套接字事件
                iocp_client_session_ptr session = (iocp_client_session_ptr)completion_key;

                if (!session || !session->is_active) {
                        if (operation_packet) free(operation_packet);
                        return;
                }

                // 客户端断开连接
                if (bytes_transferred == 0 && operation_packet && operation_packet->operation_type == IOCP_OPERATION_RECEIVE) {
                        printf("Client disconnected: %s:%d", session->client_ip, session->client_port);
                        iocp_cleanup_client_session(session);
                        free(operation_packet);
                        return;
                }

                // 处理不同类型的操作
                if (operation_packet) {
                        switch (operation_packet->operation_type) {
                        case IOCP_OPERATION_RECEIVE:
                                iocp_handle_receive_completion(operation_packet, bytes_transferred, session);
                                break;
                        case IOCP_OPERATION_SEND:
                                // 发送完成处理
                                free(operation_packet);
                                break;
                        default:
                                free(operation_packet);
                                break;
                        }
                }
        }
}

#else

// 非Windows平台的空实现
joy_iocp_server_p joy_net_iocp_server_create(const char* bind_ip, int bind_port)
{
        printf("IOCP is only supported on Windows platform");
        return NULL;
}

void joy_net_iocp_server_destroy(joy_iocp_server_p server)
{
        (void)server;
}

void joy_net_iocp_process_events(joy_iocp_server_p server, int timeout_ms)
{
        (void)server;
        (void)timeout_ms;
}

#endif // JOY_WIN

/* 枚举网卡在 Android 上需要 API >= 24；更低版本退回到"主机名解析"兜底。
 * （Android NDK 的 getifaddrs 从 API 24 起可用。） */
#if defined(__ANDROID__)
#  if defined(__ANDROID_API__) && __ANDROID_API__ >= 24
#    define JOY_HAVE_IFADDRS 1
#  endif
#elif defined(__EMSCRIPTEN__)
// 浏览器内没有真实网卡，getifaddrs 既未实现（链接会报 undefined symbol）也没意义，
// 直接走下面的"本机名解析"兜底即可。
#elif !defined(JOY_WIN)
#  define JOY_HAVE_IFADDRS 1
#endif

#if defined(JOY_HAVE_IFADDRS)
#  include <ifaddrs.h>
#  include <net/if.h>
#endif

/* 只收 IPv4 —— 调用方（如服务器 IP 下拉列表）一般只要 IPv4，
 * 混进 fe80:: 之类的链路本地 IPv6 会干扰选择。
 *
 * 原来的实现有两个问题：
 *   1) 只有 Windows 分支，其它平台直接 return 0（安卓上列表是空的）；
 *   2) AF_UNSPEC 会把 IPv6 一起收进来。
 */
int joy_net_get_local_ips(char ips[][JOY_MAX_IP], int max_count)
{
        if (!ips || max_count <= 0) return 0;

        // 浏览器内没有真实网卡/本地 IP，原始 socket 与 bind 也不可用；
        // Web 联机走 WebSocket/WebRTC 到外部服务器，故此处直接返回 0。
        // （getifaddrs 在 Emscripten 也未实现，会在链接期报错，这里一并规避。）
#if defined(__EMSCRIPTEN__)
        return 0;
#endif

        int count = 0;

#if defined(__ANDROID__) && !defined(JOY_HAVE_IFADDRS)
        /* Android 在 NDK 平台 < 24 时没有 getifaddrs，且 gethostname 兜底只能拿到
         * 127.0.0.1。这里用 /proc/net/dev 枚举接口 + ioctl(SIOCGIFADDR) 取 IPv4，
         * 无需权限、不限 API 等级，从而能拿到 wlan0 的 192.168.x.x 等局域网地址。 */
#ifndef SIOCGIFADDR
#  define SIOCGIFADDR 0x8915
#endif
        FILE* f = fopen("/proc/net/dev", "r");
        if (f) {
                char line[256];
                int sock = socket(AF_INET, SOCK_DGRAM, 0);
                while (count < max_count && fgets(line, sizeof(line), f)) {
                        char* p = line;
                        while (*p == ' ' || *p == '\t') p++;
                        if (*p == '\0' || *p == '|' || *p == '\n') continue; /* 表头 */
                        char* colon = strchr(p, ':');
                        if (!colon) continue;
                        *colon = '\0';
                        char name[IFNAMSIZ];
                        strncpy(name, p, IFNAMSIZ - 1);
                        name[IFNAMSIZ - 1] = '\0';
                        if (strncmp(name, "lo", 2) == 0) continue; /* 跳过回环 */
                        if (sock >= 0) {
                                struct ifreq ifr;
                                memset(&ifr, 0, sizeof(ifr));
                                strncpy(ifr.ifr_name, name, IFNAMSIZ - 1);
                                if (ioctl(sock, SIOCGIFADDR, &ifr) == 0) {
                                        struct sockaddr_in* sin = (struct sockaddr_in*)&ifr.ifr_addr;
                                        if (inet_ntop(AF_INET, &sin->sin_addr, ips[count], JOY_MAX_IP))
                                                count++;
                                }
                        }
                }
                if (sock >= 0) close(sock);
                fclose(f);
        }
        if (count > 0) return count;
#endif

#if defined(JOY_WIN)
        /* Windows 无 getifaddrs：用 IP Helper 枚举网卡单播 IPv4 地址。
         * 下面的 gethostname+getaddrinfo 兜底只会把本机名解析成回环 127.0.0.1，
         * 拿不到真实局域网 IP，所以 Windows 必须走这条路径。 */
        ULONG abufLen = 0;
        DWORD adr = GetAdaptersAddresses(AF_INET,
                        GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                        NULL, NULL, &abufLen);
        if (adr == ERROR_BUFFER_OVERFLOW && abufLen > 0) {
                PIP_ADAPTER_ADDRESSES addrs = (PIP_ADAPTER_ADDRESSES)malloc(abufLen);
                if (addrs) {
                        adr = GetAdaptersAddresses(AF_INET,
                                        GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                                        NULL, addrs, &abufLen);
                        if (adr == NO_ERROR) {
                                for (PIP_ADAPTER_ADDRESSES a = addrs; a && count < max_count; a = a->Next) {
                                        for (PIP_ADAPTER_UNICAST_ADDRESS u = a->FirstUnicastAddress;
                                             u && count < max_count; u = u->Next) {
                                                if (u->Address.lpSockaddr->sa_family != AF_INET) continue;
                                                void* addr = &((struct sockaddr_in*)u->Address.lpSockaddr)->sin_addr;
                                                if (inet_ntop(AF_INET, addr, ips[count], JOY_MAX_IP))
                                                        count++;
                                        }
                                }
                        }
                        free(addrs);
                }
        }
        if (count > 0) return count;
#endif

#if defined(JOY_HAVE_IFADDRS)
        /* POSIX / Android：直接枚举网卡。比"解析本机名"可靠得多 ——
         * 安卓上 gethostname 往往拿不到主机名，解析不出局域网 IP。 */
        struct ifaddrs* ifa = NULL;
        if (getifaddrs(&ifa) == 0) {
                struct ifaddrs* p;
                for (p = ifa; p && count < max_count; p = p->ifa_next) {
                        if (!p->ifa_addr || p->ifa_addr->sa_family != AF_INET) continue;
                        void* addr = &((struct sockaddr_in*)p->ifa_addr)->sin_addr;
                        if (inet_ntop(AF_INET, addr, ips[count], JOY_MAX_IP))
                                count++;
                }
                freeifaddrs(ifa);
        }
        if (count > 0) return count;
#endif

        /* Windows，或上面没取到时的兜底：用本机名解析 */
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) != 0) return 0;
        struct addrinfo hints, *res, *rp;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;              /* 只要 IPv4 */
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo(hostname, NULL, &hints, &res) != 0) return 0;
        for (rp = res; rp && count < max_count; rp = rp->ai_next) {
                if (rp->ai_family != AF_INET) continue;
                void* addr = &((struct sockaddr_in*)rp->ai_addr)->sin_addr;
                if (inet_ntop(AF_INET, addr, ips[count], JOY_MAX_IP))
                        count++;
        }
        freeaddrinfo(res);
        return count;
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef JOY_HAVE_BACKTRACE
#undef JOY_HAVE_IFADDRS
#undef SIOCGIFADDR

/* ==================== 结束 src/jsys.c ==================== */
