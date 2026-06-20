#pragma once
#include <string>
#include <vector>
#include <joy/jr2d.h>

// 条件比较模式
enum class AnimCondMode {
    GREATER,      // param > threshold
    LESS,         // param < threshold
    EQUALS,       // param == threshold
    NOT_EQUALS,   // param != threshold
    IF_TRUE,      // bool param == true
    IF_FALSE,     // bool param == false
    TRIGGER,      // trigger was set
};

// 单帧定义
struct AnimClipDef {
    std::string path;       // 纹理路径
    jrect_t src_rect = {};  // 源裁剪矩形
    float duration = 0.15f; // 持续时间（秒）
};

// 条件定义
struct AnimCondDef {
    std::string param_name;
    AnimCondMode mode = AnimCondMode::GREATER;
    float threshold = 0;
};

// 过渡定义
struct AnimTransitionDef {
    int from_state = -1;                // -1 = Any State
    int to_state = 0;
    std::vector<AnimCondDef> conditions;
    float exit_time = 0;                // 源状态最小播放时间
};

// 状态定义
struct AnimStateDef {
    std::string name;
    std::vector<AnimClipDef> clips;     // 帧列表
    bool loop = true;
    float speed = 1.0f;
};

// ========== ECS 组件 ==========

// ★ 动画状态机定义（挂载在专用实体上，供多个 Animator 引用）
struct AnimatorDef {
    std::vector<AnimStateDef> states;
    std::vector<AnimTransitionDef> transitions;
    int default_state = 0;
};
