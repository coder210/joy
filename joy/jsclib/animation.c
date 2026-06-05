#include <assert.h>
#include "jsclib.h"
#include "../janimations.h"

static duk_ret_t janim_create(duk_context* ctx)
{
	SDL_Renderer* ren = (SDL_Renderer*)duk_get_pointer(ctx, 1);
	if (!ren) { duk_error(ctx, DUK_ERR_ERROR, "animation.create: invalid renderer"); return 0; }
	duk_push_pointer(ctx, sprite_animation_create(ren));
	return 1;
}

static duk_ret_t janim_destroy(duk_context* ctx)
{
	sprite_animation_p anim = (sprite_animation_p)duk_get_pointer(ctx, 1);
	if (anim) sprite_animation_destroy(anim);
	return 0;
}

static duk_ret_t janim_reset(duk_context* ctx)
{
	sprite_animation_p anim = (sprite_animation_p)duk_get_pointer(ctx, 1);
	if (anim) sprite_animation_reset(anim);
	return 0;
}

static duk_ret_t janim_addclip(duk_context* ctx)
{
	sprite_animation_p anim = (sprite_animation_p)duk_get_pointer(ctx, 1);
	if (!anim) return 0;
	const char* path = duk_to_string(ctx, 2);
	SDL_FRect src;
	src.x = (float)duk_to_number(ctx, 4);
	src.y = (float)duk_to_number(ctx, 5);
	src.w = (float)duk_to_number(ctx, 6);
	src.h = (float)duk_to_number(ctx, 7);
	sprite_animation_add_clip(anim, path, (float)duk_to_number(ctx, 3), src);
	return 0;
}

static duk_ret_t janim_is_finished(duk_context* ctx)
{
	sprite_animation_p anim = (sprite_animation_p)duk_get_pointer(ctx, 1);
	duk_push_boolean(ctx, anim ? sprite_animation_is_finished(anim) : 1);
	return 1;
}

static duk_ret_t janim_set_position(duk_context* ctx)
{
	sprite_animation_p anim = (sprite_animation_p)duk_get_pointer(ctx, 1);
	if (anim) sprite_animation_set_position(anim, (float)duk_to_number(ctx, 2), (float)duk_to_number(ctx, 3));
	return 0;
}

static duk_ret_t janim_set_scale(duk_context* ctx)
{
	sprite_animation_p anim = (sprite_animation_p)duk_get_pointer(ctx, 1);
	if (anim) sprite_animation_set_scale(anim, (float)duk_to_number(ctx, 2), (float)duk_to_number(ctx, 3));
	return 0;
}

static duk_ret_t janim_update(duk_context* ctx)
{
	sprite_animation_p anim = (sprite_animation_p)duk_get_pointer(ctx, 1);
	if (anim) sprite_animation_update(anim, (float)duk_to_number(ctx, 2));
	return 0;
}

static duk_ret_t janim_draw(duk_context* ctx)
{
	sprite_animation_p anim = (sprite_animation_p)duk_get_pointer(ctx, 1);
	if (!anim) return 0;
	SDL_FRect camera = {0};
	if (!duk_is_undefined(ctx, 2)) {
		camera.x = (float)duk_to_number(ctx, 2);
		camera.y = (float)duk_to_number(ctx, 3);
		camera.w = (float)duk_to_number(ctx, 4);
		camera.h = (float)duk_to_number(ctx, 5);
	}
	sprite_animation_draw(anim, &camera);
	return 0;
}

void jsopen_animation(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, janim_create, 1);       duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, janim_destroy, 1);      duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, janim_reset, 1);        duk_put_prop_string(ctx, -2, "reset");
	duk_push_c_function(ctx, janim_addclip, 7);      duk_put_prop_string(ctx, -2, "addclip");
	duk_push_c_function(ctx, janim_is_finished, 1);   duk_put_prop_string(ctx, -2, "is_finished");
	duk_push_c_function(ctx, janim_set_position, 3); duk_put_prop_string(ctx, -2, "set_position");
	duk_push_c_function(ctx, janim_set_scale, 3);    duk_put_prop_string(ctx, -2, "set_scale");
	duk_push_c_function(ctx, janim_update, 2);       duk_put_prop_string(ctx, -2, "update");
	duk_push_c_function(ctx, janim_draw, 5);         duk_put_prop_string(ctx, -2, "draw");
	duk_put_global_string(ctx, "animation");
}