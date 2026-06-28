#ifndef SYNC_SYSTEM_H
#define SYNC_SYSTEM_H
#include "../flecs.h"
#include <joy/jnetwork.h>
#include "../context.h"
#include "../components/connection_component.h"
#include "../components/net_component.h"

static int GetTargetFrameId(int curr_frameid, int context_frameid)
{
        int diff = context_frameid - curr_frameid;

        if (diff >= 50)      return curr_frameid + 9;
        if (diff >= 30)      return curr_frameid;
        if (diff >= 10)      return curr_frameid + 4;
        if (diff > 2)        return curr_frameid + 2;

        return curr_frameid;
}

static void sync_system(flecs::entity e, ConnectionComponent& conn)
{
        NetComponent* net = e.world().get_mut<NetComponent>();
        int taget_frameid = GetTargetFrameId(conn.frameid, net->frame_id);
        for (int i = conn.frameid; i < taget_frameid; i++) {
                auto it = self->commands.find(i);
                if (it != self->commands.end()) {
                        netserver_send(self->netserver, conn.conv, it->second.c_str(), it->second.size());
                }
                else {
                        log_error("Frame %d not found in commands, skip sending.", i);
                }
        }
        conn.frameid = taget_frameid;
}

#endif
