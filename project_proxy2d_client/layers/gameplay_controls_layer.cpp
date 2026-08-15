#include <string.h>
#include "gameplay_controls_layer.h"
#include "../asset_manager.h"

// 与 loading 场景按钮一致的配色：白底 + 黑色描边（button_draw 自带线框）+ 黑字
static void setup_button_style(button_p btn) {
	jcolor_t normal  = { 255, 255, 255, 255 }; // 白
	jcolor_t hover   = { 255, 255, 255, 255 }; // 白
	jcolor_t pressed = { 255, 255, 255, 255 }; // 白
	button_set_normal_color(btn, normal);
	button_set_hover_color(btn, hover);
	button_set_pressed_color(btn, pressed);
}

struct gameplay_controls_layer {
	scene_node_p scene_node;
	button_p btn_up, btn_down, btn_left, btn_right, btn_attack, btn_ranged;
};


static void send_gameplay_control_event(int event_code)
{
	push_user_event(GAMEPLAY_CONTROL_EVENT, event_code);
}

static void on_event(scene_node_p n, const void* e)
{
	gameplay_controls_layer_p self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
	jevent_callbacks_t cbs = {};
	cbs.on_mousepressed = [](float x, float y, int btn, void* arg) {
		auto* self = (gameplay_controls_layer_p)arg;
		auto hit = [&](button_p b) { return button_is_pressed(b, x, y); };
		if (self->btn_up    && hit(self->btn_up))    send_gameplay_control_event(GAMEPLAY_CONTROL_UP);
		if (self->btn_down  && hit(self->btn_down))  send_gameplay_control_event(GAMEPLAY_CONTROL_DOWN);
		if (self->btn_left  && hit(self->btn_left))  send_gameplay_control_event(GAMEPLAY_CONTROL_LEFT);
		if (self->btn_right && hit(self->btn_right)) send_gameplay_control_event(GAMEPLAY_CONTROL_RIGHT);
		if (self->btn_attack && hit(self->btn_attack)) {} // 攻击按钮按下只记录状态,松手才触发
		if (self->btn_ranged && hit(self->btn_ranged)) send_gameplay_control_event(GAMEPLAY_CONTROL_RANGED); // 远程攻击按住置位
	};
	cbs.on_mousereleased = [](float x, float y, int btn, void* arg) {
		auto* self = (gameplay_controls_layer_p)arg;
		auto rel = [&](button_p b, int evt) { button_set_pressed(b, false); send_gameplay_control_event(evt); };
		if (self->btn_up    && button_is_hovered(self->btn_up, x, y))
			rel(self->btn_up, GAMEPLAY_CONTROL_RELEASE_UP);
		if (self->btn_down  && button_is_hovered(self->btn_down, x, y))
			rel(self->btn_down, GAMEPLAY_CONTROL_RELEASE_DOWN);
		if (self->btn_left  && button_is_hovered(self->btn_left, x, y))
			rel(self->btn_left, GAMEPLAY_CONTROL_RELEASE_LEFT);
		if (self->btn_right && button_is_hovered(self->btn_right, x, y))
			rel(self->btn_right, GAMEPLAY_CONTROL_RELEASE_RIGHT);
		if (self->btn_attack && button_is_hovered(self->btn_attack, x, y))
			rel(self->btn_attack, GAMEPLAY_CONTROL_ATTACK);
		if (self->btn_ranged && button_is_hovered(self->btn_ranged, x, y))
			rel(self->btn_ranged, GAMEPLAY_CONTROL_RELEASE_RANGED);
	};
	cbs.on_mousemoved = [](float x, float y, float dx, float dy, void* arg) {
		auto* self = (gameplay_controls_layer_p)arg;
		if (self->btn_up)     button_is_hovered(self->btn_up, x, y);
		if (self->btn_down)   button_is_hovered(self->btn_down, x, y);
		if (self->btn_left)   button_is_hovered(self->btn_left, x, y);
		if (self->btn_right)  button_is_hovered(self->btn_right, x, y);
		if (self->btn_attack) button_is_hovered(self->btn_attack, x, y);
		if (self->btn_ranged) button_is_hovered(self->btn_ranged, x, y);
	};
	dispatch_event(cbs, e, self);
}

static void on_update(scene_node_p n, float dt)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
}

static void on_render(scene_node_p n, const void* arg)
{
	jr2d_p renderer = (jr2d_p)arg;
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
}

static void on_destroy(scene_node_p n)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
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

static void load_dpad_panel(gameplay_controls_layer_p self, Context* ctx)
{
	int logical_width, logical_height;
	r2d_get_logical_presentation(ctx->r2d, &logical_width, &logical_height, NULL);
	font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();

	scene_node_p root_node = scene_node_create();
	scene_node_set_position(root_node, 0, logical_height - 150);
	scene_node_set_zorder(root_node, 100);
	scene_node_add_child(self->scene_node, root_node);

	self->btn_up = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_up);
	button_set_textx(self->btn_up, simhei_font, "U", (int)strlen("U"), { 0, 0, 0, 255 });
	scene_node_add_child(root_node, make_button_node(self->btn_up, 50, 0));

	self->btn_down = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_down);
	button_set_textx(self->btn_down, simhei_font, "D", (int)strlen("D"), { 0, 0, 0, 255 });
	scene_node_add_child(root_node, make_button_node(self->btn_down, 50, 100));

	self->btn_left = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_left);
	button_set_textx(self->btn_left, simhei_font, "L", (int)strlen("L"), { 0, 0, 0, 255 });
	scene_node_add_child(root_node, make_button_node(self->btn_left, 0, 50));

	self->btn_right = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_right);
	button_set_textx(self->btn_right, simhei_font, "R", (int)strlen("R"), { 0, 0, 0, 255 });
	scene_node_add_child(root_node, make_button_node(self->btn_right, 100, 50));
}


static void load_action_panel(gameplay_controls_layer_p self, Context* ctx)
{
	font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();
	int logical_width, logical_height;
	r2d_get_logical_presentation(ctx->r2d, &logical_width, &logical_height, NULL);

	scene_node_p root_node = scene_node_create();
	scene_node_set_position(root_node, logical_width - 100, logical_height - 100);
	scene_node_set_zorder(root_node, 100);
	scene_node_add_child(self->scene_node, root_node);

	self->btn_attack = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_attack);
	button_set_textx(self->btn_attack, simhei_font, "A", (int)strlen("A"), { 0, 0, 0, 255 });
	scene_node_add_child(root_node, make_button_node(self->btn_attack, 0, 0));

	// 远程攻击按钮（E），按住持续射击
	self->btn_ranged = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_ranged);
	button_set_textx(self->btn_ranged, simhei_font, "E", (int)strlen("E"), { 0, 0, 0, 255 });
	scene_node_add_child(root_node, make_button_node(self->btn_ranged, 60, 0));
}

gameplay_controls_layer_p create_gameplay_controls_layer(Context* ctx)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)malloc(sizeof(gameplay_controls_layer_t));
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

	load_dpad_panel(self, ctx);
	load_action_panel(self, ctx);

	return self;
}

scene_node_p gameplay_controls_layer_get_node(gameplay_controls_layer_p gameplay_controls_layer)
{
	return gameplay_controls_layer->scene_node;
}
