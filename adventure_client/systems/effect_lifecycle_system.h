#ifndef EFFECT_LIFECYCLE_SYSTEM_H
#define EFFECT_LIFECYCLE_SYSTEM_H
#include "../flecs.h"
#include "../components/attack_ray_effect_component.h"

static void EffectLifecycleSystem(flecs::entity e, attack_ray_effect_component& effect)
{
        //auto ctx = e.world().get_mut<Context>();
        //effect.lifetime -= ctx->FIXED_TIMESTEP;  // 使用固定步长，与物理同步
        //if (effect.lifetime <= 0.0f) {
        //        e.destruct();
        //}
}


#endif