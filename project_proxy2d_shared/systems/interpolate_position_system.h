#ifndef INTERPOLATE_POSITION_SYSTEM_H
#define INTERPOLATE_POSITION_SYSTEM_H
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"

// 包含 IdComponent，各端的 IdComponent 在不同位置
// 客户端：project_proxy2d_client/components/player_component.h
// 服务端：project_proxy2d_server/components/connection_component.h
// 此处统一用前向声明避免冲突，由调用方确保 IdComponent 已包含
#include "../components/id_component.h"

static void interpolate_position_system(entt::registry& registry, Context* /*ctx*/, float dt) {
	// LogicPositionComponent.x/y 是碰撞盒左上角（rectanglef_t 语义），
	// TransformComponent.position 定义为碰撞盒中心，故插值目标 = 左上角 + 半宽高
	auto view = registry.view<IdComponent, LogicPositionComponent, LogicRectComponent, TransformComponent>();
	(void)dt;
	for (auto e : view) {
		auto& lp   = view.get<LogicPositionComponent>(e);
		auto& rect = view.get<LogicRectComponent>(e);
		auto& t    = view.get<TransformComponent>(e);
		float target_x = fp_to_float(lp.x) + fp_to_float(rect.width) * 0.5f;
		float target_y = fp_to_float(lp.y) + fp_to_float(rect.height) * 0.5f;
		// 玩家/敌人/子弹统一硬对齐逻辑位置：
		// 指数平滑的稳态滞后与速度成正比（τ=0.1s 时：玩家 5m/s→滞后 0.5m、玩家子弹 9m/s→0.9m），
		// 滞后过大会让视觉位置与命中判定脱节。直接对齐后，20Hz 逻辑 tick 下渲染 60fps
		// 每帧最多移动约 0.15m，视觉依旧连续。
		t.position.x = target_x;
		t.position.y = target_y;
	}
}

#endif
