#include "../joy.h"
#include "jsclib.h"

#define PTR(n) duk_get_pointer(ctx,n)
#define STR(n) duk_to_string(ctx,n)
#define INT(n) ((int)duk_to_number(ctx,n))

static duk_ret_t jnet_kcpserver_create(duk_context* ctx) { duk_push_pointer(ctx, kcpserver_create(STR(0), INT(1))); return 1; }
static duk_ret_t jnet_kcpserver_destroy(duk_context* ctx) { kcpserver_p s=(kcpserver_p)PTR(0); if(s) kcpserver_destroy(s); return 0; }
static duk_ret_t jnet_kcpserver_send(duk_context* ctx) { kcpserver_p s=(kcpserver_p)PTR(0); int conv=INT(1); const char* data=STR(2); kcpserver_send(s,conv,data,strlen(data)); return 0; }
static duk_ret_t jnet_kcpserver_broadcast(duk_context* ctx) { kcpserver_p s=(kcpserver_p)PTR(0); const char* data=STR(1); kcpserver_broadcast(s,data,strlen(data)); return 0; }
static duk_ret_t jnet_kcpserver_update(duk_context* ctx) { kcpserver_p s=(kcpserver_p)PTR(0); kcpserver_update(s); return 0; }
/* poll: 返回消息对象 {type, conv, data} 或 undefined（无消息） */
static duk_ret_t jnet_kcpserver_poll(duk_context* ctx) { kcpserver_p s=(kcpserver_p)PTR(0); net_message_t msg={0}; if(kcpserver_poll_message(s,&msg)){ duk_push_object(ctx); duk_push_number(ctx,(duk_double_t)msg.type); duk_put_prop_string(ctx,-2,"type"); duk_push_number(ctx,(duk_double_t)msg.conv); duk_put_prop_string(ctx,-2,"conv"); duk_push_lstring(ctx,msg.data,msg.len); duk_put_prop_string(ctx,-2,"data"); SDL_free(msg.data); }else{ duk_push_undefined(ctx); } return 1; }

static duk_ret_t jnet_kcpclient_create(duk_context* ctx) { duk_push_pointer(ctx, kcpclient_create(STR(0), INT(1))); return 1; }
static duk_ret_t jnet_kcpclient_destroy(duk_context* ctx) { kcpclient_p c=(kcpclient_p)PTR(0); if(c) kcpclient_destroy(c); return 0; }
static duk_ret_t jnet_kcpclient_send(duk_context* ctx) { kcpclient_p c=(kcpclient_p)PTR(0); const char* data=STR(1); kcpclient_send(c,data,strlen(data)); return 0; }
static duk_ret_t jnet_kcpclient_update(duk_context* ctx) { kcpclient_p c=(kcpclient_p)PTR(0); kcpclient_update(c); return 0; }
/* poll: 返回消息对象 {type, conv, data} 或 undefined（无消息） */
static duk_ret_t jnet_kcpclient_poll(duk_context* ctx) { kcpclient_p c=(kcpclient_p)PTR(0); net_message_t msg={0}; if(kcpclient_poll_message(c,&msg)){ duk_push_object(ctx); duk_push_number(ctx,(duk_double_t)msg.type); duk_put_prop_string(ctx,-2,"type"); duk_push_number(ctx,(duk_double_t)msg.conv); duk_put_prop_string(ctx,-2,"conv"); duk_push_lstring(ctx,msg.data,msg.len); duk_put_prop_string(ctx,-2,"data"); SDL_free(msg.data); }else{ duk_push_undefined(ctx); } return 1; }

/* ===== netserver（统一 API） ===== */
static duk_ret_t jnet_netserver_create(duk_context* ctx) {
    const char* t = duk_to_string(ctx, 0);
    net_server_type type;
    if (strcmp(t, "kcp") == 0) type = NET_SERVER_KCP;
    else if (strcmp(t, "tcp") == 0) type = NET_SERVER_TCP;
    else if (strcmp(t, "ws") == 0) type = NET_SERVER_WEBSOCKET;
    else { duk_push_null(ctx); return 1; }
    duk_push_pointer(ctx, netserver_create(type, duk_to_string(ctx,1), (int)duk_to_number(ctx,2)));
    return 1;
}
static duk_ret_t jnet_netserver_destroy(duk_context* ctx) { netserver_p s=(netserver_p)PTR(0); if(s) netserver_destroy(s); return 0; }
static duk_ret_t jnet_netserver_send(duk_context* ctx) { netserver_p s=(netserver_p)PTR(0); netserver_send(s,INT(1),STR(2),(int)strlen(STR(2))); return 0; }
static duk_ret_t jnet_netserver_broadcast(duk_context* ctx) { netserver_p s=(netserver_p)PTR(0); netserver_broadcast(s,STR(1),(int)strlen(STR(1))); return 0; }
static duk_ret_t jnet_netserver_update(duk_context* ctx) { netserver_p s=(netserver_p)PTR(0); netserver_update(s); return 0; }
static duk_ret_t jnet_netserver_poll(duk_context* ctx) {
    netserver_p s=(netserver_p)PTR(0); net_message_t msg={0};
    if(netserver_poll_message(s,&msg)){ duk_push_object(ctx);
        duk_push_number(ctx,(duk_double_t)msg.type); duk_put_prop_string(ctx,-2,"type");
        duk_push_number(ctx,(duk_double_t)msg.conv); duk_put_prop_string(ctx,-2,"conv");
        duk_push_lstring(ctx,msg.data,msg.len); duk_put_prop_string(ctx,-2,"data");
        SDL_free(msg.data);
    }else{ duk_push_undefined(ctx); } return 1;
}
static duk_ret_t jnet_netserver_conn_count(duk_context* ctx) { duk_push_number(ctx,(duk_double_t)netserver_connection_count((netserver_p)PTR(0))); return 1; }

/* ===== netclient（统一 API） ===== */
static duk_ret_t jnet_netclient_create(duk_context* ctx) {
    const char* t = duk_to_string(ctx, 0);
    net_client_type type;
    if (strcmp(t, "kcp") == 0) type = NET_CLIENT_KCP;
    else if (strcmp(t, "tcp") == 0) type = NET_CLIENT_TCP;
    else if (strcmp(t, "ws") == 0) type = NET_CLIENT_WEBSOCKET;
    else { duk_push_null(ctx); return 1; }
    duk_push_pointer(ctx, netclient_create(type, duk_to_string(ctx,1), (int)duk_to_number(ctx,2)));
    return 1;
}
static duk_ret_t jnet_netclient_destroy(duk_context* ctx) { netclient_p c=(netclient_p)PTR(0); if(c) netclient_destroy(c); return 0; }
static duk_ret_t jnet_netclient_send(duk_context* ctx) { netclient_p c=(netclient_p)PTR(0); netclient_send(c,STR(1),(int)strlen(STR(1))); return 0; }
static duk_ret_t jnet_netclient_update(duk_context* ctx) { netclient_p c=(netclient_p)PTR(0); netclient_update(c); return 0; }
static duk_ret_t jnet_netclient_poll(duk_context* ctx) {
    netclient_p c=(netclient_p)PTR(0); net_message_t msg={0};
    if(netclient_poll_message(c,&msg)){ duk_push_object(ctx);
        duk_push_number(ctx,(duk_double_t)msg.type); duk_put_prop_string(ctx,-2,"type");
        duk_push_number(ctx,(duk_double_t)msg.conv); duk_put_prop_string(ctx,-2,"conv");
        duk_push_lstring(ctx,msg.data,msg.len); duk_put_prop_string(ctx,-2,"data");
        SDL_free(msg.data);
    }else{ duk_push_undefined(ctx); } return 1;
}

void jsopen_network(duk_context* ctx)
{
	duk_push_object(ctx);
	/* kcpserver */
	duk_push_object(ctx);
	duk_push_c_function(ctx, jnet_kcpserver_create, 2);    duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jnet_kcpserver_destroy, 1);   duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jnet_kcpserver_send, 3);      duk_put_prop_string(ctx, -2, "send");
	duk_push_c_function(ctx, jnet_kcpserver_broadcast, 2); duk_put_prop_string(ctx, -2, "broadcast");
	duk_push_c_function(ctx, jnet_kcpserver_update, 1);    duk_put_prop_string(ctx, -2, "update");
	duk_push_c_function(ctx, jnet_kcpserver_poll, 1);      duk_put_prop_string(ctx, -2, "poll");
	duk_put_prop_string(ctx, -2, "kcpserver");
	/* kcpclient */
	duk_push_object(ctx);
	duk_push_c_function(ctx, jnet_kcpclient_create, 2);    duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jnet_kcpclient_destroy, 1);   duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jnet_kcpclient_send, 2);      duk_put_prop_string(ctx, -2, "send");
	duk_push_c_function(ctx, jnet_kcpclient_update, 1);    duk_put_prop_string(ctx, -2, "update");
	duk_push_c_function(ctx, jnet_kcpclient_poll, 1);      duk_put_prop_string(ctx, -2, "poll");
	duk_put_prop_string(ctx, -2, "kcpclient");
	/* netserver */
	duk_push_object(ctx);
	duk_push_c_function(ctx, jnet_netserver_create, 3);     duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jnet_netserver_destroy, 1);    duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jnet_netserver_send, 3);       duk_put_prop_string(ctx, -2, "send");
	duk_push_c_function(ctx, jnet_netserver_broadcast, 2);  duk_put_prop_string(ctx, -2, "broadcast");
	duk_push_c_function(ctx, jnet_netserver_update, 1);     duk_put_prop_string(ctx, -2, "update");
	duk_push_c_function(ctx, jnet_netserver_poll, 1);       duk_put_prop_string(ctx, -2, "poll");
	duk_push_c_function(ctx, jnet_netserver_conn_count, 1); duk_put_prop_string(ctx, -2, "connection_count");
	duk_put_prop_string(ctx, -2, "netserver");
	/* netclient */
	duk_push_object(ctx);
	duk_push_c_function(ctx, jnet_netclient_create, 3);     duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jnet_netclient_destroy, 1);    duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jnet_netclient_send, 2);       duk_put_prop_string(ctx, -2, "send");
	duk_push_c_function(ctx, jnet_netclient_update, 1);     duk_put_prop_string(ctx, -2, "update");
	duk_push_c_function(ctx, jnet_netclient_poll, 1);       duk_put_prop_string(ctx, -2, "poll");
	duk_put_prop_string(ctx, -2, "netclient");
	duk_put_global_string(ctx, "network");
}
