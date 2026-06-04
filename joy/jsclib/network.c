#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "jsclib.h"
#include "../jnetwork.h"

#define KCP_SERVER_TAG "kcpserver"
#define KCP_CLIENT_TAG "kcpclient"

static void kcsrv_finalize(js_State *J, void *p)
{
        (void)J; if (p) kcpserver_destroy((kcpserver_p)p);
}
static void kccli_finalize(js_State *J, void *p)
{
        (void)J; if (p) kcpclient_destroy((kcpclient_p)p);
}

static void jnet_kcpserver_create(js_State *J)
{
        const char *ip = js_tostring(J, 1);
        int port = js_tonumber(J, 2);
        kcpserver_p s = kcpserver_create(ip, port);
        js_newuserdata(J, KCP_SERVER_TAG, s, kcsrv_finalize);
}

static void jnet_kcpserver_destroy(js_State *J)
{
        kcpserver_p s = js_touserdata(J, 1, KCP_SERVER_TAG);
        if (s) { kcpserver_destroy(s); }
        js_pushnull(J);
}

static void jnet_kcpserver_send(js_State *J)
{
        kcpserver_p s = js_touserdata(J, 1, KCP_SERVER_TAG);
        int conv = js_tonumber(J, 2);
        const char *data = js_tostring(J, 3);
        size_t len = strlen(data);
        kcpserver_send(s, conv, data, len);
}

static void jnet_kcpserver_broadcast(js_State *J)
{
        kcpserver_p s = js_touserdata(J, 1, KCP_SERVER_TAG);
        const char *data = js_tostring(J, 2);
        kcpserver_broadcast(s, data, strlen(data));
}

static void jnet_kcpserver_update(js_State *J)
{
        kcpserver_p s = js_touserdata(J, 1, KCP_SERVER_TAG);
        kcpserver_update(s);
}

static void jnet_kcpserver_poll(js_State *J)
{
        kcpserver_p s = js_touserdata(J, 1, KCP_SERVER_TAG);
        net_message_t msg = {0};
        if (kcpserver_poll_message(s, &msg)) {
                js_pushboolean(J, 1);
                js_newobject(J);
                js_pushnumber(J, msg.type);   js_setproperty(J, -2, "type");
                js_pushnumber(J, msg.conv);   js_setproperty(J, -2, "conv");
                js_pushlstring(J, msg.data, msg.len); js_setproperty(J, -2, "data");
                SDL_free(msg.data);
        } else {
                js_pushboolean(J, 0);
                js_pushundefined(J);
        }
}

static void jnet_kcpclient_create(js_State *J)
{
        const char *ip = js_tostring(J, 1);
        int port = js_tonumber(J, 2);
        kcpclient_p c = kcpclient_create(ip, port);
        js_newuserdata(J, KCP_CLIENT_TAG, c, kccli_finalize);
}

static void jnet_kcpclient_destroy(js_State *J)
{
        kcpclient_p c = js_touserdata(J, 1, KCP_CLIENT_TAG);
        if (c) { kcpclient_destroy(c); }
        js_pushnull(J);
}

static void jnet_kcpclient_send(js_State *J)
{
        kcpclient_p c = js_touserdata(J, 1, KCP_CLIENT_TAG);
        const char *data = js_tostring(J, 2);
        kcpclient_send(c, data, strlen(data));
}

static void jnet_kcpclient_update(js_State *J)
{
        kcpclient_p c = js_touserdata(J, 1, KCP_CLIENT_TAG);
        kcpclient_update(c);
}

static void jnet_kcpclient_poll(js_State *J)
{
        kcpclient_p c = js_touserdata(J, 1, KCP_CLIENT_TAG);
        net_message_t msg = {0};
        if (kcpclient_poll_message(c, &msg)) {
                js_pushboolean(J, 1);
                js_newobject(J);
                js_pushnumber(J, msg.type);   js_setproperty(J, -2, "type");
                js_pushnumber(J, msg.conv);   js_setproperty(J, -2, "conv");
                js_pushlstring(J, msg.data, msg.len); js_setproperty(J, -2, "data");
                SDL_free(msg.data);
        } else {
                js_pushboolean(J, 0);
                js_pushundefined(J);
        }
}

void jsopen_network(js_State *J)
{
        js_newobject(J);

        js_newobject(J);
        js_newcfunction(J, jnet_kcpserver_create, "create", 2);    js_setproperty(J, -2, "create");
        js_newcfunction(J, jnet_kcpserver_destroy, "destroy", 1);  js_setproperty(J, -2, "destroy");
        js_newcfunction(J, jnet_kcpserver_send, "send", 3);        js_setproperty(J, -2, "send");
        js_newcfunction(J, jnet_kcpserver_broadcast, "broadcast", 2); js_setproperty(J, -2, "broadcast");
        js_newcfunction(J, jnet_kcpserver_update, "update", 1);    js_setproperty(J, -2, "update");
        js_newcfunction(J, jnet_kcpserver_poll, "poll", 1);        js_setproperty(J, -2, "poll");
        js_setproperty(J, -2, "kcpserver");

        js_newobject(J);
        js_newcfunction(J, jnet_kcpclient_create, "create", 2);    js_setproperty(J, -2, "create");
        js_newcfunction(J, jnet_kcpclient_destroy, "destroy", 1);  js_setproperty(J, -2, "destroy");
        js_newcfunction(J, jnet_kcpclient_send, "send", 2);        js_setproperty(J, -2, "send");
        js_newcfunction(J, jnet_kcpclient_update, "update", 1);    js_setproperty(J, -2, "update");
        js_newcfunction(J, jnet_kcpclient_poll, "poll", 1);        js_setproperty(J, -2, "poll");
        js_setproperty(J, -2, "kcpclient");

        js_setglobal(J, "network");
}
