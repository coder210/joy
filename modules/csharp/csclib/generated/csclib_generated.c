/* 自动生成，勿手改：tools/gen_csclib.js */
#include <stdint.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include "joy_csharp.h"
#include <joy/joy.h>

static intptr_t w_font_create(MonoString* filename, int fontsize)
{
        char* s_filename = mono_string_to_utf8(filename);
        intptr_t _ret = (intptr_t)font_create(s_filename, fontsize);
        mono_free(s_filename);
        return _ret;
}

static intptr_t w_image_create(intptr_t r2d, MonoString* filename)
{
        char* s_filename = mono_string_to_utf8(filename);
        intptr_t _ret = (intptr_t)image_create((jr2d_p)r2d, s_filename);
        mono_free(s_filename);
        return _ret;
}

void csopen_generated(void)
{
        csx_register_internal_call("Joy.Font::font_create", (const void*)w_font_create);
        csx_register_internal_call("Joy.Image::image_create", (const void*)w_image_create);
}
