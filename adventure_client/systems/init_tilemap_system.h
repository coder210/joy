#ifndef INIT_TILEMAP_SYSTEM_H
#define INIT_TILEMAP_SYSTEM_H
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"
#include "../components/id_component.h"
#include "../components/logic_rect_component.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"
#include "../context.h"


static void init_tilemap_system(entt::registry& registry, Context* ctx)
{
        log_info("[Startup] system: init_tilemap");
        ctx->tilemap = jtilemap_load_file("adventure_client/tilemap/map.lua");
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
                    entt::entity e = ctx->ecs.create();
                    ctx->ecs.emplace<LogicRectComponent>(e, pw, ph);
                    ctx->ecs.emplace<LogicPositionComponent>(e, px, py);
                    ctx->ecs.emplace<TransformComponent>(e, fp_to_float(px), fp_to_float(py), 0.0f, 1.0f, 1.0f);
                    }, ctx);

                ctx->tilemap_render = jtilemap_render_create(ctx->r2d, "adventure_client/", NULL);
        }
}

#endif
