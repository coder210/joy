#include <assert.h>
#include "jsclib.h"
#include "../joy.h"

#define PTR(n) duk_get_pointer(ctx,n)
#define STR(n) duk_to_string(ctx,n)
#define NUM(n) ((float)duk_to_number(ctx,n))
#define INT(n) ((int)duk_to_number(ctx,n))

static duk_ret_t janim_create(duk_context* ctx)
{
	jr2d_p r2d = (jr2d_p)PTR(0);
	if (!r2d) {
		duk_error(ctx, DUK_ERR_ERROR, "animation.create: invalid renderer");
		return 0;
	}
	duk_push_pointer(ctx, animation_create(r2d));
	return 1;
}

static duk_ret_t janim_destroy(duk_context* ctx)
{
	animation_p anim = (animation_p)PTR(0);
	if (anim) animation_destroy(anim);
	return 0;
}

static duk_ret_t janim_reset(duk_context* ctx)
{
	animation_p anim = (animation_p)PTR(0);
	if (anim)
		animation_reset(anim);
	return 0;
}

static duk_ret_t janim_addclip(duk_context* ctx)
{
	animation_p anim = (animation_p)PTR(0);
	if (!anim)
		return 0;
	const char* path = STR(1);
	jrect_t src; src.x = NUM(3);
	src.y = NUM(4);
	src.w = NUM(5);
	src.h = NUM(6);
	animation_add_clip(anim, path, NUM(2), src);
	return 0;
}

static duk_ret_t janim_is_finished(duk_context* ctx)
{
	animation_p anim = (animation_p)PTR(0);
	duk_push_boolean(ctx, anim ? animation_is_finished(anim) : 1);
	return 1;
}

static duk_ret_t janim_set_position(duk_context* ctx) 
{ 
	animation_p anim = (animation_p)PTR(0);
	if (anim)
		animation_set_position(anim, NUM(1), NUM(2));
	return 0;
}

static duk_ret_t janim_set_scale(duk_context* ctx)
{ 
	animation_p anim = (animation_p)PTR(0); 
	if (anim) 
		animation_set_scale(anim, NUM(1), NUM(2)); 
	return 0;
}

static duk_ret_t janim_update(duk_context* ctx)
{ 
	animation_p anim = (animation_p)PTR(0);
	if (anim)
		animation_update(anim, NUM(1));
	return 0;
}

static duk_ret_t janim_draw(duk_context* ctx)
{ 
	animation_p anim = (animation_p)PTR(0);
	if (!anim)
		return 0;
	jrect_t camera = { 0 };
	if (!duk_is_undefined(ctx, 1)) { 
		camera.x = NUM(1); 
		camera.y = NUM(2); 
		camera.w = NUM(3); 
		camera.h = NUM(4);
	}
	animation_draw(anim, &camera);
	return 0;
}

void jsopen_animation(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, janim_create, 1); duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, janim_destroy, 1); duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, janim_reset, 1); duk_put_prop_string(ctx, -2, "reset");
	duk_push_c_function(ctx, janim_addclip, 7); duk_put_prop_string(ctx, -2, "addclip");
	duk_push_c_function(ctx, janim_is_finished, 1); duk_put_prop_string(ctx, -2, "is_finished");
	duk_push_c_function(ctx, janim_set_position, 3); duk_put_prop_string(ctx, -2, "set_position");
	duk_push_c_function(ctx, janim_set_scale, 3); duk_put_prop_string(ctx, -2, "set_scale");
	duk_push_c_function(ctx, janim_update, 2); duk_put_prop_string(ctx, -2, "update");
	duk_push_c_function(ctx, janim_draw, 5); duk_put_prop_string(ctx, -2, "draw");
	duk_put_global_string(ctx, "animation");
}