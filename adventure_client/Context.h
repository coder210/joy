#ifndef CONTEXT_H
#define CONTEXT_H
#include <SDL3/SDL.h>
#include "adventure.pb.h"
#include "flecs.h"
#include <joy2d/jmath.h>
#include <joy2d/jnetwork.h>
#include <map>
#include <queue>
#include "Component.h"
#include "DebugLayer.h"
#include "MobileInputLayer.h"

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


        // 按键状态
        bool upPressed = false;
        bool downPressed = false;
        bool leftPressed = false;
        bool rightPressed = false;
        bool attackPressed = false;
  
        int globalSequence = 0;


        // 固定逻辑步
        Uint64 lastTime = SDL_GetTicks();
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 60.0f;

        int current_input_mask = 0;
        float heartbeatTimer = 0.0f;        // 已经存在（heartbeat_timer），注意命名一致性

        bool attack_triggered = false;   // 攻击键按下待发送

        flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
        flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
        flecs::query<IdComponent, LogicPositionComponent, TransformComponent> sync_query;
        flecs::query<IdComponent, LogicRectComponent, TransformComponent> renderer_query;
        flecs::query<AttackRayEffectComponent> renderer_attack_rayeffect_query;
};





#endif