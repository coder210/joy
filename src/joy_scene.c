/******************************************************************************
 * src/joy_scene.c —— scene 域唯一实现文件（一个域一个 .c，配 joy_scene.h）
 * 职责：场景 / 动画 / tilemap / ECS / 过程地图 / 行为树
 * 合并自：jscene.c / janimator.c / jtilemap.c / jecs.c / jprocmap.c / jbehavior.c
 * 说明：本域公开 API 声明在配套头 src/joy_scene.h，由 include/joy/japi.h 聚合包含；
 *       本文件是该域唯一的编译单元。
 *       内部符号（static / 私有宏 / typedef）保持各分片独立，未跨分片改名。
 ******************************************************************************/

#include "joy_scene.h"

/* ==================== 合并自 src/jscene.c ==================== */
/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jscene.c
Description: 场景管理实现
Author: ydlc
Version: 1.0
Date: 2026.5.5
History:
************************************************/
#include <SDL3/SDL.h>
#include "joy.h"

// ============================================================
// 节点结构体（定义在 .c 中，外部不可见）
// ============================================================

struct joy_scene_node {
    float local_x, local_y;
    float rotation;
    float scale_x, scale_y;
    float world_x, world_y;

    struct joy_scene_node* parent;
    struct joy_scene_node* first_child;
    struct joy_scene_node* next_sibling;
    int child_count;

    joy_scene_node_update_fn  on_update;
    joy_scene_node_render_fn  on_render;
    joy_scene_node_destroy_fn on_destroy;
    joy_scene_node_event_fn   on_event;

    void* userdata;
    int zorder;
    bool is_root;

    char name[JOY_MIN_BUFFER];   /* 节点名称，用于查找 */
    char tag[JOY_MIN_BUFFER];    /* 节点标签，用于分组查找 */
};

// ============================================================
// 节点实现
// ============================================================

joy_scene_node_p joy_scene_node_create(void)
{
    struct joy_scene_node* n = (struct joy_scene_node*)SDL_malloc(sizeof(struct joy_scene_node));
    SDL_assert(n);
    SDL_memset(n, 0, sizeof(struct joy_scene_node));
    n->scale_x = 1.0f;
    n->scale_y = 1.0f;
    n->is_root = false;
    return n;
}

void joy_scene_node_destroy(joy_scene_node_p node)
{
    if (!node) return;
    joy_scene_node_p child = node->first_child;
    while (child) {
        joy_scene_node_p next = child->next_sibling;
        joy_scene_node_destroy(child);
        child = next;
    }
    if (node->on_destroy) {
        node->on_destroy(node);
    }
    SDL_free(node);
}

void joy_scene_node_add_child(joy_scene_node_p parent, joy_scene_node_p child)
{
    if (!parent || !child) return;
    if (child->parent) {
        joy_scene_node_remove_child(child->parent, child);
    }
    child->parent = parent;
    child->next_sibling = parent->first_child;
    parent->first_child = child;
    parent->child_count++;
}

void joy_scene_node_remove_child(joy_scene_node_p parent, joy_scene_node_p child)
{
    if (!parent || !child) return;
    struct joy_scene_node* prev = NULL;
    struct joy_scene_node* curr = parent->first_child;
    while (curr) {
        if (curr == child) {
            if (prev) {
                prev->next_sibling = curr->next_sibling;
            } else {
                parent->first_child = curr->next_sibling;
            }
            child->parent = NULL;
            child->next_sibling = NULL;
            parent->child_count--;
            return;
        }
        prev = curr;
        curr = curr->next_sibling;
    }
}

joy_scene_node_p joy_scene_node_get_parent(joy_scene_node_p node) { return node ? node->parent : NULL; }
joy_scene_node_p joy_scene_node_get_first_child(joy_scene_node_p node) { return node ? node->first_child : NULL; }
joy_scene_node_p joy_scene_node_get_next_sibling(joy_scene_node_p node) { return node ? node->next_sibling : NULL; }
int joy_scene_node_get_child_count(joy_scene_node_p node) { return node ? node->child_count : 0; }

void joy_scene_node_set_position(joy_scene_node_p node, float x, float y)
{
    if (!node) return;
    node->local_x = x;
    node->local_y = y;
}

void joy_scene_node_get_position(joy_scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    if (out_x) *out_x = node->local_x;
    if (out_y) *out_y = node->local_y;
}

void joy_scene_node_set_world_position(joy_scene_node_p node, float x, float y)
{
    if (!node) return;
    node->world_x = x;
    node->world_y = y;
}

void joy_scene_node_get_world_position(joy_scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    float wx = node->local_x;
    float wy = node->local_y;
    joy_scene_node_p p = node->parent;
    while (p) {
        wx += p->local_x;
        wy += p->local_y;
        p = p->parent;
    }
    if (out_x) *out_x = wx;
    if (out_y) *out_y = wy;
}

void joy_scene_node_set_rotation(joy_scene_node_p node, float radians) { if (node) node->rotation = radians; }
float joy_scene_node_get_rotation(joy_scene_node_p node) { return node ? node->rotation : 0.0f; }

void joy_scene_node_set_scale(joy_scene_node_p node, float scale_x, float scale_y)
{
    if (!node) return;
    node->scale_x = scale_x;
    node->scale_y = scale_y;
}

void joy_scene_node_get_scale(joy_scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    if (out_x) *out_x = node->scale_x;
    if (out_y) *out_y = node->scale_y;
}

void joy_scene_node_set_zorder(joy_scene_node_p node, int zorder) { if (node) node->zorder = zorder; }
int joy_scene_node_get_zorder(joy_scene_node_p node) { return node ? node->zorder : 0; }

void joy_scene_node_set_userdata(joy_scene_node_p node, void* ud) { if (node) node->userdata = ud; }
void* joy_scene_node_get_userdata(joy_scene_node_p node) { return node ? node->userdata : NULL; }

/* ===== name / tag ===== */
void joy_scene_node_set_name(joy_scene_node_p node, const char* name) {
    if (node && name) SDL_strlcpy(node->name, name, sizeof(node->name));
}
const char* joy_scene_node_get_name(joy_scene_node_p node) { return node ? node->name : ""; }

void joy_scene_node_set_tag(joy_scene_node_p node, const char* tag) {
    if (node && tag) SDL_strlcpy(node->tag, tag, sizeof(node->tag));
}
const char* joy_scene_node_get_tag(joy_scene_node_p node) { return node ? node->tag : ""; }


/* ===== 递归查找 ===== */

/* 从 root 开始向下 DFS，找 name 匹配的第一个节点 */
static joy_scene_node_p find_name_r(joy_scene_node_p root, const char* name) {
    if (!root || !name) return NULL;
    if (SDL_strcmp(root->name, name) == 0) return root;
    joy_scene_node_p child = root->first_child;
    while (child) {
        joy_scene_node_p found = find_name_r(child, name);
        if (found) return found;
        child = child->next_sibling;
    }
    return NULL;
}

joy_scene_node_p joy_scene_find_node_by_name(joy_scene_node_p root, const char* name) {
    return find_name_r(root, name);
}

/* 从 root 开始 DFS，收集所有 tag 匹配的节点到 out 数组 */
static int find_tag_r(joy_scene_node_p root, const char* tag, joy_scene_node_p* out, int max, int count) {
    if (!root || !tag) return count;
    if (SDL_strcmp(root->tag, tag) == 0 && count < max) {
        out[count++] = root;
    }
    joy_scene_node_p child = root->first_child;
    while (child) {
        count = find_tag_r(child, tag, out, max, count);
        child = child->next_sibling;
    }
    return count;
}

int joy_scene_find_nodes_by_tag(joy_scene_node_p root, const char* tag, joy_scene_node_p* out, int max) {
    if (!root || !tag) return 0;
    return find_tag_r(root, tag, out, max, 0);
}

void joy_scene_node_set_update_callback(joy_scene_node_p node, joy_scene_node_update_fn cb) { if (node) node->on_update = cb; }
void joy_scene_node_set_render_callback(joy_scene_node_p node, joy_scene_node_render_fn cb) { if (node) node->on_render = cb; }
void joy_scene_node_set_destroy_callback(joy_scene_node_p node, joy_scene_node_destroy_fn cb) { if (node) node->on_destroy = cb; }
void joy_scene_node_set_event_callback(joy_scene_node_p node, joy_scene_node_event_fn cb) { if (node) node->on_event = cb; }

void joy_scene_node_update(joy_scene_node_p node, float delta_time)
{
    if (!node) return;

    node->world_x = node->local_x;
    node->world_y = node->local_y;
    joy_scene_node_p p = node->parent;
    while (p) {
        node->world_x += p->local_x;
        node->world_y += p->local_y;
        p = p->parent;
    }

    if (node->on_update) node->on_update(node, delta_time);

    joy_scene_node_p child = node->first_child;
    while (child) {
        joy_scene_node_update(child, delta_time);
        child = child->next_sibling;
    }
}

void joy_scene_node_render(joy_scene_node_p node, const void* arg)
{
    if (!node) return;
    if (node->on_render) node->on_render(node, arg);
    joy_scene_node_p child = node->first_child;
    while (child) {
        joy_scene_node_render(child, arg);
        child = child->next_sibling;
    }
}

void joy_scene_node_handle_event(joy_scene_node_p node, const void* e)
{
    if (!node) return;
    if (node->on_event) node->on_event(node, e);
    joy_scene_node_p child = node->first_child;
    while (child) {
        joy_scene_node_handle_event(child, e);
        child = child->next_sibling;
    }
}

// ============================================================
// 场景结构体
// ============================================================

struct joy_scene {
    joy_scene_node_p* root_nodes;
    int root_count;
    int root_capacity;
    joy_scene_node_p popup_node;   // 弹层节点，渲染时保证在最上层

    joy_scene_load_fn    on_load;
    joy_scene_update_fn  on_update;
    joy_scene_render_fn  on_render;
    joy_scene_destroy_fn on_destroy;
    joy_scene_event_fn   on_event;

    joy_scene_fixed_update_fn on_fixed_update; /* 固定步长更新回调(Unity FixedUpdate 对应)，NULL=不用 */
    float fixed_step;                      /* 固定步长(s)；<=0 表示关闭固定步长调度 */
    float fixed_accum;                     /* 固定步长累加器 */

    void* userdata;
    char name[JOY_MAX_BUFFER];
};

// ============================================================
// 场景实现
// ============================================================

joy_scene_p joy_scene_create(const char* name)
{
    joy_scene_p s = (joy_scene_p)SDL_malloc(sizeof(struct joy_scene));
    SDL_assert(s);
    SDL_memset(s, 0, sizeof(struct joy_scene));
    if (name) SDL_strlcpy(s->name, name, sizeof(s->name));
    s->root_capacity = 16;
    s->root_nodes = (joy_scene_node_p*)SDL_malloc(sizeof(joy_scene_node_p) * s->root_capacity);
    SDL_assert(s->root_nodes);
    return s;
}

void joy_scene_destroy(joy_scene_p scene)
{
    if (!scene) return;
    if (scene->on_destroy) scene->on_destroy(scene);
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_destroy(scene->root_nodes[i]);
    }
    if (scene->popup_node) joy_scene_node_destroy(scene->popup_node);
    SDL_free(scene->root_nodes);
    SDL_free(scene);
}

void joy_scene_add_root_node(joy_scene_p scene, joy_scene_node_p root)
{
    if (!scene || !root) return;
    if (scene->root_count >= scene->root_capacity) {
        scene->root_capacity *= 2;
        scene->root_nodes = (joy_scene_node_p*)SDL_realloc(scene->root_nodes, sizeof(joy_scene_node_p) * scene->root_capacity);
        SDL_assert(scene->root_nodes);
    }
    root->is_root = true;
    scene->root_nodes[scene->root_count++] = root;
}

void joy_scene_remove_root_node(joy_scene_p scene, joy_scene_node_p root)
{
    if (!scene || !root) return;
    for (int i = 0; i < scene->root_count; i++) {
        if (scene->root_nodes[i] == root) {
            for (int j = i; j < scene->root_count - 1; j++) {
                scene->root_nodes[j] = scene->root_nodes[j + 1];
            }
            scene->root_count--;
            root->is_root = false;
            return;
        }
    }
}

int joy_scene_get_root_count(joy_scene_p scene) { return scene ? scene->root_count : 0; }

joy_scene_node_p joy_scene_get_root_at(joy_scene_p scene, int index)
{
    if (!scene || index < 0 || index >= scene->root_count) return NULL;
    return scene->root_nodes[index];
}

/* ===== 场景级递归查找（依赖 struct joy_scene 定义） ===== */
joy_scene_node_p joy_scene_find_node_by_name_deep(joy_scene_p scene, const char* name) {
    if (!scene || !name) return NULL;
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_p found = find_name_r(scene->root_nodes[i], name);
        if (found) return found;
    }
    return NULL;
}

int joy_scene_find_nodes_by_tag_deep(joy_scene_p scene, const char* tag, joy_scene_node_p* out, int max) {
    if (!scene || !tag || !out || max <= 0) return 0;
    int count = 0;
    for (int i = 0; i < scene->root_count && count < max; i++) {
        count = find_tag_r(scene->root_nodes[i], tag, out, max, count);
    }
    return count;
}

void joy_scene_update(joy_scene_p scene, float delta_time)
{
    if (!scene) return;
    if (scene->fixed_step > 0.0f) {
        scene->fixed_accum += delta_time;
        while (scene->fixed_accum >= scene->fixed_step) {
            scene->fixed_accum -= scene->fixed_step;
            if (scene->on_fixed_update) scene->on_fixed_update(scene, scene->fixed_step);
        }
    }
    if (scene->on_update) scene->on_update(scene, delta_time);
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_update(scene->root_nodes[i], delta_time);
    }
}

void joy_scene_render(joy_scene_p scene, const void* arg)
{
    if (!scene) return;
    if (scene->on_render) scene->on_render(scene);
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_render(scene->root_nodes[i], arg);
    }
    // 弹层节点最后渲染，保证在所有普通节点之上
    if (scene->popup_node) joy_scene_node_render(scene->popup_node, arg);
}

void joy_scene_handle_event(joy_scene_p scene, const void* e)
{
    if (!scene || !e) return;
    if (scene->on_event) scene->on_event(scene, e);
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_handle_event(scene->root_nodes[i], e);
    }
}

void joy_scene_set_load_callback(joy_scene_p scene, joy_scene_load_fn cb) { if (scene) scene->on_load = cb; }
void joy_scene_set_update_callback(joy_scene_p scene, joy_scene_update_fn cb)
{
        if (scene) scene->on_update = cb;
}
void joy_scene_set_render_callback(joy_scene_p scene, joy_scene_render_fn cb) { if (scene) scene->on_render = cb; }
void joy_scene_set_destroy_callback(joy_scene_p scene, joy_scene_destroy_fn cb) { if (scene) scene->on_destroy = cb; }
void joy_scene_set_event_callback(joy_scene_p scene, joy_scene_event_fn cb) { if (scene) scene->on_event = cb; }

/* 启用固定步长更新(Unity FixedUpdate 对应)。step<=0 关闭，回调可传 NULL */
void joy_scene_set_fixed_update_callback(joy_scene_p scene, joy_scene_fixed_update_fn cb, float step) {
	if (scene) {
		scene->on_fixed_update = cb;
		scene->fixed_step = step;
		scene->fixed_accum = 0.0f;
	}
}
void joy_scene_set_userdata(joy_scene_p scene, void* ud) { if (scene) scene->userdata = ud; }
void* joy_scene_get_userdata(joy_scene_p scene) { return scene ? scene->userdata : NULL; }
const char* joy_scene_get_name(joy_scene_p scene) { return scene ? scene->name : ""; }

void joy_scene_set_popup_node(joy_scene_p scene, joy_scene_node_p node) { if (scene) scene->popup_node = node; }
joy_scene_node_p joy_scene_get_popup_node(joy_scene_p scene) { return scene ? scene->popup_node : NULL; }

// ============================================================
// 场景管理器结构体
// ============================================================

struct joy_scene_manager {
    joy_scene_p* scenes;
    int scene_count;
    int scene_capacity;
};

// ============================================================
// 场景管理器实现
// ============================================================

joy_scene_manager_p joy_scene_manager_create(void)
{
    struct joy_scene_manager* mgr = (struct joy_scene_manager*)SDL_malloc(sizeof(struct joy_scene_manager));
    SDL_assert(mgr);
    SDL_memset(mgr, 0, sizeof(struct joy_scene_manager));
    mgr->scene_capacity = 8;
    mgr->scenes = (joy_scene_p*)SDL_malloc(sizeof(joy_scene_p) * mgr->scene_capacity);
    SDL_assert(mgr->scenes);
    return mgr;
}

void joy_scene_manager_destroy(joy_scene_manager_p mgr)
{
    if (!mgr) return;
    for (int i = 0; i < mgr->scene_count; i++) {
        joy_scene_destroy(mgr->scenes[i]);
    }
    SDL_free(mgr->scenes);
    SDL_free(mgr);
}

void joy_scene_manager_push(joy_scene_manager_p mgr, joy_scene_p scene)
{
    if (!mgr || !scene) return;
    if (mgr->scene_count >= mgr->scene_capacity) {
        mgr->scene_capacity *= 2;
        mgr->scenes = (joy_scene_p*)SDL_realloc(mgr->scenes, sizeof(joy_scene_p) * mgr->scene_capacity);
        SDL_assert(mgr->scenes);
    }
    if (scene->on_load) scene->on_load(scene);
    mgr->scenes[mgr->scene_count++] = scene;
}

void joy_scene_manager_replace(joy_scene_manager_p mgr, joy_scene_p scene)
{
    if (!mgr || !scene) return;
    if (mgr->scene_count > 0) {
        joy_scene_p old = mgr->scenes[mgr->scene_count - 1];
        if (old != scene) {
            joy_scene_destroy(old);
        }
        mgr->scenes[mgr->scene_count - 1] = scene;
    } else {
        if (mgr->scene_capacity < 1) {
            mgr->scene_capacity = 8;
            mgr->scenes = (joy_scene_p*)SDL_realloc(mgr->scenes, sizeof(joy_scene_p) * mgr->scene_capacity);
        }
        mgr->scenes[mgr->scene_count++] = scene;
    }
    if (scene->on_load) scene->on_load(scene);
}

void joy_scene_manager_pop(joy_scene_manager_p mgr)
{
    if (!mgr || mgr->scene_count <= 0) return;
    joy_scene_destroy(mgr->scenes[--mgr->scene_count]);
}

void joy_scene_manager_update(joy_scene_manager_p mgr, float delta_time)
{
    if (!mgr || mgr->scene_count == 0) return;
    joy_scene_update(mgr->scenes[mgr->scene_count - 1], delta_time);
}

void joy_scene_manager_render(joy_scene_manager_p mgr, const void* arg)
{
    if (!mgr || mgr->scene_count == 0) return;
    joy_scene_render(mgr->scenes[mgr->scene_count - 1], arg);
}

void joy_scene_manager_handle_event(joy_scene_manager_p mgr, const void* e)
{
    if (!mgr || mgr->scene_count == 0 || !e) return;
    joy_scene_handle_event(mgr->scenes[mgr->scene_count - 1], e);
}

joy_scene_p joy_scene_manager_get_current(joy_scene_manager_p mgr)
{
    if (!mgr || mgr->scene_count == 0) return NULL;
    return mgr->scenes[mgr->scene_count - 1];
}

int joy_scene_manager_get_count(joy_scene_manager_p mgr)
{
    return mgr ? mgr->scene_count : 0;
}

/* ==================== 结束 src/jscene.c ==================== */

/* ==================== 合并自 src/janimator.c ==================== */
#include <SDL3/SDL.h>
#include "external/klib/kvec.h"
#include "joy.h"

/* ========== Internal Types ========== */

#define JANIM_MAX_NAME 64

typedef struct {
    char param_name[JANIM_MAX_NAME];
    joy_anim_cond_mode_t mode;
    float threshold;
} joy_anim_condition_t;

typedef struct {
    int from_state;   /* -1 for Any State */
    int to_state;
    kvec_t(joy_anim_condition_t) conditions;
    float exit_time;  /* min seconds source state must play before allowing transition */
} joy_anim_transition_t;

typedef struct {
    joy_image_p image;
    float duration;
    joy_rect_t src_rect;
    bool is_color;     /* true = solid color, no image */
    uint8_t color_r, color_g, color_b, color_a;
} joy_anim_motion_t;

typedef struct {
    char name[JANIM_MAX_NAME];
    kvec_t(joy_anim_motion_t) clips;      /* frame list */
    bool loop;
    float speed;

    /* blend tree */
    bool is_blended;
    char blend_param[JANIM_MAX_NAME];
    kvec_t(joy_anim_motion_t) motions;
    kvec_t(float) thresholds;

    /* runtime */
    int frame;
    float elapsed;      /* time within current clip */
    float state_time;   /* total time since entering this state */
    bool just_entered;
} joy_anim_state_t;

typedef struct {
    char name[JANIM_MAX_NAME];
    joy_anim_param_type_t type;
    union {
        float f;
        int i;
        int b;    /* 0/1 for bool */
    } v;
    bool trigger_consumed;  /* trigger fires once then consumed */
} joy_anim_param_t;

struct joy_animator {
    joy_r2d_p renderer;
    kvec_t(joy_anim_state_t) states;
    kvec_t(joy_anim_transition_t) transitions;
    kvec_t(joy_anim_param_t) params;

    int default_state;
    int current_state;

    joy_anim_state_cb on_enter;
    joy_anim_state_cb on_exit;
    void* cb_userdata;
    bool flip_h;  /* horizontal flip */
};

/* ========== Helpers ========== */

static int find_state(joy_animator_p anim, const char* name)
{
    unsigned int i;
    for (i = 0; i < kv_size(anim->states); i++) {
        if (SDL_strcmp(kv_A(anim->states, i).name, name) == 0)
            return (int)i;
    }
    return -1;
}

static int find_param(joy_animator_p anim, const char* name)
{
    unsigned int i;
    for (i = 0; i < kv_size(anim->params); i++) {
        if (SDL_strcmp(kv_A(anim->params, i).name, name) == 0)
            return (int)i;
    }
    return -1;
}

static joy_anim_param_t* get_or_create_param(joy_animator_p anim, const char* name,
    joy_anim_param_type_t type)
{
    int idx = find_param(anim, name);
    if (idx >= 0) {
        joy_anim_param_t* p = &kv_A(anim->params, idx);
        p->type = type;
        return p;
    }
    joy_anim_param_t p;
    SDL_strlcpy(p.name, name, JANIM_MAX_NAME);
    p.type = type;
    p.v.f = 0.0f;
    p.trigger_consumed = true;  /* trigger starts unset */
    kv_push(joy_anim_param_t, anim->params, p);
    return &kv_A(anim->params, kv_size(anim->params) - 1);
}

static void enter_state(joy_animator_p anim, int idx)
{
    if (anim->on_exit && anim->current_state >= 0 &&
        (unsigned)anim->current_state < kv_size(anim->states))
    {
        anim->on_exit(kv_A(anim->states, anim->current_state).name,
            anim->cb_userdata);
    }

    anim->current_state = idx;
    kv_A(anim->states, idx).frame = 0;
    kv_A(anim->states, idx).elapsed = 0.0f;
    kv_A(anim->states, idx).state_time = 0.0f;
    kv_A(anim->states, idx).just_entered = true;

    if (anim->on_enter) {
        anim->on_enter(kv_A(anim->states, idx).name, anim->cb_userdata);
    }
}

/* ========== Lifecycle ========== */

joy_animator_p joy_scene_animator_create(joy_r2d_p r2d)
{
    joy_animator_p anim = (joy_animator_p)SDL_calloc(1, sizeof(joy_animator_t));
    if (!anim) return NULL;
    anim->renderer = r2d;
    kv_init(anim->states);
    kv_init(anim->transitions);
    kv_init(anim->params);
    anim->default_state = -1;
    anim->current_state = -1;
    anim->on_enter = NULL;
    anim->on_exit = NULL;
    anim->cb_userdata = NULL;
    anim->flip_h = false;
    return anim;
}

void joy_scene_animator_destroy(joy_animator_p anim)
{
    unsigned int i, j;
    if (!anim) return;

    /* free state clips/motions */
    for (i = 0; i < kv_size(anim->states); i++) {
        joy_anim_state_t* s = &kv_A(anim->states, i);
        for (j = 0; j < kv_size(s->clips); j++)
            if (kv_A(s->clips, j).image)
                joy_r2d_image_destroy(kv_A(s->clips, j).image);
        kv_destroy(s->clips);
        for (j = 0; j < kv_size(s->motions); j++)
            if (kv_A(s->motions, j).image)
                joy_r2d_image_destroy(kv_A(s->motions, j).image);
        kv_destroy(s->motions);
        kv_destroy(s->thresholds);
    }
    kv_destroy(anim->states);

    /* free transitions */
    for (i = 0; i < kv_size(anim->transitions); i++)
        kv_destroy(kv_A(anim->transitions, i).conditions);
    kv_destroy(anim->transitions);

    kv_destroy(anim->params);
    SDL_free(anim);
}

/* ========== Parameters ========== */

void joy_scene_animator_set_float(joy_animator_p anim, const char* name, float v)
{
    joy_anim_param_t* p = get_or_create_param(anim, name, JANIM_PARAM_FLOAT);
    p->v.f = v;
}

void joy_scene_animator_set_int(joy_animator_p anim, const char* name, int v)
{
    joy_anim_param_t* p = get_or_create_param(anim, name, JANIM_PARAM_INT);
    p->v.i = v;
}

void joy_scene_animator_set_bool(joy_animator_p anim, const char* name, bool v)
{
    joy_anim_param_t* p = get_or_create_param(anim, name, JANIM_PARAM_BOOL);
    p->v.b = v ? 1 : 0;
}

void joy_scene_animator_set_trigger(joy_animator_p anim, const char* name)
{
    joy_anim_param_t* p = get_or_create_param(anim, name, JANIM_PARAM_TRIGGER);
    p->trigger_consumed = false;
}

float joy_scene_animator_get_float(joy_animator_p anim, const char* name)
{
    int idx = find_param(anim, name);
    if (idx < 0) return 0.0f;
    return kv_A(anim->params, idx).v.f;
}

int joy_scene_animator_get_int(joy_animator_p anim, const char* name)
{
    int idx = find_param(anim, name);
    if (idx < 0) return 0;
    return kv_A(anim->params, idx).v.i;
}

bool joy_scene_animator_get_bool(joy_animator_p anim, const char* name)
{
    int idx = find_param(anim, name);
    if (idx < 0) return false;
    return kv_A(anim->params, idx).v.b != 0;
}

/* ========== States ========== */

void joy_scene_animator_add_state(joy_animator_p anim, const char* name)
{
    joy_anim_state_t s;
    SDL_strlcpy(s.name, name, JANIM_MAX_NAME);
    kv_init(s.clips);
    s.loop = true;
    s.speed = 1.0f;
    s.is_blended = false;
    s.blend_param[0] = '\0';
    kv_init(s.motions);
    kv_init(s.thresholds);
    s.frame = 0;
    s.elapsed = 0.0f;
    s.state_time = 0.0f;
    s.just_entered = false;
    kv_push(joy_anim_state_t, anim->states, s);

    /* first state becomes default */
    if (anim->default_state < 0) {
        anim->default_state = (int)(kv_size(anim->states) - 1);
        anim->current_state = (int)(kv_size(anim->states) - 1);
    }
}

void joy_scene_animator_set_default_state(joy_animator_p anim, const char* name)
{
    int idx = find_state(anim, name);
    if (idx >= 0) {
        anim->default_state = idx;
        enter_state(anim, idx);
    }
}

void joy_scene_animator_state_add_clip(joy_animator_p anim, const char* state_name,
    const char* image_path, float duration, joy_rect_t src_rect)
{
    int sidx = find_state(anim, state_name);
    if (sidx < 0) return;
    joy_anim_state_t* s = &kv_A(anim->states, sidx);
    joy_anim_motion_t m;
    m.image = joy_r2d_image_create(anim->renderer, image_path);
    m.duration = duration;
    m.src_rect = src_rect;
    m.is_color = false;
    kv_push(joy_anim_motion_t, s->clips, m);
}

void joy_scene_animator_state_add_color(joy_animator_p anim, const char* state_name,
    float duration, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    int sidx = find_state(anim, state_name);
    if (sidx < 0) return;
    joy_anim_state_t* s = &kv_A(anim->states, sidx);
    joy_anim_motion_t m;
    m.image = NULL;
    m.duration = duration;
    m.src_rect = (joy_rect_t){ 0, 0, (float)w, (float)h };
    m.is_color = true;
    m.color_r = r; m.color_g = g; m.color_b = b; m.color_a = a;
    kv_push(joy_anim_motion_t, s->clips, m);
}

void joy_scene_animator_state_set_loop(joy_animator_p anim, const char* state_name, bool loop)
{
    int sidx = find_state(anim, state_name);
    if (sidx >= 0)
        kv_A(anim->states, sidx).loop = loop;
}

void joy_scene_animator_state_set_speed(joy_animator_p anim, const char* state_name, float speed)
{
    int sidx = find_state(anim, state_name);
    if (sidx >= 0)
        kv_A(anim->states, sidx).speed = speed;
}

/* ========== Blend Tree ========== */

void joy_scene_animator_state_set_blend(joy_animator_p anim, const char* state_name,
    const char* param_name)
{
    int sidx = find_state(anim, state_name);
    if (sidx < 0) return;
    joy_anim_state_t* s = &kv_A(anim->states, sidx);
    s->is_blended = true;
    SDL_strlcpy(s->blend_param, param_name, JANIM_MAX_NAME);
}

void joy_scene_animator_state_add_motion(joy_animator_p anim, const char* state_name,
    const char* image_path, float duration, joy_rect_t src_rect, float threshold)
{
    int sidx = find_state(anim, state_name);
    if (sidx < 0) return;
    joy_anim_state_t* s = &kv_A(anim->states, sidx);
    joy_anim_motion_t m;
    m.image = joy_r2d_image_create(anim->renderer, image_path);
    m.duration = duration;
    m.src_rect = src_rect;
    m.is_color = false;
    kv_push(joy_anim_motion_t, s->motions, m);
    kv_push(float, s->thresholds, threshold);
}

/* ========== Transitions ========== */

int joy_scene_animator_add_transition(joy_animator_p anim, const char* from, const char* to)
{
    int from_idx = find_state(anim, from);
    int to_idx = find_state(anim, to);
    if (from_idx < 0 || to_idx < 0) return -1;
    joy_anim_transition_t t;
    t.from_state = from_idx;
    t.to_state = to_idx;
    t.exit_time = 0.0f;
    kv_init(t.conditions);
    kv_push(joy_anim_transition_t, anim->transitions, t);
    return (int)(kv_size(anim->transitions) - 1);
}

int joy_scene_animator_add_any_transition(joy_animator_p anim, const char* to)
{
    int to_idx = find_state(anim, to);
    if (to_idx < 0) return -1;
    joy_anim_transition_t t;
    t.from_state = -1;  /* Any State */
    t.to_state = to_idx;
    t.exit_time = 0.0f;
    kv_init(t.conditions);
    kv_push(joy_anim_transition_t, anim->transitions, t);
    return (int)(kv_size(anim->transitions) - 1);
}

void joy_scene_animator_transition_add_cond(joy_animator_p anim, int trans_idx,
    const char* param_name, int mode, float threshold)
{
    if (trans_idx < 0 || (unsigned)trans_idx >= kv_size(anim->transitions))
        return;
    joy_anim_condition_t c;
    SDL_strlcpy(c.param_name, param_name, JANIM_MAX_NAME);
    c.mode = (joy_anim_cond_mode_t)mode;
    c.threshold = threshold;
    kv_push(joy_anim_condition_t, kv_A(anim->transitions, trans_idx).conditions, c);
}

void joy_scene_animator_transition_set_exit_time(joy_animator_p anim, int trans_idx, float seconds)
{
    if (trans_idx < 0 || (unsigned)trans_idx >= kv_size(anim->transitions))
        return;
    kv_A(anim->transitions, trans_idx).exit_time = seconds;
}

/* ========== Transition checking ========== */

static bool check_condition(joy_animator_p anim, const joy_anim_condition_t* c)
{
    int pidx = find_param(anim, c->param_name);
    if (pidx < 0) return false;
    joy_anim_param_t* p = &kv_A(anim->params, pidx);

    switch (c->mode) {
    case JANIM_COND_GREATER:
        return (p->type == JANIM_PARAM_FLOAT) ? (p->v.f > c->threshold) :
               (p->type == JANIM_PARAM_INT)   ? (p->v.i > (int)c->threshold) :
               false;
    case JANIM_COND_LESS:
        return (p->type == JANIM_PARAM_FLOAT) ? (p->v.f < c->threshold) :
               (p->type == JANIM_PARAM_INT)   ? (p->v.i < (int)c->threshold) :
               false;
    case JANIM_COND_EQUALS: {
        float eps = 0.001f;
        return (p->type == JANIM_PARAM_FLOAT) ? (SDL_fabsf(p->v.f - c->threshold) < eps) :
               (p->type == JANIM_PARAM_INT)   ? (p->v.i == (int)c->threshold) :
               false;
    }
    case JANIM_COND_NOT_EQUALS: {
        float eps = 0.001f;
        return (p->type == JANIM_PARAM_FLOAT) ? (SDL_fabsf(p->v.f - c->threshold) >= eps) :
               (p->type == JANIM_PARAM_INT)   ? (p->v.i != (int)c->threshold) :
               false;
    }
    case JANIM_COND_IF_TRUE:
        return (p->type == JANIM_PARAM_BOOL) && (p->v.b != 0);
    case JANIM_COND_IF_FALSE:
        return (p->type == JANIM_PARAM_BOOL) && (p->v.b == 0);
    case JANIM_COND_TRIGGER: {
        /* don't consume here; check_transition will consume after all conditions pass */
        return (p->type == JANIM_PARAM_TRIGGER && !p->trigger_consumed);
    }
    default:
        return false;
    }
}

static bool check_transition(joy_animator_p anim, const joy_anim_transition_t* t)
{
    unsigned int i;
    if (kv_size(t->conditions) == 0)
        return true;
    for (i = 0; i < kv_size(t->conditions); i++) {
        if (!check_condition(anim, &kv_A(t->conditions, i)))
            return false;
    }
    /* all conditions passed → now consume triggers */
    for (i = 0; i < kv_size(t->conditions); i++) {
        const joy_anim_condition_t* c = &kv_A(t->conditions, i);
        if (c->mode == JANIM_COND_TRIGGER) {
            int pidx = find_param(anim, c->param_name);
            if (pidx >= 0)
                kv_A(anim->params, pidx).trigger_consumed = true;
        }
    }
    return true;
}

/* ========== Update ========== */

void joy_scene_animator_update(joy_animator_p anim, float dt)
{
    unsigned int i;
    unsigned int nstates;
    int cur;
    joy_anim_state_t* cs;

    if (!anim) return;
    nstates = kv_size(anim->states);
    if (nstates == 0) return;

    cur = anim->current_state;
    if (cur < 0 || (unsigned)cur >= nstates) {
        if (anim->default_state >= 0 && (unsigned)anim->default_state < nstates) {
            enter_state(anim, anim->default_state);
            cur = anim->current_state;
        } else {
            return;
        }
    }

    cs = &kv_A(anim->states, cur);
    cs->state_time += dt;
    cs->just_entered = false;

    /* Check Any State transitions first */
    for (i = 0; i < kv_size(anim->transitions); i++) {
        joy_anim_transition_t* t = &kv_A(anim->transitions, i);
        if (t->from_state == -1 && t->to_state != cur) {
            if (check_transition(anim, t)) {
                enter_state(anim, t->to_state);
                cs = &kv_A(anim->states, anim->current_state);
                goto after_transition;
            }
        }
    }

    /* Normal transitions: only if exit_time has elapsed */
    for (i = 0; i < kv_size(anim->transitions); i++) {
        joy_anim_transition_t* t = &kv_A(anim->transitions, i);
        if (t->from_state == cur && t->to_state != cur) {
            if (cs->state_time < t->exit_time) continue;  /* exit time not met */
            if (check_transition(anim, t)) {
                enter_state(anim, t->to_state);
                cs = &kv_A(anim->states, anim->current_state);
                goto after_transition;
            }
        }
    }

after_transition:
    if (cs->is_blended) {
        int pidx = find_param(anim, cs->blend_param);
        float val = 0.0f;
        if (pidx >= 0) {
            joy_anim_param_t* p = &kv_A(anim->params, pidx);
            val = (p->type == JANIM_PARAM_FLOAT) ? p->v.f : (float)p->v.i;
        }
        unsigned int mi;
        int best = 0;
        for (mi = 0; mi < kv_size(cs->thresholds); mi++) {
            if (val >= kv_A(cs->thresholds, mi))
                best = (int)mi;
        }
        if ((unsigned)best >= kv_size(cs->motions))
            best = (int)kv_size(cs->motions) - 1;
        cs->frame = best;
    } else {
        if (kv_size(cs->clips) > 0) {
            float adjusted_dt = dt * cs->speed;
            cs->elapsed += adjusted_dt;
            joy_anim_motion_t* clip = &kv_A(cs->clips, cs->frame);
            if (cs->elapsed >= clip->duration) {
                cs->elapsed -= clip->duration;
                cs->frame++;
                if ((unsigned)cs->frame >= kv_size(cs->clips)) {
                    if (cs->loop)
                        cs->frame = 0;
                    else
                        cs->frame = (int)kv_size(cs->clips) - 1;
                }
            }
        }
    }
}

/* ========== Draw ========== */

void joy_scene_animator_draw(joy_animator_p anim, float x, float y)
{
    if (!anim) return;
    if (anim->current_state < 0 || (unsigned)anim->current_state >= kv_size(anim->states))
        return;
    joy_anim_state_t* cs = &kv_A(anim->states, anim->current_state);

    joy_anim_motion_t* m;
    if (cs->is_blended) {
        if ((unsigned)cs->frame >= kv_size(cs->motions)) return;
        m = &kv_A(cs->motions, cs->frame);
    } else {
        if ((unsigned)cs->frame >= kv_size(cs->clips)) return;
        m = &kv_A(cs->clips, cs->frame);
    }

    if (m->is_color) {
        joy_rect_t r = { x, y, m->src_rect.w, m->src_rect.h };
        joy_r2d_set_color(anim->renderer, m->color_r, m->color_g, m->color_b, m->color_a);
        joy_r2d_shape_draw_rectangle(anim->renderer, "fill", r);
    } else if (m->image) {
        float sx = anim->flip_h ? -1.0f : 1.0f;
        joy_r2d_image_draw_ex(m->image, &m->src_rect, (joy_vec2_t){x, y}, 0.0f,
            (joy_vec2_t){sx, 1.0f}, (joy_vec2_t){0.0f, 0.0f});
    } else {
        joy_rect_t r = { x, y, m->src_rect.w, m->src_rect.h };
        joy_r2d_set_color(anim->renderer, 80, 80, 80, 255);
        joy_r2d_shape_draw_rectangle(anim->renderer, "fill", r);
        joy_r2d_set_color(anim->renderer, 120, 120, 120, 255);
        joy_r2d_shape_draw_rectangle(anim->renderer, "line", r);
    }
}

void joy_scene_animator_set_flip_h(joy_animator_p anim, bool flip)
{
    if (anim) anim->flip_h = flip;
}

/* ========== Query ========== */

const char* joy_scene_animator_current_state(joy_animator_p anim)
{
    if (!anim || anim->current_state < 0 ||
        (unsigned)anim->current_state >= kv_size(anim->states))
        return "";
    return kv_A(anim->states, anim->current_state).name;
}

/* ========== Callbacks ========== */

void joy_scene_animator_set_on_enter(joy_animator_p anim, joy_anim_state_cb cb, void* userdata)
{
    anim->on_enter = cb;
    anim->cb_userdata = userdata;
}

void joy_scene_animator_set_on_exit(joy_animator_p anim, joy_anim_state_cb cb, void* userdata)
{
    anim->on_exit = cb;
    anim->cb_userdata = userdata;
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef JANIM_MAX_NAME

/* ==================== 结束 src/janimator.c ==================== */

/* ==================== 合并自 src/jtilemap.c ==================== */
/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jtilemap.c
Description: Tiled 地图数据解析实现（Lua 导出格式）
Author: ydlc
Version: 1.0
Date: 2026.5.22
History:
*************************************************/
#include "external/stb_image.h"
#include "external/klib/kvec.h"
#include "external/klib/khash.h"
#include "external/cJSON.h"
#include <stdlib.h>
#include <string.h>
/* 公共头不再间接提供 SDL 声明：本 TU 用到 SDL_LoadFile / SDL_free，
 * 必须自己包含 SDL 头，否则 Android clang 在 C99 下报隐式函数声明。 */
#include <SDL3/SDL.h>
#include "joy.h"






/* ---------- 动画帧 ---------- */
typedef struct {
        int tileid;         /* tileset 内局部瓦片 ID */
        int duration;       /* 持续时间（毫秒） */
} joy_tilemap_anim_frame_t;
typedef joy_tilemap_anim_frame_t* joy_tilemap_anim_frame_p;

/* ---------- 动画序列 ---------- */
typedef struct {
        kvec_t(joy_tilemap_anim_frame_t) frames;
        int total_duration;          /* 所有帧总时长（毫秒），缓存加速 */
} joy_tilemap_anim_t;
typedef joy_tilemap_anim_t* joy_tilemap_anim_p;

/* ---------- 独立瓦片定义（用于 columns=0 的 tileset，如 deco） ---------- */
typedef struct {
        int    id;            /* tileset 内局部 ID */
        char* image;         /* 独立图片路径 */
        int    width;
        int    height;
} joy_tilemap_tile_def_t;
typedef joy_tilemap_tile_def_t* joy_tilemap_tile_def_p;

/* ---------- 图块集 ---------- */
struct joy_tilemap_tileset {
        char* name;
        int    firstgid;
        int    tilewidth;
        int    tileheight;
        int    spacing;
        int    margin;
        int    columns;
        int    tilecount;
        char* image;
        int    imagewidth;
        int    imageheight;

        /* 动画：anims[n] 对应 tileset 内部局部瓦片 id = anims[n].frames[0].tileid（首个帧的 id） */
        kvec_t(joy_tilemap_anim_t) anims;

        /* 独立瓦片定义（用于 columns=0 的 tileset） */
        kvec_t(joy_tilemap_tile_def_t) tile_defs;

        /* 运行时加速：局部瓦片 id -> anims[] 下标，-1 表示无动画 */
        int* tile_anim_map;   /* 数组长度 = tilecount，仅在加载后有效 */
        int  tile_anim_map_alloced;
};




/* ---------- 图层基类 ---------- */
typedef struct joy_tilemap_layer {
        int   type;          /* JTILEMAP_LAYER_* */
        char* name;
        bool  visible;
        float opacity;
        float offsetx, offsety;
        float parallaxx, parallaxy;
} joy_tilemap_layer_t, * joy_tilemap_layer_p;

/* ---------- 瓦片图层 ---------- */
struct joy_tilemap_tile_layer {
        joy_tilemap_layer_t base;
        int   width;         /* 瓦片列数 */
        int   height;        /* 瓦片行数 */
        int* data;          /* 展平 GID 数组，行优先：data[y * width + x] */
};

/* ---------- 对象图层 ---------- */
struct joy_tilemap_object_layer {
        joy_tilemap_layer_t base;
        kvec_t(joy_tilemap_object_t) objects;
};


/* ---------- 图片图层 ---------- */
typedef struct {
        joy_tilemap_layer_t base;
        char* image;
        bool  repeatx;
        bool  repeaty;
} joy_tilemap_image_layer_t;
typedef joy_tilemap_image_layer_t* joy_tilemap_image_layer_p;

/* ---------- 运行时动画状态 ---------- */
typedef struct {
        uint32_t start_time;          /* 此瓦片启动时间（ms） */
} joy_tilemap_anim_state_t;

/* ---------- 主地图 ---------- */
struct joy_tilemap {
        char* version;
        char* tiledversion;
        char* orientation;
        char* renderorder;

        int width;          /* 瓦片列数 */
        int height;         /* 瓦片行数 */
        int tilewidth;      /* 默认瓦片宽度 */
        int tileheight;     /* 默认瓦片高度 */

        kvec_t(joy_tilemap_tileset_t) tilesets;

        /* layers 数组中每一项是 joy_tilemap_layer_p（实际为 *tile_layer / *object_layer / *image_layer） */
        kvec_t(void*) layers;

        /* 运行时：每加载一个 tile layer 时收集所有有动画的 gid 及启动时间 */
        kvec_t(int)          anim_gids;       /* 有动画的全局 GID */
        kvec_t(joy_tilemap_anim_state_t) anim_states;
};

/* GID 解析结果类型已在 joy.h 中完整定义（joy_tilemap_gid_resolve_t），此处不再重复定义 */


KHASH_MAP_INIT_STR(tex_cache, joy_image_p)

struct joy_tilemap_render_t {
        joy_r2d_p                 r2d;
        char* asset_base;
        joy_tilemap_path_remap_t  remap;
        float                  view_w;
        float                  view_h;
        float                  tile_px;   /* 每格目标像素；<=0 时用 map.tilewidth（兼容默认） */
        khash_t(tex_cache)* tex_cache;
};


/* 提取干净 GID（去掉翻转标志） */
JOY_INLINE int jtilemap_clean_gid(int raw_gid) { return raw_gid & JTILEMAP_GID_MASK; }
/* 提取翻转标志 */
JOY_INLINE int jtilemap_gid_flags(int raw_gid) { return raw_gid & ~JTILEMAP_GID_MASK; }

/* ==================== 构建 tileset 动画加速表 ==================== */

static void build_tileset_anim_map(joy_tilemap_tileset_t* ts)
{
    if (ts->tilecount <= 0) return;

    /* 分配加速表，初始化为 -1（无动画） */
    ts->tile_anim_map_alloced = ts->tilecount;
    ts->tile_anim_map = (int*)calloc((size_t)ts->tilecount, sizeof(int));
    if (!ts->tile_anim_map) return;

    for (int i = 0; i < ts->tilecount; i++)
        ts->tile_anim_map[i] = -1;

    /* 遍历 anims，标记局部瓦片 id → anim 索引 */
    for (size_t a = 0; a < kv_size(ts->anims); a++) {
        joy_tilemap_anim_t* anim = &kv_A(ts->anims, a);
        if (kv_size(anim->frames) > 0) {
            /* Tiled 中 animation id 表示瓦片的局部 id，对应 anim 结构体携带该瓦片的动画 */
            /* 这里 a 就是该动画在 anims 中的下标 */
            /* 注意：Tiled 导出的 tiles[n] 的 n 是 id 字段，但 anims 是按顺序添加的 */
            /* 我们需要从帧列表第一个 tileid 得知它属于哪个瓦片 */
            int tile_id = kv_A(anim->frames, 0).tileid;
            if (tile_id >= 0 && tile_id < ts->tilecount)
                ts->tile_anim_map[tile_id] = (int)a;
        }
    }
}

/* ==================== 收集所有有动画的 GID ==================== */

static void collect_anim_gids(joy_tilemap_p tm)
{
    kv_init(tm->anim_gids);
    kv_init(tm->anim_states);

    for (size_t t = 0; t < kv_size(tm->tilesets); t++) {
        joy_tilemap_tileset_t* ts = &kv_A(tm->tilesets, t);
        if (!ts->tile_anim_map) continue;

        for (int i = 0; i < ts->tilecount; i++) {
            if (ts->tile_anim_map[i] >= 0) {
                int gid = ts->firstgid + i;
                kv_push(int, tm->anim_gids, gid);
                joy_tilemap_anim_state_t state;
                state.start_time = 0;
                kv_push(joy_tilemap_anim_state_t, tm->anim_states, state);
            }
        }
    }
}

/* ==================== JSON 解析后端（cJSON，无需 Lua VM） ==================== */
/* 与上方 Lua 后端字段一一对应：Tiled 的 Lua 导出与 JSON 导出结构同构，
   此处用现成的 cJSON 解析，使地图加载摆脱对 Lua 虚拟机的依赖。 */

static int cj_get_integer(cJSON* obj, const char* key, int def)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (it && it->type == cJSON_Number) return (int)it->valuedouble;
    return def;
}
static float cj_get_number(cJSON* obj, const char* key, float def)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (it && it->type == cJSON_Number) return (float)it->valuedouble;
    return def;
}
static bool cj_get_boolean(cJSON* obj, const char* key, bool def)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (!it) return def;
    if (it->type == cJSON_True)  return true;
    if (it->type == cJSON_False) return false;
    return def;
}
static char* cj_get_string(cJSON* obj, const char* key)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (it && it->type == cJSON_String && it->valuestring)
        return joy_core_utils_strdup(it->valuestring, NULL);
    return NULL;
}
/* 当字段可能是字符串或数字时，统一返回字符串（renderorder 等） */
static char* cj_get_string_or_number(cJSON* obj, const char* key)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (!it) return NULL;
    if (it->type == cJSON_String && it->valuestring) return joy_core_utils_strdup(it->valuestring, NULL);
    if (it->type == cJSON_Number) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%lld", (long long)(int)it->valuedouble);
        return joy_core_utils_strdup(buf, NULL);
    }
    return NULL;
}

static void parse_animation_cjson(cJSON* arr, joy_tilemap_anim_t* anim)
{
    kv_init(anim->frames);
    anim->total_duration = 0;
    if (!arr || arr->type != cJSON_Array) return;
    int n = cJSON_GetArraySize(arr);
    for (int i = 0; i < n; i++) {
        cJSON* f = cJSON_GetArrayItem(arr, i);
        if (!f || f->type != cJSON_Object) continue;
        joy_tilemap_anim_frame_t frame;
        frame.tileid   = cj_get_integer(f, "tileid", 0);
        frame.duration = cj_get_integer(f, "duration", 100);
        kv_push(joy_tilemap_anim_frame_t, anim->frames, frame);
        anim->total_duration += frame.duration;
    }
}

static bool parse_tileset_cjson(cJSON* j, joy_tilemap_tileset_t* ts)
{
    memset(ts, 0, sizeof(*ts));
    kv_init(ts->anims);
    kv_init(ts->tile_defs);
    ts->tile_anim_map = NULL;
    ts->tile_anim_map_alloced = 0;

    ts->name        = cj_get_string(j, "name");
    ts->firstgid    = cj_get_integer(j, "firstgid", 1);
    ts->tilewidth   = cj_get_integer(j, "tilewidth", 64);
    ts->tileheight  = cj_get_integer(j, "tileheight", 64);
    ts->spacing     = cj_get_integer(j, "spacing", 0);
    ts->margin      = cj_get_integer(j, "margin", 0);
    ts->columns     = cj_get_integer(j, "columns", 0);
    ts->tilecount   = cj_get_integer(j, "tilecount", 0);
    ts->image       = cj_get_string(j, "image");
    ts->imagewidth  = cj_get_integer(j, "imagewidth", 0);
    ts->imageheight = cj_get_integer(j, "imageheight", 0);

    cJSON* tiles = cJSON_GetObjectItem(j, "tiles");
    if (tiles && tiles->type == cJSON_Array) {
        int n = cJSON_GetArraySize(tiles);
        for (int i = 0; i < n; i++) {
            cJSON* t = cJSON_GetArrayItem(tiles, i);
            if (!t || t->type != cJSON_Object) continue;
            int tile_id = cj_get_integer(t, "id", -1);

            cJSON* anim = cJSON_GetObjectItem(t, "animation");
            if (anim && anim->type == cJSON_Array) {
                joy_tilemap_anim_t a;
                parse_animation_cjson(anim, &a);
                kv_push(joy_tilemap_anim_t, ts->anims, a);
            }

            cJSON* img = cJSON_GetObjectItem(t, "image");
            if (img && img->type == cJSON_String && img->valuestring) {
                joy_tilemap_tile_def_t def;
                def.id = tile_id;
                def.image = joy_core_utils_strdup(img->valuestring, NULL);
                def.width  = cj_get_integer(t, "width", 64);
                def.height = cj_get_integer(t, "height", 64);
                kv_push(joy_tilemap_tile_def_t, ts->tile_defs, def);
            }
        }
    }
    return true;
}

static joy_tilemap_tile_layer_p parse_tile_layer_cjson(cJSON* j)
{
    joy_tilemap_tile_layer_p layer = (joy_tilemap_tile_layer_p)calloc(1, sizeof(joy_tilemap_tile_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_TILE;
    layer->base.name      = cj_get_string(j, "name");
    layer->base.visible   = cj_get_boolean(j, "visible", true);
    layer->base.opacity   = cj_get_number(j, "opacity", 1.0f);
    layer->base.offsetx   = cj_get_number(j, "offsetx", 0.0f);
    layer->base.offsety   = cj_get_number(j, "offsety", 0.0f);
    layer->base.parallaxx = cj_get_number(j, "parallaxx", 1.0f);
    layer->base.parallaxy = cj_get_number(j, "parallaxy", 1.0f);

    layer->width  = cj_get_integer(j, "width", 0);
    layer->height = cj_get_integer(j, "height", 0);

    int count = layer->width * layer->height;
    layer->data = (int*)calloc((size_t)(count > 0 ? count : 0), sizeof(int));

    cJSON* data = cJSON_GetObjectItem(j, "data");
    if (data && data->type == cJSON_Array) {
        int n = cJSON_GetArraySize(data);
        int m = (count > 0 ? count : 0);
        for (int i = 0; i < n && i < m; i++) {
            cJSON* v = cJSON_GetArrayItem(data, i);
            if (!v || v->type != cJSON_Number) continue;
            /* Tiled 的 GID 是 32 位无符号：高 3 位为水平/垂直/对角翻转标志。
             * 直接 (int)double 转换时，带 0x80000000/0x40000000/0x20000000 的
             * GID（如 3221225483 = 0xC000000B）会因超过 INT_MAX 溢出成 INT_MIN，
             * 清位后变 0 → 该瓦片被当作空格跳过，表现为"部分墙的贴图不显示"。
             * 这里先按 int64 截取低 32 位再转 int，保证翻转位与 GID 都不丢。 */
            layer->data[i] = (int)(uint32_t)(int64_t)v->valuedouble;
        }
    }
    return layer;
}

static joy_tilemap_object_layer_p parse_object_layer_cjson(cJSON* j)
{
    joy_tilemap_object_layer_p layer = (joy_tilemap_object_layer_p)calloc(1, sizeof(joy_tilemap_object_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_OBJECT;
    layer->base.name      = cj_get_string(j, "name");
    layer->base.visible   = cj_get_boolean(j, "visible", true);
    layer->base.opacity   = cj_get_number(j, "opacity", 1.0f);
    layer->base.offsetx   = cj_get_number(j, "offsetx", 0.0f);
    layer->base.offsety   = cj_get_number(j, "offsety", 0.0f);
    layer->base.parallaxx = cj_get_number(j, "parallaxx", 1.0f);
    layer->base.parallaxy = cj_get_number(j, "parallaxy", 1.0f);

    kv_init(layer->objects);

    cJSON* objs = cJSON_GetObjectItem(j, "objects");
    if (objs && objs->type == cJSON_Array) {
        int n = cJSON_GetArraySize(objs);
        for (int i = 0; i < n; i++) {
            cJSON* o = cJSON_GetArrayItem(objs, i);
            if (!o || o->type != cJSON_Object) continue;
            joy_tilemap_object_t obj;
            memset(&obj, 0, sizeof(obj));

            obj.id       = cj_get_integer(o, "id", 0);
            obj.name     = cj_get_string(o, "name");
            obj.type     = cj_get_string(o, "type");
            obj.x        = cj_get_number(o, "x", 0.0f);
            obj.y        = cj_get_number(o, "y", 0.0f);
            obj.width    = cj_get_number(o, "width", 0.0f);
            obj.height   = cj_get_number(o, "height", 0.0f);
            obj.rotation = cj_get_number(o, "rotation", 0.0f);
            obj.visible  = cj_get_boolean(o, "visible", true);

            /* 形状：优先读 Tiled Lua 导出的 shape 字符串（与 Lua 后端一致），
               否则按 Tiled JSON 原生的 point/ellipse/polygon/polyline 标志判定。 */
            char* shape_str = cj_get_string(o, "shape");
            if (shape_str) {
                if      (strcmp(shape_str, "rectangle") == 0) obj.shape = JTILEMAP_SHAPE_RECT;
                else if (strcmp(shape_str, "ellipse") == 0)   obj.shape = JTILEMAP_SHAPE_ELLIPSE;
                else if (strcmp(shape_str, "polygon") == 0)   obj.shape = JTILEMAP_SHAPE_POLYGON;
                else if (strcmp(shape_str, "polyline") == 0)  obj.shape = JTILEMAP_SHAPE_POLYLINE;
                else if (strcmp(shape_str, "point") == 0)     obj.shape = JTILEMAP_SHAPE_POINT;
                free(shape_str);
            } else if (cj_get_boolean(o, "point", false))      obj.shape = JTILEMAP_SHAPE_POINT;
            else if (cj_get_boolean(o, "ellipse", false))     obj.shape = JTILEMAP_SHAPE_ELLIPSE;
            else if (cJSON_GetObjectItem(o, "polygon"))       obj.shape = JTILEMAP_SHAPE_POLYGON;
            else if (cJSON_GetObjectItem(o, "polyline"))      obj.shape = JTILEMAP_SHAPE_POLYLINE;
            else                                             obj.shape = JTILEMAP_SHAPE_RECT;

            kv_push(joy_tilemap_object_t, layer->objects, obj);
        }
    }
    return layer;
}

static joy_tilemap_image_layer_p parse_image_layer_cjson(cJSON* j)
{
    joy_tilemap_image_layer_p layer = (joy_tilemap_image_layer_p)calloc(1, sizeof(joy_tilemap_image_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_IMAGE;
    layer->base.name      = cj_get_string(j, "name");
    layer->base.visible   = cj_get_boolean(j, "visible", true);
    layer->base.opacity   = cj_get_number(j, "opacity", 1.0f);
    layer->base.offsetx   = cj_get_number(j, "offsetx", 0.0f);
    layer->base.offsety   = cj_get_number(j, "offsety", 0.0f);
    layer->base.parallaxx = cj_get_number(j, "parallaxx", 1.0f);
    layer->base.parallaxy = cj_get_number(j, "parallaxy", 1.0f);

    layer->image   = cj_get_string(j, "image");
    layer->repeatx = cj_get_boolean(j, "repeatx", false);
    layer->repeaty = cj_get_boolean(j, "repeaty", false);

    return layer;
}

joy_tilemap_p joy_scene_tilemap_load_cjson(cJSON* root)
{
    if (!root || root->type != cJSON_Object) {
        joy_core_log_error("joy_scene_tilemap_load_cjson: root is not an object");
        return NULL;
    }

    joy_tilemap_p tm = (joy_tilemap_p)calloc(1, sizeof(joy_tilemap_t));
    if (!tm) {
        joy_core_log_error("joy_scene_tilemap_load_cjson: out of memory");
        return NULL;
    }

    kv_init(tm->tilesets);
    kv_init(tm->layers);

    tm->version      = cj_get_string(root, "version");
    tm->tiledversion = cj_get_string(root, "tiledversion");
    tm->orientation  = cj_get_string(root, "orientation");
    tm->renderorder  = cj_get_string_or_number(root, "renderorder");
    tm->width        = cj_get_integer(root, "width", 0);
    tm->height       = cj_get_integer(root, "height", 0);
    tm->tilewidth    = cj_get_integer(root, "tilewidth", 64);
    tm->tileheight   = cj_get_integer(root, "tileheight", 64);

    joy_core_log_info("jtilemap: loading %s map %dx%d, tile %dx%d",
             tm->orientation ? tm->orientation : "unknown",
             tm->width, tm->height, tm->tilewidth, tm->tileheight);

    cJSON* ts_arr = cJSON_GetObjectItem(root, "tilesets");
    if (ts_arr && ts_arr->type == cJSON_Array) {
        int n = cJSON_GetArraySize(ts_arr);
        for (int i = 0; i < n; i++) {
            cJSON* tj = cJSON_GetArrayItem(ts_arr, i);
            if (!tj || tj->type != cJSON_Object) continue;
            joy_tilemap_tileset_t ts;
            if (parse_tileset_cjson(tj, &ts)) {
                build_tileset_anim_map(&ts);
                kv_push(joy_tilemap_tileset_t, tm->tilesets, ts);
            }
        }
    }
    joy_core_log_info("jtilemap: parsed %zu tilesets", kv_size(tm->tilesets));

    cJSON* ly_arr = cJSON_GetObjectItem(root, "layers");
    if (ly_arr && ly_arr->type == cJSON_Array) {
        int n = cJSON_GetArraySize(ly_arr);
        for (int i = 0; i < n; i++) {
            cJSON* lj = cJSON_GetArrayItem(ly_arr, i);
            if (!lj || lj->type != cJSON_Object) continue;
            char* layer_type_str = cj_get_string(lj, "type");
            void* layer_obj = NULL;

            if (layer_type_str) {
                if (strcmp(layer_type_str, "tilelayer") == 0) {
                    layer_obj = parse_tile_layer_cjson(lj);
                } else if (strcmp(layer_type_str, "objectgroup") == 0) {
                    layer_obj = parse_object_layer_cjson(lj);
                } else if (strcmp(layer_type_str, "imagelayer") == 0) {
                    layer_obj = parse_image_layer_cjson(lj);
                } else if (strcmp(layer_type_str, "group") == 0) {
                    joy_core_log_info("jtilemap: skipping group layer");
                }
                free(layer_type_str);
            }

            if (layer_obj)
                kv_push(void*, tm->layers, layer_obj);
        }
    }
    joy_core_log_info("jtilemap: parsed %zu layers", kv_size(tm->layers));

    collect_anim_gids(tm);
    return tm;
}

joy_tilemap_p joy_scene_tilemap_load_file_json(const char* filename)
{
    if (!filename) {
        joy_core_log_error("joy_scene_tilemap_load_file_json: filename is NULL");
        return NULL;
    }

    size_t sz = 0;
    void* raw = SDL_LoadFile(filename, &sz);
    if (!raw) {
        joy_core_log_error("joy_scene_tilemap_load_file_json: cannot read %s", filename);
        return NULL;
    }
    char* buf = (char*)malloc(sz + 1);
    if (!buf) { SDL_free(raw); return NULL; }
    memcpy(buf, raw, sz);
    buf[sz] = '\0';
    SDL_free(raw);

    cJSON* root = cJSON_Parse(buf);
    free(buf);
    if (!root) {
        const char* err = cJSON_GetErrorPtr();
        joy_core_log_error("joy_scene_tilemap_load_file_json: parse error: %s", err ? err : "?");
        return NULL;
    }

    joy_tilemap_p tm = joy_scene_tilemap_load_cjson(root);
    cJSON_Delete(root);
    return tm;
}

/* ==================== 获取 deco 独立瓦片图片路径 ==================== */

const char* joy_scene_tilemap_get_tile_image(const joy_tilemap_tileset_p ts, int local_id)
{
    if (!ts) return NULL;
    for (size_t i = 0; i < kv_size(ts->tile_defs); i++) {
        if (kv_A(ts->tile_defs, i).id == local_id)
            return kv_A(ts->tile_defs, i).image;
    }
    return NULL;
}

bool joy_scene_tilemap_get_tile_def_size(const joy_tilemap_tileset_p ts, int local_id,
                                          int* out_w, int* out_h)
{
    if (!ts) return false;
    for (size_t i = 0; i < kv_size(ts->tile_defs); i++) {
        if (kv_A(ts->tile_defs, i).id == local_id) {
            if (out_w) *out_w = kv_A(ts->tile_defs, i).width;
            if (out_h) *out_h = kv_A(ts->tile_defs, i).height;
            return true;
        }
    }
    return false;
}

/* ==================== 释放 ==================== */

static void destroy_anim(joy_tilemap_anim_t* anim)
{
    kv_destroy(anim->frames);
}

static void destroy_tileset(joy_tilemap_tileset_t* ts)
{
    free(ts->name);
    free(ts->image);
    for (size_t i = 0; i < kv_size(ts->anims); i++)
        destroy_anim(&kv_A(ts->anims, i));
    kv_destroy(ts->anims);
    for (size_t i = 0; i < kv_size(ts->tile_defs); i++)
        free(kv_A(ts->tile_defs, i).image);
    kv_destroy(ts->tile_defs);
    free(ts->tile_anim_map);
}

static void destroy_object(joy_tilemap_object_t* obj)
{
    free(obj->name);
    free(obj->type);
}

static void destroy_layer(void* layer)
{
    joy_tilemap_layer_p base = (joy_tilemap_layer_p)layer;
    free(base->name);

    switch (base->type) {
    case JTILEMAP_LAYER_TILE: {
        joy_tilemap_tile_layer_p tl = (joy_tilemap_tile_layer_p)layer;
        free(tl->data);
        break;
    }
    case JTILEMAP_LAYER_OBJECT: {
        joy_tilemap_object_layer_p ol = (joy_tilemap_object_layer_p)layer;
        for (size_t i = 0; i < kv_size(ol->objects); i++)
            destroy_object(&kv_A(ol->objects, i));
        kv_destroy(ol->objects);
        break;
    }
    case JTILEMAP_LAYER_IMAGE: {
        joy_tilemap_image_layer_p il = (joy_tilemap_image_layer_p)layer;
        free(il->image);
        break;
    }
    }

    free(layer);
}

int joy_scene_tilemap_get_width(const joy_tilemap_p tm) {
    return tm ? tm->width : 0;
}

int joy_scene_tilemap_get_height(const joy_tilemap_p tm) {
    return tm ? tm->height : 0;
}

int joy_scene_tilemap_get_tilewidth(const joy_tilemap_p tm) {
    return tm ? tm->tilewidth : 0;
}

int joy_scene_tilemap_get_tileheight(const joy_tilemap_p tm) {
    return tm ? tm->tileheight : 0;
}

void joy_scene_tilemap_destroy(joy_tilemap_p tm)
{
    if (!tm) return;

    free(tm->version);
    free(tm->tiledversion);
    free(tm->orientation);
    free(tm->renderorder);

    for (size_t i = 0; i < kv_size(tm->tilesets); i++)
        destroy_tileset(&kv_A(tm->tilesets, i));
    kv_destroy(tm->tilesets);

    for (size_t i = 0; i < kv_size(tm->layers); i++)
        destroy_layer(kv_A(tm->layers, i));
    kv_destroy(tm->layers);

    kv_destroy(tm->anim_gids);
    kv_destroy(tm->anim_states);

    free(tm);
}

/* ==================== GID 解析 ==================== */

joy_tilemap_gid_resolve_t joy_scene_tilemap_resolve_gid(const joy_tilemap_p tm, int gid)
{
    joy_tilemap_gid_resolve_t result;
    memset(&result, 0, sizeof(result));

    if (!tm || gid <= 0) return result;

    /* 找到 firstgid ≤ gid 且最大的 tileset */
    joy_tilemap_tileset_p best_ts = NULL;
    int best_firstgid = 0;

    for (size_t i = 0; i < kv_size(tm->tilesets); i++) {
        joy_tilemap_tileset_p ts = &kv_A(tm->tilesets, i);
        if (ts->firstgid <= gid && ts->firstgid >= best_firstgid) {
            best_ts = ts;
            best_firstgid = ts->firstgid;
        }
    }

    if (!best_ts) return result;

    result.tileset = best_ts;
    result.local_id = gid - best_ts->firstgid;

    /* 查询动画 */
    if (best_ts->tile_anim_map &&
        result.local_id >= 0 && result.local_id < best_ts->tile_anim_map_alloced) {
        int anim_idx = best_ts->tile_anim_map[result.local_id];
        if (anim_idx >= 0) {
            result.has_anim = true;
            result.anim_idx = anim_idx;
        }
    }

    return result;
}

/* ==================== 动画帧计算 ==================== */

int joy_scene_tilemap_get_anim_tileid(const joy_tilemap_p tm, int gid, uint32_t time_ms)
{
    joy_tilemap_gid_resolve_t res = joy_scene_tilemap_resolve_gid(tm, gid);
    if (!res.has_anim) return res.local_id;

    joy_tilemap_anim_t* anim = &kv_A(res.tileset->anims, (size_t)res.anim_idx);
    if (kv_size(anim->frames) == 0) return res.local_id;

    /* 计算当前帧 */
    int total = anim->total_duration;
    if (total <= 0) {
        /* 防御：没有总时长则取第一帧 */
        return kv_A(anim->frames, 0).tileid;
    }

    int elapsed = (int)(time_ms % (uint32_t)total);
    int accum = 0;
    for (size_t f = 0; f < kv_size(anim->frames); f++) {
        accum += kv_A(anim->frames, f).duration;
        if (elapsed < accum)
            return kv_A(anim->frames, f).tileid;
    }

    return kv_A(anim->frames, kv_size(anim->frames) - 1).tileid;
}

/* ==================== 纹理坐标计算 ==================== */

bool joy_scene_tilemap_get_tile_uv(const joy_tilemap_p tm, int gid,
                                   uint32_t time_ms, float out_rect[4])
{
    if (!tm || !out_rect) return false;

    memset(out_rect, 0, 4 * sizeof(float));
    if (gid <= 0) return false;

    joy_tilemap_gid_resolve_t res = joy_scene_tilemap_resolve_gid(tm, gid);
    if (!res.tileset) return false;

    joy_tilemap_tileset_p ts = res.tileset;

    /* 确定当前显示的局部瓦片 ID（考虑动画） */
    int local_id = res.local_id;
    if (res.has_anim) {
        local_id = joy_scene_tilemap_get_anim_tileid(tm, gid, time_ms);
    }

    /* 处理无列信息的 tileset（仅 deco 类型，每个瓦片独立图片） */
    if (ts->columns <= 0) {
        /* 这种 tileset 没有固定网格，无法用 columns 计算 UV，返回零矩形 */
        return false;
    }

    /* 计算行/列 */
    int col = local_id % ts->columns;
    int row = local_id / ts->columns;

    out_rect[0] = (float)(ts->margin + col * (ts->tilewidth  + ts->spacing));
    out_rect[1] = (float)(ts->margin + row * (ts->tileheight + ts->spacing));
    out_rect[2] = (float)ts->tilewidth;
    out_rect[3] = (float)ts->tileheight;

    return true;
}

/* ==================== 图层查询 ==================== */

joy_tilemap_tile_layer_p joy_scene_tilemap_find_tile_layer(const joy_tilemap_p tm,
                                                         const char* name)
{
    if (!tm || !name) return NULL;

    for (size_t i = 0; i < kv_size(tm->layers); i++) {
        joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
        if (base->type == JTILEMAP_LAYER_TILE && base->name &&
            strcmp(base->name, name) == 0) {
            return (joy_tilemap_tile_layer_p)base;
        }
    }
    return NULL;
}

joy_tilemap_object_layer_p joy_scene_tilemap_find_object_layer(const joy_tilemap_p tm,
                                                            const char* name)
{
    if (!tm || !name) return NULL;

    for (size_t i = 0; i < kv_size(tm->layers); i++) {
        joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
        if (base->type == JTILEMAP_LAYER_OBJECT && base->name &&
            strcmp(base->name, name) == 0) {
            return (joy_tilemap_object_layer_p)base;
        }
    }
    return NULL;
}

void joy_scene_tilemap_object_layer_foreach(joy_tilemap_object_layer_p ol, void (*callback)(joy_tilemap_object_p obj, void* arg), void*arg)
{
    if (ol) {
        for (size_t i = 0; i < kv_size(ol->objects); i++) {
            joy_tilemap_object_t* obj = &kv_A(ol->objects, i);
            if (obj->shape != JTILEMAP_SHAPE_RECT)
                continue;
            callback(obj, arg);
        }
        joy_core_log_info("[tilemap] loaded %zu collision walls", kv_size(ol->objects));
    }
}

/* 遍历对象层内全部对象（不过滤形状：point/ellipse/polyline 等也回调；用于出生点解析） */
void joy_scene_tilemap_object_layer_foreach_all(joy_tilemap_object_layer_p ol, void (*callback)(joy_tilemap_object_p obj, void* arg), void* arg)
{
    if (ol) {
        for (size_t i = 0; i < kv_size(ol->objects); i++) {
            joy_tilemap_object_t* obj = &kv_A(ol->objects, i);
            callback(obj, arg);
        }
    }
}

int joy_scene_tilemap_get_tile(const joy_tilemap_tile_layer_p layer, int tx, int ty)
{
    if (!layer || !layer->data) return 0;
    if (tx < 0 || tx >= layer->width || ty < 0 || ty >= layer->height)
        return 0;
    return layer->data[ty * layer->width + tx];
}




/* 返回输入串去掉前导 ./ 与分隔符后的偏移, 不复制。
 * 原来的 static char buf[1024] 被所有 render 实例共享, 且曾直接作为哈希表的
 * key 保存, destroy 时 free 静态数组会破坏堆。 */
static const char* default_remap(const char* path)
{
        if (!path || !*path) return path;
        while (*path == '.' || *path == '/' || *path == '\\') path++;
        return path;
}

/* 哈希表一律接管 key 的堆副本, destroy 才能无条件 free(key)。
 * 失败条目(value=NULL)也入表, 避免每帧重复尝试加载同一缺失资源。 */
static void tex_cache_store(joy_tilemap_render_p r, const char* path, joy_image_p img)
{
        char* key = joy_core_utils_strdup(path, NULL);
        if (!key) return;
        int ret;
        khiter_t ki = kh_put(tex_cache, r->tex_cache, key, &ret);
        if (ki == kh_end(r->tex_cache)) { free(key); return; }
        if (ret == 0) free(key);   /* 桶已存在, 沿用原 key, 不能替换 */
        kh_value(r->tex_cache, ki) = img;
}

static joy_image_p load_texture_internal(joy_tilemap_render_p r, const char* path)
{
        if (!path || !*path) return NULL;
        khiter_t k = kh_get(tex_cache, r->tex_cache, path);
        if (k != kh_end(r->tex_cache))
                return kh_value(r->tex_cache, k);

        char full_path[1024];
        if (r->asset_base)
                snprintf(full_path, sizeof(full_path), "%s%s", r->asset_base, path);
        else
                snprintf(full_path, sizeof(full_path), "%s", path);

        /* 走 SDL_LoadFile + stbi_load_from_memory，而不是 stbi_load(路径) ——
         * stbi_load 内部是 stdio 的 fopen，安卓上图集打包在 APK 内读不到，
         * 结果是每个 tileset 纹理都加载失败（渲染时退化成同一张/空白）。 */
        size_t file_sz = 0;
        stbi_uc* filedata = (stbi_uc*)SDL_LoadFile(full_path, &file_sz);
        if (!filedata) {
                tex_cache_store(r, path, NULL);
                return NULL;
        }

        int w = 0, h = 0, ch = 0;
        unsigned char* data = stbi_load_from_memory(filedata, (int)file_sz, &w, &h, &ch, 4);
        SDL_free(filedata);
        if (!data) {
                tex_cache_store(r, path, NULL);
                return NULL;
        }

        joy_image_p img = joy_r2d_image_create_from_data(r->r2d, data, w, h);
        stbi_image_free(data);
        if (!img) {
                tex_cache_store(r, path, NULL);
                return NULL;
        }

        tex_cache_store(r, path, img);
        return img;
}

static joy_image_p load_tileset_tex(joy_tilemap_render_p r, const joy_tilemap_tileset_t* ts)
{
        if (!ts || !ts->image) return NULL;
        const char* mapped = r->remap ? r->remap(ts->image) : ts->image;
        return load_texture_internal(r, mapped);
}

static void render_tile_layer_internal(joy_tilemap_render_p r, joy_tilemap_p tm,
        const joy_tilemap_tile_layer_p layer,
        float cam_x, float cam_y, uint32_t time_ms)
{
        if (!layer->base.visible) return;
        /* 每个格子的画布像素：tile_px 预设 >0 时以它为准（调用者可设成游戏 PPM），
         * 否则回退到 map.tilewidth（Tiled 默认网格），保证两种调用都正确。 */
        float tilew = (r->tile_px > 0.0f) ? r->tile_px : (float)tm->tilewidth;
        float tileh = (r->tile_px > 0.0f) ? r->tile_px : (float)tm->tileheight;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

        int sx = (int)floorf(cam_x / tilew); if (sx < 0) sx = 0;
        int sy = (int)floorf(cam_y / tileh); if (sy < 0) sy = 0;
        int ex = (int)ceilf((cam_x + r->view_w) / tilew) + 1; if (ex > layer->width)  ex = layer->width;
        int ey = (int)ceilf((cam_y + r->view_h) / tileh) + 1; if (ey > layer->height) ey = layer->height;

        uint8_t alpha = (uint8_t)(layer->base.opacity * 255.0f);

        for (int ty = sy; ty < ey; ty++) {
                for (int tx = sx; tx < ex; tx++) {
                        int raw_gid = joy_scene_tilemap_get_tile(layer, tx, ty);
                        if (raw_gid == 0) continue;
                        int gid = jtilemap_clean_gid(raw_gid);
                        int flags = jtilemap_gid_flags(raw_gid);
                        joy_tilemap_gid_resolve_t res = joy_scene_tilemap_resolve_gid(tm, gid);
                        if (!res.tileset) continue;

                        /* 瓦片网格步长取 map 的 tilewidth/tileheight（这是 Tiled 里
                         * 相邻格子的画布间距），而不是 tileset 的。Tiled 允许二者不同
                         * （map.tilewidth 决定网格，tileset.tilewidth 是瓦片原始像素），
                         * 此时瓦片按 map 网格排布、并缩放到 map.tile 尺寸显示。
                         */
                        float ts_tw = (float)res.tileset->tilewidth;   /* 瓦片原始像素 */
                        float ts_th = (float)res.tileset->tileheight;
                        float scale_x = tilew / ts_tw;                 /* map.grid / tile 像素，Tiled 会拉伸瓦片 */
                        float scale_y = tileh / ts_th;
                        float dst_x = tx * tilew - cam_x;
                        float dst_y = ty * tileh - cam_y;
                        float dst_w = ts_tw * scale_x;                 /* 缩放后目标尺寸 = map.tile 尺寸 */
                        float dst_h = ts_th * scale_y;
                        /* 抗缝隙：瓦片quad取整并各边+1px 重叠，避免非整数 ppm 下
                         * 相邻瓦片间露出底色形成白线/网格（近白地面透出）。 */
                        int dst_ix = (int)floorf(dst_x + 0.5f);
                        int dst_iy = (int)floorf(dst_y + 0.5f);
                        int dst_iw = (int)floorf(dst_w + 0.5f) + 1;
                        int dst_ih = (int)floorf(dst_h + 0.5f) + 1;

                        const char* deco_img = joy_scene_tilemap_get_tile_image(res.tileset, res.local_id);
                        if (deco_img) {
                                const char* mapped = r->remap ? r->remap(deco_img) : deco_img;
                                joy_image_p timg = load_texture_internal(r, mapped);
                                if (!timg) continue;
                                joy_r2d_image_set_alpha(timg, alpha);
                                int dw = dst_iw, dh = dst_ih;
                                joy_scene_tilemap_get_tile_def_size(res.tileset, res.local_id, &dw, &dh);
                                joy_r2d_image_draw1(timg, NULL, &(joy_rect_t){ (float)dst_ix, (float)dst_iy, (float)dw, (float)dh });
                                continue;
                        }

                        float uv[4];
                        if (!joy_scene_tilemap_get_tile_uv(tm, gid, time_ms, uv)) continue;

                        joy_image_p timg = load_tileset_tex(r, res.tileset);
                        if (!timg) {
                                joy_r2d_set_color(r->r2d, 255, 0, 255, 255);
                                joy_r2d_shape_draw_rectangle(r->r2d, "fill", (joy_rect_t) { (float)dst_ix, (float)dst_iy, (float)dst_iw, (float)dst_ih });
                                continue;
                        }

                        joy_r2d_image_set_alpha(timg, alpha);
                        joy_rect_t src = { uv[0], uv[1], uv[2], uv[3] };

                        /* Tiled 翻转标志（bit D/H/V）→ SDL 旋转角 + 水平/垂直翻转。
                         * 官方语义（正交地图）：先对角镜像（transpose，x/y 互换），
                         * 再水平翻转，再垂直翻转。用 (angle, flip_h, flip_v) 精确表达
                         * 这 8 种组合（暴力枚举见 _test_flip.py）：
                         *   D=0:  angle=0°，flip_h=H, flip_v=V
                         *   D=1:  angle=90°，flip_h=V, flip_v=!H
                         * joy_r2d_image_draw_ex 的 rotation 参数按弧度处理（内部 *180/π）。
                         */
                        bool tiled_h = (flags & JTILEMAP_FLIP_H) != 0;
                        bool tiled_v = (flags & JTILEMAP_FLIP_V) != 0;
                        bool tiled_d = (flags & JTILEMAP_FLIP_D) != 0;

                        bool sdl_flip_h, sdl_flip_v;
                        float angle_rad;
                        if (!tiled_d) {
                                angle_rad = 0.0f;
                                sdl_flip_h = tiled_h;
                                sdl_flip_v = tiled_v;
                        } else {
                                angle_rad = 90.0f * 3.1415926f / 180.0f;
                                sdl_flip_h = tiled_v;
                                sdl_flip_v = !tiled_h;
                        }

                        /* 旋转/翻转绕瓦片中心：position 传目标中心，origin 传源中心。 */
                        joy_vec2_t pos = { (float)dst_ix + dst_iw * 0.5f, (float)dst_iy + dst_ih * 0.5f };
                        joy_vec2_t org = { ts_tw * 0.5f, ts_th * 0.5f };
                        joy_vec2_t scl = {
                                (sdl_flip_h ? -1.0f : 1.0f) * ((float)dst_iw / ts_tw),
                                (sdl_flip_v ? -1.0f : 1.0f) * ((float)dst_ih / ts_th)
                        };
                        joy_r2d_image_draw_ex(timg, &src, pos, angle_rad, scl, org);
                }
        }
}

static void render_object_layer_internal(const joy_tilemap_object_layer_p layer,
        float cam_x, float cam_y, joy_r2d_p r2d)
{
        if (!layer->base.visible) return;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;
        joy_r2d_set_color(r2d, 255, 85, 0, 255);
        for (size_t i = 0; i < kv_size(layer->objects); i++) {
                joy_tilemap_object_t* obj = &kv_A(layer->objects, i);
                if (!obj->visible || obj->shape != JTILEMAP_SHAPE_RECT) continue;
                if (obj->width <= 0.0f || obj->height <= 0.0f || obj->width > 10000.0f || obj->height > 10000.0f) continue;
                joy_r2d_shape_draw_rectangle(r2d, "line", (joy_rect_t) { obj->x - cam_x, obj->y - cam_y, obj->width, obj->height });
        }
}

static void render_image_layer_internal(joy_tilemap_render_p r,
        const joy_tilemap_image_layer_p layer,
        float cam_x, float cam_y)
{
        if (!layer->base.visible || !layer->image) return;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

        const char* mapped = r->remap ? r->remap(layer->image) : layer->image;
        joy_image_p timg = load_texture_internal(r, mapped);
        if (!timg) return;

        float tex_w = 0, tex_h = 0;
        joy_r2d_image_get_size(timg, &tex_w, &tex_h);

        float vp_w = r->view_w, vp_h = r->view_h;
        float start_x = (layer->repeatx ? fmodf(-cam_x, tex_w) : -cam_x);
        float start_y = (layer->repeaty ? fmodf(-cam_y, tex_h) : -cam_y);
        if (start_x > 0) start_x -= tex_w;
        if (start_y > 0) start_y -= tex_h;

        for (float py = start_y; py < vp_h; py += tex_h)
                for (float px = start_x; px < vp_w; px += tex_w)
                        joy_r2d_image_draw1(timg, NULL, &(joy_rect_t){ px, py, tex_w, tex_h });
}

/* ======================== 公共 API ======================== */

joy_tilemap_render_p joy_scene_tilemap_render_create(joy_r2d_p renderer,
        const char* asset_base_path,
        joy_tilemap_path_remap_t remap)
{
        joy_tilemap_render_p r = (joy_tilemap_render_p)calloc(1, sizeof(joy_tilemap_render_t));
        if (!r) return NULL;
        r->r2d = renderer;
        r->remap = remap ? remap : default_remap;
        r->asset_base = asset_base_path ? joy_core_utils_strdup(asset_base_path, NULL) : NULL;
        r->view_w = 1024.0f;
        r->view_h = 760.0f;
        r->tile_px = 0.0f;   /* 默认 0：用 map.tilewidth 做每格像素 */
        r->tex_cache = kh_init(tex_cache);
        return r;
}

void joy_scene_tilemap_render_set_tile_px(joy_tilemap_render_p r, float px)
{
        if (r) r->tile_px = px;
}

void joy_scene_tilemap_render_all(joy_tilemap_render_p r, joy_tilemap_p tm,
        float camera_x, float camera_y,
        uint32_t time_ms)
{
        if (!r || !tm || !r->r2d) return;

        {
                int vp_w = 0, vp_h = 0, mode;
                joy_r2d_get_logical_presentation(r->r2d, &vp_w, &vp_h, &mode);
                if (vp_w <= 0 || vp_h <= 0)
                        joy_r2d_get_size(r->r2d, &vp_w, &vp_h);
                r->view_w = (float)vp_w;
                r->view_h = (float)vp_h;
        }

        for (size_t i = 0; i < kv_size(tm->layers); i++) {
                joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
                switch (base->type) {
                case JTILEMAP_LAYER_TILE:
                        render_tile_layer_internal(r, tm, (joy_tilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
                        break;
                case JTILEMAP_LAYER_OBJECT:
                        render_object_layer_internal((joy_tilemap_object_layer_p)base, camera_x, camera_y, r->r2d);
                        break;
                case JTILEMAP_LAYER_IMAGE:
                        render_image_layer_internal(r, (joy_tilemap_image_layer_p)base, camera_x, camera_y);
                        break;
                }
        }
}

/* 只渲染瓦片层（Ground/Object 等），跳过对象层（Collision/Player 的调试线框）与图片层。
 * 运行时关卡渲染应使用本接口：对象的碰撞/出生线框属于编辑期辅助，不该出现在游戏画面里。 */
void joy_scene_tilemap_render_tiles(joy_tilemap_render_p r, joy_tilemap_p tm,
        float camera_x, float camera_y,
        uint32_t time_ms)
{
        if (!r || !tm || !r->r2d) return;

        {
                int vp_w = 0, vp_h = 0, mode;
                joy_r2d_get_logical_presentation(r->r2d, &vp_w, &vp_h, &mode);
                if (vp_w <= 0 || vp_h <= 0)
                        joy_r2d_get_size(r->r2d, &vp_w, &vp_h);
                r->view_w = (float)vp_w;
                r->view_h = (float)vp_h;
        }

        for (size_t i = 0; i < kv_size(tm->layers); i++) {
                joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
                if (base->type == JTILEMAP_LAYER_TILE)
                        render_tile_layer_internal(r, tm, (joy_tilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
        }
}

bool joy_scene_tilemap_render_layer(joy_tilemap_render_p r, joy_tilemap_p tm,
        const char* layer_name,
        float camera_x, float camera_y,
        uint32_t time_ms)
{
        if (!r || !tm || !r->r2d || !layer_name) return false;

        for (size_t i = 0; i < kv_size(tm->layers); i++) {
                joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
                if (!base->name || strcmp(base->name, layer_name) != 0) continue;
                switch (base->type) {
                case JTILEMAP_LAYER_TILE:
                        render_tile_layer_internal(r, tm, (joy_tilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
                        return true;
                case JTILEMAP_LAYER_OBJECT:
                        render_object_layer_internal((joy_tilemap_object_layer_p)base, camera_x, camera_y, r->r2d);
                        return true;
                case JTILEMAP_LAYER_IMAGE:
                        render_image_layer_internal(r, (joy_tilemap_image_layer_p)base, camera_x, camera_y);
                        return true;
                }
        }
        return false;
}

void joy_scene_tilemap_render_destroy(joy_tilemap_render_p r)
{
        if (!r) return;
        if (r->tex_cache) {
                const char* key;
                joy_image_p timg;
                kh_foreach(r->tex_cache, key, timg, {
                        if (timg) joy_r2d_image_destroy(timg);
                        free((void*)key);
                        });
                kh_destroy(tex_cache, r->tex_cache);
        }
        if (r->asset_base) free(r->asset_base);
        free(r);
}

/* ==================== 结束 src/jtilemap.c ==================== */

/* ==================== 合并自 src/jecs.c ==================== */
#include "SDL3/SDL.h"
#include "external/klib/khash.h"
#include "external/klib/klist.h"
#include "external/klib/kvec.h"
#include "joy.h"

// ==================== Bitset 实现 ====================
typedef struct joy_bitset {
        uint64_t* words;
        int nwords;      // 当前 word 数量
        int capacity;    // 当前能表示的最大实体 ID + 1
} joy_bitset_t;

static joy_bitset_t* bitset_create(int initial_capacity) {
        joy_bitset_t* bs = SDL_malloc(sizeof(joy_bitset_t));
        // 初始化容量, 向上取整为 64 的倍数
        bs->nwords = (initial_capacity + 63) / 64;
        bs->capacity = bs->nwords * 64;
        bs->words = SDL_calloc(bs->nwords, sizeof(uint64_t));
        return bs;
}

static void bitset_destroy(joy_bitset_t* bs) {
        if (bs) {
                SDL_free(bs->words);
                SDL_free(bs);
        }
}

static void bitset_ensure_capacity(joy_bitset_t* bs, int min_capacity) {
        if (min_capacity > bs->capacity) {
                int new_nwords = (min_capacity + 63) / 64;
                uint64_t* new_words = SDL_realloc(bs->words, new_nwords * sizeof(uint64_t));
                // 新分配的位清零
                for (int i = bs->nwords; i < new_nwords; i++) {
                        new_words[i] = 0;
                }
                bs->words = new_words;
                bs->nwords = new_nwords;
                bs->capacity = new_nwords * 64;
        }
}

static void bitset_set(joy_bitset_t* bs, int id) {
        if (id < 0) return;
        bitset_ensure_capacity(bs, id + 1);
        int word = id / 64;
        int bit = id % 64;
        bs->words[word] |= (1ULL << bit);
}

static void bitset_clear(joy_bitset_t* bs, int id) {
        if (id < 0 || id >= bs->capacity) return;
        int word = id / 64;
        int bit = id % 64;
        bs->words[word] &= ~(1ULL << bit);
}

static bool bitset_test(joy_bitset_t* bs, int id) {
        if (id < 0 || id >= bs->capacity) return false;
        int word = id / 64;
        int bit = id % 64;
        return (bs->words[word] & (1ULL << bit)) != 0;
}

static void bitset_and(joy_bitset_t* dest, joy_bitset_t* src) {
        int min_words = dest->nwords < src->nwords ? dest->nwords : src->nwords;
        for (int i = 0; i < min_words; i++) {
                dest->words[i] &= src->words[i];
        }
        // dest 中超出 src 的部分全部清零
        for (int i = min_words; i < dest->nwords; i++) {
                dest->words[i] = 0;
        }
}

static void bitset_copy(joy_bitset_t* dest, joy_bitset_t* src) {
        bitset_ensure_capacity(dest, src->capacity);
        int words_to_copy = dest->nwords < src->nwords ? dest->nwords : src->nwords;
        SDL_memcpy(dest->words, src->words, words_to_copy * sizeof(uint64_t));
        // 若 dest 更大, 多余部分保持原样
}

// ==================== Sparse Set with Bitset ====================
KHASH_INIT(ksparse, int, int, 1, kh_int_hash_func, kh_int_hash_equal)

typedef struct joy_sparse_set {
        kvec_t(int) density;
        khash_t(ksparse)* sparse;
        joy_bitset_t* bitset;  // 辅助位图
} joy_sparse_set_t, * joy_sparse_set_p;

static joy_sparse_set_p sparse_set_create() {
        joy_sparse_set_p sparse_set = (joy_sparse_set_p)SDL_malloc(sizeof(joy_sparse_set_t));
        kv_init(sparse_set->density);
        sparse_set->sparse = kh_init(ksparse);
        sparse_set->bitset = bitset_create(256);  // 初始支持 256 个实体
        return sparse_set;
}

static void sparse_set_destroy(joy_sparse_set_p sparse_set) {
        kv_destroy(sparse_set->density);
        kh_destroy(ksparse, sparse_set->sparse);
        bitset_destroy(sparse_set->bitset);
        SDL_free(sparse_set);
}

static bool sparse_set_contain(joy_sparse_set_p sparse_set, int value) {
        khint_t k = kh_get(ksparse, sparse_set->sparse, value);
        return k != kh_end(sparse_set->sparse);
}

static void sparse_set_remove(joy_sparse_set_p sparse_set, int value) {
        khint_t k = kh_get(ksparse, sparse_set->sparse, value);
        if (k != kh_end(sparse_set->sparse)) {
                int index = kh_val(sparse_set->sparse, k);
                int len = kv_size(sparse_set->density);
                int last_value = kv_A(sparse_set->density, len - 1);

                kv_A(sparse_set->density, index) = last_value;
                kv_pop(sparse_set->density);

                kh_del(ksparse, sparse_set->sparse, k);
                bitset_clear(sparse_set->bitset, value);

                // 更新被移动元素的 sparse 映射
                if (value != last_value) {
                        khint_t k2 = kh_get(ksparse, sparse_set->sparse, last_value);
                        if (k2 != kh_end(sparse_set->sparse)) {
                                kh_val(sparse_set->sparse, k2) = index;
                        }
                }
        }
}

static void sparse_set_insert(joy_sparse_set_p sparse_set, int value) {
        if (!sparse_set_contain(sparse_set, value)) {
                kv_push(int, sparse_set->density, value);
                int index = kv_size(sparse_set->density) - 1;
                int ret;
                khint_t k = kh_put(ksparse, sparse_set->sparse, value, &ret);
                kh_val(sparse_set->sparse, k) = index;
                bitset_set(sparse_set->bitset, value);
        }
}


typedef struct joy_ecs_component_data {
        size_t size;
        void* data;
}joy_ecs_component_data_t, *joy_ecs_component_data_p;

typedef unsigned long joy_ecs_cid_t;
#define __int_free(x)
KLIST_INIT(kecs_system, joy_ecs_system_t, __int_free)

// 组件存储: 内含稀疏集合与位图
typedef struct joy_component_store {
        joy_sparse_set_p sparse_set;
        joy_bitset_t* bitset;
} joy_component_store_t, *joy_component_store_p;

KHASH_INIT(kecs_component, joy_ecs_cid_t, joy_component_store_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kecs_component_data, joy_ecs_cid_t, joy_ecs_component_data_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kecs_meta, joy_ecs_cid_t, size_t, 1, kh_int_hash_func, kh_int_hash_equal)

typedef khash_t(kecs_component_data) ecs_entity_t, * joy_ecs_entity_p;
KHASH_INIT(kecs_entity, joy_ecs_id_t, joy_ecs_entity_p, 1, kh_int_hash_func, kh_int_hash_equal)

struct joy_ecs_world {
        joy_ecs_id_t global_id;
        khash_t(kecs_meta)* metas;
        klist_t(kecs_system)* systems;
        khash_t(kecs_component)* components;
        khash_t(kecs_entity)* entities;
};

joy_ecs_world_p joy_scene_ecs_create()
{
        joy_ecs_world_p world = (joy_ecs_world_p)SDL_malloc(sizeof(joy_ecs_world_t));
        world->metas = kh_init(kecs_meta);
        world->systems = kl_init(kecs_system);
        world->components = kh_init(kecs_component);
        world->entities = kh_init(kecs_entity);
        world->global_id = 1;
        return world;
}


void joy_scene_ecs_destroy(joy_ecs_world_p world)
{
        joy_ecs_id_t id;
        joy_ecs_entity_p entity;
        khint_t i, j, p;
        joy_sparse_set_p sparse_set;
        joy_ecs_component_data_p component_data;

        // 销毁实体
        for (khint_t i = kh_begin(world->entities); i != kh_end(world->entities); i++) {
                if (kh_exist(world->entities, i)) {
                        joy_ecs_entity_p entity = kh_val(world->entities, i);
                        // 销毁实体的所有组件
                        for (khint_t j = kh_begin(entity); j != kh_end(entity); j++) {
                                if (kh_exist(entity, j)) {
                                        component_data = kh_val(entity, j);
                                        SDL_free(component_data->data);
                                        SDL_free(component_data);
                                }
                        }
                        kh_destroy(kecs_component_data, entity);
                }
        }

        // 销毁组件存储: 稀疏集合与 store->bitset 是两份独立分配, 都要释放
        for (khint_t i = kh_begin(world->components); i != kh_end(world->components); i++) {
                if (kh_exist(world->components, i)) {
                        joy_component_store_t* store = kh_val(world->components, i);
                        sparse_set_destroy(store->sparse_set);
                        bitset_destroy(store->bitset);
                        SDL_free(store);
                }
        }

        kl_destroy(kecs_system, world->systems);
        kh_destroy(kecs_meta, world->metas);
        kh_destroy(kecs_entity, world->entities);
        kh_destroy(kecs_component, world->components);
        SDL_free(world);
}

void joy_scene_ecs_process(joy_ecs_world_p world, float dt)
{
        joy_ecs_system_t system;
        kliter_t(kecs_system)* p;
        p = kl_begin(world->systems);
        while (p != kl_end(world->systems)) {
                system = kl_val(p);
                system.cb(world, system.arg, dt);
                p = kl_next(p);
        }
}

joy_ecs_id_t joy_scene_ecs_spawn(joy_ecs_world_p world)
{
        joy_ecs_id_t id;
        khint_t k;
        int ret;
        id = world->global_id++;
        k = kh_put(kecs_entity, world->entities, id, &ret);
        kh_val(world->entities, k) = kh_init(kecs_component_data);
        return id;
}

void joy_scene_ecs_kill(joy_ecs_world_p world, joy_ecs_id_t id)
{
        joy_ecs_entity_p entity;
        joy_component_store_t* store;
        joy_ecs_cid_t component_id;
        khint_t k, p;
        joy_ecs_component_data_p component_data;

        k = kh_get(kecs_entity, world->entities, id);
        if (k == kh_end(world->entities)) {
                return;
        }

        entity = kh_val(world->entities, k);
        kh_del(kecs_entity, world->entities, k);

        // 从组件存储中移除该实体
        for (k = kh_begin(entity); k != kh_end(entity); k++) {
                if (kh_exist(entity, k)) {
                        component_id = kh_key(entity, k);
                        component_data = kh_val(entity, k);
                        SDL_free(component_data->data);
                        SDL_free(component_data);

                        p = kh_get(kecs_component, world->components, component_id);
                        if (p != kh_end(world->components)) {
                                store = kh_val(world->components, p);
                                sparse_set_remove(store->sparse_set, id);
                                bitset_clear(store->bitset, id);
                        }
                }
        }
        kh_destroy(kecs_component_data, entity);
}

void joy_scene_ecs_define(joy_ecs_world_p world, const char* name, size_t size)
{
        joy_ecs_cid_t component_id;
        khint_t k;
        int ret;
        component_id = joy_core_murmur3_hash(name);
        k = kh_get(kecs_meta, world->metas, component_id);
        if (k == kh_end(world->metas)) {
                k = kh_put(kecs_meta, world->metas, component_id, &ret);
                kh_val(world->metas, k) = size;
        }
}

void joy_scene_ecs_set(joy_ecs_world_p world, joy_ecs_id_t id, const char* name, const void* ptr, size_t size)
{
        joy_ecs_entity_p entity;
        joy_ecs_cid_t component_id;
        joy_component_store_t* store;
        size_t meta;
        khint_t k;
        joy_ecs_component_data_p component_data;
        int ret;

        component_id = joy_core_murmur3_hash(name);
        k = kh_get(kecs_meta, world->metas, component_id);
        if (k == kh_end(world->metas))
                return;
        meta = kh_val(world->metas, k);

        k = kh_get(kecs_entity, world->entities, id);
        if (k == kh_end(world->entities))
                return;

        entity = kh_val(world->entities, k);
        k = kh_get(kecs_component_data, entity, component_id);

        if (k == kh_end(entity)) {
                /* 缓冲按注册时的 meta 分配, 拷贝长度不能超过它, 否则越界写 */
                size_t copy_size = size < meta ? size : meta;
                component_data = (joy_ecs_component_data_p)SDL_malloc(sizeof(joy_ecs_component_data_t));
                component_data->data = SDL_malloc(meta);
                component_data->size = copy_size;
                SDL_memcpy(component_data->data, ptr, copy_size);
                k = kh_put(kecs_component_data, entity, component_id, &ret);
                kh_val(entity, k) = component_data;

                // 更新组件存储
                k = kh_get(kecs_component, world->components, component_id);
                if (k == kh_end(world->components)) {
                        k = kh_put(kecs_component, world->components, component_id, &ret);
                        store = (joy_component_store_p)SDL_malloc(sizeof(joy_component_store_t));
                        store->sparse_set = sparse_set_create();
                        store->bitset = bitset_create(world->global_id + 64); // 预留空间
                        kh_val(world->components, k) = store;
                }
                else {
                        store = kh_val(world->components, k);
                }

                sparse_set_insert(store->sparse_set, id);
                bitset_set(store->bitset, id);
        }
        else {
                size_t copy_size = size < meta ? size : meta;
                component_data = kh_val(entity, k);
                component_data->size = copy_size;
                SDL_memcpy(component_data->data, ptr, copy_size);
        }
}

size_t joy_scene_ecs_get(joy_ecs_world_p world, joy_ecs_id_t id, const char* name, void** data)
{
        joy_ecs_cid_t component_id;
        joy_ecs_entity_p entity;
        joy_ecs_component_data_p component_data;
        khint_t k;
        component_id = joy_core_murmur3_hash(name);
        k = kh_get(kecs_entity, world->entities, id);
        if (k == kh_end(world->entities)) {
                return 0;
        }
        entity = kh_val(world->entities, k);
        k = kh_get(kecs_component_data, entity, component_id);
        if (k == kh_end(entity)) {
                return 0;
        }
        component_data = kh_val(entity, k);
        *data = component_data->data;
        return component_data->size;
}

void joy_scene_ecs_remove(joy_ecs_world_p world, joy_ecs_id_t id, const char* name)
{
        joy_ecs_entity_p entity;
        joy_component_store_t* store;
        joy_ecs_component_data_p component_data;
        khint_t k;

        joy_ecs_cid_t component_id = joy_core_murmur3_hash(name);
        k = kh_get(kecs_entity, world->entities, id);
        if (k != kh_end(world->entities)) {
                entity = kh_val(world->entities, k);
                k = kh_get(kecs_component_data, entity, component_id);
                if (k != kh_end(entity)) {
                        component_data = kh_val(entity, k);
                        SDL_free(component_data->data);
                        SDL_free(component_data);
                        kh_del(kecs_component_data, entity, k);
                }
        }

        k = kh_get(kecs_component, world->components, component_id);
        if (k != kh_end(world->components)) {
                store = kh_val(world->components, k);
                sparse_set_remove(store->sparse_set, id);
                bitset_clear(store->bitset, id);
        }
}
//
//static joy_vector_t(kecs)
//_intersection(joy_ecs_id_t* a_arr, int a_arr_len, joy_ecs_id_t* b_arr, int b_arr_len)
//{
//        joy_vector_t(kecs) vec;
//        joy_ecs_id_t a, b;
//
//        vector_init(kecs, &vec);
//        for (size_t i = 0; i < a_arr_len; i++) {
//                a = a_arr[i];
//                for (size_t j = 0; j < b_arr_len; j++) {
//                        b = b_arr[j];
//                        if (a == b) {
//                                vector_push(kecs, &vec, a);
//                                break;
//                        }
//                }
//        }
//
//        return vec;
//}

joy_ecs_id_array_t joy_scene_ecs_query(joy_ecs_world_p world, int arg_cnt, ...)
{
        joy_ecs_id_array_t result = { 0 };
        if (arg_cnt <= 0) {
                return result;
        }

        char** args = (char**)SDL_malloc(arg_cnt * sizeof(const char*));
        va_list va;
        va_start(va, arg_cnt);

        for (int i = 0; i < arg_cnt; i++) {
                args[i] = (char*)va_arg(va, const char*);
        }
        va_end(va);

        result = joy_scene_ecs_queryx(world, arg_cnt, args);
        SDL_free(args);

        return result;
}

joy_ecs_id_array_t joy_scene_ecs_queryx(joy_ecs_world_p world, int arg_cnt, char**args)
{
        joy_ecs_id_array_t result = { 0 };

        if (arg_cnt <= 0) {
                return result;
        }

        joy_bitset_t* temp_bitset = bitset_create(world->global_id + 64);
        joy_bitset_t* result_bitset = bitset_create(world->global_id + 64);
        bool first = true;

        for (int i = 0; i < arg_cnt; ++i) {
                const char* comp_name = args[i];
                joy_ecs_cid_t component_id = joy_core_murmur3_hash(comp_name);
                khint_t k = kh_get(kecs_component, world->components, component_id);

                if (k == kh_end(world->components)) {
                        bitset_destroy(temp_bitset);
                        bitset_destroy(result_bitset);
                        return result;
                }

                joy_component_store_t* store = kh_val(world->components, k);

                if (first) {
                        bitset_copy(result_bitset, store->bitset);
                        first = false;
                }
                else {
                        bitset_copy(temp_bitset, store->bitset);
                        bitset_and(result_bitset, temp_bitset);
                }
        }

        if (!first) {
                const char* first_comp = args[0];
                joy_ecs_cid_t first_cid = joy_core_murmur3_hash(first_comp);
                khint_t k = kh_get(kecs_component, world->components, first_cid);

                if (k != kh_end(world->components)) {
                        joy_component_store_t* store = kh_val(world->components, k);
                        joy_sparse_set_p sparse_set = store->sparse_set;

                        for (int i = 0; i < kv_size(sparse_set->density); i++) {
                                /* 结果是固定容量数组, 命中数超上限就截断, 不能越界写 */
                                if (result.num >= (int)(sizeof(result.ids) / sizeof(result.ids[0])))
                                        break;
                                joy_ecs_id_t id = kv_A(sparse_set->density, i);
                                if (bitset_test(result_bitset, id)) {
                                        result.ids[result.num++] = id;
                                }
                        }
                }
        }

        bitset_destroy(temp_bitset);
        bitset_destroy(result_bitset);
        return result;
}


void joy_scene_ecs_register(joy_ecs_world_p world, joy_ecs_system_t system)
{
        *kl_pushp(kecs_system, world->systems) = system;
}


typedef struct joy_position {
        float x, y;
}joy_position_t;

typedef struct joy_velocity {
        float x, y;
}joy_velocity_t;


int joy_scene_ecs_test()
{
       // joy_sparse_set_p sparse_set;
       // sparse_set = sparse_set_create();

       // for (int i = 0; i < 100000; i++) {
       //         sparse_set_insert(sparse_set, i);
       // }

       // sparse_set_remove(sparse_set, 1000);

       // int index = 1000;
       // bool found = false;
       ///* if (!sparse_set_contian(sparse_set, index)) {
       //         SDL_Log("%d not exists", index);
       //         for (size_t i = 0; i < kv_size(sparse_set->density); i++) {
       //                 int value = kv_A(sparse_set->density, i);
       //                 if (value == index) {
       //                         found = true;
       //                         break;
       //                 }

       //         }

       //         if (!found) {
       //                 SDL_Log("%d not exists 2", index);
       //         }
       // }
       // sparse_set_destroy(sparse_set);*/


       // joy_ecs_world_p world;
       // world = ecs_create();
       // ecs_define(world, "joy_position_t", sizeof(joy_position_t));
       // ecs_define(world, "joy_velocity_t", sizeof(joy_velocity_t));

       // joy_ecs_id_t id1 = ecs_spawn(world);
       // joy_ecs_id_t id2 = ecs_spawn(world);
       // /*ecs_set(world, id1, "joy_position_t", &(joy_position_t){0.2, 0.3});
       // ecs_set(world, id1, "joy_velocity_t", &(joy_velocity_t){2.2, 4.3});
       // ecs_set(world, id2, "joy_position_t", &(joy_position_t){2.2, 3.3});
       // ecs_set(world, id2, "joy_velocity_t", &(joy_velocity_t){3.3, 4.4});*/

       // ecs_remove(world, id1, "joy_position_t");
       // //ecs_set(world, id1, "joy_velocity_t", &(joy_velocity_t){2, 5});
       // //ecs_kill(world, id2);

       // /*  joy_vector_t entities = ecs_query(world, 2, "joy_position_t", "joy_velocity_t");
       //   for (int i = 0; i < vector_size(&entities); i++) {
       //           joy_ecs_id_t id = vector_a(&entities, i);
       //           joy_position_t* position = (joy_position_t*)ecs_get(world, id, "joy_position_t");
       //           joy_velocity_t* velocity = (joy_velocity_t*)ecs_get(world, id, "joy_velocity_t");
       //           SDL_Log("%d <%f,%f> <%f,%f>", id, position->x, position->y, velocity->x, velocity->y);
       //   }*/
       // joy_vector_t(kecs) entities = ecs_query(world, 1, "joy_position_t");
       // for (int i = 0; i < vector_size(kecs, &entities); i++) {
       //         joy_ecs_id_t id = vector_a(kecs, &entities, i);
       //         //joy_position_t* position = (joy_position_t*)ecs_get(world, id, "joy_position_t");
       //         //SDL_Log("%d <%f,%f>", id, position->x, position->y);
       // }



       // ecs_destroy(world);
        return 0;
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef __int_free

/* ==================== 结束 src/jecs.c ==================== */

/* ==================== 合并自 src/jprocmap.c ==================== */
/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jprocmap.c
Description: 程序化地图生成（BSP 房间 + 门洞版）实现
Author: ydlc / joy
Date: 2026.8.29
History:
        2026.8.29  从 project_proxy3d/shared/systems/procedural_map.h 迁移并参数化。
        所有坐标用"十分之一米"整数；生成结果仅由 seed 决定 → 完全确定性，可用作帧同步依据。
        参数（网格尺寸/出生安全区/障碍密度与高度/簇大小范围）全部集中在 joy_pm_config_t 中。
*************************************************/

#include "joy.h"

/* ============ 内部辅助 ============ */

static int jpm_rand_mod(joy_random_num_p rng, int mod)
{
        if (mod <= 0) return 0;
        return (int)(joy_core_utils_random_next(rng) % (uint32_t)mod);
}

/* 格 → 世界坐标（十分之一米）：世界原点落在网格中心附近，与默认 config 完全一致 */
static int jpm_grid_center10(const joy_pm_config_t* cfg, int g)
{
        return (g - cfg->grid_n / 2) * 10 + 5;
}

/* 该格中心是否落入任意出生安全区 */
static bool jpm_in_spawn_zone(const joy_pm_config_t* cfg, int gx, int gz)
{
        int x10 = jpm_grid_center10(cfg, gx);
        int z10 = jpm_grid_center10(cfg, gz);
        for (int s = 0; s < cfg->spawn_count; s++) {
                const joy_pm_spawn_t* sp = &cfg->spawns[s];
                int dx = x10 - sp->cx10, dz = z10 - sp->cz10;
                if (dx * dx + dz * dz < sp->radius10 * sp->radius10) return true;
        }
        return false;
}

/* 整数栅格化（含 pad 外扩）：把我方障碍实体重建为可通行栅格。内部使用 joy_scene_procmap_rasterize 的统一口径 */
static void jpm_rasterize_impl(const joy_pm_config_t* cfg, bool walkable[JPM_MAX_CELLS],
                               const joy_pm_obstacle_t* obs, int n)
{
        int grid_n = cfg->grid_n;
        for (int i = 0; i < grid_n * grid_n; i++) walkable[i] = true;

        const int offset10 = cfg->offset10;
        const int pad10    = cfg->pad10;
        for (int k = 0; k < n; k++) {
                const joy_pm_obstacle_t* o = &obs[k];
                int gx0 = (o->x10 - o->hx10 - pad10 + offset10) / 10;
                int gx1 = (o->x10 + o->hx10 + pad10 + offset10) / 10;
                int gz0 = (o->z10 - o->hz10 - pad10 + offset10) / 10;
                int gz1 = (o->z10 + o->hz10 + pad10 + offset10) / 10;
                if (gx0 < 0) gx0 = 0; if (gx1 > grid_n - 1) gx1 = grid_n - 1;
                if (gz0 < 0) gz0 = 0; if (gz1 > grid_n - 1) gz1 = grid_n - 1;
                for (int gz = gz0; gz <= gz1; gz++)
                        for (int gx = gx0; gx <= gx1; gx++)
                                walkable[gz * grid_n + gx] = false;
        }
}

/* 4 方向 BFS：返回从起点可达的开放格总数（用于连通性校验） */
static int jpm_bfs_impl(const bool walkable[JPM_MAX_CELLS], int grid_n, int sx, int sz)
{
        int queue[JPM_MAX_CELLS];
        int head = 0, tail = 0;
        bool visited[JPM_MAX_CELLS];
        for (int i = 0; i < grid_n * grid_n; i++) visited[i] = false;

        int start = sz * grid_n + sx;
        if (start < 0 || start >= grid_n * grid_n || !walkable[start]) return 0;
        queue[tail++] = start;
        visited[start] = true;
        const int dx4[4] = { 1, -1, 0, 0 };
        const int dz4[4] = { 0, 0, 1, -1 };
        while (head < tail) {
                int cur = queue[head++];
                int cx = cur % grid_n, cz = cur / grid_n;
                for (int d = 0; d < 4; d++) {
                        int nx = cx + dx4[d], nz = cz + dz4[d];
                        if (nx < 0 || nx >= grid_n || nz < 0 || nz >= grid_n) continue;
                        int ni = nz * grid_n + nx;
                        if (!walkable[ni] || visited[ni]) continue;
                        visited[ni] = true;
                        queue[tail++] = ni;
                }
        }
        return tail;
}

/* 连接每一格与所属障碍（重叠格取最小编号，确定性）；开放格为 -1 */
static void jpm_build_cell_mask(const joy_pm_config_t* cfg, const joy_pm_obstacle_t* obs, int n,
                                int cellobs[JPM_MAX_CELLS])
{
        int grid_n = cfg->grid_n;
        const int offset10 = cfg->offset10, pad10 = cfg->pad10;
        for (int i = 0; i < grid_n * grid_n; i++) cellobs[i] = -1;
        for (int k = 0; k < n; k++) {
                const joy_pm_obstacle_t* o = &obs[k];
                int x0 = (o->x10 - o->hx10 - pad10 + offset10) / 10;
                int x1 = (o->x10 + o->hx10 + pad10 + offset10) / 10;
                int z0 = (o->z10 - o->hz10 - pad10 + offset10) / 10;
                int z1 = (o->z10 + o->hz10 + pad10 + offset10) / 10;
                if (x0 < 0) x0 = 0; if (x1 > grid_n - 1) x1 = grid_n - 1;
                if (z0 < 0) z0 = 0; if (z1 > grid_n - 1) z1 = grid_n - 1;
                for (int zz = z0; zz <= z1; zz++)
                        for (int xx = x0; xx <= x1; xx++) {
                                int i = zz * grid_n + xx;
                                if (cellobs[i] == -1) cellobs[i] = k;
                        }
        }
}

/* 连通性修复：从玩家主区穿越障碍做多源 BFS，找到连接主区与"封闭口袋"的最小
 * 卷积障碍链，把链上的障碍整块移除，直到所有开放格均与玩家可达。
 * 必须整块移除障碍实体（非仅凿开格子图）——游戏运行时可通行网格是从障碍碰撞体
 * 重新栅格化的（不读本布局 walkable），只有真删障碍实体，游戏网格与修复结果才一致。
 * 完全确定性：只由布局决定，无随机；每轮至少移除 1 块，单调收敛、必然终止。 */
static void jpm_repair_connectivity(const joy_pm_config_t* cfg, joy_pm_layout_t* out)
{
        int grid_n = cfg->grid_n;
        const int dx4[4] = { 1, -1, 0, 0 };
        const int dz4[4] = { 0, 0, 1, -1 };
        const int sph = grid_n / 2;   /* 玩家主格（网格中心） */

        for (;;) {
                jpm_rasterize_impl(cfg, out->walkable, out->obs, out->count);
                bool* w = out->walkable;
                int cells = grid_n * grid_n;

                /* 主连通区（开放格，从玩家 BFS） */
                bool inmain[JPM_MAX_CELLS];
                for (int i = 0; i < cells; i++) inmain[i] = false;
                {
                        int mq[JPM_MAX_CELLS], mh = 0, mt = 0;
                        int s = sph * grid_n + sph;
                        if (w[s]) { mq[mt++] = s; inmain[s] = true; }
                        while (mh < mt) {
                                int c = mq[mh++], cx = c % grid_n, cz = c / grid_n;
                                for (int d = 0; d < 4; d++) {
                                        int nx = cx + dx4[d], nz = cz + dz4[d];
                                        if (nx < 0 || nx >= grid_n || nz < 0 || nz >= grid_n) continue;
                                        int ni = nz * grid_n + nx;
                                        if (!w[ni] || inmain[ni]) continue;
                                        inmain[ni] = true; mq[mt++] = ni;
                                }
                        }
                }

                /* 找一个未连通的开放格（某个封闭口袋的入口） */
                int target = -1;
                for (int i = 0; i < cells; i++) if (w[i] && !inmain[i]) { target = i; break; }
                if (target < 0) break;   /* 全部连通 */

                /* 每格属于哪块障碍 */
                int cellobs[JPM_MAX_CELLS];
                jpm_build_cell_mask(cfg, out->obs, out->count, cellobs);

                /* 多源 BFS：从主区开放格出发穿越所有格，parent 记录来向 */
                int parent[JPM_MAX_CELLS];
                for (int i = 0; i < cells; i++) parent[i] = inmain[i] ? -1 : -2;
                {
                        int q[JPM_MAX_CELLS], h = 0, t = 0;
                        for (int i = 0; i < cells; i++) if (inmain[i]) q[t++] = i;
                        while (h < t) {
                                int c = q[h++], cx = c % grid_n, cz = c / grid_n;
                                for (int d = 0; d < 4; d++) {
                                        int nx = cx + dx4[d], nz = cz + dz4[d];
                                        if (nx < 0 || nx >= grid_n || nz < 0 || nz >= grid_n) continue;
                                        int ni = nz * grid_n + nx;
                                        if (parent[ni] != -2) continue;
                                        parent[ni] = c; q[t++] = ni;
                                }
                        }
                }

                /* 沿 parent 从 target 回凿：链上穿过的障碍整块标记移除 */
                bool rm[JPM_MAX_OBS];
                for (int k = 0; k < out->count; k++) rm[k] = false;
                for (int c = target; c != -1 && parent[c] != -1; c = parent[c])
                        if (cellobs[c] >= 0) rm[cellobs[c]] = true;

                int n = 0;
                for (int k = 0; k < out->count; k++) if (!rm[k]) out->obs[n++] = out->obs[k];
                if (n == out->count) {
                        /* 防御：本轮未移除任何障碍（异常情况），强制移除 target 所在障碍避免死循环 */
                        int fo = cellobs[target];
                        if (fo >= 0) {
                                int n2 = 0;
                                for (int k = 0; k < out->count; k++) if (k != fo) out->obs[n2++] = out->obs[k];
                                n = n2;
                        } else break;
                }
                out->count = n;
        }
}

/* ============ BSP 房间切分 ============ */

#define BSP_MAX_WALL 64   /* 最多隔墙数（水平/竖直），grid_n<=64 足够 */

/* 每道隔墙定义为：一条固定列/行的 1 格厚墙，沿另一轴在 [a,b] 内延伸。
 * 切分时该列/行不再分给两个子区域，因此祖先墙与后代墙永不交叉，
 * 门洞可安全地凿在任意位置而不被其它墙挡住。 */
static int  bsp_wall_axis[BSP_MAX_WALL]; /* 0=竖直墙(固定列)，1=水平墙(固定行) */
static int  bsp_wall_fix[BSP_MAX_WALL];  /* 固定列或行的下标 */
static int  bsp_wall_a[BSP_MAX_WALL];    /* 沿另一轴的起点 */
static int  bsp_wall_b[BSP_MAX_WALL];    /* 沿另一轴的终点 */
static int  bsp_wall_count;

/* 递归切分矩形 [x0..x1]×[y0..y1]。min_size 为房间最小边长，深度受 max_depth 限制。
 * 每刀优先切更长的一侧，并在两子区域间留一条隔墙。完全确定性（仅依赖 rng）。 */
static void bsp_split_rec(joy_random_num_p rng, int min_size, int depth, int max_depth,
                          int x0, int y0, int x1, int y1)
{
        int w = x1 - x0 + 1, h = y1 - y0 + 1;
        bool can_split_col = w >= 2 * min_size;   /* 可竖切（产生竖直隔墙） */
        bool can_split_row = h >= 2 * min_size;   /* 可横切（产生水平隔墙） */
        if (!can_split_col && !can_split_row) return;   /* 叶子：这就是一个房间 */
        if (depth >= max_depth) return;

        /* 取向：优先切更长的一侧，使房间更方正 */
        int axis;
        if (can_split_col && can_split_row) axis = (w >= h) ? 0 : 1;
        else axis = can_split_col ? 0 : 1;

        if (axis == 0) {
                int lo = x0 + min_size, hi = x1 - min_size;
                int bc = lo + jpm_rand_mod(rng, hi - lo + 1);
                if (bsp_wall_count < BSP_MAX_WALL) {
                        bsp_wall_axis[bsp_wall_count] = 0;
                        bsp_wall_fix[bsp_wall_count] = bc;
                        bsp_wall_a[bsp_wall_count] = y0;
                        bsp_wall_b[bsp_wall_count] = y1;
                        bsp_wall_count++;
                }
                bsp_split_rec(rng, min_size, depth + 1, max_depth, x0, y0, bc - 1, y1);
                bsp_split_rec(rng, min_size, depth + 1, max_depth, bc + 1, y0, x1, y1);
        } else {
                int lo = y0 + min_size, hi = y1 - min_size;
                int br = lo + jpm_rand_mod(rng, hi - lo + 1);
                if (bsp_wall_count < BSP_MAX_WALL) {
                        bsp_wall_axis[bsp_wall_count] = 1;
                        bsp_wall_fix[bsp_wall_count] = br;
                        bsp_wall_a[bsp_wall_count] = x0;
                        bsp_wall_b[bsp_wall_count] = x1;
                        bsp_wall_count++;
                }
                bsp_split_rec(rng, min_size, depth + 1, max_depth, x0, y0, x1, br - 1);
                bsp_split_rec(rng, min_size, depth + 1, max_depth, x0, br + 1, x1, y1);
        }
}

/* ============ 公开接口 ============ */

void joy_scene_procmap_config_default(joy_pm_config_t* cfg)
{
        cfg->grid_n       = 20;
        cfg->offset10     = 100;   /* 10m */
        cfg->pad10        = 6;     /* 0.6m，与寻路外扩一致 */
        cfg->density      = 25;
        cfg->max_obs      = JPM_MAX_OBS;
        cfg->height_min10 = 5;     /* 0.5m */
        cfg->height_max10 = 18;    /* 1.8m */
        cfg->cluster_min  = 5;
        cfg->cluster_max  = 10;
        cfg->spawn_count  = 0;
        /* 默认追加两组与游戏一致的安全区：玩家 (0,0) + 敌人 (10m,10m)，半径 2.2m */
        joy_scene_procmap_config_add_spawn(cfg, 0, 0, 22);
        joy_scene_procmap_config_add_spawn(cfg, 100, 100, 22);
}

void joy_scene_procmap_config_add_spawn(joy_pm_config_p cfg, int cx10, int cz10, int radius10)
{
        if (cfg->spawn_count >= JPM_MAX_SPAWN) return;
        cfg->spawns[cfg->spawn_count].cx10 = cx10;
        cfg->spawns[cfg->spawn_count].cz10 = cz10;
        cfg->spawns[cfg->spawn_count].radius10 = radius10;
        cfg->spawn_count++;
}

uint32_t joy_scene_procmap_wave_seed(uint32_t base_seed, int wave)
{
        return base_seed + (uint32_t)wave * 1000003u;
}

void joy_scene_procmap_rasterize(const joy_pm_config_t* cfg, bool walkable[JPM_MAX_CELLS],
                   const joy_pm_obstacle_t* obs, int n)
{
        jpm_rasterize_impl(cfg, walkable, obs, n);
}

int joy_scene_procmap_bfs(const bool walkable[JPM_MAX_CELLS], int grid_n, int sx, int sz)
{
        return jpm_bfs_impl(walkable, grid_n, sx, sz);
}

void joy_scene_procmap_generate_seed(const joy_pm_config_t* cfg, uint32_t seed, joy_pm_layout_t* out)
{
        int grid_n = cfg->grid_n;
        const int min_size = 5;   /* 房间最小边长（格） */
        joy_random_num_p rng = joy_core_utils_random_create(seed);
        out->seed = (int)seed;
        out->count = 0;

        if (grid_n >= 2 * min_size) {
                int max_depth = grid_n <= 24 ? 4 : 5;
                bsp_wall_count = 0;

                /* 1. BSP 递归切分：每刀在两子区域之间留一道 1 格厚隔墙 */
                bsp_split_rec(rng, min_size, 0, max_depth, 0, 0, grid_n - 1, grid_n - 1);

                /* 2. 把隔墙切成障碍实体：每道墙凿一个门洞（连通相邻房间），
                 *    并跳过落在出生安全区的格（安全区天然形成开口）。 */
                int n = 0;
                const int door_w = 3;   /* 门洞宽（格）≥3，抵消两侧障碍 0.6m 外扩后仍有净通道 */
                for (int w = 0; w < bsp_wall_count && n < cfg->max_obs; w++) {
                        int a = bsp_wall_a[w], b = bsp_wall_b[w];
                        int span = b - a + 1;
                        int door = span >= door_w ? a + jpm_rand_mod(rng, span - door_w + 1) : -1;

                        if (bsp_wall_axis[w] == 0) {
                                /* 竖直隔墙：固定列 fx，沿行延伸 */
                                int fx = bsp_wall_fix[w];
                                int r = a;
                                while (r <= b && n < cfg->max_obs) {
                                        if ((door >= 0 && r >= door && r < door + door_w)
                                            || jpm_in_spawn_zone(cfg, fx, r)) { r++; continue; }
                                        int r0 = r;
                                        while (r <= b && !((door >= 0 && r >= door && r < door + door_w))
                                               && !jpm_in_spawn_zone(cfg, fx, r)) r++;
                                        int r1 = r - 1;
                                        int zmn = (r0 - grid_n / 2) * 10;
                                        int zmx = (r1 - grid_n / 2 + 1) * 10;
                                        joy_pm_obstacle_t* o = &out->obs[n++];
                                        o->gx0 = fx; o->gz0 = r0; o->gw = 1; o->gh = r1 - r0 + 1;
                                        o->x10  = jpm_grid_center10(cfg, fx);
                                        o->z10  = (zmn + zmx) / 2;
                                        o->hx10 = 5;
                                        o->hz10 = (zmx - zmn) / 2;
                                        o->sy10 = cfg->height_min10 + jpm_rand_mod(rng,
                                                cfg->height_max10 - cfg->height_min10 + 1);
                                }
                        } else {
                                /* 水平隔墙：固定行 fr，沿列延伸 */
                                int fr = bsp_wall_fix[w];
                                int c = a;
                                while (c <= b && n < cfg->max_obs) {
                                        if ((door >= 0 && c >= door && c < door + door_w)
                                            || jpm_in_spawn_zone(cfg, c, fr)) { c++; continue; }
                                        int c0 = c;
                                        while (c <= b && !((door >= 0 && c >= door && c < door + door_w))
                                               && !jpm_in_spawn_zone(cfg, c, fr)) c++;
                                        int c1 = c - 1;
                                        int xmn = (c0 - grid_n / 2) * 10;
                                        int xmx = (c1 - grid_n / 2 + 1) * 10;
                                        joy_pm_obstacle_t* o = &out->obs[n++];
                                        o->gx0 = c0; o->gz0 = fr; o->gw = c1 - c0 + 1; o->gh = 1;
                                        o->x10  = (xmn + xmx) / 2;
                                        o->z10  = jpm_grid_center10(cfg, fr);
                                        o->hx10 = (xmx - xmn) / 2;
                                        o->hz10 = 5;
                                        o->sy10 = cfg->height_min10 + jpm_rand_mod(rng,
                                                cfg->height_max10 - cfg->height_min10 + 1);
                                }
                        }
                }
                out->count = n;
        }

        /* 3. 连通性兜底：BSP 门洞已保证全连通，此步仅作安全网 */
        jpm_repair_connectivity(cfg, out);

        joy_core_utils_random_destroy(rng);
}

void joy_scene_procmap_generate(const joy_pm_config_t* cfg, joy_random_num_p rng, joy_pm_layout_t* out)
{
        uint32_t seed = joy_core_utils_random_next(rng);
        joy_scene_procmap_generate_seed(cfg, seed, out);
}

int joy_scene_procmap_sample_open_tiles(joy_random_num_p rng, const joy_pm_config_t* cfg,
                          const joy_pm_layout_t* L, int count, int min_dist10,
                          int* out_tiles)
{
        int grid_n = cfg->grid_n;
        /* 以玩家安全区（spawns[0]）圆心作为距离基准，缺省回退到网格中心 */
        int basex = grid_n / 2 * 10;
        int basez = grid_n / 2 * 10;
        if (cfg->spawn_count > 0) { basex = cfg->spawns[0].cx10; basez = cfg->spawns[0].cz10; }

        int open[JPM_MAX_CELLS], n = 0;
        for (int gz = 0; gz < grid_n; gz++) {
                for (int gx = 0; gx < grid_n; gx++) {
                        int i = gz * grid_n + gx;
                        if (!L->walkable[i]) continue;
                        int x10 = jpm_grid_center10(cfg, gx);
                        int z10 = jpm_grid_center10(cfg, gz);
                        int dx = x10 - basex, dz = z10 - basez;
                        if (dx * dx + dz * dz < min_dist10 * min_dist10) continue;   /* 远离玩家 */
                        open[n++] = i;
                }
        }
        /* Fisher-Yates 洗牌，取前 count 个 */
        for (int i = n - 1; i > 0 && count > 0; i--) {
                int j = jpm_rand_mod(rng, i + 1);
                int t = open[i]; open[i] = open[j]; open[j] = t;
        }
        int take = n < count ? n : count;
        for (int i = 0; i < take; i++) out_tiles[i] = open[i];
        return take;
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef BSP_MAX_WALL

/* ==================== 结束 src/jprocmap.c ==================== */

/* ==================== 合并自 src/jbehavior.c ==================== */
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
joy_bt_status_t joy_scene_bt_node_run(joy_bt_node_t* node, float dt) {
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

void joy_scene_bt_node_reset(joy_bt_node_t* node) {
    if (node) node->status = BT_FAILURE;
}

void joy_scene_bt_node_destroy(joy_bt_node_t* node) {
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

joy_bt_node_t* joy_scene_bt_action_create(joy_bt_action_fn fn) {
    joy_bt_action_t* a = (joy_bt_action_t*)calloc(1, sizeof(joy_bt_action_t));
    assert(a);
    a->base.tick = action_tick;
    a->fn = fn;
    return (joy_bt_node_t*)a;
}

joy_bt_node_t* joy_scene_bt_action_createv(joy_bt_action_fn fn, void* userdata) {
    joy_bt_action_t* a = (joy_bt_action_t*)joy_scene_bt_action_create(fn);
    a->userdata = userdata;
    return (joy_bt_node_t*)a;
}

void joy_scene_bt_action_set_userdata(joy_bt_node_t* node, void* userdata) {
    if (node) ((joy_bt_action_t*)node)->userdata = userdata;
}

void* joy_scene_bt_action_get_userdata(joy_bt_node_t* node) {
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

joy_bt_node_t* joy_scene_bt_condition_create(joy_bt_condition_fn pred) {
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

joy_bt_node_t* joy_scene_bt_wait_create(float seconds) {
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
        joy_bt_status_t s = joy_scene_bt_node_run(child, dt);
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

joy_bt_node_t* joy_scene_bt_sequence_create(joy_bt_node_t** children, int count) {
    return combo_create(COMBO_SEQUENCE, children, count);
}

joy_bt_node_t* joy_scene_bt_selector_create(joy_bt_node_t** children, int count) {
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
        joy_bt_status_t s = joy_scene_bt_node_run(p->children[i], dt);
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

joy_bt_node_t* joy_scene_bt_parallel_create(joy_bt_node_t** children, int count, int success_count) {
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
    joy_bt_status_t s = joy_scene_bt_node_run(((joy_bt_decorator1_t*)self)->child, dt);
    if (s == BT_SUCCESS) return BT_FAILURE;
    if (s == BT_FAILURE) return BT_SUCCESS;
    return BT_RUNNING;
}

static joy_bt_status_t succeeder_tick(joy_bt_node_t* self, float dt) {
    joy_scene_bt_node_run(((joy_bt_decorator1_t*)self)->child, dt);
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
    joy_bt_status_t s = joy_scene_bt_node_run(r->child, dt);
    if (s == BT_RUNNING) return BT_RUNNING;
    r->count++;
    if (r->times > 0 && r->count >= r->times) {
        stop_child(r->child);
        return BT_SUCCESS;
    }
    stop_child(r->child);
    joy_scene_bt_node_reset(r->child);
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

joy_bt_node_t* joy_scene_bt_statemachine_create(joy_bt_state_slot_t* states, int count, const char* initial) {
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

void joy_scene_bt_statemachine_transition(joy_bt_node_t* node, const char* state_name) {
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

/* ==================== 结束 src/jbehavior.c ==================== */
