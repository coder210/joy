#include "jsclib.h"
#include "../jutils.h"
#include <stdlib.h>
#include <string.h>
#include "SDL3/SDL.h"

static void jutils_current_datetime(js_State *J)
{
        const char *format = js_tostring(J, 1);
        char buf[1024] = {0};
        utils_current_datetime(format, buf, sizeof(buf));
        js_pushstring(J, buf);
}

static void jutils_current_time(js_State *J)
{
        int64_t time;
        utils_get_current_time(&time);
        js_pushnumber(J, (double)time);
}

static void jutils_bit2short(js_State *J)
{
        const char *buf = js_tostring(J, 1);
        int r = utils_bit2short((uint8_t*)buf);
        js_pushnumber(J, r);
}

static void jutils_short2bit(js_State *J)
{
        char buf[2];
        short value = (short)js_tonumber(J, 1);
        utils_short2bit((uint8_t*)buf, value);
        js_pushlstring(J, buf, 2);
}

static void jutils_bit2int(js_State *J)
{
        const char *buf = js_tostring(J, 1);
        int r = utils_bit2int((uint8_t*)buf);
        js_pushnumber(J, r);
}

static void jutils_int2bit(js_State *J)
{
        char buf[4];
        int value = (int)js_tonumber(J, 1);
        utils_int2bit((uint8_t*)buf, value);
        js_pushlstring(J, buf, 4);
}

static void jutils_des_encrypt(js_State *J)
{
        size_t keysz, textsz, out_len;
        const char *key = js_tostring(J, 1);
        const char *text = js_tostring(J, 2);
        keysz = strlen(key);
        textsz = strlen(text);
        char *result = utils_des_crypt(key, text, textsz, &out_len);
        js_pushlstring(J, result, out_len);
        SDL_free(result);
}

static void jutils_des_decrypt(js_State *J)
{
        size_t keysz, textsz, out_len;
        const char *key = js_tostring(J, 1);
        const char *text = js_tostring(J, 2);
        keysz = strlen(key);
        textsz = strlen(text);
        char *result = utils_des_decrypt(key, text, textsz, &out_len);
        js_pushlstring(J, result, out_len);
        SDL_free(result);
}

static void jutils_encrypt_file(js_State *J)
{
        const char *path = js_tostring(J, 1);
        const char *key = js_tostring(J, 2);
        size_t path_len = strlen(path);
        size_t keysz = strlen(key);
        size_t datasz, out_len;

        char *data = (char*)SDL_LoadFile(path, &datasz);
        if (data) {
                size_t new_file_sz = path_len + 2;
                char *new_file = (char*)SDL_malloc(new_file_sz);
                SDL_strlcpy(new_file, path, new_file_sz);
                SDL_strlcat(new_file, "s", new_file_sz);
                new_file[path_len + 1] = 0;
                char *result = utils_des_crypt(key, data, datasz, &out_len);
                bool ok = SDL_SaveFile(new_file, result, out_len);
                js_pushboolean(J, ok);
                js_pushstring(J, new_file);
                SDL_free(result);
                SDL_free(data);
                SDL_free(new_file);
        } else {
                js_pushboolean(J, false);
                js_pushundefined(J);
        }
}

static void jutils_decrypt_file(js_State *J)
{
        const char *path = js_tostring(J, 1);
        const char *key = js_tostring(J, 2);
        size_t path_len = strlen(path);
        size_t keysz = strlen(key);
        size_t datasz, out_len;

        char *data = (char*)SDL_LoadFile(path, &datasz);
        if (data) {
                size_t new_file_sz = path_len + 1;
                char *new_file = (char*)SDL_malloc(new_file_sz);
                SDL_strlcpy(new_file, path, new_file_sz - 1);
                SDL_strlcat(new_file, "x", new_file_sz);
                char *result = utils_des_decrypt(key, data, datasz, &out_len);
                bool ok = SDL_SaveFile(new_file, result, out_len);
                js_pushboolean(J, ok);
                js_pushstring(J, new_file);
                SDL_free(result);
                SDL_free(data);
                SDL_free(new_file);
        } else {
                js_pushboolean(J, false);
                js_pushundefined(J);
        }
}

void jsopen_utils(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jutils_current_datetime, "current_datetime", 1); js_setproperty(J, -2, "current_datetime");
        js_newcfunction(J, jutils_current_time, "current_time", 0);         js_setproperty(J, -2, "current_time");
        js_newcfunction(J, jutils_bit2short, "bit2short", 1);               js_setproperty(J, -2, "bit2short");
        js_newcfunction(J, jutils_short2bit, "short2bit", 1);               js_setproperty(J, -2, "short2bit");
        js_newcfunction(J, jutils_bit2int, "bit2int", 1);                   js_setproperty(J, -2, "bit2int");
        js_newcfunction(J, jutils_int2bit, "int2bit", 1);                   js_setproperty(J, -2, "int2bit");
        js_newcfunction(J, jutils_des_encrypt, "des_encrypt", 2);           js_setproperty(J, -2, "des_encrypt");
        js_newcfunction(J, jutils_des_decrypt, "des_decrypt", 2);           js_setproperty(J, -2, "des_decrypt");
        js_newcfunction(J, jutils_encrypt_file, "encrypt_file", 2);         js_setproperty(J, -2, "encrypt_file");
        js_newcfunction(J, jutils_decrypt_file, "decrypt_file", 2);         js_setproperty(J, -2, "decrypt_file");
        js_setglobal(J, "utils");
}
