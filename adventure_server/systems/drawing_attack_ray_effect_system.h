#ifndef DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#define DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../flecs.h"
#include "../components/attack_ray_effect_component.h"
#include "../context.h"

static const int PIXELS_PER_METER = 50;

static void DrawingAttackRayEffectSystem(flecs::entity e, AttackRayEffectComponent& effect)
{
	Context* ctx = (Context*)e.world().get_ctx();
	jsdl_rect_t line;
	line.x = (effect.x - ctx->camera_x) * PIXELS_PER_METER;
	line.y = (effect.y - ctx->camera_y) * PIXELS_PER_METER;
	line.w = effect.w * PIXELS_PER_METER;
	line.h = effect.h * PIXELS_PER_METER;
	jsdl_set_render_draw_color(ctx->renderer, 255, 255, 255, 255);
	shape_draw_rectangle(ctx->renderer, "fill", line);
}

#endif
