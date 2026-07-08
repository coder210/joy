#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <vector>
#include <unordered_map>
#include <queue>
#include "flecs.h"
#include <joy/joy.h>
#include "./components/player_action_component.h"
#include "./components/animator_def.h"
#include "./components/animator_component.h"
#include "./components/id_component.h"
#include "./components/ai_component.h"
#include "./components/logic_rect_component.h"
#include "./components/logic_position_component.h"
#include "./components/logic_velocity_component.h"
#include "./components/enemy_component.h"
#include "./components/connection_component.h"
#include "./components/player_state_input.h"
#include "./components/player_event_input.h"
#include "./components/player_component.h"
#include "./components/transform_component.h"
#include "./components/attack_ray_effect_component.h"
#include "./components/net_component.h"


#define PIXELS_PER_METER 50

// 状态快照（用于预测回滚）
struct state_snapshot {
        fp_t position_x, position_y;
};

struct Session {
        int conv;
        flecs::entity player_entity;  // 对应的玩家实体（加载完成后才赋值）
        bool is_loaded = false;       // 是否已完成初始加载
        uint32_t last_processed_seq; // 最后已处理的序列号
        int frameid; // 客户端最后已接收的帧号
};

struct context {
        bool running;
        game_timer_t game_timer;
        jwindow_p window;
        jr2d_p r2d;
        float camera_x, camera_y; // 摄像机世界坐标(米)
        netclient_p netclient;
        flecs::world ecs;

        // ---- tilemap + 音频 ----
        jtilemap_p          tilemap = NULL;
        jtilemap_render_p   tilemap_render = NULL;
        uint32_t            audio_dev = 0;
        audio_shot_p        step_sound = NULL;
        audio_bgm_p         bgm = NULL;
        int local_dir = 0; // 本地玩家方向
        // 远程玩家方向追踪
        std::map<flecs::entity_t, int>  remote_dirs;

        // AI 查询
        flecs::query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent> enemy_query;

        std::map<int, int> server_inputs;
        bool ready = false;
        int server_frameid = 0;
        int server_entity_id = 0;

        // 固定逻辑步
        uint64_t lastTime = get_ticks();



        int current_input_mask = 0;
        float heartbeatTimer = 0.0f;
        bool attack_triggered = false;   // 攻击键按下待发送

        std::unordered_map<int, Session> sessions;   // key = conv

        std::queue<std::string> command_queue;

        // ========== 预测回滚相关 ==========
        uint32_t local_conv = 0;                     // 本地玩家的 conv（连接标识）
        std::map<int, std::map<int, state_snapshot>> snapshots;        // 按 frame_id 递增存储的历史快照
        std::vector<int> pending_inputs;     // 已发送但未确认的输入

        flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
        flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
        flecs::query<IdComponent, LogicPositionComponent, TransformComponent> sync_query;
        flecs::query<IdComponent, LogicRectComponent, TransformComponent> drawing_entity_query;
        flecs::query<AttackRayEffectComponent> attack_ray_effect_query;

        // ---- 预构建渲染 query（避免 on_render 每帧 filter_builder） ----
        flecs::query<TransformComponent, LogicRectComponent,
                Animator, AnimParams, PlayerActionComponent, PlayerComponent> render_player_query;
        flecs::query<TransformComponent, LogicRectComponent,
                Animator, AnimParams, PlayerActionComponent, EnemyComponent> render_enemy_query;
        flecs::query<IdComponent, TransformComponent> healthbar_query;


        // ---- 固定步长逻辑 ----
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 20.0f;
};


static int get_entity_id(context* ctx)
{
    return ctx->server_entity_id++;
}


#endif