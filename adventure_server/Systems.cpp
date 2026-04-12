#include <SDL3/SDL.h>
#include <joy2d/jshapes.h>
#include "Systems.h"
#include "Context.h"

const int PIXELS_PER_METER = 50;


//ecs_iter_t
void StartupSystem(flecs::world& world)
{
        auto ctx = world.get_mut<Context>();
        adventure::S2CMap map;
        map.set_frame_id(ctx->g_frameid);
        map.set_global_entity_id(ctx->g_id);
        ctx->body_query = world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();
        ctx->connection_query = world.query<ConnectionComponent>();
        ctx->player_query = world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        ctx->body_query.each([&](flecs::entity e, IdComponent& id,
                LogicRectComponent& r, LogicPositionComponent& pos) {
                        adventure::S2CEntity* ent = map.add_entities();
                        ent->set_id(id.id);
                        if (e.has<PlayerComponent>()) {
                                ent->set_type(adventure::S2C_TYPE_PLAYER);
                                ent->set_player_conv(e.get_mut<PlayerComponent>()->conv);
                        }
                        else {
                                ent->set_type(adventure::S2C_TYPE_NORMAL);
                        }
                        ent->set_hp(id.hp);
                        ent->set_position_x(pos.x);
                        ent->set_position_y(pos.y);
                });

        ctx->worlds[map.frame_id()] = map.SerializeAsString();

        ctx->resources = new Resources(ctx->renderer);
        ctx->debugLayer = new DebugLayer(ctx->resources);
}


// ###########################
// Flecs 系统：渲染插值
// ###########################
void LerpSystem(flecs::entity e,
        LogicPositionComponent& lp, TransformComponent& t)
{
        auto ctx = e.world().get_mut<Context>();
        float target_position_x = fp_to_float(lp.x);
        float target_position_y = fp_to_float(lp.y);
        // 正确的固定步插值 alpha（0~1）
        float alpha = ctx->accumulator / ctx->FIXED_TIMESTEP;
        alpha = std::clamp(alpha, 0.0f, 1.0f);
        t.position_x = ft_lerp(t.position_x, target_position_x, alpha);
        t.position_y = ft_lerp(t.position_y, target_position_y, alpha);
        /*t.position_x = target_position_x;
        t.position_y = target_position_y;*/
}

// 在 FixedLogicUpdate 中运行
void EffectLifecycleSystem(flecs::entity e, AttackRayEffectComponent& effect)
{
        auto ctx = e.world().get_mut<Context>();
        effect.lifetime -= ctx->FIXED_TIMESTEP;  // 使用固定步长，与物理同步
        if (effect.lifetime <= 0.0f) {
                e.destruct();
        }
}

void RendererSystem(flecs::entity e, IdComponent& id, LogicRectComponent& rect, TransformComponent& t)
{
        auto ctx = e.world().get_mut<Context>();
        SDL_FRect body = { 0 };
        SDL_FRect header = { 0 };
        body.x = t.position_x * PIXELS_PER_METER;
        body.y = t.position_y * PIXELS_PER_METER;
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
                SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(ctx->renderer, &body);
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        //SDL_RenderDebugText(ctx->renderer, body.x, body.y + body.h * 0.5f, std::to_string(id.hp).c_str());

}

void RendererAttackRayEffectSystem(flecs::entity e, AttackRayEffectComponent& effect)
{
        auto ctx = e.world().get_mut<Context>();
        SDL_FRect line;
        line.x = effect.x * PIXELS_PER_METER;
        line.y = effect.y * PIXELS_PER_METER;
        line.w = effect.w * PIXELS_PER_METER;
        line.h = effect.h * PIXELS_PER_METER;
        SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
        shape_draw_rectangle(ctx->renderer, "fill", line);
}
