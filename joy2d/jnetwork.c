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

// http_parser for WebSocket handshake
#include "external/http_parser.h"

// ==============================
// WebSocket Protocol Constants (RFC 6455)
// ==============================
#define WS_OPCODE_CONTINUATION  0x0
#define WS_OPCODE_TEXT          0x1
#define WS_OPCODE_BINARY        0x2
#define WS_OPCODE_CLOSE         0x8
#define WS_OPCODE_PING          0x9
#define WS_OPCODE_PONG          0xA

#define WS_MAGIC_STRING         "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// WebSocket frame header size (minimum, for payload < 126)
#define WS_FRAME_HEADER_MIN     2
#define WS_FRAME_HEADER_EXT16   4
#define WS_FRAME_HEADER_EXT64   10

// Connection state
typedef enum {
    WS_CONN_STATE_HANDSHAKE,  // Waiting for HTTP Upgrade
    WS_CONN_STATE_OPEN,       // WebSocket connected
    WS_CONN_STATE_CLOSING,     // Closing handshake
    WS_CONN_STATE_CLOSED,     // Connection closed
} ws_conn_state;

// SHA-1 context for handshake
typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    unsigned char buffer[64];
} ws_sha1_context;

static void ws_sha1_init(ws_sha1_context* ctx);
static void ws_sha1_update(ws_sha1_context* ctx, const unsigned char* data, uint32_t len);
static void ws_sha1_final(unsigned char* digest, ws_sha1_context* ctx);

// Base64 encoding
static void ws_base64_encode(const unsigned char* in, int inlen, char* out);

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

// ==============================
// Native WebSocket Server (非 Emscripten 平台)
// ==============================
typedef struct wsnetserver_conn {
        int64_t sockfd;
        char ip[JOY_MAX_IP];
        int port;
        int timeout;
        int conv;                      // Connection ID
        delay_t updating_delay;
        ws_conn_state state;           // Connection state
        char* recv_buf;               // Buffer for partial frame
        int recv_len;                 // Buffer length
        int recv_cap;                 // Buffer capacity
} wsnetserver_conn_t, *wsnetserver_conn_p;

KHASH_INIT(wsnconn, int, wsnetserver_conn_p, 1, kh_int_hash_func, kh_int_hash_equal)

struct wsnetserver {
        int64_t sockfd;
        int conv;
        khash_t(wsnconn) * conns;
        klist_t(kmq) * mq;
        net_callback cb;
        void* userdata;
};

// SHA-1 implementation
#define WS_ROL(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

static void ws_sha1_transform(ws_sha1_context* ctx, const unsigned char* buffer)
{
        uint32_t a, b, c, d, e;
        const uint32_t* w = (const uint32_t*)buffer;

        a = ctx->state[0];
        b = ctx->state[1];
        c = ctx->state[2];
        d = ctx->state[3];
        e = ctx->state[4];

#define WS_K0  0x5A827999
#define WS_K1  0x6ED9EBA1
#define WS_K2  0x8F1BBCDC
#define WS_K3  0xCA62C1D6
#define WS_K4  0x8F1BBCDC

#define WS_F1(x, y, z) (z ^ (x & (y ^ z)))
#define WS_F2(x, y, z) (x ^ y ^ z)
#define WS_F3(x, y, z) ((x & y) | (z & (x | y)))
#define WS_F4(x, y, z) (x ^ y ^ z)

#define WS_MIX(i) \
        temp = WS_ROL(a, 5) + WS_F##i(b, c, d) + e + w[i] + WS_K##i; \
        e = d; d = c; c = WS_ROL(b, 30); b = a; a = temp;

        uint32_t temp;
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(1);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(2);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(3);
        WS_MIX(4);
        WS_MIX(4);
        WS_MIX(4);
        WS_MIX(4);
        WS_MIX(4);
        WS_MIX(4);
        WS_MIX(4);
        WS_MIX(4);
        WS_MIX(4);
        WS_MIX(4);

#undef WS_K0
#undef WS_K1
#undef WS_K2
#undef WS_K3
#undef WS_F1
#undef WS_F2
#undef WS_F3
#undef WS_F4
#undef WS_MIX

        ctx->state[0] += a;
        ctx->state[1] += b;
        ctx->state[2] += c;
        ctx->state[3] += d;
        ctx->state[4] += e;
}

static void ws_sha1_init(ws_sha1_context* ctx)
{
        ctx->state[0] = 0x67452301;
        ctx->state[1] = 0xEFCDAB89;
        ctx->state[2] = 0x98BADCFE;
        ctx->state[3] = 0x10325476;
        ctx->state[4] = 0xC3D2E1F0;
        ctx->count[0] = ctx->count[1] = 0;
}

static void ws_sha1_update(ws_sha1_context* ctx, const unsigned char* data, uint32_t len)
{
        uint32_t i, j;

        j = ctx->count[0];
        if ((ctx->count[0] += len << 3) < j)
                ctx->count[1]++;
        ctx->count[1] += (len >> 29);
        j = (j >> 3) & 63;
        if ((j + len) > 63) {
                memcpy(&ctx->buffer[j], data, (i = 64 - j));
                ws_sha1_transform(ctx, ctx->buffer);
                for (; i + 63 < len; i += 64)
                        ws_sha1_transform(ctx, &data[i]);
                j = 0;
        } else {
                i = 0;
        }
        memcpy(&ctx->buffer[j], &data[i], len - i);
}

static void ws_sha1_final(unsigned char* digest, ws_sha1_context* ctx)
{
        unsigned int i;
        unsigned char finalcount[8];
        unsigned char c;

        for (i = 0; i < 8; i++) {
                finalcount[i] = (unsigned char)((ctx->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
        }
        c = 0x80;
        ws_sha1_update(ctx, &c, 1);
        while ((ctx->count[0] & 504) != 448) {
                c = 0;
                ws_sha1_update(ctx, &c, 1);
        }
        ws_sha1_update(ctx, finalcount, 8);
        for (i = 0; i < 20; i++)
                digest[i] = (unsigned char)((ctx->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
}

// Base64 encoding table
static const char ws_base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void ws_base64_encode(const unsigned char* in, int inlen, char* out)
{
        int i, j;
        unsigned char o1, o2, o3;

        for (i = 0, j = 0; i < inlen; i += 3) {
                o1 = in[i];
                o2 = (i + 1 < inlen) ? in[i + 1] : 0;
                o3 = (i + 2 < inlen) ? in[i + 2] : 0;

                out[j++] = ws_base64_table[(o1 >> 2) & 0x3F];
                out[j++] = ws_base64_table[((o1 << 4) | (o2 >> 4)) & 0x3F];
                out[j++] = (i + 1 < inlen) ? ws_base64_table[((o2 << 2) | (o3 >> 6)) & 0x3F] : '=';
                out[j++] = (i + 2 < inlen) ? ws_base64_table[o3 & 0x3F] : '=';
        }
        out[j] = '\0';
}

// WebSocket frame building
static int ws_build_frame(const char* data, int len, char opcode, char* out, int out_cap)
{
        int header_len;
        int payload_len = len;

        if (payload_len < 126) {
                header_len = 2;
                out[0] = 0x80 | opcode;  // FIN + opcode
                out[1] = (char)payload_len;
        } else if (payload_len <= 65535) {
                header_len = 4;
                out[0] = 0x80 | opcode;
                out[1] = 126;
                out[2] = (payload_len >> 8) & 0xFF;
                out[3] = payload_len & 0xFF;
        } else {
                header_len = 10;
                out[0] = 0x80 | opcode;
                out[1] = 127;
                // 64-bit length (big-endian)
                out[2] = 0;
                out[3] = 0;
                out[4] = 0;
                out[5] = 0;
                out[6] = (payload_len >> 24) & 0xFF;
                out[7] = (payload_len >> 16) & 0xFF;
                out[8] = (payload_len >> 8) & 0xFF;
                out[9] = payload_len & 0xFF;
        }

        if (header_len + payload_len > out_cap)
                return -1;

        memcpy(out + header_len, data, payload_len);
        return header_len + payload_len;
}

// WebSocket handshake (returns 1 on success, 0 on need more data, -1 on error)
static int ws_handle_handshake(int64_t sockfd, char* buf, int buf_len, char* resp, int resp_cap)
{
        (void)sockfd;  // Unused
        char ws_key[256] = {0};

        // Simple header parsing for WebSocket key
        char* end = buf + buf_len;
        char* line_start = buf;
        int key_found = 0;

        // Find end of headers (double CRLF)
        char* header_end = NULL;
        for (char* p = buf; p < end - 3; p++) {
                if (p[0] == '\r' && p[1] == '\n' && p[2] == '\r' && p[3] == '\n') {
                        header_end = p;
                        break;
                }
        }

        if (!header_end)
                return 0;  // Need more data

        // Case-insensitive header comparison
        int header_len = 18;  // "Sec-WebSocket-Key:" length
        for (char* p = line_start; p < header_end && !key_found; ) {
                char* line_end = p;
                while (line_end < header_end && *line_end != '\r' && *line_end != '\n')
                        line_end++;

                if (line_end > p && line_end - p > header_len) {
                        // Check if this line starts with "Sec-WebSocket-Key:"
                        int match = 1;
                        const char* h = "sec-websocket-key:";
                        const char* line = p;
                        for (int i = 0; i < header_len && match; i++) {
                                char c1 = line[i];
                                char c2 = h[i];
                                // Case insensitive comparison
                                if (c1 >= 'A' && c1 <= 'Z') c1 = c1 - 'A' + 'a';
                                if (c2 >= 'A' && c2 <= 'Z') c2 = c2 - 'A' + 'a';
                                if (c1 != c2) match = 0;
                        }
                        if (match) {
                                char* key_start = p + header_len;
                                while (key_start < line_end && (*key_start == ' ' || *key_start == '\t'))
                                        key_start++;
                                int key_len = (int)(line_end - key_start);
                                if (key_len > 0 && key_len < 256) {
                                        memcpy(ws_key, key_start, key_len);
                                        ws_key[key_len] = '\0';
                                        key_found = 1;
                                }
                        }
                }

                // Move to next line
                p = line_end;
                while (p < header_end && (*p == '\r' || *p == '\n'))
                        p++;
        }

        if (!key_found)
                return -1;

        // Compute accept key
        ws_sha1_context sha;
        unsigned char sha_digest[20];
        char accept_key[64];

        ws_sha1_init(&sha);
        ws_sha1_update(&sha, (unsigned char*)ws_key, (uint32_t)strlen(ws_key));
        ws_sha1_update(&sha, (unsigned char*)WS_MAGIC_STRING, (uint32_t)strlen(WS_MAGIC_STRING));
        ws_sha1_final(sha_digest, &sha);
        ws_base64_encode(sha_digest, 20, accept_key);

        // Build response
        SDL_snprintf(resp, resp_cap,
                "HTTP/1.1 101 Switching Protocols\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Accept: %s\r\n"
                "\r\n",
                accept_key);

        return 1;
}

// Process WebSocket frame (returns payload length, -1 for close frame, -2 for error)
static int ws_parse_frame(char* buf, int buf_len, char* payload_out, int payload_cap)
{
        if (buf_len < 2)
                return -2;

        int fin = (buf[0] & 0x80) != 0;
        int opcode = buf[0] & 0x0F;
        int masked = (buf[1] & 0x80) != 0;
        int payload_len = buf[1] & 0x7F;

        int header_len;
        int mask_key_offset;

        if (payload_len == 126) {
                if (buf_len < 4) return -2;
                payload_len = (unsigned char)buf[2] << 8 | (unsigned char)buf[3];
                header_len = 4;
                mask_key_offset = 4;
        } else if (payload_len == 127) {
                if (buf_len < 10) return -2;
                payload_len = 0;
                for (int i = 0; i < 8; i++)
                        payload_len = (payload_len << 8) | (unsigned char)buf[2 + i];
                header_len = 10;
                mask_key_offset = 10;
        } else {
                header_len = 2;
                mask_key_offset = 2;
        }

        if (!masked)
                return -2;  // Client frames must be masked

        int frame_len = mask_key_offset + payload_len;
        if (buf_len < frame_len)
                return -2;  // Need more data

        // Unmask payload (XOR with mask key)
        char* mask_key = buf + mask_key_offset;
        for (int i = 0; i < payload_len && i < payload_cap; i++) {
                payload_out[i] = buf[mask_key_offset + i] ^ mask_key[i % 4];
        }

        return payload_len;
}

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



static EM_BOOL ws_on_open(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData)
{
        wsclient_p ws = (wsclient_p)userData;
        if (!ws) return false;

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
        return true;
}

static EM_BOOL ws_on_message(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData)
{
        wsclient_p ws = (wsclient_p)userData;
        if (!ws) return false;

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
        return true;
}

static EM_BOOL ws_on_close(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData)
{
        wsclient_p ws = (wsclient_p)userData;
        if (!ws) return false;

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
        return true;
}

static EM_BOOL ws_on_error(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData)
{
        wsclient_p ws = (wsclient_p)userData;
        if (!ws) return false;

        log_error("WebSocket error");
        return true;
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
                .protocols = NULL
        };

        ws->socket = emscripten_websocket_new(&ws_attrs);
        if (ws->socket <= 0) {
                log_error("Failed to create WebSocket: %d", ws->socket);
                SDL_free(ws);
                return NULL;
        }

        emscripten_websocket_set_onopen_callback(ws->socket, ws, ws_on_open);
        emscripten_websocket_set_onmessage_callback(ws->socket, ws, ws_on_message);
        emscripten_websocket_set_onclose_callback(ws->socket, ws, ws_on_close);
        emscripten_websocket_set_onerror_callback(ws->socket, ws, ws_on_error);

        log_info("WebSocket creating: %s", url);
        return ws;
}

void wsclient_destroy(wsclient_p ws)
{
        if (!ws) return;

        if (ws->connected && ws->socket != 0) {
                emscripten_websocket_close(ws->socket, 1000, "closing");
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

#endif // __EMSCRIPTEN__ (wsclient)

// ==============================
// Native WebSocket Server (非 Emscripten 平台)
// ==============================

wsnetserver_p wsnetserver_create(const char* ip, int port)
{
        wsnetserver_p ws = (wsnetserver_p)SDL_malloc(sizeof(wsnetserver_t));
        if (!ws) return NULL;
        SDL_memset(ws, 0, sizeof(wsnetserver_t));

        ws->sockfd = sys_tcp();
        ws->conv = 1000;
        ws->conns = kh_init(wsnconn);
        ws->mq = kl_init(kmq);
        ws->cb = NULL;
        ws->userdata = NULL;

        sys_set_sock_accpettimeo(ws->sockfd, 1);
        if (!sys_bind(ws->sockfd, ip, port)) {
                log_error("wsnetserver bind error");
        }
        if (!sys_listen(ws->sockfd)) {
                log_error("wsnetserver listen error");
        }
        log_info("wsnetserver listening on %s:%d, sockfd=%lld", ip, port, (long long)ws->sockfd);
        return ws;
}

int wsnetserver_destroy(wsnetserver_p ws)
{
        if (!ws) return 0;

        // Close all connections
        khint_t k;
        for (k = kh_begin(ws->conns); k != kh_end(ws->conns); ++k) {
                if (kh_exist(ws->conns, k)) {
                        wsnetserver_conn_p conn = kh_val(ws->conns, k);
                        if (conn) {
                                sys_closesocket(conn->sockfd);
                                if (conn->recv_buf) SDL_free(conn->recv_buf);
                                SDL_free(conn);
                        }
                }
        }

        // Free messages
        kliter_t(kmq)* p;
        for (p = kl_begin(ws->mq); p != kl_end(ws->mq); p = kl_next(p)) {
                net_message_t* msg = &kl_val(p);
                if (msg->data) SDL_free(msg->data);
        }
        kl_destroy(kmq, ws->mq);

        kh_destroy(wsnconn, ws->conns);
        sys_closesocket(ws->sockfd);
        SDL_free(ws);
        return 1;
}

void wsnetserver_send(wsnetserver_p ws, int conv, const char* data, int len)
{
        if (!ws || conv < 0 || !data) return;

        khint_t k = kh_get(wsnconn, ws->conns, conv);
        if (k != kh_end(ws->conns)) {
                wsnetserver_conn_p conn = kh_val(ws->conns, k);
                if (conn && conn->state == WS_CONN_STATE_OPEN) {
                        char frame[JOY_MAX_BUFFER + 16];
                        int frame_len = ws_build_frame(data, len, WS_OPCODE_BINARY, frame, sizeof(frame));
                        if (frame_len > 0) {
                                sys_send(conn->sockfd, frame, frame_len);
                        }
                }
        }
}

void wsnetserver_broadcast(wsnetserver_p ws, const char* data, int len)
{
        if (!ws || !data) return;

        char frame[JOY_MAX_BUFFER + 16];
        int frame_len = ws_build_frame(data, len, WS_OPCODE_BINARY, frame, sizeof(frame));
        if (frame_len <= 0) return;

        khint_t k;
        for (k = kh_begin(ws->conns); k != kh_end(ws->conns); ++k) {
                if (kh_exist(ws->conns, k)) {
                        wsnetserver_conn_p conn = kh_val(ws->conns, k);
                        if (conn && conn->state == WS_CONN_STATE_OPEN) {
                                sys_send(conn->sockfd, frame, frame_len);
                        }
                }
        }
}

void wsnetserver_offline(wsnetserver_p ws, int conv)
{
        if (!ws || conv < 0) return;

        khint_t k = kh_get(wsnconn, ws->conns, conv);
        if (k != kh_end(ws->conns)) {
                wsnetserver_conn_p conn = kh_val(ws->conns, k);
                if (conn) {
                        // Send close frame
                        char close_frame[2] = {0x88, 0x00};  // FIN + CLOSE opcode, empty payload
                        sys_send(conn->sockfd, close_frame, 2);
                        conn->timeout = -1;
                }
        }
}

static void wsnetserver_accept(wsnetserver_p ws, int64_t sockfd, const char* ip, int port)
{
        int ret, conv;
        khint_t k;
        wsnetserver_conn_p conn;

        conn = (wsnetserver_conn_p)SDL_malloc(sizeof(wsnetserver_conn_t));
        if (!conn) return;

        SDL_memset(conn, 0, sizeof(wsnetserver_conn_t));
        conn->sockfd = sockfd;
        SDL_strlcpy(conn->ip, ip, sizeof(conn->ip));
        conn->port = port;
        conn->timeout = 120;
        conn->state = WS_CONN_STATE_HANDSHAKE;
        conn->updating_delay.time = sys_current_time();
        conn->updating_delay.timeout = 1000;
        conv = ws->conv++;
        conn->conv = conv;

        k = kh_put(wsnconn, ws->conns, conv, &ret);
        if (ret == -1) {
                SDL_free(conn);
                return;
        }
        kh_val(ws->conns, k) = conn;

        log_info("wsnetserver: new connection %d from %s:%d (awaiting handshake)", conv, ip, port);
}

static void wsnetserver_handle_frame(wsnetserver_p ws, wsnetserver_conn_p conn, char* payload, int payload_len)
{
        if (payload_len < 0) {
                // Close frame
                if (payload_len == -1) {
                        log_info("wsnetserver: client %d sent close frame", conn->conv);
                        conn->state = WS_CONN_STATE_CLOSING;
                }
                return;
        }

        net_message_t msg;
        msg.type = NET_TYPE_MESSAGE;
        msg.conv = conn->conv;
        msg.data = (char*)SDL_malloc(payload_len);
        if (!msg.data) return;
        memcpy(msg.data, payload, payload_len);
        msg.len = payload_len;

        if (ws->cb) {
                ws->cb(&msg, ws->userdata);
        } else {
                *kl_pushp(kmq, ws->mq) = msg;
        }
}

void wsnetserver_update(wsnetserver_p ws)
{
        if (!ws) return;

        uint64_t current_time = sys_current_time();
        int port;
        char ip[JOY_MAX_IP];
        int64_t sockfd;
        char buf[JOY_MAX_BUFFER];
        char resp[256];

        // Accept new connections
        sockfd = sys_accept(ws->sockfd, ip, &port);
        if (sockfd > 0) {
                wsnetserver_accept(ws, sockfd, ip, port);
        }

        // Process existing connections
        khint_t p;
        for (p = kh_begin(ws->conns); p != kh_end(ws->conns); ) {
                if (!kh_exist(ws->conns, p)) {
                        p++;
                        continue;
                }

                int conv = kh_key(ws->conns, p);
                wsnetserver_conn_p conn = kh_val(ws->conns, p);

                if (conn->timeout < 0) {
                        // Connection marked for removal
                        sys_closesocket(conn->sockfd);
                        if (conn->recv_buf) SDL_free(conn->recv_buf);
                        SDL_free(conn);
                        kh_del(wsnconn, ws->conns, p);

                        // Queue disconnected message
                        net_message_t msg;
                        msg.type = NET_TYPE_DISCONNECTED;
                        msg.conv = conv;
                        msg.data = SDL_strdup("disconnected");
                        msg.len = SDL_strlen(msg.data);
                        if (ws->cb) {
                                ws->cb(&msg, ws->userdata);
                        } else {
                                *kl_pushp(kmq, ws->mq) = msg;
                        }
                        continue;
                }

                // Update timeout counter
                if (utils_wait_delay(&conn->updating_delay, current_time)) {
                        conn->timeout--;
                }

                // Handle based on state
                if (conn->state == WS_CONN_STATE_HANDSHAKE) {
                        int len = sys_recv(conn->sockfd, buf);
                        if (len > 0) {
                                int result = ws_handle_handshake(conn->sockfd, buf, len, resp, sizeof(resp));
                                if (result == 1) {
                                        sys_send(conn->sockfd, resp, (int)strlen(resp));
                                        conn->state = WS_CONN_STATE_OPEN;
                                        log_info("wsnetserver: handshake complete for conv=%d", conn->conv);

                                        // Queue connected message
                                        net_message_t msg;
                                        msg.type = NET_TYPE_CONNECTED;
                                        msg.conv = conn->conv;
                                        msg.data = SDL_strdup("connected");
                                        msg.len = SDL_strlen(msg.data);
                                        if (ws->cb) {
                                                ws->cb(&msg, ws->userdata);
                                        } else {
                                                *kl_pushp(kmq, ws->mq) = msg;
                                        }
                                } else if (result == -1) {
                                        log_error("wsnetserver: invalid handshake from conv=%d", conn->conv);
                                        conn->timeout = -1;
                                }
                                // result == 0 means need more data, wait for next recv
                        } else if (len == 0) {
                                // Client closed connection during handshake
                                conn->timeout = -1;
                        }
                } else if (conn->state == WS_CONN_STATE_OPEN || conn->state == WS_CONN_STATE_CLOSING) {
                        int len = sys_recv(conn->sockfd, buf);
                        if (len > 0) {
                                // Expand buffer if needed
                                int new_cap = conn->recv_len + len;
                                if (new_cap > conn->recv_cap) {
                                        if (new_cap > JOY_MAX_BUFFER * 2) {
                                                log_error("wsnetserver: recv buffer overflow conv=%d", conn->conv);
                                                conn->timeout = -1;
                                                continue;
                                        }
                                        char* new_buf = (char*)SDL_realloc(conn->recv_buf, new_cap);
                                        if (!new_buf) {
                                                conn->timeout = -1;
                                                continue;
                                        }
                                        conn->recv_buf = new_buf;
                                        conn->recv_cap = new_cap;
                                }
                                memcpy(conn->recv_buf + conn->recv_len, buf, len);
                                conn->recv_len += len;

                                // Process frames
                                char payload[JOY_MAX_BUFFER];
                                while (conn->recv_len > 0) {
                                        int payload_len = ws_parse_frame(conn->recv_buf, conn->recv_len, payload, sizeof(payload));
                                        if (payload_len == -2) {
                                                // Need more data
                                                break;
                                        } else if (payload_len < 0) {
                                                // Error or close frame
                                                conn->state = WS_CONN_STATE_CLOSING;
                                                conn->timeout = -1;
                                                break;
                                        } else {
                                                // Valid frame - calculate header size and shift buffer
                                                int header_len;
                                                int mask_key_offset = 2;
                                                int frame_payload_len = payload_len;

                                                if ((conn->recv_buf[1] & 0x7F) < 126) {
                                                        header_len = 2;
                                                } else if ((conn->recv_buf[1] & 0x7F) == 126) {
                                                        header_len = 4;
                                                } else {
                                                        header_len = 10;
                                                }

                                                int frame_len = header_len + frame_payload_len;

                                                // Get opcode to check for close
                                                int opcode = conn->recv_buf[0] & 0x0F;
                                                if (opcode == WS_OPCODE_CLOSE) {
                                                        conn->state = WS_CONN_STATE_CLOSING;
                                                        conn->timeout = -1;
                                                }

                                                wsnetserver_handle_frame(ws, conn, payload, payload_len);

                                                // Shift remaining data
                                                memmove(conn->recv_buf, conn->recv_buf + frame_len, conn->recv_len - frame_len);
                                                conn->recv_len -= frame_len;
                                        }
                                }
                        } else if (len == 0) {
                                // Client closed connection
                                conn->timeout = -1;
                        }
                }

                p++;
        }
}

int wsnetserver_connection_count(wsnetserver_p ws)
{
        if (!ws) return 0;
        return kh_size(ws->conns);
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
