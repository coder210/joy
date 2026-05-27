#include <SDL3/SDL.h>
#include <string.h>
#include "gameplay_controls_layer.h"
#include "../asset_manager.h"

static void setup_button_texture(button_p btn, SDL_Renderer* renderer) {
	btn->image = image_create(renderer, "joy2d_editor_ui/buttons/button_blue_9slides.png");
	btn->pressed_image = image_create(renderer, "joy2d_editor_ui/buttons/button_blue_9slides_pressed.png");
	// 有纹理后去掉纯色背景
	SDL_Color transparent = {0, 0, 0, 0};
	button_set_normal_color(btn, transparent);
	button_set_hover_color(btn, transparent);
	button_set_pressed_color(btn, transparent);
}

struct gameplay_controls_layer {
	scene_node_p scene_node;
};


static void send_gameplay_control_event(int event_code)
{
	SDL_Event event;
	SDL_zero(event);
	event.type = GAMEPLAY_CONTROL_EVENT;
	event.user.code = event_code;
	SDL_PushEvent(&event);
}

static void on_event(scene_node_p n, const void* e)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
	SDL_Event* event = (SDL_Event*)e;
}

static void on_update(scene_node_p n, float dt)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
}

static void on_render(scene_node_p n, const void* arg)
{
	SDL_Renderer* renderer = (SDL_Renderer*)arg;
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
}

static void on_destroy(scene_node_p n)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
}

static void load_dpad_panel(gameplay_controls_layer_p self, context* ctx)
{
	int logical_width, logical_height;
	SDL_GetRenderLogicalPresentation(ctx->renderer, &logical_width, &logical_height, NULL);
	font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();

	scene_node_p root_node = scene_node_create();
	scene_node_set_position(root_node, 0, logical_height - 150);
	scene_node_set_zorder(root_node, 100);
	scene_node_add_child(self->scene_node, root_node);

	button_p up_button = button_create(ctx->renderer, 50, 50);
	setup_button_texture(up_button, ctx->renderer);
	button_set_textx(up_button, simhei_font, "U", (int)strlen("U"), { 220, 200, 160, 255 });
	button_set_callback(up_button,
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_UP); },
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_UP); },
		nullptr);
	scene_node_p up_button_node = scene_node_create();
	scene_node_set_position(up_button_node, 50, 0);
	scene_node_set_userdata(up_button_node, up_button);
	scene_node_set_zorder(up_button_node, 100);
	scene_node_set_update_callback(up_button_node, [](scene_node_p n, float dt) {
		button_p self = (button_p)scene_node_get_userdata(n);
		scene_node_get_world_position(n, &self->position.x, &self->position.y);
	});

	scene_node_set_event_callback(up_button_node, [](scene_node_p n, const void* e) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Event* event = (SDL_Event*)e;
		button_handle_event(self, event);
	});

	scene_node_set_render_callback(up_button_node, [](scene_node_p n, const void* arg) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Renderer* renderer = (SDL_Renderer*)arg;
		button_draw(self);
	});

	scene_node_add_child(root_node, up_button_node);

	// down
	button_p down_button = button_create(ctx->renderer, 50, 50);
	setup_button_texture(down_button, ctx->renderer);
	button_set_textx(down_button, simhei_font, "D", (int)strlen("D"), { 220, 200, 160, 255 });
	button_set_callback(down_button,
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_DOWN); },
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_DOWN); },
		nullptr);
	scene_node_p down_button_node = scene_node_create();
	scene_node_set_position(down_button_node, 50, 100);
	scene_node_set_userdata(down_button_node, down_button);
	scene_node_set_zorder(down_button_node, 100);
	scene_node_set_update_callback(down_button_node, [](scene_node_p n, float dt) {
		button_p self = (button_p)scene_node_get_userdata(n);
		scene_node_get_world_position(n, &self->position.x, &self->position.y);
	});

	scene_node_set_event_callback(down_button_node, [](scene_node_p n, const void* e) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Event* event = (SDL_Event*)e;
		button_handle_event(self, event);
	});

	scene_node_set_render_callback(down_button_node, [](scene_node_p n, const void* arg) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Renderer* renderer = (SDL_Renderer*)arg;
		button_draw(self);
	});

	scene_node_add_child(root_node, down_button_node);

	// left
	button_p left_button = button_create(ctx->renderer, 50, 50);
	setup_button_texture(left_button, ctx->renderer);
	button_set_textx(left_button, simhei_font, "L", (int)strlen("L"), { 220, 200, 160, 255 });
	button_set_callback(left_button,
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_LEFT); },
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_LEFT); },
		nullptr);
	scene_node_p left_button_node = scene_node_create();
	scene_node_set_position(left_button_node, 0, 50);
	scene_node_set_userdata(left_button_node, left_button);
	scene_node_set_zorder(left_button_node, 100);
	scene_node_set_update_callback(left_button_node, [](scene_node_p n, float dt) {
		button_p self = (button_p)scene_node_get_userdata(n);
		scene_node_get_world_position(n, &self->position.x, &self->position.y);
	});

	scene_node_set_event_callback(left_button_node, [](scene_node_p n, const void* e) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Event* event = (SDL_Event*)e;
		button_handle_event(self, event);
	});

	scene_node_set_render_callback(left_button_node, [](scene_node_p n, const void* arg) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Renderer* renderer = (SDL_Renderer*)arg;
		button_draw(self);
	});

	scene_node_add_child(root_node, left_button_node);

	// right
	button_p right_button = button_create(ctx->renderer, 50, 50);
	setup_button_texture(right_button, ctx->renderer);
	button_set_textx(right_button, simhei_font, "R", (int)strlen("R"), { 220, 200, 160, 255 });
	button_set_callback(right_button,
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_RIGHT); },
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_RIGHT); },
		nullptr);
	scene_node_p right_button_node = scene_node_create();
	scene_node_set_position(right_button_node, 100, 50);
	scene_node_set_userdata(right_button_node, right_button);
	scene_node_set_zorder(right_button_node, 100);
	scene_node_set_update_callback(right_button_node, [](scene_node_p n, float dt) {
		button_p self = (button_p)scene_node_get_userdata(n);
		scene_node_get_world_position(n, &self->position.x, &self->position.y);
	});

	scene_node_set_event_callback(right_button_node, [](scene_node_p n, const void* e) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Event* event = (SDL_Event*)e;
		button_handle_event(self, event);
	});

	scene_node_set_render_callback(right_button_node, [](scene_node_p n, const void* arg) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Renderer* renderer = (SDL_Renderer*)arg;
		button_draw(self);
	});

	scene_node_add_child(root_node, right_button_node);
}


static void load_action_panel(gameplay_controls_layer_p self, context* ctx)
{
	font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();
	int logical_width, logical_height;
	SDL_GetRenderLogicalPresentation(ctx->renderer, &logical_width, &logical_height, NULL);

	scene_node_p root_node = scene_node_create();
	scene_node_set_position(root_node, logical_width - 100, logical_height - 100);
	scene_node_set_zorder(root_node, 100);
	scene_node_add_child(self->scene_node, root_node);

	// attack
	button_p attack_button = button_create(ctx->renderer, 50, 50);
	setup_button_texture(attack_button, ctx->renderer);
	button_set_textx(attack_button, simhei_font, "A", (int)strlen("A"), { 220, 200, 160, 255 });
	button_set_callback(attack_button,
		nullptr,
		[](button_p, void*) { send_gameplay_control_event(GAMEPLAY_CONTROL_ATTACK); },
		nullptr);
	scene_node_p attack_button_node = scene_node_create();
	scene_node_set_position(attack_button_node, 0, 0);
	scene_node_set_userdata(attack_button_node, attack_button);
	scene_node_set_zorder(attack_button_node, 100);
	scene_node_set_update_callback(attack_button_node, [](scene_node_p n, float dt) {
		button_p self = (button_p)scene_node_get_userdata(n);
		scene_node_get_world_position(n, &self->position.x, &self->position.y);
	});

	scene_node_set_event_callback(attack_button_node, [](scene_node_p n, const void* e) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Event* event = (SDL_Event*)e;
		button_handle_event(self, event);
	});

	scene_node_set_render_callback(attack_button_node, [](scene_node_p n, const void* arg) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Renderer* renderer = (SDL_Renderer*)arg;
		button_draw(self);
	});

	scene_node_add_child(root_node, attack_button_node);
}

gameplay_controls_layer_p create_gameplay_controls_layer(context* ctx)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)SDL_malloc(sizeof(gameplay_controls_layer_t));
	SDL_assert(self);
	self->scene_node = scene_node_create();
	SDL_assert(self->scene_node);
	scene_node_set_userdata(self->scene_node, self);
	scene_node_set_event_callback(self->scene_node, on_event);
	scene_node_set_update_callback(self->scene_node, on_update);
	scene_node_set_render_callback(self->scene_node, on_render);
	scene_node_set_destroy_callback(self->scene_node, on_destroy);
	scene_node_set_position(self->scene_node, 0, 0);
	scene_node_set_zorder(self->scene_node, 1);
	scene_node_set_scale(self->scene_node, 1, 1);

	load_dpad_panel(self, ctx);
	load_action_panel(self, ctx);

	return self;
}

scene_node_p gameplay_controls_layer_get_node(gameplay_controls_layer_p gameplay_controls_layer)
{
	return gameplay_controls_layer->scene_node;
}
