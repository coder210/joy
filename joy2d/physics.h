/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: physics.h
Description: 
Author: ydlc
Version: 1.0
Date: 2023.6.1
History:
*************************************************/
#ifndef PHYSICS_H
#define PHYSICS_H
#include <stdint.h>
#include "config.h"
#include "collision.h"

typedef struct rigidbody {
	uint32_t id;
	int is_static;
	vec2f_t position;
	vec2f_t linear_velocity;
	fp_t angular_velocity;
	fp_t rotation;
	fp_t width;
	fp_t height;
}rigidbody_t, *rigidbody_p;

typedef struct enemy {
	uint32_t id;
	int hp;
}enemy_t, *enemy_p;

typedef enum {
	KEYCODE_NONE = 0x0,
	KEYCODE_UP = 0x1,
	KEYCODE_DOWN = 0x2,
	KEYCODE_LEFT = 0x3,
	KEYCODE_RIGHT = 0x4,
	KEYCODE_ENTER = 0xD,
	KEYCODE_ESC = 0x1B,
	KEYCODE_SPACE = 0x20,
}keycode_k;

typedef struct world world2df_t, *world2df_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API world2df_p world2df_create();
	JOY_API void world2df_destroy(world2df_p world);
	JOY_API void world2df_set_entityid(world2df_p world, uint32_t entity_id);
	JOY_API uint32_t world2df_get_entityid(world2df_p world);
	JOY_API int world2df_checksum(const world2df_p world, char* output);
	JOY_API int world2df_serialize(const world2df_p world, char* output);
	JOY_API int world2df_deserialize(world2df_p world, const char* data, int len);
	JOY_API rigidbody_p world2df_create_rigidbody(world2df_p world, fp_t width, fp_t height);
	JOY_API void world2df_destroy_rigidbody(world2df_p world, uint32_t id);
	JOY_API rigidbody_p world2df_get_rigidbody(world2df_p world, uint32_t id);
	JOY_API rigidbody_p world2df_get_player(world2df_p world, int conv);
	JOY_API int world2df_move_player(world2df_p world, int conv, short keycode);
	JOY_API bool world2df_get_playerid(world2df_p world, int conv, uint32_t *entity_id);
	JOY_API int world2df_connections_count(world2df_p world);
	JOY_API void world2df_add_connection(world2df_p world, int conv, uint32_t entity_id);
	JOY_API void world2df_add_emeny(world2df_p world, uint32_t entity_id);
	JOY_API int world2df_remove_connection(world2df_p world, int conv);
	JOY_API void world2df_update_connection(world2df_p world, int conv, uint32_t entity_id);
	JOY_API int world2df_collide(world2df_p world, uint32_t a_id, uint32_t b_id, contact2df_t* contact);
	JOY_API void world2df_foreach_body(world2df_p world, void(*cb)(rigidbody_p, void*), void* userdata);
	JOY_API void world2df_foreach_connection(world2df_p world, void(*cb)(int, uint32_t, void*), void* userdata);
	JOY_API int world2df_enemies_count(world2df_p world);
	JOY_API void world2df_update_emeny(world2df_p world, vec2f_t map_size);

#ifdef __cplusplus
}
#endif

#endif // !PHYSICS_H
