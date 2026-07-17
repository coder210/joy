#ifndef HANDLE_COMMAND_SYSTEM_H
#define HANDLE_COMMAND_SYSTEM_H
#include "../context.h"
#include "../components/net_component.h"

//static const fp_t MOVE_SPEED = fp_from_float(5.0f);

static void attack_target(Context* ctx, entt::registry& registry, LogicPositionComponent* p,
    LogicRectComponent& currRect, IdComponent& currId, int dir)
{
    fp_t atk_range = fp_from_float(0.7f);
    fp_t ox = 0, oy = 0;
    if (dir == 0)      oy = atk_range;
    else if (dir == 1) ox = -atk_range;
    else if (dir == 2) ox = atk_range;
    else if (dir == 3) oy = -atk_range;
    rectanglef_t atk_rect = { fp_add(p->x, ox), fp_add(p->y, oy), currRect.width, currRect.height };

    auto body_view = registry.view<IdComponent, LogicRectComponent, LogicPositionComponent>();
    for (auto e : body_view) {
        auto& id = body_view.get<IdComponent>(e);
        auto& r = body_view.get<LogicRectComponent>(e);
        auto& pos = body_view.get<LogicPositionComponent>(e);
        if (currId.id == id.id) continue;
        if (!registry.all_of<PlayerComponent>(e)) continue; // 只攻击玩家
        rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
        contact2df_t contact;
        if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
        }
    }
}

static void resolve_collision(Context* ctx, entt::registry& registry, LogicPositionComponent* p,
    LogicRectComponent& currRect, IdComponent& currId)
{
    auto body_view = registry.view<IdComponent, LogicRectComponent, LogicPositionComponent>();
    for (auto e : body_view) {
        auto& other_id = body_view.get<IdComponent>(e);
        auto& r = body_view.get<LogicRectComponent>(e);
        auto& other_pos = body_view.get<LogicPositionComponent>(e);
        if (other_id.id == currId.id) return;

        rectanglef_t curr_rect = { p->x, p->y, currRect.width, currRect.height };
        rectanglef_t other_rect = { other_pos.x, other_pos.y, r.width, r.height };

        contact2df_t contact;
        if (collision2df_get_rectangles(curr_rect, fp_zero(),
            other_rect, fp_zero(), &contact)) {
            fp_t depth = contact.depth;
            if (depth < fp_zero()) {
                depth = fp_sub(fp_zero(), depth);
            }
            fp_t nx = fp_sub(fp_zero(), contact.normal.x);
            fp_t ny = fp_sub(fp_zero(), contact.normal.y);
            p->x = fp_add(p->x, fp_mul(nx, depth));
            p->y = fp_add(p->y, fp_mul(ny, depth));
        }
    }
}

static void update_ai(Context* ctx, entt::registry& registry)
{
    auto enemy_view = registry.view<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
    auto player_view = registry.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
    for (auto e : enemy_view) {
        auto& ai = enemy_view.get<AIComponent>(e);
        auto& id = enemy_view.get<IdComponent>(e);
        auto& rect = enemy_view.get<LogicRectComponent>(e);
        auto& pos = enemy_view.get<LogicPositionComponent>(e);

        fp_t speed = fp_from_float(2.0f);
        fp_t step = speed;

        fp_t detect_sq = fp_mul(ai.detect_radius, ai.detect_radius);
        fp_t nearest_dist_sq = detect_sq;
        fp_t nearest_player_x = 0, nearest_player_y = 0;
        bool found = false;

        for (auto pe : player_view) {
            auto& pp = player_view.get<LogicPositionComponent>(pe);
            fp_t dist_sq = collision2df_get_squared_distance_point_circle(
                vec2f_t{ pp.x, pp.y }, circlef_t{ {pos.x, pos.y}, ai.detect_radius });
            if (dist_sq < nearest_dist_sq) {
                nearest_dist_sq = dist_sq;
                nearest_player_x = pp.x;
                nearest_player_y = pp.y;
                found = true;
            }
        }

        if (found) {
            int atk_dir = 0;
            fp_t dx = fp_sub(nearest_player_x, pos.x);
            fp_t dy = fp_sub(nearest_player_y, pos.y);
            bool h = (dx > 0 ? dx : -dx) > (dy > 0 ? dy : -dy);
            atk_dir = h ? (dx > 0 ? 2 : 1) : (dy > 0 ? 0 : 3);
            fp_t attack_sq = fp_mul(ai.attack_radius, ai.attack_radius);
            if (nearest_dist_sq < attack_sq) {
                ai.timer++;
                if (ai.timer >= 60) {
                    ai.timer = 0;
                    ai.state = 2;
                    attack_target(ctx, registry, &pos, rect, id, atk_dir);
                    if (registry.all_of<AnimParams>(e)) {
                        auto& p = registry.get<AnimParams>(e);
                        p.attack = true;
                        p.dir = atk_dir;
                    }
                }
            }
            else {
                ai.state = 1;
                if (registry.all_of<PlayerActionComponent>(e)) {
                    auto& act = registry.get<PlayerActionComponent>(e);
                    act.state = PlayerActionComponent::WALK;
                    if (registry.all_of<AnimParams>(e)) {
                        registry.get<AnimParams>(e).speed = 1.0f;
                    }
                }
                fp_t dx = fp_sub(nearest_player_x, pos.x);
                fp_t dy = fp_sub(nearest_player_y, pos.y);
                fp_t dist = fp_sqrt(fp_add(fp_mul(dx, dx), fp_mul(dy, dy)));
                if (dist > fp_zero()) {
                    pos.x = fp_add(pos.x, fp_mul(fp_div(dx, dist), step));
                    pos.y = fp_add(pos.y, fp_mul(fp_div(dy, dist), step));
                    resolve_collision(ctx, registry, &pos, rect, id);
                }
            }
        }
        else {
            ai.state = 0;
            fp_t dx = fp_sub(ai.target_x, pos.x);
            fp_t dy = fp_sub(ai.target_y, pos.y);
            fp_t dist_sq = fp_add(fp_mul(dx, dx), fp_mul(dy, dy));
            fp_t arrived = fp_from_float(0.5f);
            if (dist_sq < fp_mul(arrived, arrived)) {
                if (registry.all_of<PlayerActionComponent>(e)) {
                    registry.get<PlayerActionComponent>(e).state = PlayerActionComponent::IDLE;
                    if (registry.all_of<AnimParams>(e)) registry.get<AnimParams>(e).speed = 0;
                }
                fp_t range = ai.patrol_radius;
                ai.target_x = fp_add(ai.patrol_center_x,
                    fp_mul(fp_from_float((float)(rand() % 100 - 50)),
                        fp_div(range, fp_from_float(50.0f))));
                ai.target_y = fp_add(ai.patrol_center_y,
                    fp_mul(fp_from_float((float)(rand() % 100 - 50)),
                        fp_div(range, fp_from_float(50.0f))));
            }
            else {
                if (registry.all_of<PlayerActionComponent>(e)) {
                    registry.get<PlayerActionComponent>(e).state = PlayerActionComponent::WALK;
                    if (registry.all_of<AnimParams>(e)) registry.get<AnimParams>(e).speed = 1.0f;
                }
                fp_t dist = fp_sqrt(dist_sq);
                pos.x = fp_add(pos.x, fp_mul(fp_div(dx, dist), step));
                pos.y = fp_add(pos.y, fp_mul(fp_div(dy, dist), step));
                resolve_collision(ctx, registry, &pos, rect, id);
            }
        }
    }
}

static void calc_move_step(Context* ctx, fp_t* out_x, fp_t* out_y, int input)
{
    fp_t step = MOVE_SPEED;
    *out_x = fp_zero();
    *out_y = fp_zero();
    if (input & INPUT_UP)    *out_y = fp_sub(*out_y, step);
    if (input & INPUT_DOWN)  *out_y = fp_add(*out_y, step);
    if (input & INPUT_LEFT)  *out_x = fp_sub(*out_x, step);
    if (input & INPUT_RIGHT) *out_x = fp_add(*out_x, step);
}

static void apply_input(Context* ctx, entt::registry& registry,
    LogicPositionComponent* p, LogicRectComponent& currRect,
    IdComponent& currId, int conv, int input)
{
    if (input & INPUT_ATTACK) {
        int dir = input_to_dir(input);
        fp_t atk_range = fp_from_float(0.7f);
        fp_t ox = 0, oy = 0;
        if (dir == 0)      oy = atk_range;
        else if (dir == 1) ox = -atk_range;
        else if (dir == 2) ox = atk_range;
        else if (dir == 3) oy = -atk_range;
        rectanglef_t atk_rect = { fp_add(p->x, ox), fp_add(p->y, oy), currRect.width, currRect.height };
        auto body_view = registry.view<IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : body_view) {
            auto& id = body_view.get<IdComponent>(e);
            auto& r = body_view.get<LogicRectComponent>(e);
            auto& pos = body_view.get<LogicPositionComponent>(e);
            if (currId.id == id.id) return;
            if (!registry.all_of<EnemyComponent>(e)) return;
            rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
            contact2df_t contact;
            if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
            }
        }
    }

    fp_t move_x, move_y;
    calc_move_step(ctx, &move_x, &move_y, input & (INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT));
    p->x = fp_add(p->x, move_x);
    p->y = fp_add(p->y, move_y);
    resolve_collision(ctx, registry, p, currRect, currId);
}

static void handle_command_system(Context* ctx, entt::registry& registry)
{
    if (ctx->command_queue.empty()) return;

    std::string command_data = ctx->command_queue.front();
    int frame_id, jn = 0, ln = 0, in = 0;
    S2CPlayerJoin joins[32];
    S2CPlayerLeave leaves[32];
    S2CPlayerInput inputs[64];
    if (!unpack_command(command_data.c_str(), (int)command_data.size(), &frame_id,
        joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in)) {
        log_info("Failed to parse command");
        ctx->command_queue.pop(); return;
    }

    for (int i = 0; i < jn; i++) {
        auto& pj = joins[i];
        log_info("%d:CMD_PLAYER_JOIN", pj.conv);
        fp_t x = pj.position_x;
        fp_t y = pj.position_y;

        entt::entity entity = registry.create();
        registry.emplace<IdComponent>(entity, get_entity_id(ctx));
        registry.emplace<LogicRectComponent>(entity, fp_from_float(.6f), fp_from_float(.6f));
        registry.emplace<LogicPositionComponent>(entity, x, y);
        registry.emplace<LogicVelocityComponent>(entity, fp_from_float(0), fp_from_float(0));
        registry.emplace<TransformComponent>(entity, fp_to_float(x), fp_to_float(y), 0.0f, 1.0f, 1.0f);
        registry.emplace<PlayerComponent>(entity, pj.conv);
        registry.emplace<PlayerActionComponent>(entity);
    }

    // 玩家离开：先收集再统一销毁（替代 defer）
    std::vector<entt::entity> to_destroy;
    for (int i = 0; i < ln; i++) {
        int conv = leaves[i].conv;
        auto player_view = registry.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : player_view) {
            auto& p = player_view.get<PlayerComponent>(e);
            if (p.conv == conv) to_destroy.push_back(e);
        }
    }
    for (auto e : to_destroy) registry.destroy(e);

    // 应用输入
    for (int i = 0; i < in; i++) {
        auto& input = inputs[i];
        int dir = input_to_dir(input.dir);
        bool moving = has_dir(input.dir);
        int input_combined = input.dir | (input.attack ? INPUT_ATTACK : 0);

        auto player_view = registry.view<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        for (auto e : player_view) {
            auto& p = player_view.get<PlayerComponent>(e);
            auto& id = player_view.get<IdComponent>(e);
            auto& r = player_view.get<LogicRectComponent>(e);
            auto& pos = player_view.get<LogicPositionComponent>(e);
            if (p.conv != input.conv) continue;
            if (registry.all_of<PlayerActionComponent>(e)) {
                auto& act = registry.get<PlayerActionComponent>(e);
                auto* params = registry.all_of<AnimParams>(e) ? &registry.get<AnimParams>(e) : nullptr;
                if (input.attack) {
                    act.state = PlayerActionComponent::ATTACK;
                    act.dir = dir;
                    if (params) { params->attack = true; params->dir = dir; }
                }
                else {
                    act.state = moving ? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
                    act.dir = dir;
                    if (params) { params->speed = moving ? 1.0f : 0.0f; params->dir = dir; }
                }
            }
            apply_input(ctx, registry, &pos, r, id, input.conv, input_combined);
        }
    }

    //UpdateAI(ctx);
    ctx->command_queue.pop();
}

#endif
