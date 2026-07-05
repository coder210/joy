#ifndef INIT_TILEMAP_SYSTEM_H
#define INIT_TILEMAP_SYSTEM_H
#include "../flecs.h"
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"
#include "../components/id_component.h"
#include "../components/logic_rect_component.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"
#include "../context.h"


static void init_tilemap_system(flecs::iter& it)
{
        log_info("[Startup] system: %s", it.system().name());
        Context* ctx = (Context*)it.world().get_ctx();

        // º”‘ÿ tilemap ≈ˆ◊≤«ΩÃÂ£®ÃÊ¥˙”≤±‡¬ÎŒß«Ω£©
        // ---- tilemap + æ´¡È‰÷»æ≥ı ºªØ ----
        ctx->tilemap = jtilemap_load_file("adventure_server/tilemap/map.lua");
        if (ctx->tilemap) {
                jtilemap_object_layer_p ol = jtilemap_find_object_layer(ctx->tilemap, "collision");
                jtilemap_object_layer_foreach(ol, [](jtilemap_object_p obj, void* arg) {
                    Context* ctx = (Context*)arg;
                    if (obj->shape != JTILEMAP_SHAPE_RECT)
                        return;
                    fp_t px = fp_from_float(obj->x / (float)SERVER_PPM);
                    fp_t py = fp_from_float(obj->y / (float)SERVER_PPM);
                    fp_t pw = fp_from_float(obj->width / (float)SERVER_PPM);
                    fp_t ph = fp_from_float(obj->height / (float)SERVER_PPM);
                    ctx->ecs.entity()
                        .set<LogicRectComponent>({ pw, ph })
                        .set<LogicPositionComponent>({ px, py })
                        .set<TransformComponent>({ fp_to_float(px), fp_to_float(py), 0,1,1 });
                    }, ctx);

                ctx->tilemap_render = jtilemap_render_create(ctx->r2d, "adventure_server/", NULL);
                
        }
                
        //// ---- ¥¥Ω®µ–»À ----
        //const int ENEMY_COUNT = 1;
        //for (int i = 0; i < ENEMY_COUNT; i++) {
        //        fp_t ex = fp_from_float(3);
        //        fp_t ey = fp_from_float(3);
        //        fp_t cx = ex;
        //        fp_t cy = ey;
        //        flecs::entity enemy_anim_def = create_warrior_anim_def(self->world,
        //                "adventure_server/textures/knights/troops/warrior/warrior_red.png");
        //        self->world.entity()
        //                .add<EnemyComponent>()
        //                .set<AIComponent>({
        //                        cx, cy,
        //                        fp_from_float(4.0f), fp_from_float(15.0f), fp_from_float(1.5f),
        //                        0, 0,
        //                        0, 0
        //                        })
        //                .set<IdComponent>({ GenId(self), 5 })
        //                .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
        //                .set<LogicPositionComponent>({ ex, ey })
        //                .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
        //                .set<TransformComponent>({ fp_to_float(ex), fp_to_float(ey), 0, 1, 1 })
        //                .set<PlayerActionComponent>({})
        //                .set<Animator>({ enemy_anim_def })
        //                .set<AnimParams>({});
        //}
        //log_info("[AI] created %d enemies", ENEMY_COUNT);

}

#endif
