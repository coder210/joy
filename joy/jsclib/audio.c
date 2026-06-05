#include "jsclib.h"
#include "../jaudio.h"

static duk_ret_t jaudio_open_device(duk_context* ctx)
{
	duk_push_number(ctx, (duk_double_t)audio_open_device());
	return 1;
}

static duk_ret_t jaudio_close_device(duk_context* ctx)
{
	audio_close_device((uint32_t)duk_to_number(ctx, 0));
	return 0;
}

static duk_ret_t jaudio_create_wav(duk_context* ctx)
{
	const char* filepath = duk_to_string(ctx, 0);
	uint32_t device_id = (uint32_t)duk_to_number(ctx, 1);
	duk_push_boolean(ctx, audio_create_wav(filepath, device_id));
	return 1;
}

static duk_ret_t jaudio_create_ogg(duk_context* ctx)
{
	const char* filepath = duk_to_string(ctx, 0);
	uint32_t device_id = (uint32_t)duk_to_number(ctx, 1);
	(void)device_id;
	duk_push_boolean(ctx, 1);
	return 1;
}

void jsopen_audio(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jaudio_open_device, 0);  duk_put_prop_string(ctx, -2, "open_device");
	duk_push_c_function(ctx, jaudio_close_device, 1); duk_put_prop_string(ctx, -2, "close_device");
	duk_push_c_function(ctx, jaudio_create_wav, 2);    duk_put_prop_string(ctx, -2, "create_wav");
	duk_push_c_function(ctx, jaudio_create_ogg, 2);    duk_put_prop_string(ctx, -2, "create_ogg");
	duk_put_global_string(ctx, "audio");
}