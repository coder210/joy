#ifndef LIV_ECS_H
#define LIV_ECS_H
#include <stdlib.h>
#include "liv-vector.h"

typedef int ecs_id_t;
typedef struct ecs_world ecs_world_t, * ecs_world_p;
VECTOR_INIT(kecs, ecs_id_t);

typedef struct ecs_system {
        void* arg;
        void (*cb)(ecs_world_p, void*, float);
} ecs_system_t, * ecs_system_p;

ecs_world_p liv_ecs_create();
void liv_ecs_destroy(ecs_world_p world);
void liv_ecs_process(ecs_world_p world, float dt);
ecs_id_t liv_ecs_spawn(ecs_world_p world);
void liv_ecs_kill(ecs_world_p world, ecs_id_t id);
void liv_ecs_define(ecs_world_p world, const char* name, size_t size);
void liv_ecs_set(ecs_world_p world, ecs_id_t id, const char* name, const void* ptr, size_t size);
size_t liv_ecs_get(ecs_world_p world, ecs_id_t id, const char* name, void** data);
void liv_ecs_remove(ecs_world_p world, ecs_id_t id, const char* name);
liv_vector_t(kecs) liv_ecs_query(ecs_world_p world, int arg_cnt, ...);
liv_vector_t(kecs) liv_ecs_queryx(ecs_world_p world, int arg_cnt, const char** args);

void liv_ecs_register(ecs_world_p world, ecs_system_t system);
int liv_ecs_test();

#endif // !LIV_ECS_H