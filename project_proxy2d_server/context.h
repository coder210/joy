#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <functional>
#include "entt.hpp"
#include "../project_proxy2d_shared/components/player_event_input.h"
#include "components/connection_component.h"
#include "../project_proxy2d_shared/components/player_state_input.h"
#include "../project_proxy2d_shared/components/id_component.h"
#include "../project_proxy2d_shared/components/logic_rect_component.h"
#include "../project_proxy2d_shared/components/logic_position_component.h"
#include "../project_proxy2d_shared/components/logic_velocity_component.h"

#include "../project_proxy2d_shared/components/transform_component.h"
#include "../project_proxy2d_shared/components/attack_ray_effect_component.h"
#include "../project_proxy2d_shared/components/player_component.h"
#include "components/local_only_component.h"
#include "../project_proxy2d_shared/components/action_state_component.h"
#include "../project_proxy2d_shared/components/sprite_sheet_component.h"
#include "../project_proxy2d_shared/components/animator_component.h"
#include "../project_proxy2d_shared/components/animator_def.h"

#include "../project_proxy2d_shared/components/ai_component.h"
#include "../project_proxy2d_shared/components/enemy_component.h"
#include "../project_proxy2d_shared/components/net_component.h"
#include "../project_proxy2d_shared/components/global_state_component.h"
#include "../project_proxy2d_shared/protocol.h"
#include <joy/joy.h>

struct Context {
        bool is_server = true;

        jwindow_p window;
        jr2d_p r2d;
      
        jtilemap_p tilemap;
        jtilemap_render_p tilemap_render;

        entt::registry ecs;

        std::vector<S2CPlayerJoin> player_joins;
        std::vector<S2CPlayerLeave> player_leaves;
        std::vector<S2CPlayerInput> player_inputs;

        std::map<int, std::string> commands;
        std::queue<std::string> command_queue;
        std::map<int, std::string> world_snapshots; // 以帧号为键的世界快照

        netserver_p net_server;

        S2CEntity entities[512]; 
        int num_entities = 0;

        // ---- 固定步长逻辑 ----
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 20.0f; // 20Hz

        std::vector<struct SystemEntry> systems;
};

// ---- System 调度（自由函数） ----
enum Phase { OnStart, PreUpdate, OnUpdate, PostUpdate };

struct SystemEntry {
        std::string name;
        Phase phase;
        std::function<void(entt::registry&, Context*, float)> fn;
};

inline void add_system(Context* ctx, Phase phase, const char* name,
        void (*fn)(entt::registry&, Context*, float))
{
        ctx->systems.push_back({ name, phase, fn });
}

inline void run_phase(Context* ctx, Phase phase, float dt) {
        for (auto& sys : ctx->systems) {
                if (sys.phase == phase)
                        sys.fn(ctx->ecs, ctx, dt);
        }
}


// 从 GlobalState 单例获取下一个全局实体 ID
static inline int get_entity_id(Context* ctx) {
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    return gs.next_entity_id++;
}


#endif