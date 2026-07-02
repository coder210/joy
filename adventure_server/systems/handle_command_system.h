#ifndef HANDLE_COMMAND_SYSTEM_H
#define HANDLE_COMMAND_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../components/net_component.h"



//static const fp_t MOVE_SPEED = fp_from_float(5.0f);


static void attack_target(Context* ctx, LogicPositionComponent* p,
    LogicRectComponent& currRect, IdComponent& currId, int dir)
{
    // 攻击判定：向攻击方向偏移的碰撞盒（0.6m → 偏移 0.7m）
    fp_t atk_range = fp_from_float(0.7f);
    fp_t ox = 0, oy = 0;
    if (dir == 0)      oy = atk_range;
    else if (dir == 1) ox = -atk_range;
    else if (dir == 2) ox = atk_range;
    else if (dir == 3) oy = -atk_range;
    rectanglef_t atk_rect = { fp_add(p->x, ox), fp_add(p->y, oy), currRect.width, currRect.height };

    ctx->body_query.each([&](flecs::entity e, IdComponent& id,
        LogicRectComponent& r, LogicPositionComponent& pos) {
            if (currId.id == id.id) return;
            if (!e.has<PlayerComponent>()) return; // 只攻击玩家
            rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
            contact2df_t contact;
            if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
                if (id.hp > 1) id.hp--;
            }
        });
    // 射线攻击已注释
    /*
    ray2df_t ray;
    ...
    collision2df_get_ray_rectangle(ray, rect);
    ...
    */
}


static void resolve_collision(Context* ctx, LogicPositionComponent* p,
    LogicRectComponent& currRect, IdComponent& currId)
{
    ctx->body_query.each([&](IdComponent& other_id,
        LogicRectComponent& r,
        LogicPositionComponent& other_pos) {
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
        });
}

static void update_ai(Context* ctx)
{
    ctx->enemy_query.each([&](flecs::entity e, EnemyComponent&, AIComponent& ai,
        IdComponent& id, LogicRectComponent& rect, LogicPositionComponent& pos) {

            fp_t speed = fp_from_float(2.0f);
            fp_t step = speed;

            // 查找检测范围内最近玩家
            fp_t detect_sq = fp_mul(ai.detect_radius, ai.detect_radius);
            fp_t nearest_dist_sq = detect_sq;
            fp_t nearest_player_x = 0, nearest_player_y = 0;
            bool found = false;

            ctx->player_query.each([&](PlayerComponent&, IdComponent&,
                LogicRectComponent&, LogicPositionComponent& pp) {
                    fp_t dist_sq = collision2df_get_squared_distance_point_circle(
                        vec2f_t{ pp.x, pp.y }, circlef_t{ {pos.x, pos.y}, ai.detect_radius });
                    if (dist_sq < nearest_dist_sq) {
                        nearest_dist_sq = dist_sq;
                        nearest_player_x = pp.x;
                        nearest_player_y = pp.y;
                        found = true;
                    }
                });

            if (found) {
                // 检测到玩家
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
                        ai.state = 2; // 攻击
                        attack_target(ctx, &pos, rect, id, atk_dir);
                        if (e.has<AnimParams>()) {
                            auto* p = e.get_mut<AnimParams>();
                            p->attack = true;
                            p->dir = atk_dir;
                        }
                    }
                }
                else {
                    ai.state = 1; // 追击
                    if (e.has<PlayerActionComponent>()) {
                        auto* act = e.get_mut<PlayerActionComponent>();
                        act->state = PlayerActionComponent::WALK;
                        if (e.has<AnimParams>()) {
                            e.get_mut<AnimParams>()->speed = 1.0f;
                        }
                    }
                    fp_t dx = fp_sub(nearest_player_x, pos.x);
                    fp_t dy = fp_sub(nearest_player_y, pos.y);
                    fp_t dist = fp_sqrt(fp_add(fp_mul(dx, dx), fp_mul(dy, dy)));
                    if (dist > fp_zero()) {
                        pos.x = fp_add(pos.x, fp_mul(fp_div(dx, dist), step));
                        pos.y = fp_add(pos.y, fp_mul(fp_div(dy, dist), step));
                        resolve_collision(ctx, &pos, rect, id);
                    }
                }
            }
            else {
                // 巡逻
                ai.state = 0;
                fp_t dx = fp_sub(ai.target_x, pos.x);
                fp_t dy = fp_sub(ai.target_y, pos.y);
                fp_t dist_sq = fp_add(fp_mul(dx, dx), fp_mul(dy, dy));
                fp_t arrived = fp_from_float(0.5f);
                if (dist_sq < fp_mul(arrived, arrived)) {
                    // 到达巡逻点 → idle
                    if (e.has<PlayerActionComponent>()) {
                        e.get_mut<PlayerActionComponent>()->state = PlayerActionComponent::IDLE;
                        if (e.has<AnimParams>()) e.get_mut<AnimParams>()->speed = 0;
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
                    // 向巡逻点移动 → walk
                    if (e.has<PlayerActionComponent>()) {
                        e.get_mut<PlayerActionComponent>()->state = PlayerActionComponent::WALK;
                        if (e.has<AnimParams>()) e.get_mut<AnimParams>()->speed = 1.0f;
                    }
                    fp_t dist = fp_sqrt(dist_sq);
                    pos.x = fp_add(pos.x, fp_mul(fp_div(dx, dist), step));
                    pos.y = fp_add(pos.y, fp_mul(fp_div(dy, dist), step));
                    resolve_collision(ctx, &pos, rect, id);
                }
            }
        });
}


static void calc_move_step(Context* ctx, fp_t* out_x, fp_t* out_y, int input)
{
    //fp_t delta = fp_from_float(ctx->FIXED_TIMESTEP);
    //fp_t step = fp_mul(MOVE_SPEED, delta);
    fp_t step = MOVE_SPEED;

    *out_x = fp_zero();
    *out_y = fp_zero();

    if (input & INPUT_UP)    *out_y = fp_sub(*out_y, step);
    if (input & INPUT_DOWN)  *out_y = fp_add(*out_y, step);
    if (input & INPUT_LEFT)  *out_x = fp_sub(*out_x, step);
    if (input & INPUT_RIGHT) *out_x = fp_add(*out_x, step);
}



static void apply_input(Context* ctx,
    LogicPositionComponent* p, LogicRectComponent& currRect,
    IdComponent& currId, int conv, int input)
{
    if (input & INPUT_ATTACK) {
        // 攻击判定：向攻击方向偏移的碰撞盒
        int dir = input_to_dir(input);
        fp_t atk_range = fp_from_float(0.7f);
        fp_t ox = 0, oy = 0;
        if (dir == 0)      oy = atk_range;
        else if (dir == 1) ox = -atk_range;
        else if (dir == 2) ox = atk_range;
        else if (dir == 3) oy = -atk_range;
        rectanglef_t atk_rect = { fp_add(p->x, ox), fp_add(p->y, oy), currRect.width, currRect.height };
        ctx->body_query.each([&](flecs::entity e, IdComponent& id, LogicRectComponent& r, LogicPositionComponent& pos) {
            if (currId.id == id.id) return;
            if (!e.has<EnemyComponent>()) return; // 玩家攻击只伤敌人
            rectanglef_t other_rect = { pos.x, pos.y, r.width, r.height };
            contact2df_t contact;
            if (collision2df_get_rectangles(atk_rect, fp_zero(), other_rect, fp_zero(), &contact)) {
                id.hp--;
            }
            });
        // 射线攻击已注释
        /*
        ray2df_t ray;
        ...
        */
    }

    fp_t move_x, move_y;
    calc_move_step(ctx, &move_x, &move_y, input & (INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT));

    p->x = fp_add(p->x, move_x);
    p->y = fp_add(p->y, move_y);

    resolve_collision(ctx, p, currRect, currId);
}


static void handle_command_system(flecs::iter& it)
{
    Context* ctx = (Context*)it.world().get_ctx();
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

        //flecs::entity anim_def = create_warrior_anim_def(ctx->ecs,
            //"adventure_server/textures/knights/troops/warrior/warrior_blue.png");
        ctx->ecs.entity()
            .set<IdComponent>({ get_entity_id(ctx), 10 })
            .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
            .set<LogicPositionComponent>({ x, y })
            .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
            .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 1, 1 })
            .set<PlayerComponent>({ pj.conv })
            .set<PlayerActionComponent>({});
            //.set<Animator>({ anim_def })
            //.set<AnimParams>({});
    }

    // 玩家离开（defer 延迟销毁，避免迭代中删除实体）
    ctx->ecs.defer_begin();
    for (int i = 0; i < ln; i++) {
        int conv = leaves[i].conv;
        ctx->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent&, LogicRectComponent&, LogicPositionComponent&) {
            if (p.conv == conv) e.destruct();
            });
    }
    ctx->ecs.defer_end();

    // 应用输入
    for (int i = 0; i < in; i++) {
        auto& input = inputs[i];
        int dir = input_to_dir(input.dir);
        bool moving = has_dir(input.dir);
        int input_combined = input.dir | (input.attack ? INPUT_ATTACK : 0);

        ctx->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
            LogicRectComponent& r, LogicPositionComponent& pos) {
                if (p.conv != input.conv) return;
                if (e.has<PlayerActionComponent>()) {
                    auto* act = e.get_mut<PlayerActionComponent>();
                    auto* params = e.has<AnimParams>() ? e.get_mut<AnimParams>() : nullptr;
                    if (input.attack) {
                        act->state = PlayerActionComponent::ATTACK;
                        act->dir = dir;
                        if (params) { params->attack = true; params->dir = dir; }
                    }
                    else {
                        act->state = moving ? PlayerActionComponent::WALK : PlayerActionComponent::IDLE;
                        act->dir = dir;
                        if (params) { params->speed = moving ? 1.0f : 0.0f; params->dir = dir; }
                    }
                }
                apply_input(ctx, &pos, r, id, input.conv, input_combined);
            });
    }

    //UpdateAI(ctx);
    ctx->command_queue.pop();
}


#endif
