#ifdef __EMSCRIPTEN__

#include <string.h>
#include <SDL3/SDL.h>
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

#include "jconfig.h"
#include "jcore.h"
#include "jutils.h"
#include "jnetwork.h"
#include "jnetwork_emscripten.h"

// ==============================
// Emscripten WebSocket Client 实现
// ==============================

// Emscripten WebSocket 事件回调
static void em_ws_onopen(int eventType, const EMScriptenWebSocketOpenEvent* e, void* userData)
{
        (void)eventType;
        (void)e;
        wsnetclient_p wc = (wsnetclient_p)userData;
        if (!wc) return;

        log_info("emscripten_ws: connected!");

        wc->connected = true;
        wc->connecting = false;
        static int client_conv_counter = 1000;
        wc->conv = client_conv_counter++;

        // 发送连接消息
        net_message_t msg;
        msg.type = NET_TYPE_CONNECTED;
        msg.conv = wc->conv;
        msg.data = SDL_strdup("connected");
        msg.len = (int)strlen(msg.data);
        if (wc->cb) {
                wc->cb(&msg, wc->userdata);
                SDL_free(msg.data);
        } else {
                *kl_pushp(kmq, wc->mq) = msg;
        }
}

static void em_ws_onmessage(int eventType, const EMScriptenWebSocketMessageEvent* e, void* userData)
{
        (void)eventType;
        wsnetclient_p wc = (wsnetclient_p)userData;
        if (!wc) return;

        if (e->numBytes > 0) {
                net_message_t msg;
                msg.type = NET_TYPE_MESSAGE;
                msg.conv = wc->conv;
                msg.len = (int)e->numBytes;
                msg.data = (char*)SDL_malloc(msg.len + 1);
                if (msg.data) {
                        memcpy(msg.data, e->data, msg.len);
                        msg.data[msg.len] = '\0';

                        if (wc->cb) {
                                wc->cb(&msg, wc->userdata);
                                SDL_free(msg.data);
                        } else {
                                *kl_pushp(kmq, wc->mq) = msg;
                        }
                }
        }
}

static void em_ws_onerror(int eventType, const EMScriptenWebSocketErrorEvent* e, void* userData)
{
        (void)eventType;
        (void)e;
        wsnetclient_p wc = (wsnetclient_p)userData;
        if (!wc) return;
        log_error("emscripten_ws: error");
}

static void em_ws_onclose(int eventType, const EMScriptenWebSocketCloseEvent* e, void* userData)
{
        (void)eventType;
        (void)e;
        wsnetclient_p wc = (wsnetclient_p)userData;
        if (!wc) return;

        log_info("emscripten_ws: closed");

        wc->connected = false;
        wc->connecting = false;

        net_message_t msg;
        msg.type = NET_TYPE_DISCONNECTED;
        msg.conv = wc->conv;
        msg.data = SDL_strdup("disconnected");
        msg.len = (int)strlen(msg.data);
        if (wc->cb) {
                wc->cb(&msg, wc->userdata);
                SDL_free(msg.data);
        } else {
                *kl_pushp(kmq, wc->mq) = msg;
        }
}

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

        log_info("emscripten_ws: === Starting connection to %s ===", url);

        // 验证 URL 格式
        if (strncmp(url, "ws://", 5) != 0 && strncmp(url, "wss://", 6) != 0) {
                log_error("emscripten_ws: FATAL - Invalid WebSocket URL format: %s", url);
                kl_destroy(kmq, wc->mq);
                SDL_free(wc);
                return NULL;
        }

        // 使用 Emscripten 原生 WebSocket API
        EmscriptenWebSocketCreateAttributes attr;
        emscripten_websocket_init_create_attributes(&attr);
        attr.url = url;
        wc->ws_socket = emscripten_websocket_new(&attr);
        if (wc->ws_socket < 0) {
                log_error("emscripten_ws: FATAL - failed to create WebSocket for %s", url);
                kl_destroy(kmq, wc->mq);
                SDL_free(wc);
                return NULL;
        }

        // 设置回调
        emscripten_websocket_set_onopen_callback(wc->ws_socket, wc, &em_ws_onopen);
        emscripten_websocket_set_onmessage_callback(wc->ws_socket, wc, &em_ws_onmessage);
        emscripten_websocket_set_onerror_callback(wc->ws_socket, wc, &em_ws_onerror);
        emscripten_websocket_set_onclose_callback(wc->ws_socket, wc, &em_ws_onclose);

        wc->initialized = true;
        log_info("emscripten_ws: connection initiated, waiting for open...");
        return wc;
}

void wsnetclient_destroy(wsnetclient_p wc)
{
        if (!wc) return;

        if (wc->ws_socket >= 0 && wc->initialized) {
                emscripten_websocket_close(wc->ws_socket, 1000, "bye");
                emscripten_websocket_delete(wc->ws_socket);
        }

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
        if (!wc || !wc->connected || !data || len <= 0) {
                return -1;
        }

        int result = emscripten_websocket_send_binary(wc->ws_socket, data, (size_t)len);
        if (result == 0) {
                return len;
        } else {
                log_error("emscripten_ws: send failed, result=%d", result);
                return -1;
        }
}

void wsnetclient_update(wsnetclient_p wc)
{
        // Emscripten 原生 WebSocket 是异步回调，无需轮询
        (void)wc;
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

#endif // __EMSCRIPTEN__
