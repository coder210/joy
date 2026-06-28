#include "../joy.h"
#include "jsclib.h"

static duk_ret_t jconsole_log(duk_context* ctx)
{
	log_info("%s", duk_to_string(ctx, 0));
	return 0;
}

void jsopen_base(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jconsole_log, 1);
	duk_put_prop_string(ctx, -2, "log");
	duk_put_global_string(ctx, "console");
}