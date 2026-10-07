#ifndef PLAYER_STATUS_HUD_PASS_H
#define PLAYER_STATUS_HUD_PASS_H

#include <joy/joy.h>
#include "../asset_manager.h"
#include "../components/player_component.h"
#include "../components/transform_component.h"
#include "../components/action_state_component.h"
#include "../components/health_component.h"

static void player_status_hud_pass(entt::registry& ecs, uint32_t local_conv,
        jr2d_p r2d, font_p font)
{
        const char* st = "--";
        const char* dirstr = "-";
        float px = 0.0f, py = 0.0f;
        float hp = -1.0f, max_hp = (float)PLAYER_MAX_HP;
        int respawn = 0;
        bool is_dead = false;
        bool found = false;

        // 取实际逻辑窗口尺寸：HUD 位置按\"窗口百分比\"换算像素，不同分辨率都不越界/错位。
        int scr_w = 480, scr_h = 320;
        r2d_get_logical_presentation(r2d, &scr_w, &scr_h, NULL);

        ecs.view<PlayerComponent, TransformComponent, ActionStateComponent>().each(
                [&](auto e, PlayerComponent& pc, TransformComponent& t, ActionStateComponent& ac)
                {
                        if ((uint32_t)pc.conv != local_conv) return;
                        px = t.position.x;
                        py = t.position.y;
                        if (ecs.all_of<HealthComponent>(e)) {
                                auto& hc = ecs.get<HealthComponent>(e);
                                hp = (float)hc.hp;
                                max_hp = (float)hc.max_hp;
                                is_dead = hc.dead;
                                respawn = hc.respawn_timer;
                        }
                        found = true;
                        switch (ac.state) {
                        case ActionStateComponent::IDLE:   st = "IDLE";   break;
                        case ActionStateComponent::WALK:   st = "WALK";   break;
                        case ActionStateComponent::ATTACK: st = "ATTACK"; break;
                        default:                           st = "?";      break;
                        }
                        switch (ac.dir) {
                        case 0: dirstr = "D"; break;
                        case 1: dirstr = "L"; break;
                        case 2: dirstr = "R"; break;
                        case 3: dirstr = "U"; break;
                        default: dirstr = "-"; break;
                        }
                });

        char buf[128];
        if (found) {
                if (is_dead)
                        snprintf(buf, sizeof(buf), "HP: 0/%.0f  POS:(%.1f, %.1f)  ST:DEAD 重生%.1fs",
                                max_hp, px, py, respawn > 0 ? respawn / 20.0f : 0.0f);
                else if (hp >= 0.0f)
                        snprintf(buf, sizeof(buf), "HP: %.0f/%.0f  POS:(%.1f, %.1f)  ST:%s %s",
                                hp, max_hp, px, py, st, dirstr);
                else
                        snprintf(buf, sizeof(buf), "HP: --  POS:(%.1f, %.1f)  ST:%s %s",
                                px, py, st, dirstr);
        }
        else {
                snprintf(buf, sizeof(buf), "HP: --  POS:(--, --)  ST: --");
        }

        // ---- 玩家图形血量条（左上角，使用 11-Health Bar/Health Bar.png）----
        // 暗淡全条作底槽，按 hp/max_hp 裁左段填充；纯渲染、不进快照。
        // 位置以\"窗口百分比\"表达：左上 2%/3%，宽 30%（上限 360px），任何分辨率都在窗口内。
        float bx = scr_w * 0.02f;
        float by = scr_h * 0.03f;
        float barH = 28.0f;
        if (found && hp >= 0.0f) {
                auto* am = AssetManager::GetInstance();
                // 路径走 AssetManager 资源索引（resources.json 的 ui/health_bar），
                // 缺失时回退内置默认，不在此硬编码路径。
                const char* hb_path = am ? am->GetResourcePath(AssetManager::kCatUI, "health_bar") : nullptr;
                if (!hb_path) hb_path = "l4d_2d/sprites/objects/11-Health Bar/Health Bar.png";
                image_p hb = am ? am->GetImage(r2d, hb_path) : NULL;
                if (hb) {
                        image_set_blend_mode(hb, JR2D_BLEND_BLEND);
                        float iw = 0, ih = 0;
                        image_get_size(hb, &iw, &ih);
                        float barW = (scr_w * 0.30f > 360.0f) ? 360.0f : scr_w * 0.30f;
                        barH = (ih > 0.0f) ? (barW * ih / iw) : 28.0f;
                        float ratio = (max_hp > 0.0f) ? (hp / max_hp) : 0.0f;
                        if (ratio < 0.0f) ratio = 0.0f;
                        if (ratio > 1.0f) ratio = 1.0f;
                        vec2_t pos = { bx, by };
                        vec2_t scl = { barW / iw, barH / ih };
                        vec2_t origin = { 0.0f, 0.0f };
                        // 底槽（暗淡）
                        image_set_alpha(hb, 70);
                        jrect_t sr_full = { 0.0f, 0.0f, iw, ih };
                        image_draw_ex(hb, &sr_full, pos, 0.0f, scl, origin);
                        // 当前血量（裁左段）
                        if (ratio > 0.001f) {
                                image_set_alpha(hb, 255);
                                jrect_t sr_fill = { 0.0f, 0.0f, iw * ratio, ih };
                                image_draw_ex(hb, &sr_fill, pos, 0.0f, scl, origin);
                        }
                        image_set_alpha(hb, 255);
                }
        }

        // 调试文字（血量条下方）
        jcolor_t c = { 40, 40, 45, 235 };
        r2d_set_color(r2d, c.r, c.g, c.b, c.a);
        jtext_printx(r2d, font, buf, (int)strlen(buf), c, bx, by + barH + scr_h * 0.015f);
}

#endif