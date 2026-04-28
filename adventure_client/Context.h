#ifndef CONTEXT_H
#define CONTEXT_H

#include <SDL3/SDL.h>
#include "adventure.pb.h"
#include "flecs.h"
#include <joy2d/jcore.h>
#include <joy2d/jmath.h>
#include <joy2d/jnetwork.h>
#include <map>
#include <queue>
#include <vector>
#include <cstdint>
#include "Component.h"
#include "DebugLayer.h"
#include "MobileInputLayer.h"

// 状态快照（用于回滚）
struct StateSnapshot {
        fp_t position_x, position_y; // 本地玩家的逻辑位置
};

struct Context {
        bool running = true;

        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
        kcpclient_p kcpclient = NULL;

        std::map<int, int> server_inputs;
        bool ready = false;
        int server_frameid = 0;
        int server_entity_id = 0;

        Resources* resources = NULL;
        DebugLayer* debugLayer = NULL;
        MobileInputLayer* mobileInputLayer = NULL;

        game_timer_t game_timer;
        simple_fps_counter_p sample_fps;

        // 按键状态
        bool upPressed = false;
        bool downPressed = false;
        bool leftPressed = false;
        bool rightPressed = false;
        bool attackPressed = false;

        // 固定逻辑步
        Uint64 lastTime = SDL_GetTicks();
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 50.0f;

        int current_input_mask = 0;
        float heartbeatTimer = 0.0f;

        bool attack_triggered = false;   // 攻击键按下待发送

        // ========== 预测回滚相关 ==========
        uint32_t local_conv = 0;                     // 本地玩家的 conv（连接标识）
        std::map<int, std::map<int, StateSnapshot>> snapshots;        // 按 frame_id 递增存储的历史快照
        std::vector<int> pending_inputs;     // 已发送但未确认的输入

        // Flecs 查询
        flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
        flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
        flecs::query<IdComponent, LogicPositionComponent, TransformComponent> sync_query;
        flecs::query<IdComponent, LogicRectComponent, TransformComponent> renderer_query;
        flecs::query<AttackRayEffectComponent> renderer_attack_rayeffect_query;
};

#endif