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
#include "Component.h"
#include "DebugLayer.h"

struct Context {
        int g_id = 1;
        int g_frameid = 1;

        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
        netserver_p server = NULL;

        game_timer_t game_timer;
        simple_fps_counter_p sample_fps;

        bool upPressed = false;
        bool downPressed = false;
        bool leftPressed = false;
        bool rightPressed = false;

        float FIXED_TIMESTEP = 1.0f / 50.0f;
        float accumulator = 0.0f;

        // �� Context �ṹ��������
        float serverTickTimer = 0.0f;        // ���뷢�ͼ�ʱ��
        float SERVER_TICK_INTERVAL = 1.0f / 20.0f;  // 30Hz

        Resources* resources = NULL;
        DebugLayer* debugLayer = NULL;

        flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
        flecs::query<ConnectionComponent> connection_query;
        flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;
        flecs::query<IdComponent, LogicRectComponent, TransformComponent> renderer_query;
        flecs::query<AttackRayEffectComponent> renderer_attack_rayeffect_query;

        std::vector<adventure::S2CPlayerJoin> player_joins;
        std::vector<adventure::S2CPlayerLeave> player_leaves;
        std::vector<adventure::S2CPlayerInput> player_inputs;

        std::map<int, std::string> commands;
        std::queue<std::string> command_queue;
        std::map<int, std::string> worlds;
};





#endif