#include <SDL3/SDL.h>
#include "jsclib.h"

static duk_ret_t jk_is_key_down(duk_context* ctx)
{
	const bool* state = SDL_GetKeyboardState(NULL);
	int scancode = (int)duk_to_number(ctx, 0);
	duk_push_boolean(ctx, state && state[scancode]);
	return 1;
}

void jsopen_keyboard(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jk_is_key_down, 1);
	duk_put_prop_string(ctx, -2, "is_key_down");
	duk_put_global_string(ctx, "keyboard");
}