#ifndef DRAWING_ENTITY_SYSTEM_H
#define DRAWING_ENTITY_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../scenes/game_scene.h"
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
        jsdl_rect_t body = { 0 };
        jsdl_rect_t header = { 0 };
        // 摄像机偏移：世界坐标 - 摄像机坐标 → 屏幕像素
        body.x = (t.position_x - ctx->camera_x) * PIXELS_PER_METER;
        body.y = (t.position_y - ctx->camera_y) * PIXELS_PER_METER;
        body.w = fp_to_float(rect.width) * PIXELS_PER_METER;
        body.h = fp_to_float(rect.height) * PIXELS_PER_METER;
        if (e.has<PlayerComponent>()) {
                return; // 玩家由 game_scene 用精灵绘制
        }
        else {
                jsdl_set_render_draw_color(ctx->renderer, 160, 140, 110, 255);
                jsdl_render_fill_rect(ctx->renderer, &body);
        }

        jsdl_set_render_draw_color(ctx->renderer, 0, 0, 0, 255);
        //SDL_RenderDebugText(ctx->renderer, body.x, body.y + body.h * 0.5f, std::to_string(id.hp).c_str());
}


#endif