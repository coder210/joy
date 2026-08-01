#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <functional>
#include "entt.hpp"
#include <joy/joy.h>
#include "../adventure3d_shared/protocol.h"
#include "../adventure3d_shared/components/action_state_component.h"
#include "../adventure3d_shared/components/animator_def.h"
#include "../adventure3d_shared/components/animator_component.h"
#include "../adventure3d_shared/components/id_component.h"
#include "../adventure3d_shared/components/ai_component.h"
#include "../adventure3d_shared/components/logic_rect_component.h"
#include "../adventure3d_shared/components/logic_position_component.h"
#include "../adventure3d_shared/components/logic_velocity_component.h"
#include "../adventure3d_shared/components/enemy_component.h"
#include "../adventure3d_shared/components/player_state_input.h"
#include "../adventure3d_shared/components/player_event_input.h"
#include "../adventure3d_shared/components/player_component.h"
#include "../adventure3d_shared/components/transform_component.h"
#include "../adventure3d_shared/components/attack_ray_effect_component.h"
#include "../adventure3d_shared/components/net_component.h"



struct state_snapshot {
	int64_t position_x, position_y, position_z;
};

//struct Session {
//    entt::entity player_entity = entt::null;  // 对应的玩家实体（加载完成后才赋值）
//};



struct Context {
        bool is_server = false;
        bool running;
        game_timer_t game_timer;
        jwindow_p window;
        jr3d_p r3d = NULL;
        render_mesh_p mesh_player = NULL;
        render_mesh_p mesh_enemy = NULL;
        render_mesh_p mesh_floor = NULL;
        netclient_p netclient;
        entt::registry ecs;
        entt::entity singleton_net = entt::null;

        // ---- 音频 ----
        uint32_t            audio_dev = 0;
        audio_shot_p        step_sound = NULL;
        audio_bgm_p         bgm = NULL;
        int local_dir = 0; // 本地玩家方向
        float local_yaw = 0.0f; // 本地玩家朝向（鼠标）
        // 远程玩家方向追踪
        std::map<entt::entity, int>  remote_dirs;

        std::map<int, int> server_inputs;
        bool ready = false;
        int server_frameid = 0;
        int server_entity_id = 0;

        // 固定逻辑步
        uint64_t lastTime = get_ticks();

        int current_input_mask = 0;
        bool attack_triggered = false;          // 按钮 tap 攻击，发送一次后自动复位
        float heartbeatTimer = 0.0f;

        //std::unordered_map<int, Session> sessions;   // key = conv

        std::queue<std::string> command_queue;

        // ========== 预测回滚相关 ==========
        uint32_t local_conv = 0;                     // 本地玩家的 conv（连接标识）
        std::map<int, std::map<int, state_snapshot>> snapshots;        // 按 frame_id 递增存储的历史快照
        std::vector<int> pending_inputs;     // 已发送但未确认的输入

        // ---- 固定步长逻辑 ----
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 20.0f;

        // system 注册
        std::vector<struct SystemEntry> systems;
};

// ========== System 调度基础设施 ==========
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

inline void run_phase(Context* ctx, Phase phase, float dt)
{
        for (auto& sys : ctx->systems) {
                if (sys.phase == phase) {
                        sys.fn(ctx->ecs, ctx, dt);
                }
        }
}


static int get_entity_id(Context* ctx)
{
    return ctx->server_entity_id++;
}


#endif
