#include <string.h>
#include <math.h>
#include "gameplay_controls_layer.h"
#include "../asset_manager.h"
#include "../components/player_component.h"
#include "../components/action_state_component.h"

static const float GC_PI = 3.14159265f;

// 与 3D 控制层一致的水墨配色：白底 + 黑描边 + 黑字
static void setup_button_style(button_p btn) {
	jcolor_t normal  = { 255, 255, 255, 255 };
	jcolor_t hover   = { 255, 255, 255, 255 };
	jcolor_t pressed = { 230, 228, 222, 255 };  // 按下略深，体现水墨落笔
	button_set_normal_color(btn, normal);
	button_set_hover_color(btn, hover);
	button_set_pressed_color(btn, pressed);
}

struct gameplay_controls_layer {
	Context* ctx;
	scene_node_p scene_node;
	button_p btn_up, btn_down, btn_left, btn_right, btn_attack, btn_ranged, btn_ray;
	button_p pressed_btn;   // 当前按住中的按钮（方向键或技能键）
	int      pressed_code;  // 松开时的事件码：技能=触发码，方向=RELEASE_*
};

// 本地玩家某项冷却剩余比例（0..1 = 剩余/总量）。无本地玩家或未冷却返回 0。
// coolType: 0=无冷却(方向), 1=近战, 2=射击, 3=射线。
static float local_cool_ratio(Context* ctx, int coolType)
{
	if (!ctx || ctx->local_conv == 0) return 0.0f;
	auto view = ctx->ecs.view<PlayerComponent, ActionStateComponent>();
	for (auto e : view) {
		auto& pc  = view.get<PlayerComponent>(e);
		if (pc.conv != (int)ctx->local_conv) continue;
		auto& act = view.get<ActionStateComponent>(e);
		int cd = 0, max = 1;
		if (coolType == 1)      { cd = act.melee_cooldown;   max = ActionStateComponent::MELEE_COOLDOWN; }
		else if (coolType == 2) { cd = act.ranged_cooldown;  max = ActionStateComponent::RANGED_COOLDOWN; }
		else if (coolType == 3) { cd = act.ray_cooldown;     max = ActionStateComponent::RAY_COOLDOWN; }
		else return 0.0f;
		if (cd <= 0) return 0.0f;
		return (float)cd / (float)max;
	}
	return 0.0f;
}

// 墨色环形冷却进度：以 (cx,cy) 圆心、半径 R 的圆环（厚 thick），从正上方顺时针填充
// 已耗进度 frac∈[0,1]。纯表现层，不影响帧同步。
static void draw_cool_ring_2d(jr2d_p r2d, float cx, float cy,
		float R, float thick, float frac)
{
	if (frac <= 0.01f) return;
	constexpr int SEGS = 40;
	if (frac > 1.0f) frac = 1.0f;
	int filled = (int)(frac * SEGS);
	if (filled < 7) filled = 7;                 // 保证可见的一小段弧
	if (filled > SEGS) filled = SEGS;
	const float a0 = -GC_PI * 0.5f;
	r2d_set_color(r2d, 40, 40, 45, 200);        // 深墨
	for (int i = 0; i < filled; i++) {
		float a1 = a0 + (float)i * (2.0f * GC_PI / SEGS);
		float a2 = a0 + (float)(i + 1) * (2.0f * GC_PI / SEGS);
		for (int k = 0; k < (int)thick; k++) {
			float rr = R + (float)k;
			shape_draw_line(r2d, cx + cosf(a1) * rr, cy + sinf(a1) * rr,
			                      cx + cosf(a2) * rr, cy + sinf(a2) * rr);
		}
	}
}

// 圆按钮右下角叠加键盘键角标（方案 A，对齐 3D）：右下 1/4 处深墨小字母。
// 用 Font12（实际 14px），文字以 (bx,by) 中心对齐。
// 关键：jtext_print 以浮点坐标作 SDL_FRect 目标，非整数像素位会被 SDL3 线性插值 →
// 小字号发虚。故把中心坐标 round 到整数像素再减半宽，保证字符贴图像素对齐。
static void draw_key_badge(jr2d_p r2d, font_p font, const char* key,
		float bx, float by)
{
	if (!r2d || !font || !key || !key[0]) return;
	float size = 14.0f;                          // 角标像素高度（与 font 实际字号一致）
	float tw = size * 0.5f;                      // 单字符宽≈fontSize/2
	jcolor_t c = { 60, 60, 70, 255 };
	float px = floorf(bx - tw * 0.5f + 0.5f);    // 对齐到最近整数像素
	float py = floorf(by - size * 0.5f + 0.5f);
	jtext_printx(r2d, font, key, (int)strlen(key), c, px, py);
}

static void send_gameplay_control_event(int event_code)
{
	push_user_event(GAMEPLAY_CONTROL_EVENT, event_code);
}

// make_button_node：把按钮挂进场景节点，渲染为圆形水墨钮（白底 + 墨色描边 + 圆心文字），
// 外加冷却环/键盘角标；交互判定复用 button 状态。
// coolType: 0=无冷却(方向), 1=近战, 2=射击, 3=射线。
struct button_meta {
	button_p btn;
	Context* ctx;
	float w, h;
	const char* label;      // 圆心文字（上/下/左/右/攻/射/线）
	const char* keyLabel;   // 键盘角标字符（W/S/A/D/J/K/L）
	int coolType;
};

static scene_node_p make_button_node(button_p btn, int x, int y,
		Context* ctx, float w, float h, const char* label, const char* keyLabel, int coolType)
{
	auto* meta = new button_meta{ btn, ctx, w, h, label, keyLabel, coolType };
	scene_node_p node = scene_node_create();
	scene_node_set_position(node, (float)x, (float)y);
	scene_node_set_userdata(node, meta);
	scene_node_set_zorder(node, 100);
	scene_node_set_update_callback(node, [](scene_node_p n, float dt) {
		(void)dt;
		auto* m = (button_meta*)scene_node_get_userdata(n);
		vec2_t world_pos;
		scene_node_get_world_position(n, &world_pos.x, &world_pos.y);
		button_set_position(m->btn, world_pos.x, world_pos.y);
	});
	scene_node_set_render_callback(node, [](scene_node_p n, const void* arg) {
		(void)arg;
		auto* m = (button_meta*)scene_node_get_userdata(n);
		if (!m || !m->ctx) return;
		vec2_t wp;
		scene_node_get_world_position(n, &wp.x, &wp.y);
		float cx = wp.x + m->w * 0.5f;
		float cy = wp.y + m->h * 0.5f;
		float R  = (m->w < m->h ? m->w : m->h) * 0.5f;

		// 白底圆 + 墨色描边（显式设色，避免继承上层残留颜色导致红/杂色边框）
		jr2d_p r2d = m->ctx->r2d;
		r2d_set_color(r2d, 255, 255, 255, 255);
		shape_draw_circle(r2d, "fill", { cx, cy }, R, 40);
		r2d_set_color(r2d, 0, 0, 0, 255);
		shape_draw_circle(r2d, "line", { cx, cy }, R, 40);
		float out = R + 2.0f;
		shape_draw_circle(r2d, "line", { cx, cy }, out, 40);

		// 圆心文字（墨色）
		if (m->label && m->label[0]) {
			font_p f = AssetManager::GetInstance()->GetSimheiFont24();
			jcolor_t tc = { 30, 30, 35, 255 };
			jtext_printx(r2d, f, m->label, (int)strlen(m->label), tc,
				cx - 12.0f, cy - 12.0f);
		}

		// 冷却环：比最外描边略大一圈（深墨）
		if (m->coolType != 0) {
			float ratio = local_cool_ratio(m->ctx, m->coolType);
			if (ratio > 0.0f)
				draw_cool_ring_2d(r2d, cx, cy, out + 1.0f, 4.0f, 1.0f - ratio);
		}

		// 键盘角标：右下 1/4 处
		if (m->keyLabel && m->keyLabel[0]) {
			float off = R * 0.68f;
			float bx = cx + off * 0.7071f;
			float by = cy + off * 0.7071f;
			font_p f = AssetManager::GetInstance()->GetSimheiFont12();
			draw_key_badge(r2d, f, m->keyLabel, bx, by);
		}
	});
	return node;
}

// 交互：方向键 = 按下置位移位 / 松开清除（持续位移）；
// 攻/射/线 = 完整点击（按下在按钮内 + 松开在按钮内且未冷却）才触发一次离散技能。
static void on_event(scene_node_p n, const void* e)
{
	gameplay_controls_layer_p self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
	jevent_callbacks_t cbs = {};
	cbs.on_mousepressed = [](float x, float y, int btn, void* arg) {
		auto* self = (gameplay_controls_layer_p)arg;
		auto hit = [&](button_p b) { return button_is_pressed(b, x, y); };
		if (self->pressed_btn) return;   // 已按住一个钮，忽略后续按下
		if (self->btn_up    && hit(self->btn_up)) {
			self->pressed_btn = self->btn_up; self->pressed_code = GAMEPLAY_CONTROL_RELEASE_UP;
			send_gameplay_control_event(GAMEPLAY_CONTROL_UP);
		} else if (self->btn_down && hit(self->btn_down)) {
			self->pressed_btn = self->btn_down; self->pressed_code = GAMEPLAY_CONTROL_RELEASE_DOWN;
			send_gameplay_control_event(GAMEPLAY_CONTROL_DOWN);
		} else if (self->btn_left && hit(self->btn_left)) {
			self->pressed_btn = self->btn_left; self->pressed_code = GAMEPLAY_CONTROL_RELEASE_LEFT;
			send_gameplay_control_event(GAMEPLAY_CONTROL_LEFT);
		} else if (self->btn_right && hit(self->btn_right)) {
			self->pressed_btn = self->btn_right; self->pressed_code = GAMEPLAY_CONTROL_RELEASE_RIGHT;
			send_gameplay_control_event(GAMEPLAY_CONTROL_RIGHT);
		} else if (self->btn_attack && hit(self->btn_attack) && local_cool_ratio(self->ctx, 1) <= 0.0f) {
			self->pressed_btn = self->btn_attack; self->pressed_code = GAMEPLAY_CONTROL_ATTACK;
		} else if (self->btn_ranged && hit(self->btn_ranged) && local_cool_ratio(self->ctx, 2) <= 0.0f) {
			self->pressed_btn = self->btn_ranged; self->pressed_code = GAMEPLAY_CONTROL_RANGED;
		} else if (self->btn_ray && hit(self->btn_ray) && local_cool_ratio(self->ctx, 3) <= 0.0f) {
			self->pressed_btn = self->btn_ray; self->pressed_code = GAMEPLAY_CONTROL_RAY;
		}
	};
	cbs.on_mousereleased = [](float x, float y, int btn, void* arg) {
		auto* self = (gameplay_controls_layer_p)arg;
		if (!self->pressed_btn) return;
		button_p b = self->pressed_btn;
		int code = self->pressed_code;
		self->pressed_btn = NULL;
		self->pressed_code = 0;
		button_set_pressed(b, false);
		if (b == self->btn_attack || b == self->btn_ranged || b == self->btn_ray) {
			// 技能：完整点击（松开仍在按钮内）且未冷却才触发一次
			if (!button_is_hovered(b, x, y)) return;
			int coolType = (b == self->btn_attack) ? 1 : (b == self->btn_ranged) ? 2 : 3;
			if (local_cool_ratio(self->ctx, coolType) > 0.0f) return;
			send_gameplay_control_event(code);
		} else {
			// 方向键：松开即清除对应位移位
			send_gameplay_control_event(code);
		}
	};
	dispatch_event(cbs, e, self);
}

static void on_update(scene_node_p n, float dt)
{
	(void)n; (void)dt;
}

static void on_render(scene_node_p n, const void* arg)
{
	(void)n; (void)arg;
}

static void on_destroy(scene_node_p n)
{
	gameplay_controls_layer_p self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
	(void)self;
}

// 按钮布局：直接采用 3D 逻辑坐标（480x320）。左下方向键（W/S/A/D 方位对齐键盘），
// 右下技能区：攻(64) 最右下拇指位，射(58) 在其左下方，线(58) 弧形顶端。
static void load_dpad_panel(gameplay_controls_layer_p self, Context* ctx)
{
	scene_node_p root_node = scene_node_create();
	scene_node_set_position(root_node, 0, 0);
	scene_node_set_zorder(root_node, 100);
	scene_node_add_child(self->scene_node, root_node);

	self->btn_up = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_up);
	scene_node_add_child(root_node, make_button_node(self->btn_up, 50, 170, ctx, 50, 50, "上", "W", 0));

	self->btn_down = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_down);
	scene_node_add_child(root_node, make_button_node(self->btn_down, 50, 270, ctx, 50, 50, "下", "S", 0));

	self->btn_left = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_left);
	scene_node_add_child(root_node, make_button_node(self->btn_left, 0, 220, ctx, 50, 50, "左", "A", 0));

	self->btn_right = button_create(ctx->r2d, 50, 50);
	setup_button_style(self->btn_right);
	scene_node_add_child(root_node, make_button_node(self->btn_right, 100, 220, ctx, 50, 50, "右", "D", 0));

	self->btn_attack = button_create(ctx->r2d, 64, 64);
	setup_button_style(self->btn_attack);
	scene_node_add_child(root_node, make_button_node(self->btn_attack, 412, 254, ctx, 64, 64, "攻", "J", 1));

	self->btn_ranged = button_create(ctx->r2d, 58, 58);
	setup_button_style(self->btn_ranged);
	scene_node_add_child(root_node, make_button_node(self->btn_ranged, 340, 254, ctx, 58, 58, "射", "K", 2));

	self->btn_ray = button_create(ctx->r2d, 58, 58);
	setup_button_style(self->btn_ray);
	scene_node_add_child(root_node, make_button_node(self->btn_ray, 408, 176, ctx, 58, 58, "线", "L", 3));
}

gameplay_controls_layer_p create_gameplay_controls_layer(Context* ctx)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)malloc(sizeof(gameplay_controls_layer_t));
	memset(self, 0, sizeof(*self));
	self->ctx = ctx;
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

	return self;
}

scene_node_p gameplay_controls_layer_get_node(gameplay_controls_layer_p gameplay_controls_layer)
{
	return gameplay_controls_layer->scene_node;
}