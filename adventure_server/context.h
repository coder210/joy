#ifndef CONTEXT_H
#define CONTEXT_H
#include <string>
#include <map>
#include <queue>
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"

struct Context {
        jwindow_p window;
        jr2d_p r2d;
      
        int global_id; // 玩家全局唯一 ID
        int frame_id;  // 当前帧 ID
        std::vector<S2CPlayerJoin> player_joins;
        std::vector<S2CPlayerLeave> player_leaves;
        std::vector<S2CPlayerInput> player_inputs;

        std::map<int, std::string> commands;
        std::queue<std::string> command_queue;
        std::map<int, std::string> worlds;
        netserver_p netserver;
};


#endif