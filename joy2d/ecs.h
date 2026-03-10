#ifndef ECS_H
#define ECS_H
#include "config.h"
#include <stdlib.h>


typedef int ecs_id_t;
typedef struct ecs_world ecs_world_t, * ecs_world_p;
//VECTOR_INIT(kecs, ecs_id_t);

typedef struct ecs_id_array {
	ecs_id_t ids[512];
	int num;
}ecs_id_array_t, *ecs_id_array_p;

typedef struct ecs_system {
        void* arg;
        void (*cb)(ecs_world_p, void*, float);
} ecs_system_t, * ecs_system_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API ecs_world_p ecs_create();
	JOY_API void ecs_destroy(ecs_world_p world);
	JOY_API void ecs_process(ecs_world_p world, float dt);
	JOY_API ecs_id_t ecs_spawn(ecs_world_p world);
	JOY_API void ecs_kill(ecs_world_p world, ecs_id_t id);
	JOY_API void ecs_define(ecs_world_p world, const char* name, size_t size);
	JOY_API void ecs_set(ecs_world_p world, ecs_id_t id, const char* name, const void* ptr, size_t size);
	JOY_API size_t ecs_get(ecs_world_p world, ecs_id_t id, const char* name, void** data);
	JOY_API void ecs_remove(ecs_world_p world, ecs_id_t id, const char* name);
	JOY_API ecs_id_array_t ecs_query(ecs_world_p world, int arg_cnt, ...);
	JOY_API ecs_id_array_t ecs_queryx(ecs_world_p world, int arg_cnt, char** args);
	JOY_API void ecs_register(ecs_world_p world, ecs_system_t system);
	JOY_API int ecs_test();

#ifdef __cplusplus
}
#endif

#endif // !ECS_H