#ifndef DRAWING_ENTITY_SYSTEM_H
#define DRAWING_ENTITY_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../scenes/game_scene.h"
#include "../../adventure_shared/protocol.h"
#include "../components/id_component.h"
#include "../components/player_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"

static void DrawingEntitySystem(flecs::entity e, 
        IdComponent& id,
        LogicRectComponent& rect, 
        TransformComponent& t)
{
        Context* ctx = (Context*)e.world().get_ctx();
        jrect_t body = { 0 };
        jrect_t header = { 0 };
        // 摄像机偏移：世界坐标 - 摄像机坐标 → 屏幕像素
        body.x = (t.position_x - ctx->camera_x) * SERVER_PPM;
        body.y = (t.position_y - ctx->camera_y) * SERVER_PPM;
        body.w = fp_to_float(rect.width) * SERVER_PPM;
        body.h = fp_to_float(rect.height) * SERVER_PPM;
        if (e.has<PlayerComponent>()) {
                return; // 玩家由 game_scene 用精灵绘制
        }
        else {
            r2d_set_color(ctx->r2d, 140, 195, 196, 255);
            shape_draw_rectangle(ctx->r2d, "fill", body);
        }

        //r2d_set_color(ctx->r2d, 0, 0, 0, 255);
        //SDL_RenderDebugText(ctx->r2d, body.x, body.y + body.h * 0.5f, std::to_string(id.hp).c_str());
}


#endif