#ifndef LEVEL_SELECT_SCENE_H
#define LEVEL_SELECT_SCENE_H
#include <joy/joy.h>
#include "../context.h"

typedef struct level_select_scene level_select_scene_t, *level_select_scene_p;

level_select_scene_p level_select_scene_create(Context* ctx, GameMode mode);
scene_p level_select_scene_get_scene(level_select_scene_p s);

#endif