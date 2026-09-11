#ifndef CHARACTER_SELECT_SCENE_H
#define CHARACTER_SELECT_SCENE_H
#include <joy/joy.h>
#include "../context.h"

typedef struct character_select_scene character_select_scene_t, *character_select_scene_p;

character_select_scene_p character_select_scene_create(Context* ctx, GameMode mode);
scene_p character_select_scene_get_scene(character_select_scene_p s);

#endif