/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-world.h
Description: 
Author: ydlc
Version: 1.0
Date: 2024.12.30
History:
*************************************************/
#ifndef LIV_WORLD_H
#define LIV_WORLD_H
#include "external/klib/khash.h"
#include "liv-math.h"

typedef int connection_id_t;

typedef struct render_entity {
        int id;
        ft_t width;
        ft_t height;
        ft_t rotation;
        vec2_t linear_velocity;
        vec2_t position;
} render_entity_t, * render_entity_p;

typedef struct render_world render_world_t, *render_world_p;

render_world_p render_world_create();
void render_world_destroy(render_world_p world);
void render_world_create_entity(render_world_p world, render_entity_p entity);
void render_world_destroy_entity(render_world_p world, int entity_id);
void render_world_add_connection(render_world_p world, connection_id_t conn_id, int entity_id);
void render_world_remove_connection(render_world_p world, connection_id_t conn_id);

#endif // !LIV_WORLD_H
