#ifndef ENEMY_COMPONENT_H
#define ENEMY_COMPONENT_H
#include <joy/anim.h>

struct EnemyComponent {};

// 敌人动画定义（与玩家动画定义分开存储，可使用不同纹理）
struct EnemyAnimDef { anim_def_t def; };

#endif
