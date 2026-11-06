/*
 * Lua 后端薄适配（IDL 驱动）。
 *
 * 遍历 joy_thunks_table()，把每条开放 API 注册成全局
 * `Joy.Api.Bindings.<PascalName>`，与 cs / js 端同名、同参数顺序、同句柄语义。
 *
 * 唯一和 Lua 相关的只有三件事：
 *   1. joy_value <-> Lua 值转换（按注册表里的 arg_kinds / ret_kind）
 *   2. 句柄用 userdata + __gc，脚本侧看不到 uint64
 *   3. 错误用 luaL_error 抛出
 *
 * 新增 API 不需要改这个文件：重新生成 thunks_generated.c 即可。
 */
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
#include "joy_lua.h"
#include <joy/joy.h>
#include <joy/api/runtime/joy_value.h>
#include <joy/api/runtime/joy_obj.h>
#include <joy/api/runtime/joy_thunk.h>
#include <stdio.h>
#include <string.h>

#define LUAX_HANDLE_MT "joy.handle"
#define LUAX_MAX_ARGS  16

typedef struct luax_handle {
        uint64_t h;
} luax_handle;

static int luax_handle_gc(lua_State* L)
{
        luax_handle* u = (luax_handle*)luaL_checkudata(L, 1, LUAX_HANDLE_MT);
        if (u->h) {
                joy_obj_release(u->h);
                u->h = 0;
        }
        return 0;
}

static void luax_push_handle(lua_State* L, uint64_t h)
{
        luax_handle* u = (luax_handle*)lua_newuserdata(L, sizeof(luax_handle));
        u->h = h;
        luaL_setmetatable(L, LUAX_HANDLE_MT);
}

static const char* luax_where(void)
{
        const char* w = joy_last_error_where();
        return w ? w : "?";
}

/* upvalue 1：C API 名（snake_case） */
static int luax_api_dispatch(lua_State* L)
{
        const char* name = lua_tostring(L, lua_upvalueindex(1));
        int count = 0;
        int i;
        int n;
        const joy_thunk_entry* tab = joy_thunks_table(&count);
        const joy_thunk_entry* e = NULL;
        joy_value_t args[LUAX_MAX_ARGS];
        joy_value_t r;
        joy_status_t st;

        for (i = 0; i < count; i++) {
                if (strcmp(tab[i].name, name) == 0) { e = &tab[i]; break; }
        }
        if (!e) return luaL_error(L, "Joy.Api.Bindings: '%s' not registered", name);

        n = lua_gettop(L);
        if (n != e->argc) {
                return luaL_error(L, "Joy.Api.Bindings.%s expects %d argument(s), got %d",
                        e->script_name, e->argc, n);
        }
        if (n > LUAX_MAX_ARGS) {
                return luaL_error(L, "Joy.Api.Bindings.%s: too many arguments", e->script_name);
        }

        for (i = 0; i < n; i++) {
                switch (e->arg_kinds[i]) {
                case JOY_V_BOOL:   args[i] = joy_v_bool(lua_toboolean(L, i + 1)); break;
                case JOY_V_I32:    args[i] = joy_v_i32((int32_t)luaL_checkinteger(L, i + 1)); break;
                case JOY_V_U32:    args[i] = joy_v_u32((uint32_t)luaL_checkinteger(L, i + 1)); break;
                case JOY_V_I64:    args[i] = joy_v_i64((int64_t)luaL_checkinteger(L, i + 1)); break;
                case JOY_V_U64:    args[i] = joy_v_u64((uint64_t)luaL_checkinteger(L, i + 1)); break;
                case JOY_V_F64:    args[i] = joy_v_f64((double)luaL_checknumber(L, i + 1)); break;
                case JOY_V_STR:    args[i] = joy_v_str(luaL_checkstring(L, i + 1)); break;
                case JOY_V_HANDLE: {
                        luax_handle* u = (luax_handle*)luaL_checkudata(L, i + 1, LUAX_HANDLE_MT);
                        args[i] = joy_v_handle(u->h);
                        break;
                }
                default:
                        return luaL_error(L, "Joy.Api.Bindings.%s: unsupported argument #%d",
                                e->script_name, i + 1);
                }
        }

        st = JOY_OK;
        r = e->fn(args, n, &st);
        if (st != JOY_OK) {
                return luaL_error(L, "Joy.Api.Bindings.%s failed: %s (%s)",
                        e->script_name, joy_status_name(st), luax_where());
        }

        switch (e->ret_kind) {
        case JOY_V_NULL:   return 0;
        case JOY_V_BOOL:   lua_pushboolean(L, r.as.b != 0); return 1;
        case JOY_V_I32:    lua_pushinteger(L, (lua_Integer)r.as.i32); return 1;
        case JOY_V_U32:    lua_pushinteger(L, (lua_Integer)r.as.u32); return 1;
        case JOY_V_I64:    lua_pushinteger(L, (lua_Integer)r.as.i64); return 1;
        case JOY_V_U64:    lua_pushinteger(L, (lua_Integer)r.as.u64); return 1;
        case JOY_V_F64:    lua_pushnumber(L, (lua_Number)r.as.f64); return 1;
        case JOY_V_STR:    lua_pushstring(L, r.as.str ? r.as.str : ""); return 1;
        case JOY_V_HANDLE: luax_push_handle(L, r.as.handle); return 1;
        default:           lua_pushnil(L); return 1;
        }
}

void luax_open_api(void* Lv)
{
        lua_State* L = (lua_State*)Lv;
        int count = 0;
        int i;
        const joy_thunk_entry* tab;

        if (!L) return;
        tab = joy_thunks_table(&count);

        luaL_newmetatable(L, LUAX_HANDLE_MT);
        lua_pushcfunction(L, luax_handle_gc);
        lua_setfield(L, -2, "__gc");
        lua_pop(L, 1);

        lua_newtable(L);  /* Joy */
        lua_newtable(L);  /* Joy.Api */
        lua_newtable(L);  /* Joy.Api.Bindings */
        for (i = 0; i < count; i++) {
                lua_pushstring(L, tab[i].name);
                lua_pushcclosure(L, luax_api_dispatch, 1);
                lua_setfield(L, -2, tab[i].script_name);
        }
        lua_setfield(L, -2, "Bindings");
        lua_setfield(L, -2, "Api");
        lua_setglobal(L, "Joy");
}

const char* luax_api_selftest(void)
{
        static char errbuf[256];
        uint32_t live0 = joy_obj_live_count();
        uint32_t dead0 = joy_obj_destroy_count();
        lua_State* L = luaL_newstate();
        if (!L) return "lua/newstate";

        luaL_openlibs(L);
        luax_open_api(L);
        if (luaL_dostring(L,
                "local t = Joy.Api.Bindings.GetTicks()\n"
                "assert(type(t) == 'number' and t >= 0, 'ticks')\n"
                "local c = Joy.Api.Bindings.JtextCacheGetCount()\n"
                "assert(type(c) == 'number', 'cache')\n"
                "local r = Joy.Api.Bindings.LcgRandomCreate(12345)\n"
                "local a = Joy.Api.Bindings.LcgRandomNext(r)\n"
                "local b = Joy.Api.Bindings.LcgRandomNext(r)\n"
                "assert(a ~= b, 'lcg')\n"
                "local ok = pcall(Joy.Api.Bindings.LcgRandomNext, 1)\n"
                "assert(not ok, 'type guard')\n"
                "r = nil\n"
                "collectgarbage('collect')\n") != 0) {
                const char* msg = lua_tostring(L, -1);
                snprintf(errbuf, sizeof(errbuf), "lua/%s", msg ? msg : "unknown");
                lua_close(L);
                return errbuf;
        }

        lua_close(L);
        if (joy_obj_live_count() != live0) return "lua/handle-leak";
        if (joy_obj_destroy_count() != dead0 + 1) return "lua/handle-not-released";
        return NULL;
}
