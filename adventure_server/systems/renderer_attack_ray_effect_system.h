#ifndef RENDERER_ATTACK_RAY_EFFECT_SYSTEM_H
#define RENDERER_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../flecs.h"
#include "../components/attack_ray_effect_component.h"

static const int PIXELS_PER_METER = 50;

static void RendererAttackRayEffectSystem(flecs::entity e, AttackRayEffectComponent& effect)
{
	/*auto ctx = e.world().get_mut<Context>();
	SDL_FRect line;
	line.x = effect.x * PIXELS_PER_METER;
	line.y = effect.y * PIXELS_PER_METER;
	line.w = effect.w * PIXELS_PER_METER;
	line.h = effect.h * PIXELS_PER_METER;
	SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
	shape_draw_rectangle(ctx->renderer, "fill", line);*/
}

#endif
