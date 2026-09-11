#ifndef VICTORY_SCENE_H
#define VICTORY_SCENE_H
#include <joy/joy.h>
#include "../context.h"

typedef struct victory_scene victory_scene_t, *victory_scene_p;

victory_scene_p victory_scene_create(Context* ctx);
scene_p victory_scene_get_scene(victory_scene_p s);

#endif