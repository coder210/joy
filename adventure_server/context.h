#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <unordered_map>
#include <queue>
#include <string>
#include "flecs.h"
#include "components/player_event_input.h"
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
        flecs::entity player_entity;  // 对应的玩家实体（加载完成后才赋值）
        int health = 10;
        uint32_t last_heartbeat = 0;
        bool is_loaded = false;       // 是否已完成初始加载
        uint32_t last_processed_seq; // 最后已处理的序列号
        int frameid; // 客户端最后已接收的帧号
};

struct Context {
        jwindow_p window;
        jr2d_p r2d;
      
        jtilemap_p tilemap;
        jtilemap_render_p tilemap_render;

        flecs::world ecs;

        flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;


        flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
        flecs::query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent> enemy_query;
        flecs::query<IdComponent, LogicRectComponent, TransformComponent> drawing_entity_query;
        flecs::query<AttackRayEffectComponent> drawing_attack_rayeffect_query;

        // 精灵渲染 query（避免 on_render 每帧重建 filter）
        flecs::query<TransformComponent, LogicRectComponent,
            Animator, AnimParams, PlayerActionComponent, PlayerComponent> render_player_query;
        flecs::query<TransformComponent, LogicRectComponent,
            Animator, AnimParams, PlayerActionComponent, EnemyComponent> render_enemy_query;

        // 位置同步 query（合并玩家和敌人）
        flecs::query<LogicPositionComponent, TransformComponent> position_sync_query;


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
        static constexpr float FIXED_TIMESTEP = 1.0f / 20.0f; // 20Hz
};


static int get_entity_id(Context* ctx)
{
    return ctx->global_entity_id++;
}


#endif