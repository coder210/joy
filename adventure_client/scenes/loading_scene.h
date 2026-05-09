#ifndef LOADING_SCENE_H
#define LOADING_SCENE_H
#include <joy2d/jscene.h>
#include "../context.h"

typedef struct loading_scene loading_scene_t, *loading_scene_p;

loading_scene_p loading_scene_create(Context2* ctx);
void loading_scene_destroy(loading_scene_p s);
scene_p loading_scene_get_scene(loading_scene_p s);

#endif
