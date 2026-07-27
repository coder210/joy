#pragma once
#include <joy/anim.h>
#include "../entt.hpp"
#include "../../adventure_shared/components/enemy_component.h"

// ========== 创建默认的 warrior 动画定义，存入 registry.ctx() ==========
static inline void create_warrior_anim_def(entt::registry& registry, const char* tex_path) {
    anim_def_t def = {0};

    const int FPS = 6;
    const float DUR = 0.15f;
    const int FW = 192, FH = 192;

    // idle
    int s_idle = anim_add_state(&def, "idle", true);
    for (int i = 0; i < FPS; i++) {
        jrect_t r = { (float)(i * FW), 0, (float)FW, (float)FH };
        anim_add_clip(&def, s_idle, tex_path, DUR, r);
    }

    // walk
    int s_walk = anim_add_state(&def, "walk", true);
    for (int i = 0; i < FPS; i++) {
        jrect_t r = { (float)(i * FW), (float)(1 * FH), (float)FW, (float)FH };
        anim_add_clip(&def, s_walk, tex_path, DUR, r);
    }

    // attack directions
    const char* atk_names[] = { "attack_d","attack_l","attack_r","attack_u" };
    int atk_rows[] = { 4, 2, 2, 6 };
    int s_atk[4];
    for (int d = 0; d < 4; d++) {
        s_atk[d] = anim_add_state(&def, atk_names[d], false);
        for (int i = 0; i < FPS; i++) {
            jrect_t r = { (float)(i * FW), (float)(atk_rows[d] * FH), (float)FW, (float)FH };
            anim_add_clip(&def, s_atk[d], tex_path, DUR, r);
        }
    }

    // transitions
    int t;
    t = anim_add_transition(&def, s_idle, s_walk);
    anim_transition_add_cond(&def, t, "speed", ANIM_COND_GREATER, 0.5f);
    anim_transition_set_exit_time(&def, t, 0.05f);

    t = anim_add_transition(&def, s_walk, s_idle);
    anim_transition_add_cond(&def, t, "speed", ANIM_COND_LESS, 0.5f);
    anim_transition_set_exit_time(&def, t, 0.05f);

    for (int d = 0; d < 4; d++) {
        t = anim_add_transition(&def, -1, s_atk[d]);
        anim_transition_add_cond(&def, t, "attack", ANIM_COND_TRIGGER, 0);
        anim_transition_add_cond(&def, t, "dir", ANIM_COND_EQUALS, (float)d);
    }
    for (int d = 0; d < 4; d++) {
        t = anim_add_transition(&def, s_atk[d], s_idle);
        anim_transition_set_exit_time(&def, t, (float)(FPS * DUR));
    }

    anim_set_default_state(&def, s_idle);
    registry.ctx().emplace<anim_def_t>(std::move(def));
}

// ========== 创建 goblin torch 动画定义，存入 registry.ctx() 作为 EnemyAnimDef ==========
static inline void create_goblin_anim_def(entt::registry& registry, const char* tex_path) {
    const int FPS = 7;
    const float DUR = 0.15f;
    const int FW = 192, FH = 192;

    anim_def_t def = {0};

    // idle (row 0)
    int s_idle = anim_add_state(&def, "idle", true);
    for (int i = 0; i < FPS; i++) {
        jrect_t r = { (float)(i * FW), 0, (float)FW, (float)FH };
        anim_add_clip(&def, s_idle, tex_path, DUR, r);
    }

    // walk (row 1)
    int s_walk = anim_add_state(&def, "walk", true);
    for (int i = 0; i < FPS; i++) {
        jrect_t r = { (float)(i * FW), (float)(1 * FH), (float)FW, (float)FH };
        anim_add_clip(&def, s_walk, tex_path, DUR, r);
    }

    // attack directions (rows 2-4)
    const char* atk_names[] = { "attack_d","attack_l","attack_r","attack_u" };
    int atk_rows[] = { 2, 3, 3, 4 };
    int s_atk[4];
    for (int d = 0; d < 4; d++) {
        s_atk[d] = anim_add_state(&def, atk_names[d], false);
        for (int i = 0; i < FPS; i++) {
            jrect_t r = { (float)(i * FW), (float)(atk_rows[d] * FH), (float)FW, (float)FH };
            anim_add_clip(&def, s_atk[d], tex_path, DUR, r);
        }
    }

    // transitions
    int t;
    t = anim_add_transition(&def, s_idle, s_walk);
    anim_transition_add_cond(&def, t, "speed", ANIM_COND_GREATER, 0.5f);
    anim_transition_set_exit_time(&def, t, 0.05f);

    t = anim_add_transition(&def, s_walk, s_idle);
    anim_transition_add_cond(&def, t, "speed", ANIM_COND_LESS, 0.5f);
    anim_transition_set_exit_time(&def, t, 0.05f);

    for (int d = 0; d < 4; d++) {
        t = anim_add_transition(&def, -1, s_atk[d]);
        anim_transition_add_cond(&def, t, "attack", ANIM_COND_TRIGGER, 0);
        anim_transition_add_cond(&def, t, "dir", ANIM_COND_EQUALS, (float)d);
    }
    for (int d = 0; d < 4; d++) {
        t = anim_add_transition(&def, s_atk[d], s_idle);
        anim_transition_set_exit_time(&def, t, (float)(FPS * DUR));
    }

    anim_set_default_state(&def, s_idle);
    registry.ctx().emplace<EnemyAnimDef>(EnemyAnimDef{ std::move(def) });
}
