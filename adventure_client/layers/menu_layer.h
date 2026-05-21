#ifndef MENU_LAYER_H
#define MENU_LAYER_H
#include <joy/jui.h>
#include <joy/jscene.h>
#include "../context.h"

#define MENU_EVENT (SDL_EVENT_USER + 0x2)

enum menu_event_code {
        MENU_EVENT_START = 1,
        MENU_EVENT_SETTINGS = 2,
        MENU_EVENT_EXIT = 3,
};

typedef struct menu_layer menu_layer_t, *menu_layer_p;

menu_layer_p create_menu_layer(context* ctx);
scene_node_p menu_layer_get_node(menu_layer_p layer);

#endif
