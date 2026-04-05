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

        float heartbeat_timer = 0.f;

        Resources* resources = NULL;
        DebugLayer* debugLayer = NULL;
        MobileInputLayer* mobileInputLayer = NULL;


        // °´¼ü×´Ì¬
        bool upPressed = false;
        bool downPressed = false;
        bool leftPressed = false;
        bool rightPressed = false;
        bool attackPressed = false;
  
        int globalSequence = 0;


        // ¹Ì¶¨Âß¼­²½
        Uint64 lastTime = SDL_GetTicks();
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 16.0f;


        flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
        flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
};





#endif