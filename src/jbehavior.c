/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jbehavior.c
Description: 行为树 (Behavior Tree) 核心实现
Author: ydlc
Version: 1.0
Date: 2021.3.18
History:
*************************************************/
#include "joy.h"

/* =================================================================
 * 基础生命周期
 * ================================================================= */
joy_bt_status_t joy_bt_node_run(joy_bt_node_t* node, float dt) {
    if (!node) return BT_FAILURE;
    if (node->status != BT_RUNNING) {
        if (node->enter) node->enter(node);
    }
    node->status = node->tick ? node->tick(node, dt) : BT_SUCCESS;
    if (node->status != BT_RUNNING) {
        if (node->exit) node->exit(node);
    }
    return node->status;
}

void joy_bt_node_reset(joy_bt_node_t* node) {
    if (node) node->status = BT_FAILURE;
}

void joy_bt_node_destroy(joy_bt_node_t* node) {
    if (!node) return;
    if (node->destroy) node->destroy(node);
    free(node);
}

static void stop_child(joy_bt_node_t* child) {
    if (child && child->status == BT_RUNNING) {
        if (child->exit) child->exit(child);
        child->status = BT_FAILURE;
    }
}

/* =================================================================
 * Action 节点
 * ================================================================= */
typedef struct {
    joy_bt_node_t base;
    joy_bt_action_fn fn;
    void* userdata;
} joy_bt_action_t;

static joy_bt_status_t action_tick(joy_bt_node_t* self, float dt) {
    joy_bt_action_t* a = (joy_bt_action_t*)self;
    return a->fn ? a->fn(dt) : BT_SUCCESS;
}

joy_bt_node_t* joy_bt_action_create(joy_bt_action_fn fn) {
    joy_bt_action_t* a = (joy_bt_action_t*)calloc(1, sizeof(joy_bt_action_t));
    assert(a);
    a->base.tick = action_tick;
    a->fn = fn;
    return (joy_bt_node_t*)a;
}

joy_bt_node_t* joy_bt_action_createv(joy_bt_action_fn fn, void* userdata) {
    joy_bt_action_t* a = (joy_bt_action_t*)joy_bt_action_create(fn);
    a->userdata = userdata;
    return (joy_bt_node_t*)a;
}

void joy_bt_action_set_userdata(joy_bt_node_t* node, void* userdata) {
    if (node) ((joy_bt_action_t*)node)->userdata = userdata;
}

void* joy_bt_action_get_userdata(joy_bt_node_t* node) {
    if (node) return ((joy_bt_action_t*)node)->userdata;
    return NULL;
}

/* =================================================================
 * Condition 节点
 * ================================================================= */
typedef struct {
    joy_bt_node_t base;
    joy_bt_condition_fn pred;
} joy_bt_condition_t;

static joy_bt_status_t condition_tick(joy_bt_node_t* self, float dt) {
    (void)dt;
    joy_bt_condition_t* c = (joy_bt_condition_t*)self;
    return (c->pred && c->pred()) ? BT_SUCCESS : BT_FAILURE;
}

joy_bt_node_t* joy_bt_condition_create(joy_bt_condition_fn pred) {
    joy_bt_condition_t* c = (joy_bt_condition_t*)calloc(1, sizeof(joy_bt_condition_t));
    assert(c);
    c->base.tick = condition_tick;
    c->pred = pred;
    return (joy_bt_node_t*)c;
}

/* =================================================================
 * Wait 节点
 * ================================================================= */
typedef struct {
    joy_bt_node_t base;
    float seconds;
    float elapsed;
} joy_bt_wait_t;

static void wait_enter(joy_bt_node_t* self) {
    ((joy_bt_wait_t*)self)->elapsed = 0;
}

static joy_bt_status_t wait_tick(joy_bt_node_t* self, float dt) {
    joy_bt_wait_t* w = (joy_bt_wait_t*)self;
    w->elapsed += dt;
    return w->elapsed >= w->seconds ? BT_SUCCESS : BT_RUNNING;
}

joy_bt_node_t* joy_bt_wait_create(float seconds) {
    joy_bt_wait_t* w = (joy_bt_wait_t*)calloc(1, sizeof(joy_bt_wait_t));
    assert(w);
    w->base.enter = wait_enter;
    w->base.tick  = wait_tick;
    w->seconds = seconds;
    return (joy_bt_node_t*)w;
}

/* =================================================================
 * Sequence / Selector
 * ================================================================= */
typedef enum { COMBO_SEQUENCE, COMBO_SELECTOR } combo_type_t;

typedef struct {
    joy_bt_node_t   base;
    combo_type_t type;
    joy_bt_node_t** children;
    int         count;
    int         index;
} joy_bt_combo_t;

static void combo_enter(joy_bt_node_t* self) {
    joy_bt_combo_t* c = (joy_bt_combo_t*)self;
    if (c->index > 0 && c->index <= c->count)
        stop_child(c->children[c->index - 1]);
    c->index = 1;
}

static joy_bt_status_t combo_tick(joy_bt_node_t* self, float dt) {
    joy_bt_combo_t* c = (joy_bt_combo_t*)self;
    if (!c->children || c->count == 0)
        return (c->type == COMBO_SEQUENCE) ? BT_SUCCESS : BT_FAILURE;

    if (c->index < 1) c->index = 1;

    while (c->index <= c->count) {
        joy_bt_node_t* child = c->children[c->index - 1];
        joy_bt_status_t s = joy_bt_node_run(child, dt);
        if (s == BT_RUNNING) return BT_RUNNING;

        if (c->type == COMBO_SEQUENCE) {
            if (s == BT_FAILURE) { stop_child(child); c->index = 1; return BT_FAILURE; }
        } else {
            if (s == BT_SUCCESS) { stop_child(child); c->index = 1; return BT_SUCCESS; }
        }
        c->index++;
    }

    joy_bt_status_t result = (c->type == COMBO_SEQUENCE) ? BT_SUCCESS : BT_FAILURE;
    c->index = 1;
    return result;
}

static void combo_exit(joy_bt_node_t* self) {
    joy_bt_combo_t* c = (joy_bt_combo_t*)self;
    if (c->index > 0 && c->index <= c->count)
        stop_child(c->children[c->index - 1]);
}

static void combo_destroy(joy_bt_node_t* self) {
    joy_bt_combo_t* c = (joy_bt_combo_t*)self;
    if (c->children) {
        free(c->children);  /* 只释放数组，不释放子节点（由 Lua GC 管理） */
    }
}

static joy_bt_node_t* combo_create(combo_type_t type, joy_bt_node_t** children, int count) {
    joy_bt_combo_t* c = (joy_bt_combo_t*)calloc(1, sizeof(joy_bt_combo_t));
    assert(c);
    c->base.enter   = combo_enter;
    c->base.tick    = combo_tick;
    c->base.exit    = combo_exit;
    c->base.destroy = combo_destroy;
    c->type = type;
    c->count = count;
    if (count > 0) {
        c->children = (joy_bt_node_t**)malloc(sizeof(joy_bt_node_t*) * count);
        assert(c->children);
        memcpy(c->children, children, sizeof(joy_bt_node_t*) * count);
    }
    return (joy_bt_node_t*)c;
}

joy_bt_node_t* joy_bt_sequence_create(joy_bt_node_t** children, int count) {
    return combo_create(COMBO_SEQUENCE, children, count);
}

joy_bt_node_t* joy_bt_selector_create(joy_bt_node_t** children, int count) {
    return combo_create(COMBO_SELECTOR, children, count);
}

/* =================================================================
 * Parallel
 * ================================================================= */
typedef struct {
    joy_bt_node_t   base;
    joy_bt_node_t** children;
    int         count;
    int         success_count;
} joy_bt_parallel_t;

static joy_bt_status_t parallel_tick(joy_bt_node_t* self, float dt) {
    joy_bt_parallel_t* p = (joy_bt_parallel_t*)self;
    int suc = 0, fail = 0;
    for (int i = 0; i < p->count; i++) {
        joy_bt_status_t s = joy_bt_node_run(p->children[i], dt);
        if (s == BT_SUCCESS) suc++;
        else if (s == BT_FAILURE) fail++;
    }
    if (suc >= p->success_count) return BT_SUCCESS;
    if (fail > p->count - p->success_count) return BT_FAILURE;
    return BT_RUNNING;
}

static void parallel_exit(joy_bt_node_t* self) {
    joy_bt_parallel_t* p = (joy_bt_parallel_t*)self;
    for (int i = 0; i < p->count; i++) stop_child(p->children[i]);
}

static void parallel_destroy(joy_bt_node_t* self) {
    joy_bt_parallel_t* p = (joy_bt_parallel_t*)self;
    if (p->children) {
        free(p->children);
    }
}

joy_bt_node_t* joy_bt_parallel_create(joy_bt_node_t** children, int count, int success_count) {
    joy_bt_parallel_t* p = (joy_bt_parallel_t*)calloc(1, sizeof(joy_bt_parallel_t));
    assert(p);
    p->base.tick    = parallel_tick;
    p->base.exit    = parallel_exit;
    p->base.destroy = parallel_destroy;
    p->count = count;
    p->success_count = success_count > 0 ? success_count : count;
    if (count > 0) {
        p->children = (joy_bt_node_t**)malloc(sizeof(joy_bt_node_t*) * count);
        assert(p->children);
        memcpy(p->children, children, sizeof(joy_bt_node_t*) * count);
    }
    return (joy_bt_node_t*)p;
}

/* =================================================================
 * Inverter / Succeeder (单子装饰节点)
 * ================================================================= */
typedef struct {
    joy_bt_node_t base;
    joy_bt_node_t* child;
} joy_bt_decorator1_t;

static joy_bt_status_t inverter_tick(joy_bt_node_t* self, float dt) {
    joy_bt_status_t s = joy_bt_node_run(((joy_bt_decorator1_t*)self)->child, dt);
    if (s == BT_SUCCESS) return BT_FAILURE;
    if (s == BT_FAILURE) return BT_SUCCESS;
    return BT_RUNNING;
}

static joy_bt_status_t succeeder_tick(joy_bt_node_t* self, float dt) {
    joy_bt_node_run(((joy_bt_decorator1_t*)self)->child, dt);
    return BT_SUCCESS;
}

static void decorator1_exit(joy_bt_node_t* self) {
    stop_child(((joy_bt_decorator1_t*)self)->child);
}

static void decorator1_destroy(joy_bt_node_t* self) {
    (void)self;
    /* 不释放子节点，由 Lua GC 管理 */
}

static joy_bt_node_t* decorator1_create(joy_bt_status_t (*tick)(joy_bt_node_t*, float)) {
    joy_bt_decorator1_t* d = (joy_bt_decorator1_t*)calloc(1, sizeof(joy_bt_decorator1_t));
    assert(d);
    d->base.tick    = tick;
    d->base.exit    = decorator1_exit;
    d->base.destroy = decorator1_destroy;
    return (joy_bt_node_t*)d;
}

joy_bt_node_t* bt_inverter_create(joy_bt_node_t* child) {
    joy_bt_node_t* n = decorator1_create(inverter_tick);
    ((joy_bt_decorator1_t*)n)->child = child;
    return n;
}

joy_bt_node_t* bt_succeeder_create(joy_bt_node_t* child) {
    joy_bt_node_t* n = decorator1_create(succeeder_tick);
    ((joy_bt_decorator1_t*)n)->child = child;
    return n;
}

/* =================================================================
 * Repeater
 * ================================================================= */
typedef struct {
    joy_bt_node_t base;
    joy_bt_node_t* child;
    int times;
    int count;
} joy_bt_repeater_t;

static void repeater_enter(joy_bt_node_t* self) {
    ((joy_bt_repeater_t*)self)->count = 0;
}

static joy_bt_status_t repeater_tick(joy_bt_node_t* self, float dt) {
    joy_bt_repeater_t* r = (joy_bt_repeater_t*)self;
    joy_bt_status_t s = joy_bt_node_run(r->child, dt);
    if (s == BT_RUNNING) return BT_RUNNING;
    r->count++;
    if (r->times > 0 && r->count >= r->times) {
        stop_child(r->child);
        return BT_SUCCESS;
    }
    stop_child(r->child);
    joy_bt_node_reset(r->child);
    return BT_RUNNING;
}

static void repeater_exit(joy_bt_node_t* self) {
    stop_child(((joy_bt_repeater_t*)self)->child);
}

static void repeater_destroy(joy_bt_node_t* self) {
    (void)self;
    /* 不释放子节点，由 Lua GC 管理 */
}

joy_bt_node_t* bt_repeater_create(joy_bt_node_t* child, int times) {
    joy_bt_repeater_t* r = (joy_bt_repeater_t*)calloc(1, sizeof(joy_bt_repeater_t));
    assert(r);
    r->base.enter   = repeater_enter;
    r->base.tick    = repeater_tick;
    r->base.exit    = repeater_exit;
    r->base.destroy = repeater_destroy;
    r->child = child;
    r->times = times;
    return (joy_bt_node_t*)r;
}

/* =================================================================
 * StateMachine
 * ================================================================= */
typedef struct {
    joy_bt_node_t        base;
    joy_bt_state_slot_t* states;
    int              count;
    int              current;
    int              prev;
} joy_bt_statemachine_t;

static void sm_enter(joy_bt_node_t* self) {
    joy_bt_statemachine_t* sm = (joy_bt_statemachine_t*)self;
    if (sm->count > 0 && sm->current >= 0 && sm->current < sm->count) {
        if (sm->states[sm->current].state_enter)
            sm->states[sm->current].state_enter(self);
    }
}

static joy_bt_status_t sm_tick(joy_bt_node_t* self, float dt) {
    joy_bt_statemachine_t* sm = (joy_bt_statemachine_t*)self;
    if (sm->current < 0) return BT_FAILURE;

    joy_bt_state_slot_t* cur = &sm->states[sm->current];
    if (cur->state_update) {
        const char* next = cur->state_update(self, dt);
        if (next) {
            int next_idx = -1;
            for (int i = 0; i < sm->count; i++) {
                if (strcmp(sm->states[i].name, next) == 0) { next_idx = i; break; }
            }
            if (next_idx >= 0 && next_idx != sm->current) {
                if (cur->state_exit) cur->state_exit(self);
                sm->prev = sm->current;
                sm->current = next_idx;
                cur = &sm->states[sm->current];
                if (cur->state_enter) cur->state_enter(self);
            }
        }
    }
    if (sm->current < 0) return BT_FAILURE;
    return BT_RUNNING;
}

static void sm_exit(joy_bt_node_t* self) {
    joy_bt_statemachine_t* sm = (joy_bt_statemachine_t*)self;
    if (sm->current >= 0 && sm->states[sm->current].state_exit)
        sm->states[sm->current].state_exit(self);
    sm->current = -1;
    sm->prev = -1;
}

static void sm_destroy(joy_bt_node_t* self) {
    joy_bt_statemachine_t* sm = (joy_bt_statemachine_t*)self;
    if (sm->states) free(sm->states);
}

joy_bt_node_t* joy_bt_statemachine_create(joy_bt_state_slot_t* states, int count, const char* initial) {
    joy_bt_statemachine_t* sm = (joy_bt_statemachine_t*)calloc(1, sizeof(joy_bt_statemachine_t));
    assert(sm);
    sm->base.enter   = sm_enter;
    sm->base.tick    = sm_tick;
    sm->base.exit    = sm_exit;
    sm->base.destroy = sm_destroy;
    sm->states = (joy_bt_state_slot_t*)malloc(sizeof(joy_bt_state_slot_t) * count);
    assert(sm->states);
    memcpy(sm->states, states, sizeof(joy_bt_state_slot_t) * count);
    sm->count = count;
    sm->current = -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(states[i].name, initial) == 0) { sm->current = i; break; }
    }
    return (joy_bt_node_t*)sm;
}

void joy_bt_statemachine_transition(joy_bt_node_t* node, const char* state_name) {
    joy_bt_statemachine_t* sm = (joy_bt_statemachine_t*)node;
    if (node->tick != sm_tick) return;
    for (int i = 0; i < sm->count; i++) {
        if (strcmp(sm->states[i].name, state_name) == 0) {
            if (sm->current >= 0 && sm->states[sm->current].state_exit)
                sm->states[sm->current].state_exit(node);
            sm->prev = sm->current;
            sm->current = i;
            if (sm->states[i].state_enter)
                sm->states[i].state_enter(node);
            return;
        }
    }
}
