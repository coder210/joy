#include <string.h>
#include "menu_layer.h"
#include "../asset_manager.h"

static void setup_button_texture(button_p btn, jr2d_p renderer) {
	button_set_image(btn, image_create(renderer, "adventure_client/ui/buttons/button_blue_9slides.png"));
	button_set_pressed_image(btn, image_create(renderer, "adventure_client/ui/buttons/button_blue_9slides_pressed.png"));
	jcolor_t transparent = {0, 0, 0, 0};
	button_set_normal_color(btn, transparent);
	button_set_hover_color(btn, transparent);
	button_set_pressed_color(btn, transparent);
}

struct menu_layer {
	scene_node_p scene_node;
	button_p btn_start, btn_settings, btn_exit;
};

static void send_menu_event(int event_code)
{
	push_user_event(MENU_EVENT, event_code);
}

static void on_event(scene_node_p n, const void* e)
{
	menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
	jevent_callbacks_t cbs = {};
	cbs.on_mousepressed = [](float x, float y, int btn, void* arg) {
		auto* self = (menu_layer_p)arg;
		auto hit = [&](button_p b) { return button_is_pressed(b, x, y); };
		if (self->btn_start && hit(self->btn_start)) 
			send_menu_event(MENU_EVENT_START);
		if (self->btn_settings && hit(self->btn_settings))
			send_menu_event(MENU_EVENT_SETTINGS);
		if (self->btn_exit && hit(self->btn_exit))
			send_menu_event(MENU_EVENT_EXIT);
	};
	cbs.on_mousereleased = [](float x, float y, int btn, void* arg) {
		auto* self = (menu_layer_p)arg;
		if (self->btn_start)   button_set_pressed(self->btn_start, false);
		if (self->btn_settings) button_set_pressed(self->btn_settings, false);
		if (self->btn_exit)    button_set_pressed(self->btn_exit, false);
	};
	cbs.on_mousemoved = [](float x, float y, float dx, float dy, void* arg) {
		auto* self = (menu_layer_p)arg;
		if (self->btn_start)   button_is_hovered(self->btn_start, x, y);
		if (self->btn_settings) button_is_hovered(self->btn_settings, x, y);
		if (self->btn_exit)    button_is_hovered(self->btn_exit, x, y);
	};
	dispatch_event(cbs, e, self);
}

static void on_update(scene_node_p n, float dt)
{
	menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
}

static void on_render(scene_node_p n, const void* arg)
{
	jr2d_p renderer = (jr2d_p)arg;
	menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
}

static void on_destroy(scene_node_p n)
{
	menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
	free(self);
}

static scene_node_p make_button_node(button_p btn, int x, int y) {
	scene_node_p node = scene_node_create();
	scene_node_set_position(node, (float)x, (float)y);
	scene_node_set_userdata(node, btn);
	scene_node_set_zorder(node, 100);
	scene_node_set_update_callback(node, [](scene_node_p n, float dt) {
		button_p b = (button_p)scene_node_get_userdata(n);
		vec2_t world_pos;
		scene_node_get_world_position(n, &world_pos.x, &world_pos.y);
		button_set_position(b, world_pos.x, world_pos.y);
	});
	scene_node_set_render_callback(node, [](scene_node_p n, const void* arg) {
		button_p b = (button_p)scene_node_get_userdata(n);
		button_draw(b);
	});
	return node;
}

static void load_menu_buttons(menu_layer_p self, Context* ctx)
{
	int logical_width, logical_height;
	r2d_get_logical_presentation(ctx->r2d, &logical_width, &logical_height, NULL);
	font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();

	int btn_width = 120, btn_height = 50;
	int btn_x = (logical_width - btn_width) / 2;
	int start_y = (logical_height - btn_height * 3 - 40) / 2;

	self->btn_start = button_create(ctx->r2d, (float)btn_width, (float)btn_height);
	setup_button_texture(self->btn_start, ctx->r2d);
	button_set_text_offset(self->btn_start, 0, 3.0f);
	button_set_textx(self->btn_start, simhei_font, "Start", (int)strlen("Start"), { 220, 200, 160, 255 });
	scene_node_add_child(self->scene_node, make_button_node(self->btn_start, btn_x, start_y));

	self->btn_settings = button_create(ctx->r2d, (float)btn_width, (float)btn_height);
	setup_button_texture(self->btn_settings, ctx->r2d);
	button_set_text_offset(self->btn_settings, 0, 3.0f);
	button_set_textx(self->btn_settings, simhei_font, "Settings", (int)strlen("Settings"), { 220, 200, 160, 255 });
	scene_node_add_child(self->scene_node, make_button_node(self->btn_settings, btn_x, start_y + btn_height + 20));

	self->btn_exit = button_create(ctx->r2d, (float)btn_width, (float)btn_height);
	setup_button_texture(self->btn_exit, ctx->r2d);
	button_set_text_offset(self->btn_exit, 0, 3.0f);
	button_set_textx(self->btn_exit, simhei_font, "Exit", (int)strlen("Exit"), { 220, 200, 160, 255 });
	scene_node_add_child(self->scene_node, make_button_node(self->btn_exit, btn_x, start_y + (btn_height + 20) * 2));
}

menu_layer_p create_menu_layer(Context* ctx)
{
	menu_layer_p self = (menu_layer_p)malloc(sizeof(menu_layer_t));
	memset(self, 0, sizeof(*self));
	self->scene_node = scene_node_create();
	scene_node_set_userdata(self->scene_node, self);
	scene_node_set_event_callback(self->scene_node, on_event);
	scene_node_set_update_callback(self->scene_node, on_update);
	scene_node_set_render_callback(self->scene_node, on_render);
	scene_node_set_destroy_callback(self->scene_node, on_destroy);
	scene_node_set_position(self->scene_node, 0, 0);
	scene_node_set_zorder(self->scene_node, 1);
	scene_node_set_scale(self->scene_node, 1, 1);

	load_menu_buttons(self, ctx);

	return self;
}

scene_node_p menu_layer_get_node(menu_layer_p layer)
{
	return layer ? layer->scene_node : NULL;
}
