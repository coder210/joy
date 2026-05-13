#include "startup_system.h"
#include "../Context.h"
#include "../Resources.h"
#include "../debug_layer.h"
#include "../components/id_component.h"
#include "../components/logic_rect_component.h"
#include "../components/logic_position_component.h"
#include "../components/player_component.h"
#include "../components/connection_component.h"

void StartupSystem(flecs::world& world)
{
	auto ctx = world.get_mut<Context>();
	adventure::S2CWorld s2c_world;
	ctx->body_query = world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();
	ctx->connection_query = world.query<ConnectionComponent>();
	ctx->player_query = world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
	ctx->body_query.each([&](flecs::entity e, IdComponent& id,
		LogicRectComponent& r, LogicPositionComponent& pos) {
		adventure::S2CEntity* ent = s2c_world.add_entities();
		ent->set_id(id.id);
		if (e.has<PlayerComponent>()) {
			ent->set_type(adventure::S2C_TYPE_PLAYER);
			ent->set_player_conv(e.get_mut<PlayerComponent>()->conv);
		}
		else {
			ent->set_type(adventure::S2C_TYPE_NORMAL);
		}
		ent->set_hp(id.hp);
		ent->set_position_x(pos.x);
		ent->set_position_y(pos.y);
	});

	ctx->worlds[ctx->g_frameid] = s2c_world.SerializeAsString();

	ctx->resources = new Resources(ctx->renderer);
	ctx->debugLayer = new DebugLayer(ctx->resources);
}
