#ifndef LERP_SYSTEM_H
#define LERP_SYSTEM_H
#include "../flecs.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"

static void lerp_system(flecs::entity e, logic_position_component& lp, transform_component& t)
{
        //auto ctx = e.world().get_mut<Context>();
        //float target_position_x = fp_to_float(lp.x);
        //float target_position_y = fp_to_float(lp.y);
        //float smooth_time = 0.1f;  // 可从 Context 中读取
        ////float alpha = std::min(1.0f, e.world().delta_time() / smooth_time);
        ////alpha = std::clamp(alpha, 0.0f, 1.0f);
        //float alpha = e.world().delta_time() * 10;
        //t.position_x = ft_lerp(t.position_x, target_position_x, alpha);
        //t.position_y = ft_lerp(t.position_y, target_position_y, alpha);
}

#endif