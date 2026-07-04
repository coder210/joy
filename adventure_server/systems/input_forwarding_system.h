#ifndef INPUT_FORWARDING_SYSTEM_H
#define INPUT_FORWARDING_SYSTEM_H
#include "../flecs.h"
#include "../context.h"

static int get_target_frame_id(int curr_frameid, int context_frameid)
{
    int diff = context_frameid - curr_frameid;

    if (diff >= 50)      return curr_frameid + 9;
    if (diff >= 30)      return curr_frameid;
    if (diff >= 10)      return curr_frameid + 4;
    if (diff > 2)        return curr_frameid + 2;

    return curr_frameid;
}

static void input_forwarding_system(flecs::iter& it)
{
    Context* ctx = (Context*)it.world().get_ctx();
    for (auto& [conv, sess] : ctx->sessions) {
            // ---- 未加载的客户端：发送初始世界快照 ----
            if (!sess.is_loaded) {
                    int taget_frameid = get_target_frame_id(sess.frameid, ctx->global_frame_id);
                    for (int i = sess.frameid; i < taget_frameid; i++) {
                            auto it = ctx->commands.find(i);
                            if (it != ctx->commands.end()) {
                                    netserver_send(ctx->net_server, sess.conv, it->second.c_str(), it->second.size());
                            }
                            else {
                                    log_error("Frame %d not found in commands, skip sending.", i);
                            }
                    }
                    sess.frameid = taget_frameid;
            }
    }
}

#endif
