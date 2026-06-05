#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "jsclib.h"
#include "../jnetwork.h"

static duk_ret_t jnet_kcpserver_create(duk_context* ctx)
{
	const char* ip = duk_to_string(ctx, 1);
	int port = (int)duk_to_number(ctx, 2);
	duk_push_pointer(ctx, kcpserver_create(ip, port));
	return 1;
}

static duk_ret_t jnet_kcpserver_destroy(duk_context* ctx)
{
	kcpserver_p s = (kcpserver_p)duk_get_pointer(ctx, 1);
	if (s) kcpserver_destroy(s);
	return 0;
}

static duk_ret_t jnet_kcpserver_send(duk_context* ctx)
{
	kcpserver_p s = (kcpserver_p)duk_get_pointer(ctx, 1);
	int conv = (int)duk_to_number(ctx, 2);
	const char* data = duk_to_string(ctx, 3);
	kcpserver_send(s, conv, data, strlen(data));
	return 0;
}

static duk_ret_t jnet_kcpserver_broadcast(duk_context* ctx)
{
	kcpserver_p s = (kcpserver_p)duk_get_pointer(ctx, 1);
	const char* data = duk_to_string(ctx, 2);
	kcpserver_broadcast(s, data, strlen(data));
	return 0;
}

static duk_ret_t jnet_kcpserver_update(duk_context* ctx)
{
	kcpserver_p s = (kcpserver_p)duk_get_pointer(ctx, 1);
	kcpserver_update(s);
	return 0;
}

static duk_ret_t jnet_kcpserver_poll(duk_context* ctx)
{
	kcpserver_p s = (kcpserver_p)duk_get_pointer(ctx, 1);
	net_message_t msg = {0};
	if (kcpserver_poll_message(s, &msg)) {
		duk_push_boolean(ctx, 1);
		duk_push_object(ctx);
		duk_push_number(ctx, (duk_double_t)msg.type);   duk_put_prop_string(ctx, -2, "type");
		duk_push_number(ctx, (duk_double_t)msg.conv);   duk_put_prop_string(ctx, -2, "conv");
		duk_push_lstring(ctx, msg.data, msg.len);         duk_put_prop_string(ctx, -2, "data");
		SDL_free(msg.data);
	} else {
		duk_push_boolean(ctx, 0);
		duk_push_undefined(ctx);
	}
	return 2;
}

static duk_ret_t jnet_kcpclient_create(duk_context* ctx)
{
	const char* ip = duk_to_string(ctx, 1);
	int port = (int)duk_to_number(ctx, 2);
	duk_push_pointer(ctx, kcpclient_create(ip, port));
	return 1;
}

static duk_ret_t jnet_kcpclient_destroy(duk_context* ctx)
{
	kcpclient_p c = (kcpclient_p)duk_get_pointer(ctx, 1);
	if (c) kcpclient_destroy(c);
	return 0;
}

static duk_ret_t jnet_kcpclient_send(duk_context* ctx)
{
	kcpclient_p c = (kcpclient_p)duk_get_pointer(ctx, 1);
	const char* data = duk_to_string(ctx, 2);
	kcpclient_send(c, data, strlen(data));
	return 0;
}

static duk_ret_t jnet_kcpclient_update(duk_context* ctx)
{
	kcpclient_p c = (kcpclient_p)duk_get_pointer(ctx, 1);
	kcpclient_update(c);
	return 0;
}

static duk_ret_t jnet_kcpclient_poll(duk_context* ctx)
{
	kcpclient_p c = (kcpclient_p)duk_get_pointer(ctx, 1);
	net_message_t msg = {0};
	if (kcpclient_poll_message(c, &msg)) {
		duk_push_boolean(ctx, 1);
		duk_push_object(ctx);
		duk_push_number(ctx, (duk_double_t)msg.type);   duk_put_prop_string(ctx, -2, "type");
		duk_push_number(ctx, (duk_double_t)msg.conv);   duk_put_prop_string(ctx, -2, "conv");
		duk_push_lstring(ctx, msg.data, msg.len);         duk_put_prop_string(ctx, -2, "data");
		SDL_free(msg.data);
	} else {
		duk_push_boolean(ctx, 0);
		duk_push_undefined(ctx);
	}
	return 2;
}

void jsopen_network(duk_context* ctx)
{
	duk_push_object(ctx);

	duk_push_object(ctx);
	duk_push_c_function(ctx, jnet_kcpserver_create, 2);    duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jnet_kcpserver_destroy, 1);   duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jnet_kcpserver_send, 3);      duk_put_prop_string(ctx, -2, "send");
	duk_push_c_function(ctx, jnet_kcpserver_broadcast, 2); duk_put_prop_string(ctx, -2, "broadcast");
	duk_push_c_function(ctx, jnet_kcpserver_update, 1);    duk_put_prop_string(ctx, -2, "update");
	duk_push_c_function(ctx, jnet_kcpserver_poll, 1);      duk_put_prop_string(ctx, -2, "poll");
	duk_put_prop_string(ctx, -2, "kcpserver");

	duk_push_object(ctx);
	duk_push_c_function(ctx, jnet_kcpclient_create, 2);    duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jnet_kcpclient_destroy, 1);   duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jnet_kcpclient_send, 2);      duk_put_prop_string(ctx, -2, "send");
	duk_push_c_function(ctx, jnet_kcpclient_update, 1);    duk_put_prop_string(ctx, -2, "update");
	duk_push_c_function(ctx, jnet_kcpclient_poll, 1);      duk_put_prop_string(ctx, -2, "poll");
	duk_put_prop_string(ctx, -2, "kcpclient");

	duk_put_global_string(ctx, "network");
}