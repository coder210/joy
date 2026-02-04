#include <SDL3/SDL.h>
#include "liv-world.h"

KHASH_INIT(krender_entity, int, render_entity_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kconnection, int, connection_id_t, 1, kh_int_hash_func, kh_int_hash_equal)


typedef struct render_world {
        int entity_id;
        khash_t(krender_entity)* entities;
        khash_t(kconnection)* connections;
}render_world_t, * render_world_p;


render_world_p render_world_create()
{
        render_world_p world;
        world = (render_world_p)SDL_malloc(sizeof(render_world_t));
        SDL_assert(world);
        world->entity_id = 1000;
        world->entities = kh_init(krender_entity);
        world->connections = kh_init(kconnection);
        return world;
}

void render_world_destroy(render_world_p world)
{
        render_entity_p entity;
        khint_t k;
        for (k = kh_begin(world->entities); k != kh_end(world->entities); k++) {
                if (kh_exist(world->entities, k)) {
                        entity = kh_val(world->entities, k);
                        SDL_free(entity);
                }
        }
        kh_destroy(krender_entity, world->entities);
        kh_destroy(kconnection, world->connections);
        SDL_free(world);
}

void render_world_create_entity(render_world_p world, render_entity_p entity)
{
        int ret;
        khint_t k;
        render_entity_p new_entity;
        new_entity = (render_entity_p)SDL_malloc(sizeof(render_entity_t));
        SDL_memset(new_entity, 0, sizeof(render_entity_t));
        new_entity->id = world->entity_id;
        new_entity->width = entity->width;
        new_entity->height = entity->height;
        new_entity->rotation = entity->rotation;
        new_entity->linear_velocity = entity->linear_velocity;
        new_entity->position = entity->position;
        k = kh_get(krender_entity, world->entities, new_entity->id);
        if (k == kh_end(world->entities)) {
                k = kh_put(krender_entity, world->entities, k, &ret);
                kh_val(world->entities, k) = new_entity;
        }
        world->entity_id++;
}

void render_world_destroy_entity(render_world_p world, int entity_id)
{
        int ret;
        render_entity_p entity;
        khint_t k;

        k = kh_get(krender_entity, world->entities, entity_id);
        if (k != kh_end(world->entities)) {
                entity = kh_val(world->entities, k);
                SDL_free(entity);
                kh_del(krender_entity, world->entities, k);
        }
}

void render_world_add_connection(render_world_p world, connection_id_t conn_id, int entity_id)
{
        int ret;
        khint_t k;
        k = kh_get(kconnection, world->connections, conn_id);
        if (k == kh_end(world->connections)) {
                k = kh_put(kconnection, world->connections, conn_id, &ret);
                kh_val(world->connections, k) = entity_id;
        }
}

void render_world_remove_connection(render_world_p world, connection_id_t conn_id)
{
        int ret;
        khint_t k;
        k = kh_get(kconnection, world->connections, conn_id);
        if (k != kh_end(world->connections)) {
                kh_del(kconnection, world->connections, k);
        }
}


