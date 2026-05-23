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
        struct context* ctx = (struct context*)e.world().get_ctx();
        SDL_FRect body = { 0 };
        SDL_FRect header = { 0 };
        // 摄像机偏移：世界坐标 - 摄像机坐标 → 屏幕像素
        body.x = (t.position_x - ctx->camera_x) * PIXELS_PER_METER;
        body.y = (t.position_y - ctx->camera_y) * PIXELS_PER_METER;
        body.w = fp_to_float(rect.width) * PIXELS_PER_METER;
        body.h = fp_to_float(rect.height) * PIXELS_PER_METER;
        if (e.has<PlayerComponent>()) {
                SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(ctx->renderer, &body);
                header.w = 0.1f * PIXELS_PER_METER;
                header.h = 0.1f * PIXELS_PER_METER;
                header.x = body.x + (body.w - header.w) * 0.5f;
                header.y = body.y - header.h;
                SDL_RenderFillRect(ctx->renderer, &header);
        }
        else {
                SDL_SetRenderDrawColor(ctx->renderer, 160, 140, 110, 255);
                SDL_RenderFillRect(ctx->renderer, &body);
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        //SDL_RenderDebugText(ctx->renderer, body.x, body.y + body.h * 0.5f, std::to_string(id.hp).c_str());
}


#endif