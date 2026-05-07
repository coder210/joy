#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <joy2d/jscene.h>
#include "../context.h"

typedef struct game_scene game_scene_t, *game_scene_p;

game_scene_p game_scene_create(Context2* ctx);
void game_scene_destroy(game_scene_p g);
scene_p game_scene_get_scene(game_scene_p g);

#endif
