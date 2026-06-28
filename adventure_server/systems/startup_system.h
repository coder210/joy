#ifndef STARTUP_SYSTEM_H
#define STARTUP_SYSTEM_H
#include "../flecs.h"
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"
#include "../components/id_component.h"
#include "../components/logic_rect_component.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"


static void startup_system(flecs::iter& it)
{
        //std::cout << it.system().name() << "\n";
        log_info("[Startup] system: %s", it.system().name());
        //game_scene_p self = (game_scene_p)it.world().get_ctx();

        // 本地方块：有碰撞检测，但不会同步到客户端（因为有 LocalOnlyComponent）
       /*self->world.entity()
               .add<LocalOnlyComponent>()
               .set<IdComponent>({ 10000, 100 })
               .set<LogicRectComponent>({ fp_from_float(0.5f), fp_from_float(0.5f) })
               .set<LogicPositionComponent>({ fp_from_float(5), fp_from_float(5) })
               .set<TransformComponent>({ 5,5,0,1,1 });*/

               // 加载 tilemap 碰撞墙体（替代硬编码围墙）

       /* self->tilemap = jtilemap_load_file("adventure_server/tilemap/map.lua");
        if (self->tilemap) {
                jtilemap_object_layer_p ol = jtilemap_find_object_layer(self->tilemap, "collision");
                if (ol) {
                        for (size_t i = 0; i < kv_size(ol->objects); i++) {
                                jtilemap_object_t* obj = &kv_A(ol->objects, i);
                                if (obj->shape != JTILEMAP_SHAPE_RECT)
                                        continue;
                                fp_t px = fp_from_float(obj->x / (float)SERVER_PPM);
                                fp_t py = fp_from_float(obj->y / (float)SERVER_PPM);
                                fp_t pw = fp_from_float(obj->width / (float)SERVER_PPM);
                                fp_t ph = fp_from_float(obj->height / (float)SERVER_PPM);
                                self->world.entity()
                                        .set<IdComponent>({ GenId(self), 100 })
                                        .set<LogicRectComponent>({ pw, ph })
                                        .set<LogicPositionComponent>({ px, py })
                                        .set<TransformComponent>({ fp_to_float(px), fp_to_float(py), 0,1,1 });
                        }
                        log_info("[tilemap] loaded %zu collision walls", kv_size(ol->objects));
                }
        }*/
        // ---- tilemap + 精灵渲染初始化 ----
        if (self->tilemap)
                self->tilemap_render = jtilemap_render_create(self->ctx->r2d, "adventure_server/", NULL);
        //// ---- 创建敌人 ----
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
