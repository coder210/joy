#ifndef DRAWING_ENTITY_SYSTEM_H
#define DRAWING_ENTITY_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../components/id_component.h"
#include "../components/player_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"

static void drawing_entity_system(flecs::entity e, 
        IdComponent& id,
        LogicRectComponent& rect, 
        TransformComponent& t)
{
        Context* ctx = (Context* )e.world().get_ctx();
        jrect_t body = { 0 };
        jrect_t header = { 0 };
        // 摄像机偏移：世界坐标 - 摄像机坐标 → 屏幕像素
        body.x = (t.position.x - ctx->camera_x) * PIXELS_PER_METER;
        body.y = (t.position.y - ctx->camera_y) * PIXELS_PER_METER;
        body.w = fp_to_float(rect.width) * PIXELS_PER_METER;
        body.h = fp_to_float(rect.height) * PIXELS_PER_METER;
        if (e.has<PlayerComponent>()) {
                // 玩家由 game_scene 的 on_render 用精灵绘制，不画方块
                r2d_set_color(ctx->r2d, 140, 195, 196, 255);
                shape_draw_rectangle(ctx->r2d, "fill", body);
                return;
        }
        else {
                r2d_set_color(ctx->r2d, 140, 195, 196, 255);
                shape_draw_rectangle(ctx->r2d, "fill", body);
        }

        r2d_set_color(ctx->r2d, 0, 0, 0, 255);
}


#endif