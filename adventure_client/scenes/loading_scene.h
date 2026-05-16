#ifndef LOADING_SCENE_H
#define LOADING_SCENE_H
#include <joy2d/jscene.h>
#include "../game_context.h"

typedef struct loading_scene loading_scene_t, *loading_scene_p;

loading_scene_p loading_scene_create(context* ctx);
scene_p loading_scene_get_scene(loading_scene_p s);

#endif
