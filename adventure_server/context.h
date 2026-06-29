#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <queue>
#include <string>
#include "../flecs.h"
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"

struct Context {
        jwindow_p window;
        jr2d_p r2d;
      
        jtilemap_p tilemap;
        jtilemap_render_p tilemap_render;

        flecs::world ecs;

        vec2_t camera; // 摄像机世界坐标(米)

        int global_entity_id; // 玩家全局唯一 ID
        int global_frame_id;  // 当前帧 ID

        std::vector<S2CPlayerJoin> player_joins;
        std::vector<S2CPlayerLeave> player_leaves;
        std::vector<S2CPlayerInput> player_inputs;

        std::map<int, std::string> commands;
        std::queue<std::string> command_queue;
        std::map<int, std::string> world_snapshots; // 以帧号为键的世界快照

        netserver_p net_server;
};


static int get_entity_id(Context* ctx)
{
    return ctx->global_entity_id++;
}


#endif