#include "../joy.h"
#include "jsclib.h"

static duk_ret_t jfont_create(duk_context* ctx) {
    const char* path = duk_to_string(ctx, 0);
    int size = (int)duk_to_number(ctx, 1);
    duk_push_pointer(ctx, font_create(path, size));
    return 1;
}

static duk_ret_t jfont_destroy(duk_context* ctx) {
    font_p f = (font_p)duk_get_pointer(ctx, 0);
    if (f) font_destroy(f);
    return 0;
}

void jsopen_font(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, jfont_create, 2);
    duk_put_prop_string(ctx, -2, "create");
    duk_push_c_function(ctx, jfont_destroy, 1);
    duk_put_prop_string(ctx, -2, "destroy");
    duk_put_global_string(ctx, "font");
}
