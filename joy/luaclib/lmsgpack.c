#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include <msgpack.h>
#include "../joy.h"

/* ===== 递归 pack：将 Lua 值编码到 msgpack sbuffer ===== */
static int pack_one(lua_State* L, msgpack_packer* pk, int idx) {
    /* 转绝对索引，避免后续栈操作导致相对索引失效 */
    if (idx < 0) idx = lua_gettop(L) + idx + 1;
    int t = lua_type(L, idx);
    switch (t) {
    case LUA_TNIL:
        return msgpack_pack_nil(pk);
    case LUA_TBOOLEAN:
        return lua_toboolean(L, idx) ? msgpack_pack_true(pk) : msgpack_pack_false(pk);
    case LUA_TNUMBER: {
        if (lua_isinteger(L, idx)) {
            lua_Integer v = lua_tointeger(L, idx);
            if (v >= 0)
                return msgpack_pack_unsigned_long_long(pk, (unsigned long long)v);
            else
                return msgpack_pack_long_long(pk, (long long)v);
        } else {
            return msgpack_pack_double(pk, (double)lua_tonumber(L, idx));
        }
    }
    case LUA_TSTRING: {
        size_t len;
        const char* s = lua_tolstring(L, idx, &len);
        int ret = msgpack_pack_str(pk, len);
        if (ret != 0) return ret;
        return msgpack_pack_str_body(pk, s, len);
    }
    case LUA_TTABLE: {
        /* 区分 array 和 map */
        lua_len(L, idx);
        lua_Integer n = lua_tointeger(L, -1);
        lua_pop(L, 1);

        if (n > 0) {
            /* 当作 array 打包 */
            int ret = msgpack_pack_array(pk, (size_t)n);
            if (ret != 0) return ret;
            for (lua_Integer i = 1; i <= n; i++) {
                lua_rawgeti(L, idx, i);
                ret = pack_one(L, pk, -1);
                lua_pop(L, 1);
                if (ret != 0) return ret;
            }
        } else {
            /* 先数 map 的 k/v 对数 */
            lua_pushnil(L);
            size_t map_size = 0;
            while (lua_next(L, idx)) {
                lua_pop(L, 1);
                map_size++;
            }
            int ret = msgpack_pack_map(pk, map_size);
            if (ret != 0) return ret;
            lua_pushnil(L);
            while (lua_next(L, idx)) {
                /* key at -2, val at -1 */
                ret = pack_one(L, pk, -2);
                if (ret == 0) ret = pack_one(L, pk, -1);
                lua_pop(L, 1);
                if (ret != 0) return ret;
            }
        }
        return 0;
    }
    default:
        log_error("msgpack.pack: unsupported type %s", lua_typename(L, t));
        return -1;
    }
}

static int l_pack(lua_State* L) {
    int nargs = lua_gettop(L);
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    for (int i = 1; i <= nargs; i++) {
        if (pack_one(L, &pk, i) != 0) {
            msgpack_sbuffer_destroy(&sbuf);
            lua_pushnil(L);
            lua_pushstring(L, "msgpack.pack failed");
            return 2;
        }
    }

    lua_pushlstring(L, sbuf.data, sbuf.size);
    msgpack_sbuffer_destroy(&sbuf);
    return 1;
}

/* ===== 递归 unpack：将 msgpack_object 转为 Lua 值 ===== */
static void push_object(lua_State* L, msgpack_object obj) {
    switch (obj.type) {
    case MSGPACK_OBJECT_NIL:
        lua_pushnil(L);
        return;
    case MSGPACK_OBJECT_BOOLEAN:
        lua_pushboolean(L, obj.via.boolean);
        return;
    case MSGPACK_OBJECT_POSITIVE_INTEGER:
        lua_pushinteger(L, (lua_Integer)obj.via.u64);
        return;
    case MSGPACK_OBJECT_NEGATIVE_INTEGER:
        lua_pushinteger(L, (lua_Integer)obj.via.i64);
        return;
    case MSGPACK_OBJECT_FLOAT32:
    case MSGPACK_OBJECT_FLOAT64:
        lua_pushnumber(L, obj.via.f64);
        return;
    case MSGPACK_OBJECT_STR:
        lua_pushlstring(L, obj.via.str.ptr, obj.via.str.size);
        return;
    case MSGPACK_OBJECT_BIN:
        lua_pushlstring(L, obj.via.bin.ptr, obj.via.bin.size);
        return;
    case MSGPACK_OBJECT_ARRAY: {
        lua_createtable(L, obj.via.array.size, 0);
        for (uint32_t i = 0; i < obj.via.array.size; i++) {
            push_object(L, obj.via.array.ptr[i]);
            lua_seti(L, -2, i + 1);
        }
        return;
    }
    case MSGPACK_OBJECT_MAP: {
        lua_createtable(L, 0, obj.via.map.size);
        for (uint32_t i = 0; i < obj.via.map.size; i++) {
            /* key */
            push_object(L, obj.via.map.ptr[i].key);
            /* val */
            push_object(L, obj.via.map.ptr[i].val);
            lua_settable(L, -3);
        }
        return;
    }
    case MSGPACK_OBJECT_EXT:
        /* ext 类型用 lightuserdata + 二进制数据表示 */
        lua_pushlstring(L, obj.via.ext.ptr, obj.via.ext.size);
        return;
    default:
        lua_pushnil(L);
        return;
    }
}

static int l_unpack(lua_State* L) {
    size_t len;
    const char* data = luaL_checklstring(L, 1, &len);
    if (!data || len == 0) {
        lua_pushnil(L);
        return 1;
    }

    size_t off = 0;
    int count = 0;
    lua_newtable(L);  /* 结果表 */

    while (off < len) {
        msgpack_unpacked result;
        msgpack_unpacked_init(&result);
        msgpack_unpack_return ret = msgpack_unpack_next(&result, data, len, &off);

        if (ret == MSGPACK_UNPACK_SUCCESS || ret == MSGPACK_UNPACK_EXTRA_BYTES) {
            count++;
            push_object(L, result.data);
            lua_seti(L, -2, count);
            msgpack_unpacked_destroy(&result);
        } else if (ret == MSGPACK_UNPACK_CONTINUE) {
            msgpack_unpacked_destroy(&result);
            break;
        } else {
            msgpack_unpacked_destroy(&result);
            break;
        }
    }

    if (count == 0) {
        lua_pop(L, 1);
        lua_pushnil(L);
        return 1;
    }
    if (count == 1) {
        /* 单值直接返回，不包表 */
        lua_rawgeti(L, -1, 1);
        lua_replace(L, -2);
        return 1;
    }
    return 1;  /* 多值返回 {val1, val2, ...} */
}

/* ===== 注册 ===== */
int luaopen_msgpack(lua_State* L) {
    luaL_checkversion(L);
    luaL_Reg lib[] = {
        {"pack",   l_pack},
        {"unpack", l_unpack},
        {NULL, NULL}
    };
    luaL_newlib(L, lib);
    return 1;
}
