#include "debug_layer.h"
#include <joy/jsdl.h>
#include <string.h>

struct debug_layer {
	scene_node_p scene_node;
};

static void on_render(scene_node_p n, const void* arg)
{
	float x, y;
	float scale_x, scale_y;
	jsdl_renderer_t* renderer = (jsdl_renderer_t*)arg;
	scene_node_get_world_position(n, &x, &y);
	scene_node_get_scale(n, &scale_x, &scale_y);
	int z = scene_node_get_zorder(n);
	jsdl_color_t c;
	if (z >= 30)      { c.r = 80; c.g = 160; c.b = 255; c.a = 255; }
	else if (z >= 20) { c.r = 80; c.g = 255; c.b = 120; c.a = 255; }
	else              { c.r = 255; c.g = 80; c.b = 80; c.a = 255; }
	jsdl_rect_t r = { x - 30, y - 30, 60 * scale_x, 60 * scale_y };
	jsdl_set_render_draw_color(renderer, c.r, c.g, c.b, c.a);
	jsdl_render_fill_rect(renderer, &r);
}

debug_layer_p create_debug_layer()
{
	debug_layer_p self = (debug_layer_p)calloc(1, sizeof(debug_layer_t));
	self->scene_node = scene_node_create();
	scene_node_set_userdata(self->scene_node, self);
	scene_node_set_render_callback(self->scene_node, on_render);
	return self;
}

scene_node_p debug_layer_get_node(debug_layer_p dl) {
	return dl ? dl->scene_node : NULL;
}
