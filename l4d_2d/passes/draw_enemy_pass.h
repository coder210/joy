#ifndef DRAW_ENEMY_PASS_H
#define DRAW_ENEMY_PASS_H
#include <math.h>
#include <string>
#include <joy/joy.h>
#include "../components/global_state_component.h"
#include "../components/enemy_component.h"
#include "../components/enemy_anim_component.h"
#include "../components/transform_component.h"
#include "../components/action_state_component.h"
#include "../components/health_component.h"
#include "../systems/enemy_catalog.h"
#include "../asset_manager.h"
#include "../protocol.h"
#include "render_math.h"

struct Context;

/* ============================================================
 * 敌人序列帧渲染（enemies.json 驱动）
 *  - 素材格式与玩家不同：玩家是横向精灵表（单张 png，帧数=宽/高），
 *    敌人是"每个动画一个文件夹、内含 1.png .. N.png 逐帧"，
 *    且各敌人的文件夹编号不一致（Hit 在 bald_pirate 是 8-Hit、big_guy 是 12-Hit），
 *    故动画目录名与帧数全部由 enemies.json 显式给出（EnemyCatalog::framePath）。
 *  - 不用 janim：janim 面向精灵表裁剪，逐帧独立文件用轻量「动画槽+帧号+计时」自管更直接。
 *  - 纯渲染层：只写 EnemyAnimComponent，绝不回写 LogicPosition/Health 等逻辑组件，
 *    不参与帧同步、不进快照。
 * ============================================================ */

// 播放帧率（帧/秒）。素材未携带帧率，统一取 12 FPS（与玩家一致）。
static const float kEnemySpriteFPS = 12.0f;
// 精灵标尺：与玩家渲染一致用 50 PPM。不可沿用 meters_to_pixels（那是地图 PPM，
// 会把精灵缩到约 1/4）。
static const float kEnemyRenderPPM = 50.0f;

// 渲染所有敌人实体（按种类取序列帧；种类缺失时回退 type 0，不会变成"不绘制"）
static void draw_enemy_pass(Context* ctx, float dt)
{
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    const float z = (gs.view_zoom > 0.0f) ? gs.view_zoom : 1.0f;
    auto* am = AssetManager::GetInstance();
    if (!am) return;

    // 注意：这里按 EnemyComponent 取，不要求 EnemyTypeComponent——
    // 缺少种类标记的敌人仍以默认种类绘制，避免实体"消失"。
    auto view = ctx->ecs.view<EnemyComponent, TransformComponent>();
    for (auto e : view) {
        if (!ctx->ecs.valid(e)) continue;

        int type = 0;
        if (ctx->ecs.all_of<EnemyTypeComponent>(e))
            type = ctx->ecs.get<EnemyTypeComponent>(e).type;
        const EnemyDef& def = g_enemyCatalog.get(type);
        auto& t = view.get<TransformComponent>(e);

        // 每实体动画状态（渲染期首次挂载；实体销毁由 ECS 自动回收）
        auto& anim = ctx->ecs.get_or_emplace<EnemyAnimComponent>(e);

        // ---- 选定目标动画槽 ----
        bool dead = ctx->ecs.all_of<HealthComponent>(e)
                 && ctx->ecs.get<HealthComponent>(e).dead;

        int state = ActionStateComponent::IDLE;
        int knockback = 0;
        if (ctx->ecs.all_of<ActionStateComponent>(e)) {
            auto& act = ctx->ecs.get<ActionStateComponent>(e);
            state     = act.state;
            knockback = act.knockback_timer;
        }

        int want;
        if (dead) {
            // 死亡动画播完一遍后切"倒地"，并停在末帧
            int   dn  = g_enemyCatalog.frameCount(def, ENEMY_ANIM_DEAD);
            float dur = (dn > 0) ? (float)dn / kEnemySpriteFPS : 0.0f;
            want = (anim.anim == ENEMY_ANIM_DEAD && dur > 0.0f && anim.total >= dur)
                 ? ENEMY_ANIM_DEAD_GROUND : ENEMY_ANIM_DEAD;
        } else if (knockback > 0) {
            want = ENEMY_ANIM_HIT;
        } else if (state == ActionStateComponent::ATTACK) {
            want = ENEMY_ANIM_ATTACK;
        } else if (state == ActionStateComponent::WALK) {
            want = ENEMY_ANIM_RUN;
        } else {
            want = ENEMY_ANIM_IDLE;
        }

        // 切槽：重置帧号与计时
        if (want != anim.anim) {
            anim.anim = want; anim.frame = 0; anim.elapsed = 0.0f; anim.total = 0.0f;
        }

        int frames = g_enemyCatalog.frameCount(def, (EnemyAnimId)anim.anim);
        if (frames <= 0) continue;   // 该动画缺失且 idle 也不可用 → 本帧不绘制

        // ---- 推进帧 ----
        anim.elapsed += dt;
        anim.total   += dt;
        const float frame_dur = 1.0f / kEnemySpriteFPS;
        bool oneshot = (anim.anim == ENEMY_ANIM_DEAD || anim.anim == ENEMY_ANIM_DEAD_GROUND
                     || anim.anim == ENEMY_ANIM_ATTACK || anim.anim == ENEMY_ANIM_HIT);
        while (anim.elapsed >= frame_dur) {
            anim.elapsed -= frame_dur;
            if (oneshot) {
                if (anim.frame < frames - 1) anim.frame++;      // 播到末帧
                else { anim.elapsed = 0.0f; break; }            // 停在末帧
            } else {
                anim.frame = (anim.frame + 1) % frames;         // 循环（idle / run）
            }
        }
        if (anim.frame >= frames) anim.frame = frames - 1;

        // ---- 朝向：dir 0=下 1=左 2=右 3=上；上/下沿用上一次朝向（与玩家一致）----
        if (ctx->ecs.all_of<ActionStateComponent>(e)) {
            int dir = ctx->ecs.get<ActionStateComponent>(e).dir;
            if (dir == 1)      anim.flip_h = true;    // 左：镜像（素材默认朝右）
            else if (dir == 2) anim.flip_h = false;   // 右：不翻转
        }

        // ---- 取帧图并绘制 ----
        std::string path = g_enemyCatalog.framePath(def, (EnemyAnimId)anim.anim, anim.frame);
        if (path.empty()) continue;
        image_p img = am->GetImage(ctx->r2d, path.c_str());
        if (!img) continue;

        float iw = 0.0f, ih = 0.0f;
        if (!image_get_size(img, &iw, &ih) || ih <= 0.0f) continue;

        // 目标高度 = 该种类渲染高度（米）× 精灵标尺 × 世界缩放
        const float sizePx = def.render_height * kEnemyRenderPPM * z;
        const float scale  = sizePx / ih;

        float sx = meters_to_pixels(t.position.x - gs.camera_position.x) * z;
        float sy = meters_to_pixels(t.position.y - gs.camera_position.y) * z;

        jrect_t src    = { 0.0f, 0.0f, iw, ih };
        vec2_t  pos    = { sx, sy };
        vec2_t  scl    = { anim.flip_h ? -scale : scale, scale };
        vec2_t  origin = { iw * 0.5f, ih * 0.5f };
        image_draw_ex(img, &src, pos, 0.0f, scl, origin);
    }
}

#endif
