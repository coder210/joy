// 注意：entt 由调用方先通过 context.h 引入（shared 目录不包含 entt.hpp）
#pragma once
#include <joy/joy.h>
#include "../components/player_component.h"
#include "../components/transform_component.h"
#include "../components/action_state_component.h"

// ============================================================
// 玩家状态 HUD（纯视觉，只读组件，不影响帧同步/ECS 逻辑）
//   显示本地玩家: HP / Position / ActionState
//   需在 r2d_begin/r2d_end 之间调用
// ============================================================
static void player_status_display_draw(entt::registry& ecs, uint32_t local_conv,
        jr2d_p r2d, font_p font)
{
        const char* st     = "--";
        const char* dirstr = "-";
        float px = 0.0f, py = 0.0f;
        bool found = false;

        auto view = ecs.view<PlayerComponent, TransformComponent, ActionStateComponent>();
        for (auto e : view) {
                auto& pc = view.get<PlayerComponent>(e);
                if ((uint32_t)pc.conv != local_conv) continue;
                auto& t  = view.get<TransformComponent>(e);
                auto& ac = view.get<ActionStateComponent>(e);
                px = t.position.x;
                py = t.position.y;
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
                break;
        }

        char buf[128];
        if (found) {
                // HP 暂无数据源（未实现伤害结算），显示 --；接入结算后改为读取 S2CEntity.hp / HealthComponent
                snprintf(buf, sizeof(buf), "HP: --  POS:(%.1f, %.1f)  ST:%s %s",
                        px, py, st, dirstr);
        } else {
                snprintf(buf, sizeof(buf), "HP: --  POS:(--, --)  ST: --");
        }

        jcolor_t c = { 40, 60, 110, 235 };   // 深蓝，适配白底背景（与 FPS 深灰区分）
        jtext_printx(r2d, font, buf, (int)strlen(buf), c, 8.0f, 32.0f);
}
