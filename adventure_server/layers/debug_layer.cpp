#include "debug_layer.h"
#include <stdio.h>
#include <joy/jcore.h>
#include "../asset_manager.h"

struct debug_layer {
	scene_node_p scene_node;
	int frame_id;
	simple_fps_counter_p simple_fps;
	text_texture_p fps_texture;
};

static void on_update(scene_node_p n, float dt)
{
	debug_layer_p self = (debug_layer_p)scene_node_get_userdata(n);
	simple_fps_update(self->simple_fps);
}

static void on_render(scene_node_p n, const void* arg)
{
	float x, y;
	float scale_x, scale_y;
	SDL_Renderer* renderer = (SDL_Renderer*)arg;
	scene_node_get_world_position(n, &x, &y);
	scene_node_get_scale(n, &scale_x, &scale_y);
	int z = scene_node_get_zorder(n);
	SDL_Color c;
	if (z >= 30)      c = { 80, 160, 255, 255 };
	else if (z >= 20) c = { 80, 255, 120, 255 };
	else              c = { 255, 80, 80, 255 };
	SDL_FRect r = { x - 30, y - 30, 60 * scale_x, 60 * scale_y };
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
	SDL_RenderFillRect(renderer, &r);
}

static void on_destroy(scene_node_p n)
{
	debug_layer_p self = (debug_layer_p)scene_node_get_userdata(n);
	simple_fps_destory(self->simple_fps);
	text_destroy(self->fps_texture);
}

debug_layer_p create_debug_layer()
{
	debug_layer_p self = (debug_layer_p)SDL_malloc(sizeof(debug_layer_t));
	SDL_assert(self);
	self->simple_fps = simple_fps_create();
	self->scene_node = scene_node_create();
	SDL_assert(self->scene_node);
	font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();
	self->fps_texture = text_createx(simhei_font, "fps:", SDL_strlen("fps:"), { 255,255,255,255 });
	scene_node_set_userdata(self->scene_node, self);
	scene_node_set_update_callback(self->scene_node, on_update);
	scene_node_set_render_callback(self->scene_node, on_render);
	scene_node_set_destroy_callback(self->scene_node, on_destroy);
	scene_node_set_position(self->scene_node, 0, 0);
	scene_node_set_zorder(self->scene_node, 1);
	scene_node_set_scale(self->scene_node, 1, 1);


	// children
	scene_node_p fps_node = scene_node_create();
	scene_node_set_position(fps_node, 0, 0);
	scene_node_set_userdata(fps_node, self);
	scene_node_set_zorder(fps_node, 100);
	scene_node_set_update_callback(fps_node, [](scene_node_p n, float dt) {
		debug_layer_p self = (debug_layer_p)scene_node_get_userdata(n);
		font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();
		char content[JOY_MAX_PATH] = { 0 };
		int fps = simple_fps_value(self->simple_fps);
		sprintf(content, "fps:%d", fps);
		text_updatex(self->fps_texture, simhei_font, content, SDL_strlen(content), { 255,255,255,255 });
		});
	scene_node_set_render_callback(fps_node, [](scene_node_p n, const void* arg) {
		SDL_Renderer* renderer = (SDL_Renderer*)arg;
		debug_layer_p self = (debug_layer_p)scene_node_get_userdata(n);
		float x, y;
		scene_node_get_world_position(n, &x, &y);
		text_print(renderer, self->fps_texture, x, y);
		});

	scene_node_add_child(self->scene_node, fps_node);

	return self;
}


scene_node_p debug_layer_get_node(debug_layer_p debug_layer)
{
	return debug_layer->scene_node;
}
