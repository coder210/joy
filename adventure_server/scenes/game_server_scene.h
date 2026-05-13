#ifndef GAME_SERVER_SCENE_H
#define GAME_SERVER_SCENE_H
#include <joy2d/jscene.h>
#include "../flecs.h"

typedef struct game_server_scene game_server_scene_t, *game_server_scene_p;

game_server_scene_p game_server_scene_create();
scene_p game_server_scene_get_scene(game_server_scene_p g);

#endif
