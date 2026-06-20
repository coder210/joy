#pragma once
#include <string>
#include <vector>
#include <joy/jr2d.h>

enum class AnimCondMode {
    GREATER, LESS, EQUALS, NOT_EQUALS, IF_TRUE, IF_FALSE, TRIGGER,
};

struct AnimClipDef {
    std::string path;
    jrect_t src_rect = {};
    float duration = 0.15f;
};

struct AnimCondDef {
    std::string param_name;
    AnimCondMode mode = AnimCondMode::GREATER;
    float threshold = 0;
};

struct AnimTransitionDef {
    int from_state = -1;
    int to_state = 0;
    std::vector<AnimCondDef> conditions;
    float exit_time = 0;
};

struct AnimStateDef {
    std::string name;
    std::vector<AnimClipDef> clips;
    bool loop = true;
    float speed = 1.0f;
};

struct AnimatorDef {
    std::vector<AnimStateDef> states;
    std::vector<AnimTransitionDef> transitions;
    int default_state = 0;
};
