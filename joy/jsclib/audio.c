#include "jsclib.h"
#include "../jaudio.h"

static void jaudio_open_device(js_State *J)
{
        js_pushnumber(J, audio_open_device());
}

static void jaudio_close_device(js_State *J)
{
        audio_close_device((uint32_t)js_tonumber(J, 1));
}

static void jaudio_create_wav(js_State *J)
{
        const char *filepath = js_tostring(J, 1);
        uint32_t device_id = (uint32_t)js_tonumber(J, 2);
        js_pushboolean(J, audio_create_wav(filepath, device_id));
}

static void jaudio_create_ogg(js_State *J)
{
        const char *filepath = js_tostring(J, 1);
        uint32_t device_id = (uint32_t)js_tonumber(J, 2);
        //audio_create_ogg(filepath, device_id);
        js_pushboolean(J, true);
}

void jsopen_audio(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jaudio_open_device, "open_device", 0);  js_setproperty(J, -2, "open_device");
        js_newcfunction(J, jaudio_close_device, "close_device", 1); js_setproperty(J, -2, "close_device");
        js_newcfunction(J, jaudio_create_wav, "create_wav", 2);    js_setproperty(J, -2, "create_wav");
        js_newcfunction(J, jaudio_create_ogg, "create_ogg", 2);    js_setproperty(J, -2, "create_ogg");
        js_setglobal(J, "audio");
}
