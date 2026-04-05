#include <SDL3/SDL.h>
#include <joy2d/jshapes.h>
#include "Systems.h"
#include "Context.h"

const int PIXELS_PER_METER = 50;


//ecs_iter_t
void StartupSystem(flecs::world& world)
{
        auto ctx = world.get_mut<Context>();
        ctx->body_query = world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();
        ctx->player_query = world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();

        ctx->resources = new Resources(ctx->renderer);
        ctx->debugLayer = new DebugLayer(ctx->resources);
        ctx->mobileInputLayer = new MobileInputLayer(ctx->resources, ctx->renderer);
}


// ###########################
// Flecs 系统：渲染插值
// ###########################
void LerpSystem(flecs::entity entity,
        LogicPositionComponent& lp, TransformComponent& t)
{
        auto ctx = entity.world().get_mut<Context>();
        float target_position_x = fp_to_float(lp.x);
        float target_position_y = fp_to_float(lp.y);
        // 正确的固定步插值 alpha（0~1）
        float alpha = ctx->accumulator / ctx->FIXED_TIMESTEP;
        t.position_x = ft_lerp(t.position_x, target_position_x, alpha);
        t.position_y = ft_lerp(t.position_y, target_position_y, alpha);
        /*t.position_x = target_position_x;
        t.position_y = target_position_y;*/
}

void RendererSystem(flecs::entity entity,
        IdComponent& id, LogicRectComponent& rect, TransformComponent& t)
{
        auto ctx = entity.world().get_mut<Context>();
        SDL_FRect body = { 0 };
        SDL_FRect header = { 0 };
        body.x = t.position_x * PIXELS_PER_METER;
        body.y = t.position_y * PIXELS_PER_METER;
        body.w = fp_to_float(rect.width) * PIXELS_PER_METER;
        body.h = fp_to_float(rect.height) * PIXELS_PER_METER;
        if (entity.has<PlayerComponent>()) {
                SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(ctx->renderer, &body);
                header.w = 0.1f * PIXELS_PER_METER;
                header.h = 0.1f * PIXELS_PER_METER;
                header.x = body.x + (body.w - header.w) * 0.5f;
                header.y = body.y - header.h;
                SDL_RenderFillRect(ctx->renderer, &header);
        }
        else {
                SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(ctx->renderer, &body);
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderDebugText(ctx->renderer, body.x, body.y + body.h * 0.5f, std::to_string(id.hp).c_str());
}

void RendererAttackRayEffectSystem(flecs::entity e, AttackRayEffectComponent& effect)
{
        // 获取渲染上下文
        auto ctx = e.world().get_mut<Context>();
        // 获取帧时间（Flecs自带，单位：秒）
        float delta_time = (float)e.world().delta_time();

        // 绘制白色矩形
        SDL_FRect line;
        line.x = effect.x * PIXELS_PER_METER;
        line.y = effect.y * PIXELS_PER_METER;
        line.w = effect.w * PIXELS_PER_METER;
        line.h = effect.h * PIXELS_PER_METER;
        SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
        shape_draw_rectangle(ctx->renderer, "fill", line);

        // 更新剩余时间
        effect.lifetime -= delta_time;

        // 时间耗尽 → 删除实体（自动消失）
        if (effect.lifetime <= 0.0f) {
                e.destruct();
        }
}
