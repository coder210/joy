#ifndef GAME_SCENE_H
#define GAME_SCENE_H
#include <joy/jscene.h>
#include "../context.h"

typedef struct game_scene game_scene_t, *game_scene_p;

game_scene_p game_scene_create(context* ctx);
scene_p game_scene_get_scene(game_scene_p g);

#endif
