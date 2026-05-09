#include "../protocol/adventure.pb.h"
#include "../flecs.h"
#include "game_scene.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <joy2d/jmath.h>
#include <joy2d/jtext.h>
#include <joy2d/jnetwork.h>
#include "../asset_manager.h"
#include "../layers/debug_layer.h"
#include "../layers/gameplay_controls_layer.h"
#include "../components/connection_component.h"
#include "../components/logic_velocity_component.h"
#include "../systems/startup_system.h"
#include "../systems/lerp_system.h"
#include "../systems/effect_lifecycle_system.h"
#include "../systems/drawing_entity_system.h"
#include "../systems/drawing_attack_ray_effect_system.h"

// 状态快照（用于回滚）
struct StateSnapshot {
        fp_t position_x, position_y; // 本地玩家的逻辑位置
};

struct game_scene {
        scene_p scene;
        Context2* ctx;
        netclient_p netclient;
        flecs::world ecs_world;

        //std::map<int, int> server_inputs;
        //bool ready = false;
        //int server_frameid = 0;
        //int server_entity_id = 0;

        //// 固定逻辑步
        //Uint64 lastTime = SDL_GetTicks();
        //float accumulator = 0.0f;
        //float FIXED_TIMESTEP = 1.0f / 50.0f;

        //int current_input_mask = 0;
        //float heartbeatTimer = 0.0f;

        //bool attack_triggered = false;   // 攻击键按下待发送

        //// ========== 预测回滚相关 ==========
        //uint32_t local_conv = 0;                     // 本地玩家的 conv（连接标识）
        //std::map<int, std::map<int, StateSnapshot>> snapshots;        // 按 frame_id 递增存储的历史快照
        //std::vector<int> pending_inputs;     // 已发送但未确认的输入

        flecs::query<id_component, logic_rect_component, logic_position_component> body_query;
        flecs::query<player_component, id_component, logic_rect_component, logic_position_component> player_query;
        flecs::query<id_component, logic_position_component, transform_component> sync_query;
        flecs::query<id_component, logic_rect_component, transform_component> renderer_query;
        flecs::query<attack_ray_effect_component> attack_rayeffect_query;

};

static void on_message(net_message_p msg, void* arg)
{
        game_scene_p self = (game_scene_p)arg;
        if (msg->type == NET_TYPE_CONNECTED) {
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_LOADING);
                std::string d = c2s.SerializeAsString();
                netclient_send(self->netclient, d.c_str(), d.size());
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                adventure::S2C s2c;
                if (s2c.ParseFromArray(msg->data, msg->len)) {
                        if (s2c.cmd() == adventure::S2C_CMD_LOADING) {
                                //HandleLoading(&s2c);
                        }
                        else if (s2c.cmd() == adventure::S2C_CMD_COMMAND) {
                                //HandleCommand(s2c);
                        }
                }
        }
        if (msg->data) SDL_free(msg->data);
}

static void on_load(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        log_info("[game scene] onstart...");

        debug_layer_p debug_layer = create_debug_layer();
        gameplay_controls_layer_p gameplay_controls_layer = create_gameplay_controls_layer(self->ctx);
        scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));
        scene_add_root_node(self->scene, gameplay_controls_layer_get_node(gameplay_controls_layer));

        self->ecs_world.component<connection_component>();
        self->ecs_world.component<logic_rect_component>();
        self->ecs_world.component<logic_position_component>();
        self->ecs_world.component<logic_velocity_component>();
        self->ecs_world.component<transform_component>();
        self->ecs_world.component<player_component>();
        self->ecs_world.component<attack_ray_effect_component>();

        self->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.1.25", 10000);
        //self->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.2.61", 10000);
        //self->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "8.148.188.213", 10000);
        //netclient_set_callback(self->netclient, on_message, self);

        self->ecs_world.system<logic_position_component, transform_component>().each(lerp_system);
        self->ecs_world.system<attack_ray_effect_component>().each(effect_lifecycle_system);
        self->sync_query = self->ecs_world.query<id_component, logic_position_component, transform_component>();
        self->renderer_query = self->ecs_world.query<id_component, logic_rect_component, transform_component>();
        self->attack_rayeffect_query = self->ecs_world.query<attack_ray_effect_component>();
        self->player_query = self->ecs_world.query<player_component, id_component, logic_rect_component, logic_position_component>();
        self->body_query = self->ecs_world.query<id_component, logic_rect_component, logic_position_component>();

}

static void on_update(scene_p s, float dt) {
        (void)s;
        (void)dt;
        // 模拟加载进度（可后续扩展）
}

static void on_render(scene_p s) {
        (void)s;
        // TODO: 绘制进度条
}

static void on_destroy(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        free(self);
}

game_scene_p game_scene_create(Context2* ctx) {
        game_scene_p self = (game_scene_p)SDL_malloc(sizeof(game_scene_t));
        SDL_assert(self);
        SDL_memset(self, 0, sizeof(game_scene_t));
        self->scene = scene_create("game");
        self->ctx = ctx;
        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_update_callback(self->scene, on_update);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);
        return self;
}

void game_scene_destroy(game_scene_p s) {
        if (s) {
                if (s->scene) {
                        scene_destroy(s->scene);
                }
                free(s);
        }
}

scene_p game_scene_get_scene(game_scene_p s) {
        return s ? s->scene : NULL;
}
