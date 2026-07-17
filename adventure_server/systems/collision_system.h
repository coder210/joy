#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include "../context.h"
#include <joy/joy.h>

static void collision_system2(Context* ctx, entt::registry& registry,
    PlayerComponent& p, LogicRectComponent& rect, LogicPositionComponent& pos)
{
    // 全部注释掉的系统，仅保留签名兼容
}

#endif // COLLISION_SYSTEM_H
