#include <SDL3/SDL.h>
#include <joy2d/jshapes.h>
#include "renderer_attack_ray_effect_system.h"
#include "../Context.h"

static const int PIXELS_PER_METER = 50;

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
