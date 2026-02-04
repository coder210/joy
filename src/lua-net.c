#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>
#include "external/lua/lapi.h"
#include "external/lua/lauxlib.h"
#include "external/kcp.h"
#include "luaclib.h"
#include "liv-sys.h"
#include "liv-net.h"


typedef struct l_kcpcb {
        lua_State* l;
        int index;
        uint64_t sockfd;
        char ip[LIVNET_MAX_IP];
        int port;
}l_kcpcb_t, * l_kcpcb_p;


static int l_sys_init_netenv(lua_State *L)
{
        bool r;
        r = sys_init_netenv();
        lua_pushboolean(L, r);
        return 1;
}

static int l_sys_release_netenv(lua_State *L)
{
        bool r;
        r = sys_release_netenv();
        lua_pushboolean(L, r);
        return 1;
}

static int l_sys_anyaddr(lua_State *L)
{
        int addr;
        addr = sys_anyaddr();
        lua_pushinteger(L, addr);
        return 1;
}

static int l_sys_closesocket(lua_State *L)
{
        bool r;
        lua_Integer sockfd;
        sockfd = luaL_checkinteger(L, 1);
        r = sys_closesocket(sockfd);
        lua_pushinteger(L, r);
        return 1;
}

static int l_sys_set_sock_sndtimeo(lua_State *L)
{
        int r, ms;
        lua_Integer sockfd;
        sockfd = luaL_checkinteger(L, 1);
        ms = (int)luaL_checkinteger(L, 2);
        r = sys_set_sock_sndtimeo(sockfd, ms);
        lua_pushinteger(L, r);
        return 1;
}

static int l_sys_set_sock_rcvtimeo(lua_State *L)
{
        int r, ms;
        lua_Integer sockfd;
        sockfd = luaL_checkinteger(L, 1);
        ms = (int)luaL_checkinteger(L, 2);
        r = sys_set_sock_rcvtimeo(sockfd, ms);
        lua_pushinteger(L, r);
        return 1;
}


static int l_sys_tcp(lua_State *L)
{
        lua_Integer sockfd;
        sockfd = sys_tcp();
        lua_pushinteger(L, sockfd);
        return 1;
}

static int l_sys_udp(lua_State *L)
{
        lua_Integer sockfd;
        sockfd = sys_udp();
        lua_pushinteger(L, sockfd);
        return 1;
}

static int l_sys_bind(lua_State *L)
{
        lua_Integer sockfd;
        const char *ip;
        int port;
        bool r;
        sockfd = luaL_checkinteger(L, 1);
        ip = luaL_checkstring(L, 2);
        port = (int)luaL_checkinteger(L, 3);
        r = sys_bind(sockfd, ip, port);
        lua_pushboolean(L, r);
        return 1;
}

static int l_sys_connect(lua_State *L)
{
        lua_Integer sockfd;
        const char *ip;
        int port;
        bool r;
        sockfd = luaL_checkinteger(L, 1);
        ip = luaL_checkstring(L, 2);
        port = (int)luaL_checkinteger(L, 3);
        r = sys_connect(sockfd, ip, port);
        lua_pushboolean(L, r);
        return 1;
}

static int l_sys_listen(lua_State *L)
{
        lua_Integer sockfd;
        bool r;
        sockfd = luaL_checkinteger(L, 1);
        r = sys_listen(sockfd);
        lua_pushboolean(L, r);
        return 1;
}

static int l_sys_accept(lua_State *L)
{
        lua_Integer sockfd;
        char ip[LIVNET_MAX_IP];
        int port;
        lua_Integer acceptfd;
        memset(ip, 0, LIVNET_MAX_IP);
        sockfd = luaL_checkinteger(L, 1);
        acceptfd = sys_accept(sockfd, ip, &port);
        lua_pushinteger(L, acceptfd);
        lua_pushstring(L, ip);
        lua_pushinteger(L, port);
        return 3;
}

static int l_sys_send(lua_State *L)
{
        lua_Integer sockfd;
        const char *buf;
        size_t buf_len;
        int n;
        sockfd = luaL_checkinteger(L, 1);
        buf = luaL_checklstring(L, 2, &buf_len);
        n = sys_send(sockfd, buf, (int)buf_len);
        lua_pushinteger(L, n);
        return 1;
}

static int l_sys_recv(lua_State *L)
{
        lua_Integer sockfd;
        char buf[LIVNET_MAX_BUFFER];
        int n;
        sockfd = luaL_checkinteger(L, 1);
        n = sys_recv(sockfd, buf);
        lua_pushinteger(L, n);
        if (n > 0) {
                lua_pushlstring(L, buf, n);
        }
        else {
                lua_pushnil(L);
        }
        return 2;
}

static int l_sys_recvfrom(lua_State *L)
{
        lua_Integer sockfd;
        char buf[LIVNET_MAX_BUFFER];
        char ip[LIVNET_MAX_IP];
        int port, n;
        memset(ip, 0, LIVNET_MAX_IP);
        sockfd = luaL_checkinteger(L, 1);
        n = sys_recvfrom(sockfd, buf, ip, &port);
        lua_pushinteger(L, n);
        if (n > 0) {
                lua_pushlstring(L, buf, n);
                lua_pushstring(L, ip);
                lua_pushinteger(L, port);
        }
        else {
                lua_pushnil(L);
                lua_pushstring(L, ip);
                lua_pushinteger(L, port);
        }
        return 4;
}


static int l_sys_sendto(lua_State *L)
{
        lua_Integer sockfd;
        const char *buf;
        const char *ip;
        size_t buf_len;
        int n, port;
        sockfd = luaL_checkinteger(L, 1);
        buf = luaL_checklstring(L, 2, &buf_len);
        buf_len = luaL_checkinteger(L, 3);
        ip = luaL_checkstring(L, 4);
        port = (int)luaL_checkinteger(L, 5);
        n = sys_sendto(sockfd, buf, (int)buf_len, ip, port);
        lua_pushinteger(L, n);
        return 1;
}

static int l_sys_current_time(lua_State *L)
{
        lua_Integer current_time;
        current_time = sys_current_time();
        lua_pushinteger(L, current_time);
        return 1;
}



static int kcp_output(const char* data, int size, ikcpcb* kcpcb, void* user)
{
        l_kcpcb_p cb = (l_kcpcb_p)user;
        lua_rawgeti(cb->l, LUA_REGISTRYINDEX, cb->index);
        lua_pushlstring(cb->l, data, size);
        lua_pushinteger(cb->l, size);
        lua_pushlightuserdata(cb->l, kcpcb);
        lua_pushinteger(cb->l, cb->sockfd);
        lua_pushstring(cb->l, cb->ip);
        lua_pushinteger(cb->l, cb->port);
        return lua_pcall(cb->l, 6, 0, 0);
}

static int l_kcp_create(lua_State* L)
{
        int conv, port;
        uint32_t sockfd;
        const char* ip;
        size_t ip_len;
        l_kcpcb_p cb;
        ikcpcb* kcpcb;

        conv = (int)luaL_checkinteger(L, 1);
        sockfd = (uint32_t)luaL_checkinteger(L, 2);
        ip = luaL_checklstring(L, 3, &ip_len);
        port = (int)luaL_checkinteger(L, 4);

        luaL_checktype(L, 5, LUA_TFUNCTION);
        cb = (l_kcpcb_p)malloc(sizeof(l_kcpcb_t));
        assert(cb);
        cb->l = L;
        cb->index = luaL_ref(L, LUA_REGISTRYINDEX);
        cb->sockfd = sockfd;
        strncpy(cb->ip, ip, ip_len);
        cb->ip[ip_len] = 0;
        cb->port = port;
        kcpcb = ikcp_create(conv, cb);
        ikcp_wndsize(kcpcb, 512, 512);
        ikcp_setoutput(kcpcb, kcp_output);
        lua_pushlightuserdata(L, kcpcb);
        return 1;
}

static int l_kcp_destroy(lua_State* L)
{
        ikcpcb* kcpcb;
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        free(kcpcb->user);
        ikcp_release(kcpcb);
        return 0;
}

static int l_kcp_get_conv(lua_State* L)
{
        ikcpcb* kcpcb;
        int conv;
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        conv = ikcp_getconv(kcpcb);
        lua_pushinteger(L, conv);
        return 1;
}

static int l_kcp_get_ip(lua_State* L)
{
        ikcpcb* kcpcb;
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        l_kcpcb_p cb = (l_kcpcb_p)kcpcb->user;
        lua_pushstring(L, cb->ip);
        return 1;
}

static int l_kcp_get_port(lua_State* L)
{
        ikcpcb* kcpcb;
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        l_kcpcb_p cb = (l_kcpcb_p)kcpcb->user;
        lua_pushinteger(L, cb->port);
        return 1;
}


static int l_kcp_get_sockfd(lua_State* L)
{
        ikcpcb* kcpcb;
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        l_kcpcb_p cb = (l_kcpcb_p)kcpcb->user;
        lua_pushinteger(L, cb->sockfd);
        return 1;
}

static inline uint32_t iclock(uint64_t clock64)
{
        return (uint32_t)(clock64 & 0xfffffffful);
}

static int l_kcp_update(lua_State* L)
{
        ikcpcb* kcpcb;
        lua_Integer current;
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        current = luaL_checkinteger(L, 2);
        ikcp_update(kcpcb, ikcp_check(kcpcb, iclock(current)));
        return 0;
}

static int l_kcp_input(lua_State* L)
{
        ikcpcb* kcpcb;
        const char* buf;
        size_t buf_len;
        int r;
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        buf = luaL_checklstring(L, 2, &buf_len);
        r = ikcp_input(kcpcb, buf, (long)buf_len);
        lua_pushinteger(L, r);
        return 1;
}

static int l_kcp_recv(lua_State* L)
{
        ikcpcb* kcpcb;
        int peeksize;
        char buf[LIVNET_MAX_BUFFER];
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        peeksize = ikcp_peeksize(kcpcb);
        if (peeksize > 0 && peeksize <= LIVNET_MAX_BUFFER) {
                ikcp_recv(kcpcb, buf, peeksize);
                lua_pushinteger(L, peeksize);
                lua_pushlstring(L, buf, peeksize);
        }
        else {
                lua_pushinteger(L, peeksize);
                lua_pushnil(L);
        }
        return 2;
}

static int l_kcp_send(lua_State* L)
{
        ikcpcb* kcpcb;
        const char* buf;
        size_t buf_len;
        assert(lua_islightuserdata(L, 1));
        kcpcb = (ikcpcb*)lua_touserdata(L, 1);
        buf = luaL_checklstring(L, 2, &buf_len);
        int r = ikcp_send(kcpcb, buf, (int)buf_len);
        lua_pushinteger(L, r);
        return 1;
}



static int l_kcpserver_create(lua_State* L)
{
        const char* ip;
        int port;
        uint64_t sockfd;
        kcpserver_p kcpserver;
        ip = luaL_checkstring(L, 1);
        port = luaL_checkinteger(L, 2);
        kcpserver = liv_ks_create(ip, port);
        lua_pushlightuserdata(L, kcpserver);
        return 1;
}

static int l_kcpserver_destroy(lua_State* L)
{
        kcpserver_p kcpserver;
        kcpserver = lua_touserdata(L, 1);
        liv_ks_destroy(kcpserver);
        return 0;
}

static int l_kcpserver_send(lua_State* L)
{
        kcpserver_p kcpserver;
        const char* data;
        int conv;
        size_t len;
        kcpserver = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        data = luaL_checklstring(L, 3, &len);
        liv_ks_send(kcpserver, conv, data, len);
        return 0;
}

static int l_kcpserver_broadcast(lua_State* L)
{
        kcpserver_p kcpserver;
        const char* data;
        size_t len;
        int result;
        kcpserver = lua_touserdata(L, 1);
        data = luaL_checklstring(L, 2, &len);
        liv_ks_broadcast(kcpserver, data, len);
        return 0;
}

static int l_kcpserver_offline(lua_State* L)
{
        kcpserver_p kcpserver;
        int conv;
        size_t len;
        kcpserver = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        liv_ks_offline(kcpserver, conv);
        return 0;
}

static int l_kcpserver_update(lua_State* L)
{
        kcpserver_p kcpserver;
        kcpserver = lua_touserdata(L, 1);
        liv_ks_update(kcpserver);
        return 0;
}

static int l_kcpserver_poll(lua_State* L)
{
        net_message_t msg = { 0 };
        kcpserver_p kcpserver;
        kcpserver = lua_touserdata(L, 1);
        if (liv_ks_poll_message(kcpserver, &msg)) {
                lua_pushboolean(L, 1);
                lua_newtable(L);
                lua_pushinteger(L, msg.type);
                lua_setfield(L, -2, "type");
                lua_pushinteger(L, msg.conv);
                lua_setfield(L, -2, "conv");
                lua_pushlstring(L, msg.data, msg.len);
                lua_setfield(L, -2, "data");
                SDL_free(msg.data);
        }
        else {
                lua_pushboolean(L, false);
                lua_pushnil(L);
        }
        return 2;
}



static int l_kcpclient_create(lua_State* L)
{
        const char* ip;
        int port;
        kcpclient_p kc;
        ip = luaL_checkstring(L, 1);
        port = luaL_checkinteger(L, 2);
        kc = liv_kc_create(ip, port);
        lua_pushlightuserdata(L, kc);
        return 1;
}

static int l_kcpclient_destroy(lua_State* L)
{
        kcpclient_p kc;
        kc = lua_touserdata(L, 1);
        liv_kc_destroy(kc);
        return 0;
}

static int l_kcpclient_getconv(lua_State* L)
{
        kcpclient_p kc;
        bool result;
        int conv;
        kc = lua_touserdata(L, 1);
        result = liv_kc_getconv(kc, &conv);
        lua_pushboolean(L, result);
        lua_pushinteger(L, conv);
        return 2;
}

static int l_kcpclient_send(lua_State* L)
{
        kcpclient_p kc;
        const char* data;
        size_t len;
        int result;
        kc = lua_touserdata(L, 1);
        data = luaL_checklstring(L, 2, &len);
        result = liv_kc_send(kc, data, len);
        lua_pushinteger(L, result);
        return 1;
}

static int l_kcpclient_update(lua_State* L)
{
        kcpclient_p kc;
        kc = lua_touserdata(L, 1);
        liv_kc_update(kc);
        return 0;
}

static int l_kcpclient_poll(lua_State* L)
{
        net_message_t msg = { 0 };
        kcpclient_p kc;
        kc = lua_touserdata(L, 1);
        if (liv_kc_poll_message(kc, &msg)) {
                lua_pushboolean(L, 1);
                lua_newtable(L);
                lua_pushinteger(L, msg.type);
                lua_setfield(L, -2, "type");
                lua_pushinteger(L, msg.conv);
                lua_setfield(L, -2, "conv");
                lua_pushlstring(L, msg.data, msg.len);
                lua_setfield(L, -2, "data");
                SDL_free(msg.data);
        }
        else {
                lua_pushboolean(L, 0);
                lua_pushnil(L);
        }
        return 2;
}



int luaopen_net(lua_State *L)
{
        luaL_checkversion(L);
        lua_newtable(L);
        luaL_Reg top_level[] = {
                {"init_netenv", l_sys_init_netenv},
                {"release_netenv", l_sys_release_netenv},
                {"anyaddr", l_sys_anyaddr},
                {"closesocket", l_sys_closesocket},
                {"set_sock_sndtimeo", l_sys_set_sock_sndtimeo},
                {"set_sock_rcvtimeo", l_sys_set_sock_rcvtimeo},
                {"tcp", l_sys_tcp},
                {"udp", l_sys_udp},
                {"bind", l_sys_bind},
                {"connect", l_sys_connect},
                {"listen", l_sys_listen},
                {"accept", l_sys_accept},
                {"send", l_sys_send},
                {"recv", l_sys_recv},
                {"recvfrom", l_sys_recvfrom},
                {"sendto", l_sys_sendto},
                {"current_time", l_sys_current_time},
                {NULL, NULL}
        };
        luaL_setfuncs(L, top_level, 0);

        luaL_Reg kcp_lib[] = {
		{"create", l_kcp_create},
		{"destroy", l_kcp_destroy},
		{"get_conv", l_kcp_get_conv},
		{"get_ip", l_kcp_get_ip},
		{"get_port", l_kcp_get_port},
		{"get_sockfd", l_kcp_get_sockfd},
		{"update", l_kcp_update},
		{"input", l_kcp_input},
		{"recv", l_kcp_recv},
		{"send", l_kcp_send},
		{NULL, NULL}
        };
        luaL_newlib(L, kcp_lib);
        lua_setfield(L, -2, "kcp");

        luaL_Reg kcpserver_lib[] = {
            {"create", l_kcpserver_create},
            {"destroy", l_kcpserver_destroy},
            {"send", l_kcpserver_send},
            {"broadcast", l_kcpserver_broadcast},
            {"update", l_kcpserver_update},
            {"poll", l_kcpserver_poll},
            {NULL, NULL}
        };
        luaL_newlib(L, kcpserver_lib);
        lua_setfield(L, -2, "kcpserver");

        luaL_Reg kcpclient_lib[] = {
            {"create", l_kcpclient_create},
            {"destroy", l_kcpclient_destroy},
            {"get_conv", l_kcpclient_getconv},
            {"send", l_kcpclient_send},
            {"update", l_kcpclient_update},
            {"poll", l_kcpclient_poll},
            {NULL, NULL}
        };
        luaL_newlib(L, kcpclient_lib);
        lua_setfield(L, -2, "kcpclient");

        return 1;
}
