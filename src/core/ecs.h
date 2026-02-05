#ifndef CORE_ECS_H
#define CORE_ECS_H
#include <stdlib.h>
#include "vector.h"

typedef int ecs_id_t;
typedef struct ecs_world ecs_world_t, * ecs_world_p;
VECTOR_INIT(kecs, ecs_id_t);

typedef struct ecs_system {
        void* arg;
        void (*cb)(ecs_world_p, void*, float);
} ecs_system_t, * ecs_system_p;

ecs_world_p ecs_create();
void ecs_destroy(ecs_world_p world);
void ecs_process(ecs_world_p world, float dt);
ecs_id_t ecs_spawn(ecs_world_p world);
void ecs_kill(ecs_world_p world, ecs_id_t id);
void ecs_define(ecs_world_p world, const char* name, size_t size);
void ecs_set(ecs_world_p world, ecs_id_t id, const char* name, const void* ptr, size_t size);
size_t ecs_get(ecs_world_p world, ecs_id_t id, const char* name, void** data);
void ecs_remove(ecs_world_p world, ecs_id_t id, const char* name);
vector_t(kecs) ecs_query(ecs_world_p world, int arg_cnt, ...);
vector_t(kecs) ecs_queryx(ecs_world_p world, int arg_cnt, const char** args);

void ecs_register(ecs_world_p world, ecs_system_t system);
int ecs_test();

#endif // !CORE_ECS_H