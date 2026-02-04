#include "liv-utils.h"
#include <stdlib.h>
#include <string.h>
#include "./external/lua/lapi.h"
#include "./external/lua/lauxlib.h"
#include "SDL3/SDL.h"
#include "luaclib.h"

static int lcurrent_datetime(lua_State *L)
{
        const char *format;
        char buf[LIVNET_MAX_BUFFER];
        memset(buf, 0, LIVNET_MAX_BUFFER);
        format = luaL_checkstring(L, 1);
        utils_current_datetime(format, buf, LIVNET_MAX_BUFFER);
        lua_pushstring(L, buf);
        return 1;
}

static int lcurrent_time(lua_State *L)
{
        int64_t time;
        utils_get_current_time(&time);
        lua_pushinteger(L, time);
        return 1;
}

static int lbit2short(lua_State *L)
{
        const char *buf;
        int r;
        buf = luaL_checkstring(L, 1);
        r = utils_bit2short((uint8_t*)buf);
        lua_pushinteger(L, r);
        return 1;
}

static int lshort2bit(lua_State *L)
{
        char buf[2];
        short value;
        value = (short)luaL_checkinteger(L, 1);
        utils_short2bit((uint8_t*)buf, value);
        lua_pushlstring(L, buf, 2);
        return 1;
}

static int lbit2int(lua_State *L)
{
        const char *buf;
        int r;
        buf = luaL_checkstring(L, 1);
        r = utils_bit2int((uint8_t*)buf);
        lua_pushinteger(L, r);
        return 1;
}

static int lint2bit(lua_State *L)
{
        char buf[4];
        short value;
        value = (short)luaL_checkinteger(L, 1);
        utils_int2bit((uint8_t*)buf, value);
        lua_pushlstring(L, buf, 4);
        return 1;
}

static int lmd5(lua_State *L)
{
        const char *input;
        size_t input_len;
        char output[64] = { 0 };
        short n;
        input = luaL_checklstring(L, 1, &input_len);
        n = utils_md5x(input, input_len, output);
        lua_pushlstring(L, output, n);
        return 1;
}


static int lmd5x(lua_State *L)
{
        const char *input;
        size_t input_len;
        char output[64] = { 0 };
        short n;
        input = luaL_checklstring(L, 1, &input_len);
        n = utils_md5x(input, input_len, output);
        lua_pushstring(L, output);
        return 1;
}

static int lb64_encode(lua_State *L)
{
	size_t size;
        const char *input;
        char *result;
	input = luaL_checklstring(L, 1, &size);
	result = utils_b64_encode(input, size);
	lua_pushstring(L, result);
	free(result);
	return 1;
}

static int lb64_decode(struct lua_State* L)
{
	size_t size, de_size;
	const char *input;
        char *result;
        input = luaL_checklstring(L, 1, &size);
	result = utils_b64_decode(input, size, &de_size);
	lua_pushlstring(L, result, de_size);
	lua_pushinteger(L, de_size);
	free(result);
	return 2;
}

static int laes_encrypt256(lua_State *L)
{
        size_t key_len, input_len, out_len;
	const char *KEY, *IV, *input;
        unsigned char *out;

        KEY = luaL_checklstring(L, 1, &key_len);
	IV = luaL_checkstring(L, 2);
	input = luaL_checklstring(L, 3, &input_len);
	if (key_len == 32) { 
                out = utils_aes_encrypt256(input, &out_len, KEY, IV);
		lua_pushlstring(L, out, out_len);
		lua_pushinteger(L, out_len);
		free(out);
	}
	else {
		lua_pushnil(L);
		lua_pushinteger(L, 0);
	}

	return 2;
}

static int laes_decrypt256(lua_State *L)
{
        size_t key_len, input_len, out_len;
	const char *KEY, *IV;
        const char *input;
        unsigned char *out;

        KEY = luaL_checklstring(L, 1, &key_len);
	IV = luaL_checkstring(L, 2);

	input = luaL_checklstring(L, 3, &input_len);
	if (key_len == 32) {
		out = utils_aes_decrypt256(input, &out_len, KEY, IV);
		lua_pushlstring(L, out, out_len);
		lua_pushinteger(L, out_len);
		free(out);
	}
	else {
		lua_pushnil(L);
		lua_pushinteger(L, 0);
	}

	return 2;
}

static int laes_encrypt128(lua_State *L)
{
        size_t key_len, input_len;
	const char *KEY, *IV;
        const char *input;
	size_t out_len;
        unsigned char *out;

        KEY = luaL_checklstring(L, 1, &key_len);
	IV = luaL_checkstring(L, 2);
	input = luaL_checklstring(L, 3, &input_len);
	if (key_len == 16) {
		out = utils_aes_encrypt128(input, &out_len, KEY, IV);
		lua_pushlstring(L, out, out_len);
		lua_pushinteger(L, out_len);
		free(out);
	}
	else {
		lua_pushnil(L);
		lua_pushinteger(L, 0);
	}
	return 2;
}

static int laes_decrypt128(lua_State *L)
{
        size_t key_len;
	int input_len, out_len;
	const char *KEY, *IV, *input;
        unsigned char *out;
        
        KEY = luaL_checklstring(L, 1, &key_len);
	IV = luaL_checkstring(L, 2);
	input = luaL_checklstring(L, 3, &input_len);
	if (key_len == 16) {
		out = utils_aes_decrypt128(input, &out_len, KEY, IV);
		lua_pushlstring(L, out, out_len);
		lua_pushinteger(L, out_len);
		free(out);
	}
	else {
		lua_pushnil(L);
		lua_pushinteger(L, 0);
	}

	return 2;
}

static int ldes_encrypt(lua_State *L)
{
	size_t keysz, textsz, out_len;
	const char *key, *text;
        char *result;
        key = luaL_checklstring(L, 1, &keysz);
        text = luaL_checklstring(L, 2, &textsz);
	result = utils_des_crypt(key, text, textsz, &out_len);
        lua_pushlstring(L, result, out_len);
        SDL_free(result);
	return 1;
}


static int ldes_decrypt(lua_State *L)
{
        size_t keysz, textsz, out_len;
	const char *key, *text;
        char *result;
        key = luaL_checklstring(L, 1, &keysz);
        text = luaL_checklstring(L, 2, &textsz);
	result = utils_des_decrypt(key, text, textsz, &out_len);
        lua_pushlstring(L, result, out_len);
        SDL_free(result);
	return 1;
}


static int lencrypt_file(lua_State *L)
{
	const char *key, *path;
        char *data, *result, *new_file;
	size_t path_len, new_file_sz, keysz, datasz, out_len;
        bool ok;

        path = luaL_checklstring(L, 1, &path_len);
        key = luaL_checklstring(L, 2, &keysz);

        data = (char*)SDL_LoadFile(path, &datasz);
        if (data) {
                new_file_sz = sizeof(char) * (path_len + 2);
                new_file = (char*)SDL_malloc(new_file_sz);
                SDL_strlcpy(new_file, path, new_file_sz);
                SDL_strlcat(new_file, "s", new_file_sz);
                new_file[path_len + 1] = 0;
                result = utils_des_crypt(key, data, datasz, &out_len);
                ok = SDL_SaveFile(new_file, result, out_len);
                lua_pushboolean(L, ok);
                lua_pushlstring(L, new_file, new_file_sz);
                SDL_free(result);
                SDL_free(data);
                SDL_free(new_file);
        }
        else{
                lua_pushboolean(L, false);
                lua_pushstring(L, "");
        }

	return 2;
}

static int ldecrypt_file(lua_State *L)
{
        const char* key, * path;
        char* data, * result, * new_file;
        size_t path_len, new_file_sz, keysz, datasz, out_len;
        bool ok;

        path = luaL_checklstring(L, 1, &path_len);
        key = luaL_checklstring(L, 2, &keysz);

        data = (char*)SDL_LoadFile(path, &datasz);
        if (data) {
                new_file_sz = sizeof(char) * (path_len + 1);
                new_file = (char*)SDL_malloc(new_file_sz);
                SDL_strlcpy(new_file, path, new_file_sz - 1);
                SDL_strlcat(new_file, "x", new_file_sz);
                result = utils_des_decrypt(key, data, datasz, &out_len);
                ok = SDL_SaveFile(new_file, result, out_len);
                lua_pushboolean(L, ok);
                lua_pushlstring(L, new_file, new_file_sz);
                SDL_free(result);
                SDL_free(data);
                SDL_free(new_file);
        }
        else {
                lua_pushboolean(L, false);
                lua_pushstring(L, "");
        }

        return 2;
}



int luaopen_utils(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"current_datetime", lcurrent_datetime},
                {"current_time", lcurrent_time},
                {"bit2short", lbit2short},
                {"short2bit", lshort2bit},
                {"bit2int", lbit2int},
                {"int2bit", lint2bit},
                {"md5", lmd5},
                {"md5x", lmd5x},
                {"b64_encode", lb64_encode},
                {"b64_decode", lb64_decode},
                {"aes_encrypt256", laes_encrypt256},
                {"aes_decrypt256", laes_decrypt256},
                {"aes_encrypt128", laes_encrypt128},
                {"aes_decrypt128", laes_decrypt128},
                {"des_encrypt", ldes_encrypt},
                {"des_decrypt", ldes_decrypt},
                {"encrypt_file", lencrypt_file},
                {"decrypt_file", ldecrypt_file},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
