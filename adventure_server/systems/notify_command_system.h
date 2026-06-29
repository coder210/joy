#ifndef NOTIFY_COMMAND_SYSTEM_H
#define NOTIFY_COMMAND_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../components/connection_component.h"
#include "../components/net_component.h"



static int get_target_frame_id(int curr_frameid, int context_frameid)
{
    int diff = context_frameid - curr_frameid;

    if (diff >= 50)      return curr_frameid + 9;
    if (diff >= 30)      return curr_frameid;
    if (diff >= 10)      return curr_frameid + 4;
    if (diff > 2)        return curr_frameid + 2;

    return curr_frameid;
}

static void notify_command_system(flecs::entity e, ConnectionComponent& conn)
{
    Context* ctx = (Context*)e.world().get_ctx();
    int taget_frameid = get_target_frame_id(conn.frameid, ctx->global_frame_id);
    for (int i = conn.frameid; i < taget_frameid; i++) {
        auto it = ctx->commands.find(i);
        if (it != ctx->commands.end()) {
            netserver_send(ctx->net_server, conn.conv, it->second.c_str(), it->second.size());
        }
        else {
            log_error("Frame %d not found in commands, skip sending.", i);
        }
    }
    conn.frameid = taget_frameid;
}


#endif
