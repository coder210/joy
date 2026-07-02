#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "../flecs.h"
#include <joy/jmath.h>

// move_bits: bit0=上(W), bit1=下(S), bit2=左(A), bit3=右(D)
static const fp_t MOVE_SPEED = fp_from_float(5.0f);

// 移动系统
static void movement_system(PlayerStateInput &psi, LogicPositionComponent &p)
{
    if (psi.move_bits == 0) return;

    fp_t step = MOVE_SPEED;
    if (psi.move_bits & 0b0001) p.y = fp_sub(p.y, step); // 上
    if (psi.move_bits & 0b0010) p.y = fp_add(p.y, step); // 下
    if (psi.move_bits & 0b0100) p.x = fp_sub(p.x, step); // 左
    if (psi.move_bits & 0b1000) p.x = fp_add(p.x, step); // 右
}




#endif
