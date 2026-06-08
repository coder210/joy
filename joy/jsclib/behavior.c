#include "jsclib.h"
#include "../jbehavior.h"
#include <stdlib.h>
#include <string.h>

static duk_ret_t jbt_action(duk_context* ctx) {
    (void)ctx;
    duk_push_pointer(ctx, bt_action_create(NULL));
    return 1;
}
static duk_ret_t jbt_condition(duk_context* ctx) {
    (void)ctx;
    duk_push_pointer(ctx, bt_condition_create(NULL));
    return 1;
}
static duk_ret_t jbt_wait(duk_context* ctx) {
    duk_push_pointer(ctx, bt_wait_create((float)duk_to_number(ctx, 0)));
    return 1;
}
static duk_ret_t jbt_sequence(duk_context* ctx) {
    int n = (int)duk_get_length(ctx, 0);
    bt_node_t** ch = (bt_node_t**)malloc(sizeof(bt_node_t*) * n);
    for (int i = 0; i < n; i++) {
        duk_get_prop_index(ctx, 0, (duk_uarridx_t)i);
        ch[i] = (bt_node_t*)duk_get_pointer(ctx, -1);
        duk_pop(ctx);
    }
    duk_push_pointer(ctx, bt_sequence_create(ch, n));
    free(ch);
    return 1;
}
static duk_ret_t jbt_selector(duk_context* ctx) {
    int n = (int)duk_get_length(ctx, 0);
    bt_node_t** ch = (bt_node_t**)malloc(sizeof(bt_node_t*) * n);
    for (int i = 0; i < n; i++) {
        duk_get_prop_index(ctx, 0, (duk_uarridx_t)i);
        ch[i] = (bt_node_t*)duk_get_pointer(ctx, -1);
        duk_pop(ctx);
    }
    duk_push_pointer(ctx, bt_selector_create(ch, n));
    free(ch);
    return 1;
}
static duk_ret_t jbt_parallel(duk_context* ctx) {
    int n = (int)duk_get_length(ctx, 0);
    int sc = duk_is_undefined(ctx, 1) ? n : (int)duk_to_number(ctx, 1);
    bt_node_t** ch = (bt_node_t**)malloc(sizeof(bt_node_t*) * n);
    for (int i = 0; i < n; i++) {
        duk_get_prop_index(ctx, 0, (duk_uarridx_t)i);
        ch[i] = (bt_node_t*)duk_get_pointer(ctx, -1);
        duk_pop(ctx);
    }
    duk_push_pointer(ctx, bt_parallel_create(ch, n, sc));
    free(ch);
    return 1;
}
static duk_ret_t jbt_inverter(duk_context* ctx) {
    duk_push_pointer(ctx, bt_inverter_create((bt_node_t*)duk_get_pointer(ctx, 0)));
    return 1;
}
static duk_ret_t jbt_succeeder(duk_context* ctx) {
    duk_push_pointer(ctx, bt_succeeder_create((bt_node_t*)duk_get_pointer(ctx, 0)));
    return 1;
}
static duk_ret_t jbt_repeater(duk_context* ctx) {
    bt_node_t* child = (bt_node_t*)duk_get_pointer(ctx, 0);
    int times = duk_is_undefined(ctx, 1) ? -1 : (int)duk_to_number(ctx, 1);
    duk_push_pointer(ctx, bt_repeater_create(child, times));
    return 1;
}
static duk_ret_t jbt_run(duk_context* ctx) {
    bt_node_t* node = (bt_node_t*)duk_get_pointer(ctx, 0);
    float dt = (float)duk_to_number(ctx, 1);
    const char* names[] = { "success", "failure", "running" };
    duk_push_string(ctx, names[bt_node_run(node, dt)]);
    return 1;
}
static duk_ret_t jbt_reset(duk_context* ctx) {
    bt_node_reset((bt_node_t*)duk_get_pointer(ctx, 0));
    return 0;
}

void jsopen_behavior(duk_context* ctx) {
    duk_push_object(ctx);
    #define REG(n,a) duk_push_c_function(ctx,jbt_##n,a); duk_put_prop_string(ctx,-2,#n)
    REG(action,1); REG(condition,1); REG(wait,1);
    REG(sequence,1); REG(selector,1); REG(parallel,2);
    REG(inverter,1); REG(succeeder,1); REG(repeater,2);
    REG(run,2); REG(reset,1);
    #undef REG
    duk_put_global_string(ctx, "behavior");
}
