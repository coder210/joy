#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <functional>
#include "entt.hpp"
#include "components/player_event_input.h"
#include "components/connection_component.h"
#include "components/player_state_input.h"
#include "components/id_component.h"
#include "components/logic_rect_component.h"
#include "components/logic_position_component.h"
#include "components/logic_velocity_component.h"

#include "components/transform_component.h"
#include "components/attack_ray_effect_component.h"
#include "components/player_component.h"
#include "components/local_only_component.h"
#include "components/player_action_component.h"
#include "components/sprite_sheet_component.h"
#include "components/animator_component.h"
#include "components/animator_def.h"

#include "components/ai_component.h"
#include "components/enemy_component.h"
#include "components/net_component.h"
#include "../adventure_shared/protocol.h"
#include <joy/joy.h>

struct Session {
        int conv;
        entt::entity player_entity = entt::null;  // 对应的玩家实体（加载完成后才赋值）
        bool is_loaded = false;       // 是否已完成初始加载
        int frameid = 0;              // 客户端最后已接收的帧号
};

struct Context {
        jwindow_p window;
        jr2d_p r2d;
      
        jtilemap_p tilemap;
        jtilemap_render_p tilemap_render;

        entt::registry ecs;
        entt::entity singleton_net = entt::null;


        vec2_t camera; // 摄像机世界坐标(米)

        int global_entity_id; // 玩家全局唯一 ID
        int global_frame_id;  // 当前帧 ID

        std::vector<S2CPlayerJoin> player_joins;
        std::vector<S2CPlayerLeave> player_leaves;
        std::vector<S2CPlayerInput> player_inputs;

        std::map<int, std::string> commands;
        std::queue<std::string> command_queue;
        std::map<int, std::string> world_snapshots; // 以帧号为键的世界快照

        std::unordered_map<int, Session> sessions;   // key = conv

        netserver_p net_server;

        S2CEntity entities[512]; 
        int num_entities = 0;

        // ---- 固定步长逻辑 ----
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 20.0f; // 20Hz

        // ---- System 注册表（自动调度） ----
        enum Phase { OnStart, PreUpdate, OnUpdate, PostUpdate };
        struct SystemEntry {
                std::string name;
                Phase phase;
                std::function<void(entt::registry&, Context*, float)> fn;
        };
        std::vector<SystemEntry> systems;

        template<typename... Components>
        void add_system(Phase phase, const char* name,
                void (*fn)(entt::registry&, Context*, float))
        {
                systems.push_back({ name, phase, fn });
        }

        void run_phase(Phase phase, float dt) {
                for (auto& sys : systems) {
                        if (sys.phase == phase)
                                sys.fn(ecs, this, dt);
                }
        }
};


static int get_entity_id(Context* ctx)
{
    return ctx->global_entity_id++;
}


#endif