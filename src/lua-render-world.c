#include "luaclib.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "external/klib/khash.h"
#include "liv-world.h"


static int l_render_world_create(lua_State* L)
{
        render_world_p world;
        world = render_world_create();
        lua_pushlightuserdata(L, world);
        return 1;
}

static int l_render_world_destroy(lua_State* L)
{
        render_world_p world;
        world = lua_touserdata(L, 1);
        render_world_destroy(world);
        return 0;
}

int luaopen_render_world(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create", l_render_world_create},
            {"destroy", l_render_world_destroy},
            {NULL, NULL},
        };
        luaL_newlib(L, l);
        return 1;
}











//static int lserver_update(lua_State* L)
//{
//        c2s_t c2s;
//        net_message_t msg;
//        lockstep_server_p ctx;
//        int len;
//
//        ctx = lua_touserdata(L, 1);
//        liv_ks_update(ctx->ks);
//        if (liv_ks_poll_message(ctx->ks, &msg)) {
//                if (msg.type == 0) {
//                        liv_log_info("connected:%d", msg.conv);
//                }
//                else if (msg.type == 1) {
//                        liv_log_info("disconnected:%d", msg.conv);
//                }
//                else if (msg.type == 2) {
//                        liv_log_info("msg:%d", msg.conv);
//                        if (c2s_deserialize(&c2s, msg.data, msg.len)) {
//                                if (c2s.cmd == C2S_CMD_READY) {
//                                        handle_cmd_ready(ctx, msg.conv, &c2s);
//                                }
//                                else if (c2s.cmd == C2S_CMD_LOADING) {
//                                        handle_cmd_loading(ctx, msg.conv, &c2s);
//                                }
//                        }
//                }
//        }
//
//        return 1;
//}
