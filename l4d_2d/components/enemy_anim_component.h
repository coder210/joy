#pragma once
// 敌人序列帧动画的运行时状态（每实体一份）。
// 与玩家的 AnimStateComponent（janim 状态机）不同：敌人素材是"每个动画一个文件夹、
// 内含 1.png..N.png 逐帧"，而非横向精灵表，故这里用更轻量的「动画槽 + 帧号 + 计时」自管推进。
// 纯渲染表现：不参与帧同步、不进快照、绝不回写逻辑组件。
struct EnemyAnimComponent {
    int   anim    = 0;      // 当前播放的动画槽（EnemyAnimId）
    int   frame   = 0;      // 当前帧序号（0 基）
    float elapsed = 0.0f;   // 当前帧已播放时长（秒）
    bool  flip_h  = false;  // 水平镜像（朝左时置 true）
    float total   = 0.0f;   // 当前动画已播放总时长（秒），用于死亡动画播完判定
};
