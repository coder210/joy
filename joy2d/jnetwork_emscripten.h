#ifndef JOY2D_JNETWORK_EMSCRIPTEN_H
#define JOY2D_JNETWORK_EMSCRIPTEN_H

#ifdef __EMSCRIPTEN__

#include "jnetwork.h"
#include "./external/klib/klist.h"

// ==============================
// Emscripten WebSocket Client (使用浏览器原生 WebSocket API)
// ==============================
// 该实现仅在 Emscripten (Web) 平台上编译使用，
// 通过 emscripten_websocket_* API 直接调用浏览器原生 WebSocket。

struct wsnetclient {
        int ws_socket;                 // emscripten WebSocket socket descriptor
        int conv;                      // Connection ID
        bool connected;                // 连接状态
        char url[512];                 // WebSocket URL
        klist_t(kmq) * mq;            // Message queue
        net_callback cb;               // Global callback
        void* userdata;                // Global user data
        bool initialized;              // Init flag
        bool connecting;               // 连接中标志
};

#endif // __EMSCRIPTEN__

#endif // JOY2D_JNETWORK_EMSCRIPTEN_H
