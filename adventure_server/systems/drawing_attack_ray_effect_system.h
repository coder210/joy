#ifndef DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#define DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../components/attack_ray_effect_component.h"
#include "../context.h"
#include "../../adventure_shared/protocol.h"

static void DrawingAttackRayEffectSystem(Context* ctx, AttackRayEffectComponent& effect)
{
	jrect_t line;
	auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
	line.x = (effect.x - cam.x) * SERVER_PPM;
	line.y = (effect.y - cam.y) * SERVER_PPM;
	line.w = effect.w * SERVER_PPM;
	line.h = effect.h * SERVER_PPM;
	r2d_set_color(ctx->r2d, 255, 255, 255, 255);
	shape_draw_rectangle(ctx->r2d, "fill", line);
}

#endif
