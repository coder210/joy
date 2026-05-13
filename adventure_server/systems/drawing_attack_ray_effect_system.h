#ifndef DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#define DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../flecs.h"
#include "../components/attack_ray_effect_component.h"
#include "../app_context.h"

static const int PIXELS_PER_METER = 50;

static void DrawingAttackRayEffectSystem(flecs::entity e, AttackRayEffectComponent& effect)
{
	Context* ctx = (Context*)e.world().get_ctx();
	SDL_FRect line;
	line.x = effect.x * PIXELS_PER_METER;
	line.y = effect.y * PIXELS_PER_METER;
	line.w = effect.w * PIXELS_PER_METER;
	line.h = effect.h * PIXELS_PER_METER;
	SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
	shape_draw_rectangle(ctx->renderer, "fill", line);
}

#endif
