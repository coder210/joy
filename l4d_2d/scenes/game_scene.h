#ifndef GAME_SCENE_H
#define GAME_SCENE_H
#include <joy/joy.h>
#include "../context.h"

struct game_scene {
    scene_p scene;
    Context* ctx;
    float step_timer = 0;
    // 逻辑帧率测量
    uint32_t fps_measure_time = 0;
    int      fps_measure_frame = 0;
    // 通关判定（与 3D game_scene 业务逻辑对齐）
    bool game_started = false;   // 敌人生成完毕、关卡正式开始
    float victory_delay = 0.0f;  // 全部肃清后进入胜利场景前的延时累积
    bool victory_started = false;// 已触发胜利切换（防重复）
};
typedef struct game_scene game_scene_t, *game_scene_p;

game_scene_p game_scene_create(Context* ctx);
scene_p game_scene_get_scene(game_scene_p g);

#endif
