#pragma once
#include <math.h>
#include <unordered_map>
#include <joy/joy.h>
#include "../components/global_state_component.h"
#include "../components/id_component.h"
#include "../components/player_component.h"
#include "../components/enemy_component.h"
#include "../components/bullet_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"
#include "../components/action_state_component.h"
#include "../components/health_component.h"
#include "../protocol.h"

struct Context;

// ============================================================
// 实体几何渲染（官方《几何决斗》画风，非照搬）
//   - 白色画布 + 浅灰网格（随相机滚动）
//   - 玩家/敌人为旋转的正方形（玩家白填充黑描边 / 敌人黑填充白描边）
//   - 标志性"浑天仪式"双轨道环：三角形↔正方形间逐点变形，
//     随移动速度变亮、变大、自旋加快（蓝系=玩家 / 金系=敌人）
//   - 朝向指示线、攻击发光射线
//   - 纯视觉，不参与帧同步（视觉几何 ≠ 碰撞几何，碰撞体以红框为准）
// ============================================================

static const float GD_PI         = 3.14159265f;
static const float GD_GRID_STEP  = 50.0f;    // 背景网格间距（逻辑像素）
static const float GD_RING_IN    = 36.0f;    // 内环半径（官方 52，按视口比例缩小）
static const float GD_RING_OUT   = 48.0f;    // 外环半径（官方 72，按视口比例缩小）
static const int   GD_RING_POINTS = 36;      // 环采样点（三角 3 边×12，方形 4 边×9）

// 每个实体的平滑速度比：走/停切换时环的速度/大小/亮度渐变过渡，消除突变观感
static std::unordered_map<entt::entity, float> g_ring_smooth;

// 每个实体上一渲染帧的视觉位置（逻辑单位）：用于以"真实位移速度"驱动环动画，
// 而非依赖 act.state 标签（ai_system/pathfinding_system 交替写 WALK/IDLE 会导致
// 静止也全速转 + 忽快忽慢抖动；位移速度天然与标签解耦、平滑无抖动）
static std::unordered_map<entt::entity, vec2_t> g_ring_last_pos;
static float g_ring_last_vt = 0.0f;
static bool  g_ring_vt_init  = false;

// ---- 三角形 ↔ 正方形 逐点插值，生成形态环采样点 ----
// blend ∈ [0,1]：0=三角形，1=正方形；rot 为环整体旋转角
// [已注释] 轨道环暂时停用（2026-08-16）：玩家/敌人的双轨道环不再绘制
// static void morph_ring_points(vec2_t center, float R, float blend, float rot, vec2_t* pts)
// {
//         vec2_t tri[GD_RING_POINTS], quad[GD_RING_POINTS];
//         for (int i = 0; i < GD_RING_POINTS; i++) {
//                 // 三角形：3 条边，每边 12 个采样点
//                 int e = i / 12;
//                 int s = i % 12;
//                 float a0 = rot + e * (2.0f * GD_PI / 3.0f);
//                 float a1 = rot + (e + 1) * (2.0f * GD_PI / 3.0f);
//                 float t  = (float)s / 11.0f;
//                 vec2_t p0 = { center.x + R * cosf(a0), center.y + R * sinf(a0) };
//                 vec2_t p1 = { center.x + R * cosf(a1), center.y + R * sinf(a1) };
//                 tri[i].x = p0.x + (p1.x - p0.x) * t;
//                 tri[i].y = p0.y + (p1.y - p0.y) * t;

//                 // 正方形：4 条边，每边 9 个采样点（+45° 起始，rot=0 时角朝上）
//                 int qe = i / 9;
//                 int qs = i % 9;
//                 float b0 = rot + 0.785398f + qe * (GD_PI * 0.5f);
//                 float b1 = rot + 0.785398f + (qe + 1) * (GD_PI * 0.5f);
//                 float q  = (float)qs / 8.0f;
//                 vec2_t q0 = { center.x + R * cosf(b0), center.y + R * sinf(b0) };
//                 vec2_t q1 = { center.x + R * cosf(b1), center.y + R * sinf(b1) };
//                 quad[i].x = q0.x + (q1.x - q0.x) * q;
//                 quad[i].y = q0.y + (q1.y - q0.y) * q;

//                 pts[i].x = tri[i].x + (quad[i].x - tri[i].x) * blend;
//                 pts[i].y = tri[i].y + (quad[i].y - tri[i].y) * blend;
//         }
// }

// ---- 双轨道环（浑天仪式）：内外环反向自旋，随速度变亮变大，三角↔方形变形 ----
// [已注释] 轨道环暂时停用（2026-08-16）：玩家/敌人的双轨道环不再绘制
// static void draw_orbits(Context* ctx, vec2_t c, float speed_ratio, bool is_player,
//         float visual_time, float seed)
// {
//         jcolor_t deep, bright;
//         if (is_player) {
//                 deep   = { 45, 65, 140, 255 };   // 深蓝
//                 bright = { 90, 140, 255, 255 };  // 亮蓝
//         } else {
//                 deep   = { 155, 115, 30, 255 };  // 深金
//                 bright = { 255, 200, 80, 255 };  // 亮金
//         }

//         float alpha = 0.5f + speed_ratio * 0.35f;      // 速度越快越亮
//         float grow  = 1.0f + speed_ratio * 0.6f;       // 速度越快越大
//         // 内外环变形相位相差 π：一边接近三角形时另一边接近正方形
//         // 0.05f → 变形周期约 126 秒，呼吸舒缓不易察觉（原 0.25f 每 ~12 秒就回到三角形起点）
//         float morph1 = 0.5f + 0.5f * sinf(visual_time * 0.05f + seed);
//         float morph2 = 0.5f + 0.5f * sinf(visual_time * 0.05f + seed + GD_PI);
//         // 内外环反向自旋，速度越快转得越快
//         float rot_in  = visual_time * (0.6f + speed_ratio * 1.4f);
//         float rot_out = -visual_time * (0.45f + speed_ratio * 1.1f);

//         vec2_t ring[GD_RING_POINTS];

//         // ---- 外环（亮色）----
//         // 发光晕圈：向外扩 4px，半透明
//         morph_ring_points(c, (GD_RING_OUT + 4.0f) * grow, morph1, rot_out, ring);
//         r2d_set_color(ctx->r2d, bright.r, bright.g, bright.b, (uint8_t)(60 * alpha));
//         for (int i = 0; i < GD_RING_POINTS; i++) {
//                 vec2_t a = ring[i], b = ring[(i + 1) % GD_RING_POINTS];
//                 shape_draw_line(ctx->r2d, a.x, a.y, b.x, b.y);
//         }
//         // 主环
//         morph_ring_points(c, GD_RING_OUT * grow, morph1, rot_out, ring);
//         r2d_set_color(ctx->r2d, bright.r, bright.g, bright.b, (uint8_t)(255 * alpha));
//         for (int i = 0; i < GD_RING_POINTS; i++) {
//                 vec2_t a = ring[i], b = ring[(i + 1) % GD_RING_POINTS];
//                 shape_draw_line(ctx->r2d, a.x, a.y, b.x, b.y);
//         }

//         // ---- 内环（深色）----
//         morph_ring_points(c, (GD_RING_IN + 3.0f) * grow, morph2, rot_in, ring);
//         r2d_set_color(ctx->r2d, deep.r, deep.g, deep.b, (uint8_t)(55 * alpha));
//         for (int i = 0; i < GD_RING_POINTS; i++) {
//                 vec2_t a = ring[i], b = ring[(i + 1) % GD_RING_POINTS];
//                 shape_draw_line(ctx->r2d, a.x, a.y, b.x, b.y);
//         }
//         morph_ring_points(c, GD_RING_IN * grow, morph2, rot_in, ring);
//         r2d_set_color(ctx->r2d, deep.r, deep.g, deep.b, (uint8_t)(255 * alpha));
//         for (int i = 0; i < GD_RING_POINTS; i++) {
//                 vec2_t a = ring[i], b = ring[(i + 1) % GD_RING_POINTS];
//                 shape_draw_line(ctx->r2d, a.x, a.y, b.x, b.y);
//         }
// }

// ---- 旋转正方形本体（4 顶点，fill + 描边）----
static void draw_rotated_square(Context* ctx, vec2_t c, float half, float rot,
        jcolor_t fill, jcolor_t stroke)
{
        vec2_t pts[4];
        for (int i = 0; i < 4; i++) {
                float a = rot + i * (GD_PI * 0.5f);
                pts[i].x = c.x + cosf(a) * half;
                pts[i].y = c.y + sinf(a) * half;
        }
        shape_draw_polygon(ctx->r2d, "fill", pts, 4, fill);
        shape_draw_polygon(ctx->r2d, "line", pts, 4, stroke);
}

// ---- 环形血条：围绕本体的一圈弧线，弧长 = 当前血量比例 ----
//   ratio>50% 保持阵营色（蓝=玩家 / 金=敌人）、25%~50% 变黄、<25% 变红并呼吸闪烁
//   死亡不画（尸体灰块已表达状态）。纯视觉，不参与帧同步。
//   ring_R：血环半径（像素），由调用方按碰撞框宽传入，保证环直径 ≡ 碰撞框宽
static void draw_hp_ring(Context* ctx, vec2_t c, float ring_R, const HealthComponent& hc,
        bool is_player)
{
        if (hc.dead) return;

        float ratio = hc.hp / hc.max_hp;
        if (ratio > 1.0f) ratio = 1.0f;
        if (ratio < 0.0f) ratio = 0.0f;

        const float R    = ring_R;          // 直径 = 碰撞框宽
        const int   segs = 24;             // 弧线采样段
        const float a0   = -GD_PI * 0.5f;  // 起始于正上方，顺时针扫过

        // 底色环：一圈浅灰半透明，表示最大血量（缺角即缺口，信息可读）
        jcolor_t dim = is_player ? jcolor_t{ 60, 80, 150, 70 } : jcolor_t{ 150, 120, 50, 70 };
        r2d_set_color(ctx->r2d, dim.r, dim.g, dim.b, dim.a);
        for (int i = 0; i < segs; i++) {
                float a1 = a0 + (float)i * (2.0f * GD_PI / segs);
                float a2 = a0 + (float)(i + 1) * (2.0f * GD_PI / segs);
                vec2_t p1 = { c.x + cosf(a1) * R, c.y + sinf(a1) * R };
                vec2_t p2 = { c.x + cosf(a2) * R, c.y + sinf(a2) * R };
                shape_draw_line(ctx->r2d, p1.x, p1.y, p2.x, p2.y);
        }

        if (ratio <= 0.0f) return;

        // 血量弧：颜色分级 + 低血量呼吸
        jcolor_t col;
        if      (ratio > 0.50f) col = is_player ? jcolor_t{ 90, 140, 255, 255 } : jcolor_t{ 255, 200, 80, 255 };
        else if (ratio > 0.25f) col = jcolor_t{ 240, 200, 60, 255 };
        else                    col = jcolor_t{ 230, 60, 50, 255 };
        if (ratio < 0.25f) {
                float pulse = 0.55f + 0.45f * sinf(ctx->visual_time * 6.0f);
                col.a = (uint8_t)(255 * pulse);
        }
        r2d_set_color(ctx->r2d, col.r, col.g, col.b, col.a);

        float sweep = ratio * (2.0f * GD_PI);
        int   n     = (int)ceilf(sweep / (2.0f * GD_PI) * segs);
        for (int i = 0; i < n; i++) {
                float a1 = a0 + (float)i * (2.0f * GD_PI / segs);
                float a2 = a0 + (float)(i + 1) * (2.0f * GD_PI / segs);
                if (a2 - a0 > sweep) a2 = a0 + sweep;
                vec2_t p1 = { c.x + cosf(a1) * R, c.y + sinf(a1) * R };
                vec2_t p2 = { c.x + cosf(a2) * R, c.y + sinf(a2) * R };
                shape_draw_line(ctx->r2d, p1.x, p1.y, p2.x, p2.y);
        }
}

// ---- 白色画布 + 浅灰网格（随相机滚动）----
static void draw_grid_background(Context* ctx)
{
        auto& gs = ctx->ecs.ctx().get<GlobalState>();
        float cx = gs.camera_position.x * SERVER_PPM;
        float cy = gs.camera_position.y * SERVER_PPM;

        // 白底
        r2d_set_color(ctx->r2d, 255, 255, 255, 255);
        jrect_t bg = { 0, 0, 480, 320 };
        shape_draw_rectangle(ctx->r2d, "fill", bg);

        // 浅灰网格（官方 rgba(224,224,224,0.3) ≈ alpha 76）
        r2d_set_color(ctx->r2d, 224, 224, 224, 76);
        float offx = fmodf(-cx, GD_GRID_STEP);
        if (offx > 0.0f) offx -= GD_GRID_STEP;
        for (float gx = offx; gx <= 480.0f; gx += GD_GRID_STEP)
                shape_draw_line(ctx->r2d, gx, 0.0f, gx, 320.0f);
        float offy = fmodf(-cy, GD_GRID_STEP);
        if (offy > 0.0f) offy -= GD_GRID_STEP;
        for (float gy = offy; gy <= 320.0f; gy += GD_GRID_STEP)
                shape_draw_line(ctx->r2d, 0.0f, gy, 480.0f, gy);
}

// ---- 主渲染入口（签名保持不变，供 client/server 场景调用）----
static void drawing_entity_system(Context* ctx, entt::entity e,
        IdComponent& id, LogicRectComponent& rect, TransformComponent& t)
{
        (void)id;
        if (!ctx->ecs.valid(e)) return;

        bool is_player = ctx->ecs.all_of<PlayerComponent>(e);
        bool is_enemy  = ctx->ecs.all_of<EnemyComponent>(e);
        bool is_bullet = ctx->ecs.all_of<BulletComponent>(e);
        if (!is_player && !is_enemy && !is_bullet) return;   // 只渲染玩家/敌人/子弹

        auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
        vec2_t c;
        c.x = (t.position.x - cam.x) * SERVER_PPM;
        c.y = (t.position.y - cam.y) * SERVER_PPM;

        // ---- 子弹：发光旋转菱形（金红色，不画轨道环/攻击射线）----
        if (is_bullet) {
                float bs = 7.0f;   // 视觉半边长（像素）
                float bspin = ctx->visual_time * 6.0f;
                vec2_t pts[4];
                for (int i = 0; i < 4; i++) {
                        float a = bspin + i * (GD_PI * 0.5f);
                        pts[i].x = c.x + cosf(a) * bs;
                        pts[i].y = c.y + sinf(a) * bs;
                }
                shape_draw_polygon(ctx->r2d, "fill", pts, 4, { 255, 150, 60, 90 });  // 辉光
                for (int i = 0; i < 4; i++) {
                        float a = bspin + i * (GD_PI * 0.5f);
                        pts[i].x = c.x + cosf(a) * bs * 0.6f;
                        pts[i].y = c.y + sinf(a) * bs * 0.6f;
                }
                shape_draw_polygon(ctx->r2d, "fill", pts, 4, { 255, 205, 95, 255 });   // 主体
                shape_draw_polygon(ctx->r2d, "line", pts, 4, { 255, 130, 35, 255 });   // 描边
                return;
        }

        // 视觉方块半边长：以碰撞盒较长边为准（旋转后角部略超碰撞盒，碰撞体以红框为准）
        float w = fp_to_float(rect.width);
        float h = fp_to_float(rect.height);
        float half = (w > h ? w : h) * 0.5f;
        if (half < 8.0f) half = 8.0f;

        float dir_rad = -GD_PI * 0.5f;   // 默认朝上（dir 0=下 1=左 2=右 3=上）
        bool is_attack = false;
        if (ctx->ecs.all_of<ActionStateComponent>(e)) {
                auto& ac = ctx->ecs.get<ActionStateComponent>(e);
                if (ac.state == ActionStateComponent::ATTACK) is_attack = true;
                switch (ac.dir) {
                case 0: dir_rad =  GD_PI * 0.5f; break;   // 下
                case 1: dir_rad =  GD_PI;        break;   // 左
                case 2: dir_rad =  0.0f;         break;   // 右
                case 3: dir_rad = -GD_PI * 0.5f; break;   // 上
                }
        }

        // ---- 环速度比：由真实位移速度驱动（逻辑单位/秒），与 act.state 标签解耦 ----
        // 修复：玩家未加入时 command_queue 为空 → ai_system 不运行 → act.state 恒为 IDLE
        // → 环慢转"正常"；玩家加入后 ai_system 把敌人写成 WALK（即使静止）→ 环全速自旋
        // → 且 pathfinding_system 又回写 IDLE → speed_ratio 0↔1 抖动。
        // 现在改为量测每帧实际位移，动则快、静则慢，任何 AI 标签都不再干扰视觉节奏。
        float dt = 0.0f;
        if (g_ring_vt_init) {
                dt = ctx->visual_time - g_ring_last_vt;
                if (dt < 0.0f) dt = 0.0f;
        }
        g_ring_last_vt = ctx->visual_time;
        g_ring_vt_init = true;

        float target_ratio = 0.0f;
        auto it = g_ring_last_pos.find(e);
        if (dt > 0.0001f && it != g_ring_last_pos.end()) {
                float dx = t.position.x - it->second.x;
                float dy = t.position.y - it->second.y;
                float move_speed = sqrtf(dx * dx + dy * dy) / dt;   // 逻辑单位/秒
                float max_speed = is_player ? 5.0f : 2.5f;          // 玩家 MOVE_SPEED=5.0 / 敌人步速 2.5
                target_ratio = move_speed / max_speed;
                if (target_ratio > 1.0f) target_ratio = 1.0f;
                it->second = { t.position.x, t.position.y };
        } else {
                // 首次记录位置：首帧视为静止，避免实体初生/复用 ID 时位移跳变
                g_ring_last_pos[e] = { t.position.x, t.position.y };
        }

        // 指数平滑趋近目标值：走/停切换时环渐变，不再 0↔1 硬切
        float& smooth = g_ring_smooth[e];
        smooth += (target_ratio - smooth) * 0.15f;
        if (fabsf(smooth - target_ratio) < 0.01f) smooth = target_ratio;
        float speed_ratio = smooth;

        // [已注释] 轨道环相位种子：同一实体稳定、不同实体错开（环停用，暂无消费）
        // float seed = t.position.x * 1.7f + t.position.y * 3.1f + (is_player ? 0.0f : 13.37f);

        // 自旋角：静止慢转、移动加快；基础 45° 使方块呈菱形（官方造型）
        float spin = 0.7f + speed_ratio * 1.8f;
        float rot  = 0.785398f + ctx->visual_time * spin;

        // [已注释] 轨道环暂时停用：本体直接压上来
        // draw_orbits(ctx, c, speed_ratio, is_player, ctx->visual_time, seed);

        // 攻击：沿朝向画发光射线（两层线模拟辉光）
        if (is_attack) {
                float ray_len = half * 3.2f;
                vec2_t r1 = { c.x + cosf(dir_rad) * half * 0.9f, c.y + sinf(dir_rad) * half * 0.9f };
                vec2_t r2 = { c.x + cosf(dir_rad) * ray_len,     c.y + sinf(dir_rad) * ray_len };
                r2d_set_color(ctx->r2d, is_player ? 90 : 255, is_player ? 140 : 200, is_player ? 255 : 80, 90);
                shape_draw_line(ctx->r2d, r1.x - 2.0f, r1.y - 2.0f, r2.x - 2.0f, r2.y - 2.0f);
                r2d_set_color(ctx->r2d, is_player ? 45 : 155, is_player ? 65 : 115, is_player ? 140 : 30, 230);
                shape_draw_line(ctx->r2d, r1.x, r1.y, r2.x, r2.y);
        }

        // 本体旋转方块（死亡玩家变灰，代表"尸体"）
        bool player_dead = is_player &&
                ctx->ecs.all_of<HealthComponent>(e) && ctx->ecs.get<HealthComponent>(e).dead;
        if (is_player) {
                jcolor_t fill   = player_dead ? jcolor_t{ 168, 170, 175, 200 } : jcolor_t{ 255, 255, 255, 255 };
                jcolor_t stroke = player_dead ? jcolor_t{ 92, 94, 100, 255 }   : jcolor_t{ 30, 30, 35, 255 };
                draw_rotated_square(ctx, c, half, rot, fill, stroke);
        } else {
                jcolor_t fill   = { 22, 22, 26, 255 };
                jcolor_t stroke = { 255, 255, 255, 255 };
                draw_rotated_square(ctx, c, half, rot, fill, stroke);
        }

        // 朝向指示线（沿移动方向，方块内侧；死亡时变暗）
        r2d_set_color(ctx->r2d, player_dead ? 120 : (is_player ? 90 : 255),
                player_dead ? 120 : (is_player ? 140 : 200),
                player_dead ? 125 : (is_player ? 255 : 80), player_dead ? 140 : 220);
        vec2_t tip = { c.x + cosf(dir_rad) * half * 0.8f, c.y + sinf(dir_rad) * half * 0.8f };
        shape_draw_line(ctx->r2d, c.x, c.y, tip.x, tip.y);

        // 环形血条：直径 ≡ 碰撞框宽，弧长=血量比例，颜色分级（死亡不画）
        if (ctx->ecs.all_of<HealthComponent>(e)) {
                auto& hc = ctx->ecs.get<HealthComponent>(e);
                float ring_R = fp_to_float(rect.width) * SERVER_PPM * 0.5f;
                draw_hp_ring(ctx, c, ring_R, hc, is_player);
        }
}
